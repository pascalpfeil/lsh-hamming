#pragma once

#include <cstddef>
#include <cstdint>
#include <random>

#include "bitvector.hpp"

namespace lsh {

class Mask {
 private:
  using offset_t = uint16_t;

  static std::vector<offset_t> init_masks(const size_t num_bits,
                                          const size_t hash_bits) {
    std::random_device rand;
    std::mt19937 gen{rand()};
    std::uniform_int_distribution<offset_t> dist(0u, num_bits - 1u);

    std::vector<offset_t> masks;
    masks.reserve(hash_bits);
    for (size_t i = 0; i < hash_bits; ++i) {
      masks.emplace_back(dist(gen));
    }
    return masks;
  }

 public:
  Mask(const size_t num_bits, const size_t hash_bits)
      : masks_(init_masks(num_bits, hash_bits)) {}

  [[nodiscard]] vector_t project(const vector_t &v) const {
    vector_t result(masks_.size());

    for (size_t i = 0; i < masks_.size(); ++i) {
      result[i] = v[masks_[i]];
    }

    return result;
  }

  [[nodiscard]] size_t hash_bits() const { return masks_.size(); }

 private:
  const std::vector<offset_t> masks_;
};

}  // namespace lsh
