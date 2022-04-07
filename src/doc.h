#pragma once

#include <string>

class Document {
 private:
  uint32_t id_;
  std::string title_;
  std::string text_;

 public:
  Document(uint32_t id, std::string title, std::string text);

  uint32_t get_id() const;
  const std::string &get_title() const;
  const std::string &get_text() const;
};
