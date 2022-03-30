#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <unordered_set>
#include <vector>

#include "bitvector.hpp"

namespace lsh {

class Mask {
 private:
  using offset_t = uint16_t;

  static std::vector<offset_t> init_offsets(const size_t num_bits,
                                            const size_t hash_bits) {
    std::random_device rand;
    std::mt19937 gen{rand()};
    std::uniform_int_distribution<offset_t> dist(0u, num_bits - 1u);

    std::unordered_set<offset_t> offsets;
    offsets.reserve(hash_bits);
    while (offsets.size() < hash_bits) {
      offsets.insert(dist(gen));
    }

    return std::vector<offset_t>(offsets.begin(), offsets.end());
  }

 public:
  Mask(const size_t num_bits, const size_t hash_bits)
      : offsets_(init_offsets(num_bits, hash_bits)) {}

  [[nodiscard]] vector_t project(const vector_t &v) const {
    vector_t result(offsets_.size());

    for (size_t i = 0; i < offsets_.size(); ++i) {
      result[i] = v[offsets_[i]];
    }

    return result;
  }

  [[nodiscard]] size_t hash_bits() const { return offsets_.size(); }

 private:
  const std::vector<offset_t> offsets_;
};

}  // namespace lsh
