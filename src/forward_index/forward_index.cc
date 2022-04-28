#include "src/forward_index/forward_index.h"

#include "absl/container/flat_hash_map.h"
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
  proto_doc.set_text(doc.get_text());
  *proto_doc.mutable_links() = {doc.get_links().begin(), doc.get_links().end()};
  auto s = data_->Put(leveldb::WriteOptions(),
                      "docs/" + std::to_string(doc.get_id()),
                      proto_doc.SerializeAsString());
  CHECK(s.ok()) << "Failed to write doc";
}

void DocIndex::AddDocument(Document* doc) {
  goggle::Doc proto_doc{};
  proto_doc.set_id(doc->get_id());
  proto_doc.set_wiki_id(doc->get_wiki_id());
  proto_doc.set_title(doc->get_title());
  proto_doc.set_text(doc->get_text());
  *proto_doc.mutable_links() = {doc->get_links().begin(),
                                doc->get_links().end()};
  auto s = data_->Put(leveldb::WriteOptions(),
                      "docs/" + std::to_string(doc->get_id()),
                      proto_doc.SerializeAsString());
  CHECK(s.ok()) << "Failed to write doc";
}

Document DocIndex::GetDocument(uint32_t id) {
  std::string out;
  auto s =
      data_->Get(leveldb::ReadOptions(), "docs/" + std::to_string(id), &out);
  CHECK(s.ok()) << "Failed to read doc";
  goggle::Doc proto_doc{};
  proto_doc.ParseFromString(out);
  return Document(proto_doc.id(), proto_doc.wiki_id(), proto_doc.title(),
                  proto_doc.text(),
                  std::vector<std::string>{proto_doc.links().begin(),
                                           proto_doc.links().end()});
}
