#include "src/trigram/trigram.h"

#include <iostream>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(TrigramIndex, Basic) {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "test_db", &db);

  TrigramIndex ix{"test", db};
  // add some documents
  ix.AddDocument(0, "hello this is a test");
  ix.AddDocument(1, "hello world");
  ix.AddDocument(2, "what a funky document hey");
  ix.AddDocument(3, "the world looks fun from space");

  auto importance = std::make_unique<std::vector<float>>(4);
  (*importance)[0] = 1.0f;
  (*importance)[1] = 0.9f;
  (*importance)[2] = 0.8f;
  (*importance)[3] = 0.7f;

  ix.PrepareForQueries(importance);

  auto dummy_check = [](uint32_t doc_id) { return true; };

  EXPECT_THAT(ix.FindPossibleDocuments("hello", importance, dummy_check, 10),
              UnorderedElementsAre(0, 1));

  LOG(INFO) << "Searching for 'hello'";
  for (auto&& doc_id :
       ix.FindPossibleDocuments("hello", importance, dummy_check, 10)) {
    LOG(INFO) << "Found docID: " << doc_id;
  }

  EXPECT_THAT(ix.FindPossibleDocuments("world", importance, dummy_check, 10),
              UnorderedElementsAre(1, 3));

  LOG(INFO) << "Searching for 'world'";
  for (auto&& doc_id :
       ix.FindPossibleDocuments("world", importance, dummy_check, 10)) {
    LOG(INFO) << "Found docID: " << doc_id;
  }

  delete db;
}
