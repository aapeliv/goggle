/*
Forward index: stores documents by ID.

This is a thin wrapper around a hash map.
*/
#pragma once

#include "absl/container/flat_hash_map.h"
#include "src/common.h"
#include "src/doc.h"

class DocIndex {
 private:
  absl::flat_hash_map<docID_t, Document> data_;

 public:
  // ctor
  DocIndex();

  // add a doc into the index
  void AddDocument(Document&& doc);

  // retrieve doc from the index. note that the returned ref may be invalidated
  // if you add more docs after!
  const Document& GetDocument(docID_t id);

  size_t DocumentCount();
};
