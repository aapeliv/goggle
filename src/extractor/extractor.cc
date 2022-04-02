#include <fstream>
#include <iostream>
#include <memory>

#include "glog/logging.h"
#include "third_party/bzip2/bzlib.h"
#include "third_party/rapidxml/rapidxml.hpp"

int main() {
  std::ifstream dump1;
  dump1.open(
      "src/extractor/"
      "enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2");
  // move input cursor to start of this stream
  dump1.seekg(683660);
  CHECK(dump1.good()) << "Failed to seek";
  // length of input stream
  size_t len = 2100345 - 683660;
  // compressed data
  auto data = std::unique_ptr<char[]>(new char[len]);
  // read the compressed data from the middle of the file
  dump1.read(data.get(), len);
  CHECK(dump1.good()) << "Failed to read";

  // decompressed length: guesstimate at 5* compressed size (for compression
  // ratio of 1 to 5)
  size_t dlen = len * 5;
  // decompressed data
  auto decompressed = std::unique_ptr<char[]>(new char[dlen]);
  // following function will fill in the true size of output here
  size_t dlen_out = dlen;

  // decompress the stream
  int res = BZ2_bzBuffToBuffDecompress(
      decompressed.get(), reinterpret_cast<unsigned int *>(&dlen_out),
      data.get(), len, 0, 0);
  CHECK(res != BZ_OUTBUFF_FULL) << "Didn't reserve enough memory.";
  CHECK(res == 0) << "Failed to decompress";

  dump1.close();
  CHECK(dump1.good()) << "Failed to close";

  LOG(INFO) << "Decompressed. Ratio was " << dlen_out / (1. * len);
  return 0;
}
