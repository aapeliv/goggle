/*
Trigram index data structure.

The index is basically an array of pointers to sets. The array holds

  alphabet_length ^ N

elements (N is the length of an n-gram). Each element there points to a list of
document IDs that have that n-gram in them.

*/
#pragma once

#include <stdint.h>

#include <array>
#include <memory>
#include <string_view>
#include <vector>

// interface for gram index
class NGramIndexValueType {
 public:
  virtual void Something() = 0;
};

// todo: this is a bit ugly
// note starting space
constexpr static auto alphabet = " abcdefghijklmnopqrstuvwxyz";
constexpr static size_t alphabet_length = 27;

// computes alphabet_length ^ N as a constexpr!
// (al = alphabet_length)
constexpr size_t pow_al(size_t N, size_t result = 1) {
  return N == 0 ? result : pow_al(N - 1, alphabet_length * result);
}

// Provides facilities for an n-gram based index
template <size_t N, typename value_type, typename docID_t>
class NGramIndex {
 public:
  using container_type = std::vector<docID_t>;

 private:
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
  Computes the index in the array of a given n-gram
  */
  size_t ConvertNGramToIx(const std::string_view& n_gram) {
    CHECK(n_gram.size() == N);
    size_t index = 0;
    for (int i = 0; i < N; ++i) {
      char c = tolower(n_gram[i]);
      bool found = false;
      for (int z = 0; z < alphabet_length; ++z) {
        if (c == alphabet[z]) {
          index += z * pow_al(i);
          found = true;
          break;
        }
      }
      CHECK(found) << "n-gram had illegal stuff: " << n_gram;
    }
    return index;
  }

  /*
  Add a document to the index
  */
  void AddDocument(docID_t doc_id, const std::string_view& text) {
    for (int i = 0; i < text.size() - N + 1; ++i) {
      auto ix = ConvertNGramToIx(text.substr(i, N));
      GetContainerAt(ix).emplace_back(doc_id);
    }
  }

  /*
  Retrieve documents from the index using a query string

  TODO: this returns a list of candidates, they may not contain the query
  string!
  */
  container_type FindPossibleDocuments(const std::string_view& query) {
    container_type remaining_docs{};
    for (int i = 0; i < query.size() - N + 1; ++i) {
      auto ix = ConvertNGramToIx(query.substr(i, N));
      container_type& docs = GetContainerAt(ix);
      if (i == 0) {
        // for the first n-gram, we just copy the docs into remaining_docs
        // copy the docs into remaining docs
        remaining_docs = {docs.begin(), docs.end()};
      } else {
        // for others we intersect
        // TODO: this is a really dumb + slow algorithm, eventually we want
        // these sorted
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

using TrigramIndex = NGramIndex<3, std::vector<uint32_t>, uint32_t>;
