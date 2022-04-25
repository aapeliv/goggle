#include <memory>

#include "glog/logging.h"
#include "httplib.h"
#include "src/doc.h"
#include "src/extractor/extractor.h"
#include "src/forward_index/forward_index.h"
#include "src/trigram/trigram.h"

int main() {
  LOG(INFO) << "Starting The Wikipedia Goggle.";

  DocIndex forward_ix{};
  TrigramIndex tri_ix{};

  extract_dump(
      "data/"
      "enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242.bz2",
      "data/"
      "enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2",
      [&](std::unique_ptr<Document> doc) {
        tri_ix.AddDocument(doc->get_id(), doc->get_text());
        forward_ix.AddDocument(
            Document(doc->get_id(), doc->get_title(), doc->get_text()));
      });

  httplib::Server srv;
  srv.Get("/", [](const httplib::Request&, httplib::Response& res) {
    res.set_content("{\"msg\": \"hello world\"}",
                    "application/json; charset=utf-8");
  });

  srv.Get("/query", [&](const httplib::Request& req, httplib::Response& res) {
    if (!req.has_param("q")) {
      res.set_content("{\"error\": \"missing query\"}",
                      "application/json; charset=utf-8");
    }
    auto query = req.get_param_value("q");
    std::stringstream ss{};
    bool is_first = true;
    ss << "{\"results\": [";
    for (auto&& doc_id : tri_ix.FindPossibleDocuments(query)) {
      auto doc = forward_ix.GetDocument(doc_id);
      LOG(INFO) << "found possible doc: " << doc_id
                << ", title = " << doc.get_title();
      if (is_first) {
        is_first = false;
      } else {
        ss << ",";
      }
      ss << "{";
      ss << "\"id\": " << doc.get_id() << ",";
      ss << "\"title\": \"" << doc.get_title() << "\"";
      // ss << \"text\": " << doc.get_text();
      ss << "}";
    }
    ss << "]}";
    res.set_content(ss.str(), "application/json; charset=utf-8");
  });

  LOG(INFO) << "Have " << forward_ix.DocumentCount() << " docs";

  LOG(INFO) << "Serving on 8080.";

  srv.listen("0.0.0.0", 8080);
}
