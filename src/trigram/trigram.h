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
#include "src/trigram/trigram_splitter.h"

// computes alphabet_length ^ N as a constexpr!
// (al = alphabet_length)
constexpr size_t pow_al(size_t N, size_t result = 1) {
  return N == 0 ? result : pow_al(N - 1, alphabet_length * result);
}

// Provides facilities for an n-gram based index
template <size_t N, typename docID_t>
class NGramIndex {
 public:
  using container_type = std::vector<docID_t>;

 private:
  bool ready_for_queries_ = false;
  constexpr static size_t ngram_count = pow_al(N);
  std::unique_ptr<std::array<container_type, ngram_count>> data_;

  /*
  This is a bit annoying to do repeatedly, so here's a helper.
  */
  container_type& GetContainerAt(size_t ix) {
    return (data_.get())->at(ix);
  }

 public:
  NGramIndex() : data_(new std::array<container_type, ngram_count>) {}

  /*
  Add a document to the index
  */
  void AddDocument(docID_t doc_id, const std::string_view& text) {
    for (auto& ix : split_into_trigrams(text)) {
      GetContainerAt(ix).emplace_back(doc_id);
    }
  }

  void PrepareForQueries() {
    for (auto& c : *data_) {
      std::sort(c.begin(), c.end());
    }
    ready_for_queries_ = true;
  }

  /*
  Retrieve documents from the index using a query string, note that this may
  (with low probability) return also documents that don't match the query.
  */
  container_type FindPossibleDocuments(const std::string_view& query) {
    CHECK(ready_for_queries_);
    auto trigrams_set = split_into_trigrams(query);
    std::vector<trigram_ix_t> trigrams{trigrams_set.begin(),
                                       trigrams_set.end()};
    std::sort(trigrams.begin(), trigrams.end(),
              [&](trigram_ix_t a, trigram_ix_t b) {
                // from cppref:
                // returns ​true if the first argument is less than (i.e. is
                // ordered before) the second
                return GetContainerAt(a).size() < GetContainerAt(b).size();
              });
    container_type remaining_docs{};
    bool is_first = true;
    for (auto& ix : trigrams) {
      container_type& docs = GetContainerAt(ix);
      if (is_first) {
        is_first = false;
        // for the first n-gram, we just copy the docs into remaining_docs
        // copy the docs into remaining docs
        remaining_docs = {docs.begin(), docs.end()};
      } else {
        // for others we intersect
        // todo: this is a really dumb + slow algorithm
        auto it = remaining_docs.begin();
        while (it != remaining_docs.end()) {
          docID_t doc_id = *it;
          // now search in new list
          bool found = false;
          for (auto&& new_doc_id : docs) {
            if (new_doc_id == doc_id) {
              found = true;
              break;
            }
          }
          if (!found) {
            remaining_docs.erase(it);
          } else {
            ++it;
          }
        }
      }
    }
    return remaining_docs;
  }
};

using TrigramIndex = NGramIndex<3, uint32_t>;
