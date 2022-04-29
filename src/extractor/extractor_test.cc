#include "src/extractor/extractor.h"

#include <memory>

#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/doc.h"

TEST(Extractor, Basic) {
  size_t title_len = 0;
  extract_dump(
      "data/"
      "enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242",
      "data/"
      "enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2",
      [&](std::unique_ptr<Document> doc) {
        title_len += doc->get_title().size();
      });
}
