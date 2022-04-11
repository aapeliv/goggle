#include "src/trigram/trigram.h"

#include <iostream>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(TrigramIndex, Basic) {
  TrigramIndex ix{};
  // add some documents
  ix.AddDocument(1, "hello this is a test");
  ix.AddDocument(2, "hello world");
  ix.AddDocument(3, "what a funky document hey");
  ix.AddDocument(4, "the world looks fun from space");

  EXPECT_THAT(ix.FindPossibleDocuments("hello"), UnorderedElementsAre(1, 2));

  LOG(INFO) << "Searching for 'hello'";
  for (auto&& doc_id : ix.FindPossibleDocuments("hello")) {
    LOG(INFO) << "Found docID: " << doc_id;
  }

  EXPECT_THAT(ix.FindPossibleDocuments("world"), UnorderedElementsAre(2, 4));

  LOG(INFO) << "Searching for 'world'";
  for (auto&& doc_id : ix.FindPossibleDocuments("world")) {
    LOG(INFO) << "Found docID: " << doc_id;
  }
}
