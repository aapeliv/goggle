#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "glog/logging.h"
#include "src/doc.h"
#include "third_party/bzip2/bzlib.h"
#include "third_party/rapidxml/rapidxml.hpp"

std::unique_ptr<Document> ParseXml(rapidxml::xml_node<>* page_node) {
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
    return nullptr;
  } else if (id == 0 || title == "" || text == "") {
    LOG(WARNING) << "Incomplete page: id=" << id << ", title=" << title;
    return nullptr;
  } else {
    LOG(INFO) << "Found page; id=" << id << ", title=" << title
              << ", text length=" << text.size();
    return std::make_unique<Document>(id, title, text);
  }
}

std::vector<size_t> extract_offsets(std::string filename) {
  std::vector<size_t> offsets{0};

  std::ifstream index(filename);
  // dumb way to get file size
  index.seekg(0, std::ios::end);
  size_t index_len_compressed = index.tellg();
  size_t index_len_decompressed_allocated = 5 * index_len_compressed;
  auto index_data =
      std::unique_ptr<char[]>(new char[index_len_decompressed_allocated]);
  {
    auto index_data_compressed =
        std::unique_ptr<char[]>(new char[index_len_compressed]);

    // back to start
    index.seekg(0);
    CHECK(index.good()) << "Failed to seek";
    index.read(index_data_compressed.get(), index_len_compressed);
    CHECK(index.good()) << "Failed to read";

    size_t index_len_decompressed_actual = index_len_decompressed_allocated;

    // decompress the stream
    int index_res = BZ2_bzBuffToBuffDecompress(
        index_data.get(),
        reinterpret_cast<unsigned int*>(&index_len_decompressed_actual),
        index_data_compressed.get(), index_len_compressed, 0, 0);
    CHECK(index_res != BZ_OUTBUFF_FULL) << "Didn't reserve enough memory.";
    CHECK(index_res == 0) << "Failed to decompress";
  }

  size_t offset = 0;

  std::stringstream ss(std::move(index_data.get()));

  std::string line;
  while (std::getline(ss, line)) {
    std::string first_bit;
    std::stringstream line_ss(line);
    std::getline(line_ss, first_bit, ':');
    size_t new_offset = std::stoi(first_bit);
    if (new_offset != offset) {
      offset = new_offset;
      offsets.push_back(offset);
    }
  }
  return offsets;
}

int main() {
  auto offsets = extract_offsets(
      "src/extractor/"
      "enwiki-20220401-pages-articles-multistream-index1.txt-p1p41242.bz2");

  LOG(INFO) << "Offsets:";
  for (auto& i : offsets) {
    LOG(INFO) << i;
  }

  std::ifstream dump(
      "src/extractor/"
      "enwiki-20220401-pages-articles-multistream1.xml-p1p41242.bz2");

  // move input cursor to start of this stream
  dump.seekg(683660);
  CHECK(dump.good()) << "Failed to seek";
  // length of input stream
  size_t len = 2100345 - 683660;
  // compressed data
  auto data = std::unique_ptr<char[]>(new char[len]);
  // read the compressed data from the middle of the file
  dump.read(data.get(), len);
  CHECK(dump.good()) << "Failed to read";

  // decompressed length: guesstimate at 5* compressed size (for compression
  // ratio of 1 to 5)
  size_t dlen = len * 5;
  // decompressed data
  auto decompressed = std::unique_ptr<char[]>(new char[dlen]);
  // following function will fill in the true size of output here
  size_t dlen_out = dlen;

  // decompress the stream
  int res = BZ2_bzBuffToBuffDecompress(
      decompressed.get(), reinterpret_cast<unsigned int*>(&dlen_out),
      data.get(), len, 0, 0);
  CHECK(res != BZ_OUTBUFF_FULL) << "Didn't reserve enough memory.";
  CHECK(res == 0) << "Failed to decompress";

  dump.close();
  CHECK(dump.good()) << "Failed to close";

  LOG(INFO) << "Decompressed. Ratio was " << dlen_out / (1. * len);

  rapidxml::xml_document<> xml;
  xml.parse<0>(decompressed.get());

  // first page
  for (auto page_node = xml.first_node(); page_node != nullptr;
       page_node = page_node->next_sibling()) {
    auto doc = ParseXml(page_node);
    if (doc != nullptr) {
      LOG(INFO) << doc->get_title();
    }
  }

  return 0;
}
