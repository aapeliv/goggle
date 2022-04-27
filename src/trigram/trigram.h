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
#include <string_view>
#include <vector>

#include "glog/logging.h"
#include "src/common.h"
#include "src/trigram/trigram_splitter.h"

// Provides facilities for an n-gram based index
class TrigramIndex {
 public:
  using container_type = std::vector<docID_t>;

 private:
  bool ready_for_queries_ = false;
  std::unique_ptr<std::array<container_type, ngram_count>> data_;

  /*
  This is a bit annoying to do repeatedly, so here's a helper.
  */
  container_type& GetContainerAt(size_t ix);

 public:
  TrigramIndex();
  void AddDocument(docID_t doc_id, const std::string_view& text);
  void PrepareForQueries(const std::unique_ptr<std::vector<float>>& importance);
  container_type FindPossibleDocuments(const std::string_view& query);
};
