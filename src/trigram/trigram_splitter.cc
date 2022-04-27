#include "src/trigram/trigram_splitter.h"

#include <string>

#include "absl/container/flat_hash_set.h"
#include "glog/logging.h"

constexpr trigram_ix_t char_to_number(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  }
  if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 1;
  }
  // if it's not alphabetical; just return 0
  return 0;
}

trigram_ix_t triplet_to_ix(trigram_ix_t a, trigram_ix_t b, trigram_ix_t c) {
  trigram_ix_t out = 0;
  out += a * alphabet_length * alphabet_length;
  out += b * alphabet_length;
  out += c;
  return out;
}

trigram_ix_t string_to_ix(std::string s) {
  CHECK(s.size() == 3);
  return triplet_to_ix(char_to_number(s[0]), char_to_number(s[1]),
                       char_to_number(s[2]));
}

absl::flat_hash_set<trigram_ix_t> split_into_trigrams(
    const std::string_view& text) {
  absl::flat_hash_set<trigram_ix_t> out{};
  // start with two spaces at the start
  trigram_ix_t t1 = 0;
  trigram_ix_t t2 = 0;
  trigram_ix_t t3 = 0;
  for (int i = 0; i < text.size(); ++i) {
    t1 = t2;
    t2 = t3;
    t3 = char_to_number(text[i]);
    out.insert(triplet_to_ix(t1, t2, t3));
    if (t2 == 0) {
      // space in the middle
      out.insert(triplet_to_ix(t1, 0, 0));
      out.insert(triplet_to_ix(0, 0, t3));
    }
    // LOG(INFO) << "trgm: " << (char)(t1 + 'a' - 1) << ", " << (char)(t2 + 'a'
    // - 1) << ", " << (char)(t3 + 'a' - 1);
  }
  out.insert(triplet_to_ix(t2, t3, 0));
  out.insert(triplet_to_ix(t3, 0, 0));
  return out;
}
