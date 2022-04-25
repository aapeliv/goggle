#pragma once

#include <string>

#include "absl/container/flat_hash_set.h"

// there's <21k trigrams, so short (16 bits, ~65k) fits them easy
using trigram_ix_t = unsigned short;

constexpr static size_t alphabet_length = 'z' - 'a' + 2;
constexpr static size_t ngram_count =
    alphabet_length * alphabet_length * alphabet_length;

constexpr trigram_ix_t char_to_number(char c);

trigram_ix_t triplet_to_ix(trigram_ix_t a, trigram_ix_t b, trigram_ix_t c);

trigram_ix_t string_to_ix(std::string s);

absl::flat_hash_set<trigram_ix_t> split_into_trigrams(
    const std::string_view& text);
