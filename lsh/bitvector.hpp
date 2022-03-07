#pragma once

#include <cstdint>

#include "boost/dynamic_bitset.hpp"

namespace lsh {

using vector_t = boost::dynamic_bitset<>;

size_t hamming_distance(const vector_t &v1, const vector_t &v2) {
  return (v1 ^ v2).count();
}

}  // namespace lsh