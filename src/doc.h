#pragma once

#include <string>
#include <vector>

class Document {
 private:
  uint32_t id_;
  std::string title_;
  std::string text_;
  std::vector<std::string> links_;

 public:
  Document(uint32_t id, std::string title, std::string text,
           std::vector<std::string> links);

  uint32_t get_id() const;
  const std::string &get_title() const;
  const std::string &get_text() const;
  const std::vector<std::string> &get_links() const;
};
