#include "src/forward_index/forward_index.h"

#include <string>

#include "glog/logging.h"
#include "leveldb/db.h"
#include "src/forward_index/doc.pb.h"

DocIndex::DocIndex(leveldb::DB* db) {
  data_ = db;
}

void DocIndex::AddDocument(const Document& doc) {
  goggle::Doc proto_doc{};
  proto_doc.set_id(doc.get_id());
  proto_doc.set_wiki_id(doc.get_wiki_id());
  proto_doc.set_title(doc.get_title());
  // proto_doc.set_text(doc.get_text());
  // *proto_doc.mutable_links() = {doc.get_links().begin(),
  // doc.get_links().end()};
  CHECK(data_
            ->Put(leveldb::WriteOptions(),
                  "doc_data/" + std::to_string(doc.get_id()),
                  proto_doc.SerializeAsString())
            .ok())
      << "Failed to write doc metadata";
  CHECK(data_
            ->Put(leveldb::WriteOptions(),
                  "doc_text/" + std::to_string(doc.get_id()), doc.get_text())
            .ok())
      << "Failed to write body";
}

void DocIndex::AddDocument(Document* doc) {
  goggle::Doc proto_doc{};
  proto_doc.set_id(doc->get_id());
  proto_doc.set_wiki_id(doc->get_wiki_id());
  proto_doc.set_title(doc->get_title());
  CHECK(data_
            ->Put(leveldb::WriteOptions(),
                  "doc_data/" + std::to_string(doc->get_id()),
                  proto_doc.SerializeAsString())
            .ok())
      << "Failed to write doc metadata";
  CHECK(data_
            ->Put(leveldb::WriteOptions(),
                  "doc_text/" + std::to_string(doc->get_id()), doc->get_text())
            .ok())
      << "Failed to write body";
}

Document DocIndex::GetDocument(uint32_t id) {
  std::string out;
  CHECK(
      data_->Get(leveldb::ReadOptions(), "doc_data/" + std::to_string(id), &out)
          .ok())
      << "Failed to read doc metadata";
  goggle::Doc proto_doc{};
  proto_doc.ParseFromString(out);
  std::string text;
  CHECK(
      data_
          ->Get(leveldb::ReadOptions(), "doc_text/" + std::to_string(id), &text)
          .ok())
      << "Failed to read doc text";
  return Document(proto_doc.id(), proto_doc.wiki_id(), proto_doc.title(),
                  std::move(text), std::vector<std::string>{});
}
