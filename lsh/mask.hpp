#pragma once

#include <cstddef>
#include <cstdint>
#include <random>

#include "bitvector.hpp"

namespace lsh {

class Mask {
 public:
  Mask(const size_t num_bits, const size_t hash_bits) {
    std::random_device rand;
    std::mt19937 gen{rand()};
    std::uniform_int_distribution<size_t> dist(0u, num_bits - 1u);

    for (size_t i = 0; i < hash_bits; ++i) {
      masks_.emplace_back(dist(gen));
    }
  }

  [[nodiscard]] vector_t project(const vector_t &v) const {
    vector_t result(masks_.size());

    for (size_t i = 0; i < masks_.size(); ++i) {
      result[i] = v[masks_[i]];
    }

    return result;
  }

  size_t hash_bits() const { return masks_.size(); }

 private:
  std::vector<uint16_t> masks_;
};

}  // namespace lsh
