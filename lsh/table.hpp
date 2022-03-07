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
 public:
  Table(const size_t num_bits, const size_t hash_bits,
        const size_t num_hash_functions)
      : num_bits_(num_bits),
        hash_bits_(hash_bits),
        num_hash_functions_(num_hash_functions) {
    assert(num_bits > hash_bits);
    assert(num_bits > num_hash_functions);

    for (size_t i = 0; i < num_hash_functions_; ++i) {
      masks_.emplace_back(num_bits_, hash_bits_);
      buckets_.emplace_back();
    }

    assert(masks_.size() == buckets_.size());
  }

  void insert(const vector_t &v) {
    assert(v.size() == num_bits_);

    for (size_t i = 0; i < buckets_.size(); ++i) {
      const vector_t key = masks_[i].project(v);
      buckets_[i][key].push_back(v);
    }
  }

  [[nodiscard]] vector_t query(const vector_t &v) {
    assert(v.size() == num_bits_);

    vector_t result;
    size_t min_distance = std::numeric_limits<size_t>::max();

    for (size_t i = 0; i < buckets_.size(); ++i) {
      const vector_t key = masks_[i].project(v);
      const auto &b = buckets_[i][key];

      for (const auto &current : b) {
        const size_t distance = hamming_distance(v, current);

        if (distance < min_distance) {
          result = current;
          min_distance = distance;
        }
      }
    }

    return result;
  }

 private:
  const size_t num_bits_;
  const size_t hash_bits_;
  const size_t num_hash_functions_;

  std::vector<Mask> masks_;
  std::vector<std::unordered_map<vector_t, std::vector<vector_t>>> buckets_;
};

}  // namespace lsh
