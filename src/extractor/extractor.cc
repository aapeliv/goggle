#include "src/extractor/extractor.h"

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <type_traits>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "glog/logging.h"
#include "src/doc.h"
#include "third_party/bzip2/bzlib.h"
#include "third_party/rapidxml/rapidxml.hpp"

using hrc = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

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
      if (title.starts_with("Category:")) ignore = true;
      if (title.starts_with("Draft:")) ignore = true;
      if (title.starts_with("File:")) ignore = true;
      if (title.starts_with("Gadget:")) ignore = true;
      if (title.starts_with("Help:")) ignore = true;
      if (title.starts_with("Media:")) ignore = true;
      if (title.starts_with("MediaWiki:")) ignore = true;
      if (title.starts_with("Module:")) ignore = true;
      if (title.starts_with("Portal:")) ignore = true;
      if (title.starts_with("Special:")) ignore = true;
      if (title.starts_with("Talk:")) ignore = true;
      if (title.starts_with("Template:")) ignore = true;
      if (title.starts_with("TimedText:")) ignore = true;
      if (title.starts_with("User:")) ignore = true;
      if (title.starts_with("Wikipedia:")) ignore = true;
    } else if (name == "revision") {
      // further child` node
      text = child_node->first_node("text")->value();
      links = extract_links(text);  // everything between [[ ]] and [[ | ]]
      // for (auto link : links)
      //   std::cout << link << "\n";
      text = remove_nonalphabetical(text);
    } else if (name == "redirect") {
      ignore = true;
    }
    if (ignore) break;
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
        0, id, title, text,
        std::vector<std::string>{links.begin(), links.end()});
  }
}

std::vector<std::pair<size_t, size_t>> extract_chunks_from_index_file(
    size_t dump_sz, std::string filename) {
  std::vector<std::pair<size_t, size_t>> chunks{0};

  std::ifstream index(filename);
  CHECK(index.good()) << "index file (" << filename
                      << ") not good: " << strerror(errno);
  size_t offset = 0;

  std::string line;
  while (std::getline(index, line)) {
    std::stringstream line_ss(line);
    size_t new_offset;
    line_ss >> new_offset;
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
  CHECK(dump.good()) << "dump file (" << dump_filename
                     << ") not good: " << strerror(errno);

  dump.seekg(0, std::ios::end);
  size_t dump_sz = dump.tellg();
  dump.seekg(0);
  LOG(INFO) << "dump_sz: " << dump_sz;

  std::vector<std::pair<size_t, size_t>> chunks{};
  chunks = extract_chunks_from_index_file(dump_sz, index_filename);

  // key: link to, value: list of pages that link here
  absl::flat_hash_map<std::string, std::vector<std::string>> backlinks{};

  auto start_time = hrc::now();
  auto total_time = std::chrono::duration_cast<ms>(start_time - start_time);

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

    int ratio = 5;
    double true_ratio = 0.;
    std::unique_ptr<char[]> decompressed{nullptr};

    bool worked = false;
    while (!worked) {
      // decompressed length: guesstimate at 5* compressed size (for compression
      // ratio of 1 to 5)
      size_t dlen = len * ratio;
      // decompressed data
      decompressed = std::unique_ptr<char[]>(new char[dlen]);
      // following function will fill in the true size of output here
      size_t dlen_out = dlen;

      // decompress the stream
      int res = BZ2_bzBuffToBuffDecompress(
          decompressed.get(), reinterpret_cast<unsigned int*>(&dlen_out),
          data.get(), len, 0, 0);
      if (res == BZ_OUTBUFF_FULL) {
        ratio *= 2;
        continue;
      }
      CHECK(res != BZ_OUTBUFF_FULL) << "Didn't reserve enough memory.";
      CHECK(res == 0) << "Failed to decompress";

      if (dlen_out + 1 >= dlen) {
        ratio *= 2;
        continue;
      }
      CHECK(dlen_out + 1 < dlen) << "Not enough space";
      decompressed.get()[dlen_out] = '\0';
      true_ratio = dlen_out / (1. * len);
      worked = true;
    }

    // std::ofstream ostr("out-" + std::to_string(start) + "-" +
    //                    std::to_string(end) + ".xml");
    // ostr << decompressed.get();

    // LOG(INFO) << decompressed.get();

    rapidxml::xml_document<> xml;
    xml.parse<0>(decompressed.get());

    for (auto page_node = xml.first_node(); page_node != nullptr;
         page_node = page_node->next_sibling()) {
      auto t0 = hrc::now();
      auto doc = ParseXml(backlinks, page_node);
      auto t1 = hrc::now();
      if (doc != nullptr) {
        // t0 = hrc::now();
        // remove_nonalphabetical(doc->get_text());
        // t1 = hrc::now();
        // LOG(INFO) << remove_nonalphabetical(doc->get_text()) << "\n";
        total_time += std::chrono::duration_cast<ms>(t1 - t0);
        process_doc(std::move(doc));
      }
    }

    auto percentage = (double)end / dump_sz;

    auto elapsed = std::chrono::duration_cast<ms>(hrc::now() - start_time);
    auto expected_total_min = elapsed.count() / percentage / 60'000.;
    auto elapsed_min = elapsed.count() / 60'000.;

    LOG(INFO) << "chunk: " << start << " to " << end << ", at "
              << 100. * percentage << "%, ratio was " << true_ratio
              << "... elapsed " << elapsed_min << " min, expect "
              << expected_total_min << " min total, "
              << expected_total_min - elapsed_min << " min left.";
  }

  dump.close();
  CHECK(dump.good()) << "Failed to close";
  return backlinks;
}
