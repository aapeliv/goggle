#include "src/trigram/trigram.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <memory>
#include <string_view>

#include "leveldb/db.h"
#include "src/trigram/trigram.pb.h"

using clk = std::chrono::steady_clock;

TrigramIndex::container_type& TrigramIndex::GetContainerAt(size_t ix) {
  return (data_.get())->at(ix);
}

TrigramIndex::TrigramIndex(std::string name, leveldb::DB* db)
    : data_(new std::array<container_type, ngram_count>),
      name_(name),
      db_(db) {}

void TrigramIndex::LoadFromDB() {
  for (int ix = 0; ix < ngram_count; ++ix) {
    std::string out;
    auto s = db_->Get(leveldb::ReadOptions(),
                      "trgm/" + name_ + "/" + std::to_string(ix), &out);
    CHECK(s.ok()) << "Failed to read trigram";
    goggle::TrigramVec vec{};
    vec.ParseFromString(out);
    (*data_)[ix] = {vec.docs().begin(), vec.docs().end()};
  }
  ready_for_queries_ = true;
}

void TrigramIndex::SaveToDB() {
  for (int ix = 0; ix < ngram_count; ++ix) {
    auto c = (data_.get())->at(ix);
    goggle::TrigramVec vec{};
    *vec.mutable_docs() = {c.begin(), c.end()};
    auto s = db_->Put(leveldb::WriteOptions(),
                      "trgm/" + name_ + "/" + std::to_string(ix),
                      vec.SerializeAsString());
    CHECK(s.ok()) << "Failed to write trigram";
  }
}

/*
Add a document to the index
*/
void TrigramIndex::AddDocument(uint32_t doc_id, const std::string_view& text) {
  for (auto& ix : split_into_trigrams(text)) {
    GetContainerAt(ix).emplace_back(doc_id);
  }
}

bool doc_order(const std::unique_ptr<std::vector<float>>& importance,
               uint32_t doc_a_id, uint32_t doc_b_id) {
  auto ia = (*importance)[doc_a_id];
  auto ib = (*importance)[doc_b_id];
  if (ia != ib) {
    return ia > ib;
  } else {
    return doc_a_id > doc_b_id;
  }
}

/*
Comparator for trigrams based on importance (e.g. page rank)
*/
void TrigramIndex::PrepareForQueries(
    const std::unique_ptr<std::vector<float>>& importance) {
  auto cmp = std::bind(doc_order, std::cref(importance), std::placeholders::_1,
                       std::placeholders::_2);
  for (auto& c : *data_) {
    std::sort(c.begin(), c.end(), cmp);
    c.shrink_to_fit();
  }
  ready_for_queries_ = true;
}

/*
Retrieve documents from the index using a query string, note that this may
return also documents that don't match the query.
*/
void TrigramIndex::FindPossibleDocuments(
    const std::string_view& query,
    const std::unique_ptr<std::vector<float>>& importance,
    std::function<bool(uint32_t)> check_doc) {
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
  LOG(INFO) << "Have trigrams:";
  for (auto&& trigram : trigrams) {
    LOG(INFO) << "Have trigram: " << ix_to_string(trigram)
              << ", size: " << GetContainerAt(trigram).size();
  }

  auto total_docs = importance->size();

  auto cmp = std::bind(doc_order, std::cref(importance), std::placeholders::_1,
                       std::placeholders::_2);

  auto start = clk::now();

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
      if (docs.size() > 0.8 * total_docs) {
        LOG(INFO) << "More than 80\% docs for this trigram, breaking";
        break;
      }
      if (remaining_docs.size() < 50) {
        LOG(INFO) << "Less than 50 docs, breaking";
        break;
      }
      if (remaining_docs.size() > 1000 &&
          remaining_docs.size() > 0.05 * docs.size()) {
        // if the two sets being intersected are huge; then we do a linear scan
        // through both for the intersection

        // we use an intersection for sorted ranges: this does a linear search
        // through the trigrams
        container_type intersection{};
        std::set_intersection(docs.begin(), docs.end(), remaining_docs.begin(),
                              remaining_docs.end(),
                              std::back_inserter(intersection), cmp);
        remaining_docs = std::move(intersection);
      } else {
        // if the two sets are not huge, we do a binary search for each doc in
        // the smaller list against the bigger one
        auto it = remaining_docs.begin();
        while (it != remaining_docs.end()) {
          uint32_t doc_id = *it;
          // the trigram lists are ordered according to
          if (!std::binary_search(docs.begin(), docs.end(), doc_id, cmp)) {
            remaining_docs.erase(it);
          } else {
            ++it;
          }
        }
      }
    }
    LOG(INFO) << "After trigram " << ix_to_string(ix) << ", have "
              << remaining_docs.size() << " docs left";
  }
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(clk::now() - start)
          .count();

  LOG(INFO) << "Matched " << remaining_docs.size() << " docs in " << duration
            << " ms";

  for (auto&& doc_id : remaining_docs) {
    if (!check_doc(doc_id)) break;
  }
}
