#include "src/forward_index/forward_index.h"

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(DocIndex, Basic) {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "test_db", &db);

  DocIndex ix{db};
  // add some documents
  ix.AddDocument(
      Document(1, 77, "Columbia", "NY, NY", std::vector<std::string>{}));
  ix.AddDocument(
      Document(2, 23, "Brown", "Providence, RI", std::vector<std::string>{}));
  ix.AddDocument(Document(3, 12, "Princeton", "Princeton, NJ",
                          std::vector<std::string>{}));
  ix.AddDocument(
      Document(4, 55, "Harvard", "Cambridge, MA", std::vector<std::string>{}));
  ix.AddDocument(
      Document(5, 11, "Dartmouth", "Hanover, NH", std::vector<std::string>{}));
  ix.AddDocument(
      Document(6, 45, "Cornell", "Cornell, NY", std::vector<std::string>{}));
  ix.AddDocument(
      Document(7, 74, "UPenn", "Pennsylvania, PA", std::vector<std::string>{}));
  ix.AddDocument(
      Document(8, 94, "Yale", "New Haven, CT", std::vector<std::string>{}));

  EXPECT_EQ(ix.GetDocument(2).get_title(), "Brown");
}
