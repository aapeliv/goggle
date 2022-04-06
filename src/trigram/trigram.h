#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <array>

// interface for gram index
class GramIndexValueType {
 public:
  virtual void Something() = 0;
};

constexpr static size_t alphabet_length = 27;

// computes alphabet_length ^ grams as a constexpr!
constexpr size_t count_ngrams(size_t grams, size_t result = 1) {
  return grams == 1 ? result : count_ngrams(grams - 1, alphabet_length * result);
}

// Provides facilities for
template<size_t grams, typename value_type, typename docT>
class GramIndex {
 private:
  constexpr static size_t ngram_count = count_ngrams(grams);
  std::unique_ptr<std::array<value_type, ngram_count>> ix_;
 public:
  GramIndex() : ix_() {}
  // AddDocument(docT doc_id, )
};

using TrigramIndex = GramIndex<3, std::vector<uint32_t>, uint32_t>;
