#pragma once

#include <string>
#include <vector>

class Document {
 private:
  // our id
  uint32_t id_;
  // wiki's ID
  uint32_t wiki_id_;
  std::string title_;
  std::string text_;
  std::vector<std::string> links_;

 public:
  Document(uint32_t id, uint32_t wiki_id, std::string title, std::string text,
           std::vector<std::string> links);

  void set_id(uint32_t id);
  uint32_t get_id() const;
  uint32_t get_wiki_id() const;
  const std::string &get_title() const;
  const std::string &get_text() const;
  const std::vector<std::string> &get_links() const;
};
