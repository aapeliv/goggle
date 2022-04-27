#include "src/trigram/trigram_splitter.h"

#include <iostream>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::UnorderedElementsAre;

TEST(Splitter, Basic) {
  EXPECT_THAT(split_into_trigrams("hELlO wOrld"),
              UnorderedElementsAre(
                  string_to_ix("   "), string_to_ix("  h"), string_to_ix(" he"),
                  string_to_ix("hel"), string_to_ix("ell"), string_to_ix("llo"),
                  string_to_ix("lo "), string_to_ix("o  "), string_to_ix("o w"),
                  string_to_ix("  w"), string_to_ix(" wo"), string_to_ix("wor"),
                  string_to_ix("orl"), string_to_ix("rld"), string_to_ix("ld "),
                  string_to_ix("d  ")));
}
