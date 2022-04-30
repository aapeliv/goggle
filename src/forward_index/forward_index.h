/*
Forward index: stores documents by ID.

This is a thin wrapper around a hash map.
*/
#pragma once

#include <memory>

#include "leveldb/db.h"
#include "src/doc.h"

class DocIndex {
 private:
  // hard to make this managed due to dumb LevelDB::Open schemantics
  leveldb::DB* data_;

 public:
  DocIndex(leveldb::DB* db);

  // add a doc into the index
  void AddDocument(const Document& doc);
  void AddDocument(Document* doc);

  // retrieve doc from the index
  Document GetDocument(uint32_t id);
  // retrieve only doc body
  std::string GetDocumentBody(uint32_t id);
};
