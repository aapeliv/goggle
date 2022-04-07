#include "src/doc.h"

#include <string>

Document::Document(uint32_t id, std::string title, std::string text)
    : id_(id), title_(title), text_(text) {}

uint32_t Document::get_id() const {
  return id_;
}

const std::string &Document::get_title() const {
  return title_;
}

const std::string &Document::get_text() const {
  return text_;
}
