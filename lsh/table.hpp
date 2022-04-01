#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "bitvector.hpp"
#include "mask.hpp"

namespace lsh {

template <bool use_fallback = false>
class Table {
  using offset_t = uint16_t;
  // We use a sorted vector for faster union as described in
  // https://lemire.me/blog/2017/01/27/how-expensive-are-the-union-and-intersection-of-two-unordered_set-in-c/.
  // The offsets will be inserted in sorted order automatically, as new elements
  // always have a higher offset w.r.t. the values_ vector.
  using offset_collection_t = std::vector<offset_t>;
  // If we use true locality sensitive hashing without fallback we use an
  // unordered_map for O(1) access, if we use a fallback when no bucket is
  // found, we use a map so we can look left and right in an ordered manner.
  // Note that using the fallback is not true locality sensitive hashing any
  // more, as this puts more emphasis on the first bits of the keys.
  using bucket_t = typename std::conditional<
      use_fallback, std::map<vector_t, offset_collection_t>,
      std::unordered_map<vector_t, offset_collection_t>>::type;

  static constexpr size_t kMaxHammingDistance =
      std::numeric_limits<size_t>::max();

  static std::vector<Mask> init_masks(const size_t num_bits,
                                      const size_t hash_bits,
                                      const size_t num_hash_functions) {
    std::vector<Mask> masks;
    for (size_t i = 0; i < num_hash_functions; ++i) {
      masks.emplace_back(num_bits, hash_bits);
    }
    return masks;
  }

 public:
  Table(const size_t num_bits, const size_t hash_bits,
        const size_t num_hash_functions)
      : num_bits_(num_bits),
        masks_(init_masks(num_bits, hash_bits, num_hash_functions)),
        buckets_(num_hash_functions) {
    assert(hash_bits > 0);
    assert(num_bits > hash_bits);
    assert(num_hash_functions > 0);
    assert(num_bits > num_hash_functions);
    assert(num_bits < kMaxHammingDistance);
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
      const bucket_t& bucket = buckets_[i];
      const vector_t key = masks_[i].project(v);

      typename bucket_t::const_iterator it;

      if constexpr (use_fallback) {
        it = bucket.lower_bound(key);
        
        if(it != bucket.begin() /* can't look further left if we are at the first entry already */) {
          if (it == bucket.end() /* found nothing that is greater or equal */) {
            it--;  // look left
          } else if (it->first != key /* found a key that is greater */) {
            const auto greater_it = it;
            it--;  // look left

            // check what is closer and set it accordingly
            for (size_t i = 0; i < key.size(); ++i) {
              const bool greater_bit = greater_it->first[i];
              const bool smaller_bit = it->first[i];

              if (greater_bit != smaller_bit) {
                const bool key_bit = key[i];
                if (greater_bit == key_bit) {
                  it = greater_it;
                } /* else it stay it, which is pointing to some smaller key */
                break;
              }
            }
          }
        } /* we have an exact match, no need to look further */
      } else {
        it = bucket.find(key);
      }

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
  const std::vector<Mask> masks_;

  std::vector<vector_t> values_;
  std::vector<bucket_t> buckets_;
};

}  // namespace lsh
