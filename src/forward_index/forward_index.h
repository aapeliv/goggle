/*
Forward index: stores documents by ID.

This is a thin wrapper around a hash map.
*/
#pragma once

#include <memory>

#include "leveldb/db.h"
#include "src/common.h"
#include "src/doc.h"

class DocIndex {
 private:
  // hard to make this managed due to dumb LevelDB::Open schemantics
  leveldb::DB* data_;

 public:
  DocIndex();
  ~DocIndex();

  // add a doc into the index
  void AddDocument(const Document& doc);

  // retrieve doc from the index
  Document GetDocument(docID_t id);
};
