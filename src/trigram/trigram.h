/*
Trigram index data structure.

The index is basically an array of pointers to sets. The array holds

  alphabet_length ^ N

elements (N is the length of an n-gram). Each element there points to a list of
document IDs that have that n-gram in them.


todo:
* Fix up n-grams: "hello world" should produce " he", "hel", "ell", "llo",
  "lo ", " wo", "wor", "orl", "rld", "ld ". But we currently miss " he", "ld ",
  and produce a bogus "o w" (trigrams shouldn't go past word boundaries)
* Think about a faster algorithm for merging (i.e. intersecting) the sets of
  docs. E.g. just sorting the vectors and going through them once suffices,
  instead of the current O(n^2) algorithm that repeatedly checks the second list
  of docs. Basically we can get easy wins with a bit of preprocessing.
*/
#pragma once

#include <ctype.h>
#include <stdint.h>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "glog/logging.h"
#include "leveldb/db.h"
#include "src/trigram/trigram_splitter.h"

// Provides facilities for an n-gram based index
class TrigramIndex {
 public:
  using container_type = std::vector<uint32_t>;

 private:
  bool ready_for_queries_ = false;
  std::unique_ptr<std::array<container_type, ngram_count>> data_;
  std::string name_;
  leveldb::DB* db_;

  /*
  This is a bit annoying to do repeatedly, so here's a helper.
  */
  container_type& GetContainerAt(size_t ix);

 public:
  TrigramIndex(std::string name, leveldb::DB* db);
  void LoadFromDB();
  void SaveToDB();
  void AddDocument(uint32_t doc_id, const std::string_view& text);
  void PrepareForQueries(const std::unique_ptr<std::vector<float>>& importance);
  // given a query string; a ranking importance function (NOTE: indexes must be
  // sorted with same importance (above)); and a check_doc function, will
  // retrieve possible docs and feed them to check_doc until that returns false
  std::vector<uint32_t> FindPossibleDocuments(
      const std::string_view& query,
      const std::unique_ptr<std::vector<float>>& importance,
      // returns true if we should keep going, false if not
      std::function<bool(uint32_t)> check_doc, size_t page_size);
};
