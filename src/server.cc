#include <chrono>
#include <memory>

#include "absl/container/flat_hash_map.h"
#include "glog/logging.h"
#include "httplib.h"
#include "src/doc.h"
#include "src/extractor/extractor.h"
#include "src/forward_index/forward_index.h"
#include "src/trigram/trigram.h"

using clk = std::chrono::steady_clock;

int main() {
  LOG(INFO) << "Starting The Wikipedia Goggle.";

  DocIndex forward_ix{};
  TrigramIndex tri_ix{};
  TrigramIndex title_tri_ix{};

  // assign each doc an ID
  int N = 0;
  // title -> id
  absl::flat_hash_map<std::string, int> ids{};
  std::vector<int> out_link_count{};

  auto backlinks = extract_dump(
      "data/"
      "enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242.bz2",
      "data/"
      "enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2",
      [&](std::unique_ptr<Document> doc) {
        tri_ix.AddDocument(N, doc->get_text());
        title_tri_ix.AddDocument(N, doc->get_title());
        // TODO
        forward_ix.AddDocument(Document(N, doc->get_id(), doc->get_title(),
                                        doc->get_text(), doc->get_links()));
        ids[doc->get_title()] = N;
        out_link_count.push_back(doc->get_links().size());
        ++N;
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
  float d = 0.7;

  // intialize page rank, set to uniform dist
  auto pagerank = std::make_unique<std::vector<float>>(N);
  for (auto& pr : *pagerank) {
    pr = 1 / N;
  }

  // manual ranking
  auto page_goodness = std::make_unique<std::vector<float>>(N);
  double total_page_goodness = 0.;
  // add in some manual page goodness
  for (int n = 0; n < N; ++n) {
    auto doc = forward_ix.GetDocument(n);
    // outgoing links
    int link_count = doc.get_links().size();
    int link_chars = 0;
    for (auto&& l : doc.get_links()) {
      link_chars += l.size();
    }
    // page size, and subtract roughly link size
    int page_chars = doc.get_text().size();
    // > ~5k words
    bool is_very_long = (page_chars - link_chars) > 25'000;
    // < ~500 words
    bool is_short = (page_chars - link_chars) > 2'500;
    bool link_page = (double)page_chars / link_count < 90.;
    bool has_few_links = link_count < 10;
    double goodness = 10 + 2 * (is_very_long * (1 - link_page)) - 2 * is_short -
                      8 * link_page - 1 * has_few_links;
    (*page_goodness)[n] = goodness;
    total_page_goodness += goodness;
  }

  for (int i = 0; i < 50; ++i) {
    double diff = 0.;
    auto new_pagerank = std::make_unique<std::vector<float>>(N);
    for (int n = 0; n < N; ++n) {
      double new_pr = (1 - d) / N * (*page_goodness)[n] / total_page_goodness;
      // grab backlinks
      for (auto& link_id : backlinks_ids[n]) {
        new_pr += (*pagerank)[link_id] / out_link_count[link_id];
      }
      (*new_pagerank)[n] = new_pr;
      diff += std::abs(new_pr - (*pagerank)[n]);
    }
    LOG(INFO) << "iterated, diff = " << diff;
    pagerank = std::move(new_pagerank);
    if (diff < 1e-6) break;
  }

  LOG(INFO) << "Done computing PageRank";
  LOG(INFO) << "Sorting indexes";

  tri_ix.PrepareForQueries(pagerank);
  title_tri_ix.PrepareForQueries(pagerank);
  LOG(INFO) << "Done sorting indexes";

  httplib::Server srv;
  srv.Get("/", [](const httplib::Request&, httplib::Response& res) {
    res.set_content("{\"msg\": \"hello world\"}",
                    "application/json; charset=utf-8");
  });

  srv.Get("/query", [&](const httplib::Request& req, httplib::Response& res) {
    auto start = clk::now();
    if (!req.has_param("q")) {
      res.set_content("{\"error\": \"missing query\"}",
                      "application/json; charset=utf-8");
    }
    auto query = req.get_param_value("q");
    int limit = -1;
    if (req.has_param("pl")) {
      limit = std::stoi(req.get_param_value("pl"));
    }
    std::stringstream ss{};
    bool is_first = true;
    ss << "{\"results\": [";
    size_t ix_matches = 0;
    size_t real_matches = 0;
    absl::flat_hash_set<int> found{};
    for (auto&& doc_id : title_tri_ix.FindPossibleDocuments(query)) {
      if (limit == 0) break;
      auto doc = forward_ix.GetDocument(doc_id);
      ++ix_matches;
      auto lower_title{doc.get_title()};
      std::transform(lower_title.begin(), lower_title.end(),
                     lower_title.begin(),
                     [](auto c) { return std::tolower(c); });
      if (lower_title.find(query) != std::string::npos) {
        --limit;
        ++real_matches;
        if (is_first) {
          is_first = false;
        } else {
          ss << ",";
        }
        ss << "{";
        ss << "\"id\": " << doc_id << ",";
        ss << "\"pagerank\": " << (*pagerank)[doc_id] * N << ",";
        ss << "\"is_title_match\": true,";
        ss << "\"title\": \"" << doc.get_title() << "\"";
        if (real_matches == 1 && req.has_param("x")) {
          ss << ",\"text\": \"" << doc.get_text() << "\"";
        }
        // ss << \"text\": " << doc.get_text();
        ss << "}";
        found.insert(doc_id);
      }
    }
    for (auto&& doc_id : tri_ix.FindPossibleDocuments(query)) {
      if (limit == 0) break;
      if (found.contains(doc_id)) continue;
      auto doc = forward_ix.GetDocument(doc_id);
      ++ix_matches;
      if (doc.get_text().find(query) != std::string::npos) {
        --limit;
        ++real_matches;
        // LOG(INFO) << "found possible doc: " << doc_id
        //           << ", title = " << doc.get_title();
        if (is_first) {
          is_first = false;
        } else {
          ss << ",";
        }
        ss << "{";
        ss << "\"id\": " << doc_id << ",";
        ss << "\"pagerank\": " << (*pagerank)[doc_id] * N << ",";
        ss << "\"is_title_match\": false,";
        ss << "\"title\": \"" << doc.get_title() << "\"";
        if (real_matches == 1 && req.has_param("x")) {
          ss << ",\"text\": \"" << doc.get_text() << "\"";
        }
        // ss << \"text\": " << doc.get_text();
        ss << "}";
      }
    }
    ss << "],";

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();

    ss << "\"duration_ms\": " << std::to_string(duration) << "}";
    res.set_content(ss.str(), "application/json; charset=utf-8");
    LOG(INFO) << "Searched for \"" << query << "\"";
    LOG(INFO) << "Matched " << ix_matches << " docs using index, i.e. "
              << (double)100. * ix_matches / N << " %";
    LOG(INFO) << "Real matches: " << real_matches << ", i.e. "
              << (double)100. * real_matches / ix_matches << " % of ix matches";
    LOG(INFO) << "Total matched " << (double)100. * real_matches / N << " %";
    LOG(INFO) << "Took " << std::to_string(duration) << " ms";
  });

  LOG(INFO) << "Serving on 8080.";

  srv.listen("0.0.0.0", 8080);
}
