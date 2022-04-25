
#include "src/forward_index/forward_index.h"

#include "absl/container/flat_hash_map.h"
#include "glog/logging.h"

DocIndex::DocIndex() : data_() {}

void DocIndex::AddDocument(Document&& doc) {
  data_.try_emplace(doc.get_id(), doc);
}

const Document& DocIndex::GetDocument(docID_t id) {
  auto doc = data_.find(id);
  CHECK(doc != data_.end()) << "given id not in DocIndex";
  return doc->second;
}

size_t DocIndex::DocumentCount() {
  return data_.size();
}
