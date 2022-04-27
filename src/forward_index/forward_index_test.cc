#include "src/forward_index/forward_index.h"

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(DocIndex, Basic) {
  DocIndex ix{};
  // add some documents
  ix.AddDocument(Document(1, "Columbia", "NY, NY", std::vector<std::string>{}));
  ix.AddDocument(
      Document(2, "Brown", "Providence, RI", std::vector<std::string>{}));
  ix.AddDocument(
      Document(3, "Princeton", "Princeton, NJ", std::vector<std::string>{}));
  ix.AddDocument(
      Document(4, "Harvard", "Cambridge, MA", std::vector<std::string>{}));
  ix.AddDocument(
      Document(5, "Dartmouth", "Hanover, NH", std::vector<std::string>{}));
  ix.AddDocument(
      Document(6, "Cornell", "Cornell, NY", std::vector<std::string>{}));
  ix.AddDocument(
      Document(7, "UPenn", "Pennsylvania, PA", std::vector<std::string>{}));
  ix.AddDocument(
      Document(8, "Yale", "New Haven, CT", std::vector<std::string>{}));

  EXPECT_EQ(ix.GetDocument(2).get_title(), "Brown");
}
