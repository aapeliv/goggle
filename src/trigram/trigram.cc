#include "src/trigram/trigram.h"

#include <array>
#include <string_view>

#include "src/common.h"

TrigramIndex::container_type& TrigramIndex::GetContainerAt(size_t ix) {
  return (data_.get())->at(ix);
}

TrigramIndex::TrigramIndex()
    : data_(new std::array<container_type, ngram_count>) {}

/*
Add a document to the index
*/
void TrigramIndex::AddDocument(docID_t doc_id, const std::string_view& text) {
  for (auto& ix : split_into_trigrams(text)) {
    GetContainerAt(ix).emplace_back(doc_id);
  }
}

void TrigramIndex::PrepareForQueries() {
  for (auto& c : *data_) {
    std::sort(c.begin(), c.end());
    c.shrink_to_fit();
  }
  ready_for_queries_ = true;
}

/*
Retrieve documents from the index using a query string, note that this may
(with low probability) return also documents that don't match the query.
*/
TrigramIndex::container_type TrigramIndex::FindPossibleDocuments(
    const std::string_view& query) {
  CHECK(ready_for_queries_);
  auto trigrams_set = split_into_trigrams(query);
  std::vector<trigram_ix_t> trigrams{trigrams_set.begin(), trigrams_set.end()};
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
  LOG(INFO) << "Matched " << remaining_docs.size() << " docs";
  return remaining_docs;
}
