#include "src/extractor/extractor.h"

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "glog/logging.h"
#include "src/doc.h"
#include "third_party/bzip2/bzlib.h"
#include "third_party/rapidxml/rapidxml.hpp"

using hrc = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

std::string strip_text(const std::string s) {
  std::string copy;

  std::regex rgx(".*<text.*>(.*)</text>.*");
  std::smatch match;

  copy = std::regex_replace(s, rgx, "$1");

  if (copy.find("== References =="))
    copy = copy.substr(0, copy.find("== References =="));

  std::regex rgx2("[{{].*[}}]");  // if between {{ }} delete whole thing
  copy = std::regex_replace(copy, rgx2, "");

  std::regex case1(R"(\[\[([^\[\]\|]+)\]\])");
  std::regex case2(R"(\[\[([^\[\]\|]+)\|([^\[\]\|]+)\]\])");
  copy = std::regex_replace(copy, case1, "$1");
  copy = std::regex_replace(copy, case2, "$2");
  std::regex rgx6("'''");
  copy = std::regex_replace(copy, rgx6, "");

  return copy;
}

absl::flat_hash_set<std::string> extract_links(std::string s) {
  absl::flat_hash_set<std::string> links;
  int i = 0;
  while (i < s.size()) {
    if (s[i] == '[' && s[i + 1] == '[') {
      int j = i + 2;
      while (s[j] != '|' && s[j] != ']') j++;
      links.insert(s.substr(i + 2, j - (i + 2)));
      i = j;
    } else {
      i++;
    }
  }
  return links;
}

std::string remove_nonalphabetical(std::string s) {
  int j = 0;
  bool last_skipped = false;
  for (int i = 0; i < s.size(); i++) {
    // Store only valid characters
    if ((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z')) {
      s[j] = std::tolower(s[i]);
      j++;
      last_skipped = false;
    } else {
      if (!last_skipped) {
        s[j] = ' ';
        j++;
      }
      last_skipped = true;
    }
  }
  return s.substr(0, j);
}

std::unique_ptr<Document> ParseXml(
    absl::flat_hash_map<std::string, std::vector<std::string>>& backlinks,
    rapidxml::xml_node<>* page_node) {
  // expect all children to just be pages
  CHECK(std::string(page_node->name()) == "page");

  uint32_t id = 0;
  std::string title{};
  std::string text{};
  absl::flat_hash_set<std::string> links{};
  bool ignore = false;
  for (auto child_node = page_node->first_node(); child_node != nullptr;
       child_node = child_node->next_sibling()) {
    std::string name{child_node->name()};
    if (name == "id") {
      id = atoi(child_node->value());
    } else if (name == "title") {
      title = child_node->value();
      if (title.starts_with("Wikipedia:")) {
        ignore = true;
      }
    } else if (name == "revision") {
      // further child` node
      text = child_node->first_node("text")->value();
      links = extract_links(text);  // everything between [[ ]] and [[ | ]]
      // for (auto link : links)
      //   std::cout << link << "\n";
      text = remove_nonalphabetical(text);
    } else if (name == "redirect") {
      // ignore redirects
      ignore = true;
      break;
    }
  }

  if (ignore) {
    // LOG(INFO) << "Found redirect page, skipping";
    return nullptr;
  } else if (id == 0 || title == "" || text == "") {
    LOG(WARNING) << "Incomplete page: id=" << id << ", title=" << title;
    return nullptr;
  } else {
    for (auto&& link : links) {
      backlinks[link].push_back(title);
    }

    // LOG(INFO) << "Found page; id=" << id << ", title=" << title << ", text
    // length=" << text.size();
    return std::make_unique<Document>(
        id, title, text, std::vector<std::string>{links.begin(), links.end()});
  }
}

std::vector<std::pair<size_t, size_t>> extract_chunks_from_index_file(
    size_t dump_sz, std::string filename) {
  std::vector<std::pair<size_t, size_t>> chunks{0};

  std::ifstream index(filename);
  // dumb way to get file size
  index.seekg(0, std::ios::end);
  size_t index_len_compressed = index.tellg();
  // back to start
  index.seekg(0);
  size_t index_len_decompressed_allocated = 5 * index_len_compressed;
  size_t index_len_decompressed_actual = index_len_decompressed_allocated;
  auto index_data =
      std::unique_ptr<char[]>(new char[index_len_decompressed_allocated]);
  {
    auto index_data_compressed =
        std::unique_ptr<char[]>(new char[index_len_compressed]);

    CHECK(index.good()) << "Failed to seek";
    index.read(index_data_compressed.get(), index_len_compressed);
    CHECK(index.good()) << "Failed to read";

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
      if (offset != 0) {
        chunks.push_back(std::make_pair(offset, new_offset));
      }
      offset = new_offset;
    }
  }
  chunks.push_back(std::make_pair(offset, dump_sz));
  return chunks;
}

// returns backlinks map
absl::flat_hash_map<std::string, std::vector<std::string>> extract_dump(
    std::string index_filename, std::string dump_filename,
    std::function<void(std::unique_ptr<Document>)> process_doc) {
  std::ifstream dump(dump_filename);

  auto t0 = hrc::now();
  auto t1 = hrc::now();
  auto total_time = std::chrono::duration_cast<ms>(t0 - t0);

  dump.seekg(0, std::ios::end);
  size_t dump_sz = dump.tellg();
  dump.seekg(0);
  LOG(INFO) << "dump_sz: " << dump_sz;

  auto chunks = extract_chunks_from_index_file(dump_sz, index_filename);

  // key: link to, value: list of pages that link here
  absl::flat_hash_map<std::string, std::vector<std::string>> backlinks{};

  for (auto& [start, end] : chunks) {
    // move input cursor to start of this stream
    dump.seekg(start);
    CHECK(dump.good()) << "Failed to seek";
    // length of input stream
    size_t len = end - start;
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

    CHECK(dlen_out + 1 < dlen) << "Not enough space";
    decompressed.get()[dlen_out] = '\0';

    // std::ofstream ostr("out-" + std::to_string(start) + "-" +
    //                    std::to_string(end) + ".xml");
    // ostr << decompressed.get();

    // LOG(INFO) << decompressed.get();

    rapidxml::xml_document<> xml;
    xml.parse<0>(decompressed.get());

    // first page
    for (auto page_node = xml.first_node(); page_node != nullptr;
         page_node = page_node->next_sibling()) {
      t0 = hrc::now();
      auto doc = ParseXml(backlinks, page_node);
      t1 = hrc::now();
      if (doc != nullptr) {
        // t0 = hrc::now();
        // remove_nonalphabetical(doc->get_text());
        // t1 = hrc::now();
        // LOG(INFO) << remove_nonalphabetical(doc->get_text()) << "\n";
        total_time += std::chrono::duration_cast<ms>(t1 - t0);
        process_doc(std::move(doc));
      }
    }

    LOG(INFO) << "chunk: " << start << " to " << end << ", at "
              << 100. * (double)end / dump_sz << "%, ratio was "
              << dlen_out / (1. * len);
    LOG(INFO) << "total text processing time: " << total_time.count()
              << " ms\n";
  }

  dump.close();
  CHECK(dump.good()) << "Failed to close";
  return backlinks;
}
