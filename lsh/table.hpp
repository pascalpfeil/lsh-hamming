#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "bitvector.hpp"
#include "mask.hpp"

namespace lsh {

class Table {
  using offset_t = uint16_t;
  // We use a sorted vector for faster union as described in
  // https://lemire.me/blog/2017/01/27/how-expensive-are-the-union-and-intersection-of-two-unordered_set-in-c/.
  // The offsets will be inserted in sorted order automatically, as new elements
  // always have a higher offset w.r.t. the values_ vector.
  using offset_collection_t = std::vector<offset_t>;

  using bucket_t = std::unordered_map<vector_t, offset_collection_t>;

  static constexpr size_t kMaxHammingDistance =
      std::numeric_limits<size_t>::max();

 public:
  Table(const size_t num_bits, const size_t hash_bits,
        const size_t num_hash_functions)
      : num_bits_(num_bits), buckets_(num_hash_functions) {
    assert(hash_bits > 0);
    assert(num_bits > hash_bits);
    assert(num_hash_functions > 0);
    assert(num_bits > num_hash_functions);
    assert(num_bits < kMaxHammingDistance);

    for (size_t i = 0; i < num_hash_functions; ++i) {
      masks_.emplace_back(num_bits, hash_bits);
    }

    assert(masks_.size() == buckets_.size());
  }

  void insert(const vector_t& v) {
    assert(v.size() == num_bits_);
    assert(values_.size() <= std::numeric_limits<offset_t>().max());

    offset_t offset = values_.size();
    values_.push_back(v);

    for (size_t i = 0; i < buckets_.size(); ++i) {
      const vector_t key = masks_[i].project(v);
      buckets_[i][key].push_back(offset);
    }
  }

  [[nodiscard]] vector_t query(const vector_t& v) const {
    assert(v.size() == num_bits_);

    offset_collection_t all_offsets;
    for (size_t i = 0; i < buckets_.size(); ++i) {
      const auto& bucket = buckets_[i];
      const vector_t key = masks_[i].project(v);

      auto it = bucket.find(key);
      if (it != bucket.end()) {
        const offset_collection_t& offsets = it->second;

        offset_collection_t union_result;
        std::set_union(offsets.begin(), offsets.end(),          //
                       all_offsets.begin(), all_offsets.end(),  //
                       std::back_inserter(union_result));
        all_offsets = union_result;
      }
    }

    offset_t result_offset;
    size_t min_distance = kMaxHammingDistance;
    for (const offset_t offset : all_offsets) {
      const size_t distance = hamming_distance(v, values_[offset]);

      if (distance < min_distance) {
        result_offset = offset;
        min_distance = distance;
      }
    }

    return min_distance < kMaxHammingDistance ? values_[result_offset]
                                              : vector_t();
  }

  [[nodiscard]] size_t num_hash_bits() const {
    return masks_.empty() ? 0 : masks_.front().hash_bits();
  }

  [[nodiscard]] size_t num_hash_functions() const { return buckets_.size(); }

  [[nodiscard]] size_t size() const { return values_.size(); }

 private:
  const size_t num_bits_;

  std::vector<vector_t> values_;
  std::vector<bucket_t> buckets_;
  std::vector<Mask> masks_;
};

}  // namespace lsh
