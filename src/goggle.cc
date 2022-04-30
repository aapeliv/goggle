#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <chrono>
#include <memory>

#include "absl/container/flat_hash_map.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "glog/logging.h"
#include "httplib.h"
#include "leveldb/cache.h"
#include "leveldb/db.h"
#include "leveldb/filter_policy.h"
#include "src/db.pb.h"
#include "src/doc.h"
#include "src/escape_json.h"
#include "src/extractor/extractor.h"
#include "src/forward_index/forward_index.h"
#include "src/trigram/trigram.h"

ABSL_FLAG(std::string, db_dir, "goggle_db/", "database directory");
ABSL_FLAG(std::string, index_file,
          "data/enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242",
          "path to index file");
ABSL_FLAG(std::string, dump_file,
          "data/enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2",
          "path to dump file");
ABSL_FLAG(std::string, frontend_serve_dir, "none",
          "directory to serve frontend from");
ABSL_FLAG(bool, enable_tls, false, "enable TLS");
ABSL_FLAG(std::string, server_key, "key.pem", "TLS key");
ABSL_FLAG(std::string, server_cert, "cert.pem", "TLS cert");

using clk = std::chrono::steady_clock;

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  absl::SetProgramUsageMessage("The Wikipedia Goggle");
  absl::ParseCommandLine(argc, argv);
  FLAGS_logtostderr = 1;

  LOG(INFO) << "Starting The Wikipedia Goggle.";

  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  options.filter_policy = leveldb::NewBloomFilterPolicy(10);
  options.block_cache =
      leveldb::NewLRUCache(1024 * 1024 * 1024);  // 1 GiB cache
  LOG(INFO) << "Opening database at " << absl::GetFlag(FLAGS_db_dir);
  leveldb::Status status =
      leveldb::DB::Open(options, absl::GetFlag(FLAGS_db_dir), &db);
  CHECK(status.ok()) << "Failed to open leveldb";

  // check if the DB is indexed and ready
  bool exists_and_ready = false;
  {
    std::string out;
    auto s = db->Get(leveldb::ReadOptions(), "db_info", &out);
    CHECK(status.ok()) << "Couldn't read db info";
    if (out.size() > 0) {
      goggle::DbInfo proto_db_info{};
      proto_db_info.ParseFromString(out);
      CHECK(proto_db_info.initd()) << "Found broken db: not initialized";
      CHECK(proto_db_info.ready()) << "Found broken db: not ready";
      exists_and_ready = true;
    }
  }

  DocIndex forward_ix{db};
  TrigramIndex tri_ix{"text", db};
  TrigramIndex title_tri_ix{"titles", db};
  auto pagerank = std::make_unique<std::vector<float>>();
  int N = 0;

  if (!exists_and_ready) {
    goggle::DbInfo proto_info_init{};
    proto_info_init.set_initd(true);
    auto s = db->Put(leveldb::WriteOptions(), "db_info",
                     proto_info_init.SerializeAsString());
    CHECK(s.ok()) << "Failed to write empty db info";

    // assign each doc an ID
    // title -> id
    absl::flat_hash_map<std::string, int> ids{};
    std::vector<int> out_link_count{};
    std::vector<float> page_goodness{};
    double total_page_goodness = 0.;

    auto backlinks = extract_dump(
        absl::GetFlag(FLAGS_index_file), absl::GetFlag(FLAGS_dump_file),
        [&](std::unique_ptr<Document> doc) {
          doc->set_id(N);
          tri_ix.AddDocument(N, doc->get_text());
          title_tri_ix.AddDocument(N, doc->get_title());
          // TODO
          forward_ix.AddDocument(doc.get());
          ids[doc->get_title()] = N;
          out_link_count.push_back(doc->get_links().size());
          ++N;

          // add in some manual page goodness
          // outgoing links
          int link_count = doc->get_links().size();
          int link_chars = 0;
          for (auto&& l : doc->get_links()) {
            link_chars += l.size();
          }
          // page size, and subtract roughly link size
          int page_chars = doc->get_text().size();
          // > ~5k words
          bool is_very_long = (page_chars - link_chars) > 25'000;
          // < ~500 words
          bool is_short = (page_chars - link_chars) > 2'500;
          bool link_page = (double)page_chars / link_count < 90.;
          bool has_few_links = link_count < 10;
          double goodness = 10 + 2 * (is_very_long * (1 - link_page)) -
                            2 * is_short - 8 * link_page - 1 * has_few_links;
          page_goodness.push_back(goodness);
          total_page_goodness += goodness;
          // end manual ranking
        });

    LOG(INFO) << "Have " << N << " docs";

    // ** Pagerank computation
    LOG(INFO) << "About to compute PageRank";

    // turn backlinks map into ids map
    absl::flat_hash_map<int, std::vector<int>> backlinks_ids{};

    for (auto&& [target, links] : backlinks) {
      if (ids.contains(target)) {
        std::vector<int> link_ids{};
        for (auto&& link : links) {
          if (ids.contains(link)) {
            link_ids.push_back(ids[link]);
          }
        }
        backlinks_ids.try_emplace(ids[target], link_ids);
      }
    }

    // note use of 4-byte floats

    // dampening factor
    double d = 0.7;

    // intialize page rank, set to uniform dist
    pagerank = std::make_unique<std::vector<float>>(N);
    for (auto& pr : *pagerank) {
      pr = (double)1 / N;
    }

    for (int i = 0; i < 50; ++i) {
      double diff = 0.;
      auto new_pagerank = std::make_unique<std::vector<float>>(N);
      for (int n = 0; n < N; ++n) {
        double new_pr = (1 - d) * page_goodness[n] / total_page_goodness;
        // grab backlinks
        for (auto& link_id : backlinks_ids[n]) {
          new_pr += d * (*pagerank)[link_id] / out_link_count[link_id];
        }
        (*new_pagerank)[n] = new_pr;
        diff += std::abs(new_pr - (*pagerank)[n]);
      }
      LOG(INFO) << "iterated, diff = " << diff;
      pagerank = std::move(new_pagerank);
      if (diff < 1e-9) break;
    }

    LOG(INFO) << "Done computing PageRank";
    LOG(INFO) << "Sorting indexes";

    tri_ix.PrepareForQueries(pagerank);
    title_tri_ix.PrepareForQueries(pagerank);

    LOG(INFO) << "Done sorting indexes";

    LOG(INFO) << "Saving stuff...";

    title_tri_ix.SaveToDB();
    tri_ix.SaveToDB();

    goggle::DbInfo proto_info{};
    proto_info.set_n(N);
    proto_info.set_initd(true);
    proto_info.set_ready(true);
    s = db->Put(leveldb::WriteOptions(), "db_info",
                proto_info.SerializeAsString());
    CHECK(s.ok()) << "Failed to write db info";

    goggle::PageRankVec proto_prs{};
    *proto_prs.mutable_prs() = {pagerank->begin(), pagerank->end()};
    s = db->Put(leveldb::WriteOptions(), "pagerank",
                proto_prs.SerializeAsString());
    CHECK(s.ok()) << "Failed to write pageranks";

    LOG(INFO) << "Done saving...";
  } else {
    // exists_and_ready: load everything
    LOG(INFO) << "Found ready database, loading index from disk";

    std::string out;
    auto s = db->Get(leveldb::ReadOptions(), "db_info", &out);
    CHECK(status.ok()) << "Couldn't read db info";
    goggle::DbInfo proto_db_info{};
    proto_db_info.ParseFromString(out);
    N = proto_db_info.n();

    s = db->Get(leveldb::ReadOptions(), "pagerank", &out);
    CHECK(s.ok()) << "Failed to read pageranks";
    goggle::PageRankVec proto_prs{};
    proto_prs.ParseFromString(out);
    *pagerank = {proto_prs.prs().begin(), proto_prs.prs().end()};

    title_tri_ix.LoadFromDB();

    tri_ix.LoadFromDB();
    LOG(INFO) << "Loaded index from disk";
  }

  LOG(INFO) << "All set up, have " << N << " docs";

  std::unique_ptr<httplib::Server> srv;

  if (absl::GetFlag(FLAGS_enable_tls)) {
    srv = std::make_unique<httplib::SSLServer>(
        absl::GetFlag(FLAGS_server_cert).c_str(),
        absl::GetFlag(FLAGS_server_key).c_str());
  } else {
    srv = std::make_unique<httplib::Server>();
  }

  if (absl::GetFlag(FLAGS_frontend_serve_dir) != "none") {
    CHECK(srv->set_mount_point("/", absl::GetFlag(FLAGS_frontend_serve_dir)))
        << "Couldn't mount frontend serve dir";
  }

  srv->Get("/query", [&](const httplib::Request& req, httplib::Response& res) {
    auto start = clk::now();
    if (!req.has_param("q")) {
      res.set_content("{\"error\": \"missing query\"}",
                      "application/json; charset=utf-8");
    }
    auto query = req.get_param_value("q");
    std::transform(query.begin(), query.end(), query.begin(),
                   [](auto c) { return std::tolower(c); });
    int limit = 8;
    if (req.has_param("pl")) {
      limit = std::min(std::stoi(req.get_param_value("pl")), 200);
    }
    auto check_doc_title = [&](uint32_t doc_id) {
      auto doc = forward_ix.GetDocument(doc_id);
      auto lower_title{doc.get_title()};
      std::transform(lower_title.begin(), lower_title.end(),
                     lower_title.begin(),
                     [](auto c) { return std::tolower(c); });
      return lower_title.find(query) != std::string::npos;
    };

    auto matches = title_tri_ix.FindPossibleDocuments(query, pagerank,
                                                      check_doc_title, limit);
    absl::flat_hash_set<int> title_matches{matches.begin(), matches.end()};
    if (limit != matches.size()) {
      auto check_doc = [&](uint32_t doc_id) {
        if (title_matches.contains(doc_id)) return false;
        auto text = forward_ix.GetDocumentBody(doc_id);
        return text.find(query) != std::string::npos;
      };
      auto out = tri_ix.FindPossibleDocuments(query, pagerank, check_doc,
                                              limit - matches.size());
      std::copy(out.begin(), out.end(), std::back_inserter(matches));
    }

    std::stringstream ss{};
    bool is_first = true;
    ss << "{\"results\": [";
    for (auto&& doc_id : matches) {
      // this repetition is not that bad given the leveldb cache, and it really
      // simplifies the programming
      auto doc = forward_ix.GetDocument(doc_id);

      if (is_first) {
        is_first = false;
      } else {
        ss << ",";
      }
      ss << "{";
      ss << "\"id\": " << doc_id << ",";
      ss << "\"url\": \"https://en.wikipedia.org/?curid=" << doc.get_wiki_id()
         << "\",";
      ss << "\"pagerank\": " << (*pagerank)[doc_id] * N << ",";
      ss << "\"title\": \"" << escape_json(doc.get_title()) << "\",";
      if (title_matches.contains(doc_id)) {
        // title match
        ss << "\"is_title_match\": true,";
        ss << "\"snippet\": \"" + escape_json(doc.get_text().substr(0, 120)) +
                  "\"";
      } else {
        // text match
        ss << "\"is_title_match\": false,";
        auto text = doc.get_text();
        auto preview_start = text.find(query);
        // a bit before
        auto pre_preview_start = std::max<int>(preview_start - 60, 0);
        std::string preview{};
        preview +=
            text.substr(pre_preview_start, preview_start - pre_preview_start);
        preview += "<b>";
        preview += text.substr(preview_start, query.size());
        preview += "</b>";
        preview += text.substr(preview_start + query.size(), 60);
        ss << "\"snippet\": \"" + escape_json(preview) + "\"";
      }
      ss << "}";
    }

    ss << "],";

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();

    ss << "\"duration_ms\": " << std::to_string(duration) << "}";
    res.set_content(ss.str(), "application/json; charset=utf-8");
    LOG(INFO) << "Searched for \"" << query << "\" with page size " << limit;
    LOG(INFO) << "Took " << std::to_string(duration) << " ms";

    res.set_header("Access-Control-Allow-Origin",
                   req.get_header_value("Origin").c_str());
    res.set_header("Allow", "GET, POST, HEAD, OPTIONS");
    res.set_header(
        "Access-Control-Allow-Headers",
        "X-Requested-With, Content-Type, Accept, Origin, Authorization");
    res.set_header("Access-Control-Allow-Methods", "OPTIONS, GET, POST, HEAD");
  });

  LOG(INFO) << "Serving on 8080.";

  srv->listen("0.0.0.0", 8080);

  delete db;
}
