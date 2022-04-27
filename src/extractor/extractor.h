#pragma once

#include <functional>
#include <memory>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "src/doc.h"
#include "third_party/rapidxml/rapidxml.hpp"

std::string strip_text(const std::string s);
std::unique_ptr<Document> ParseXml(rapidxml::xml_node<>* page_node);
std::vector<std::pair<size_t, size_t>> extract_chunks_from_index_file(
    size_t dump_sz, std::string filename);
absl::flat_hash_map<std::string, std::vector<std::string>> extract_dump(
    std::string index_filename, std::string dump_filename,
    std::function<void(std::unique_ptr<Document>)> process_doc);
