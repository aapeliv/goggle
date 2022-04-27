#include "src/doc.h"

#include <string>
#include <vector>

Document::Document(uint32_t id, std::string title, std::string text,
                   std::vector<std::string> links)
    : id_(id), title_(title), text_(text), links_(links) {}

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
