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

  rapidxml::xml_document<> xml;
  xml.parse<0>(decompressed.get());

  // first page
  for (auto page_node = xml.first_node(); page_node != nullptr;
       page_node = page_node->next_sibling()) {
    LOG(INFO) << page_node->name();
    // expect all children to just be pages
    CHECK(std::string(page_node->name()) == "page");

    uint32_t id = 0;
    std::string title{};
    std::string text{};
    bool is_redirect = false;

    for (auto child_node = page_node->first_node(); child_node != nullptr;
         child_node = child_node->next_sibling()) {
      std::string name{child_node->name()};
      if (name == "id") {
        id = atoi(child_node->value());
      } else if (name == "title") {
        title = child_node->value();
      } else if (name == "revision") {
        // further child node
        text = child_node->first_node("text")->value();
      } else if (name == "redirect") {
        is_redirect = true;
        break;
      }
    }

    if (is_redirect) {
      LOG(INFO) << "Found redirect page, skipping";
    } else if (id == 0 || title == "" || text == "") {
      LOG(WARNING) << "Incomplete page: id=" << id << ", title=" << title;
    } else {
      LOG(INFO) << "Found page; id=" << id << ", title=" << title << ", text:";
      LOG(INFO) << text;
    }
  }

  return 0;
}
