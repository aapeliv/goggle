#include "src/forward_index/forward_index.h"

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(DocIndex, Basic) {
  DocIndex ix{};
  // add some documents
  ix.AddDocument(Document(1, "Columbia", "NY, NY"));
  ix.AddDocument(Document(2, "Brown", "Providence, RI"));
  ix.AddDocument(Document(3, "Princeton", "Princeton, NJ"));
  ix.AddDocument(Document(4, "Harvard", "Cambridge, MA"));
  ix.AddDocument(Document(5, "Dartmouth", "Hanover, NH"));
  ix.AddDocument(Document(6, "Cornell", "Cornell, NY"));
  ix.AddDocument(Document(7, "UPenn", "Pennsylvania, PA"));
  ix.AddDocument(Document(8, "Yale", "New Haven, CT"));

  EXPECT_EQ(ix.GetDocument(2).get_title(), "Brown");
}
