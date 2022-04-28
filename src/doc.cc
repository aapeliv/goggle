#include "src/doc.h"

#include <string>
#include <vector>

Document::Document(uint32_t id, uint32_t wiki_id, std::string title,
                   std::string text, std::vector<std::string> links)
    : id_(id), wiki_id_(wiki_id), title_(title), text_(text), links_(links) {}

void Document::set_id(uint32_t id) {
  id_ = id;
}

uint32_t Document::get_wiki_id() const {
  return wiki_id_;
}

uint32_t Document::get_id() const {
  return id_;
}

const std::string &Document::get_title() const {
  return title_;
}

const std::string &Document::get_text() const {
  return text_;
}

const std::vector<std::string> &Document::get_links() const {
  return links_;
}
