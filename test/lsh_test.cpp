#include <gtest/gtest.h>

#include "lsh/bitvector.hpp"
#include "lsh/table.hpp"

TEST(LSHFallback, EmptyIfTableEmpty) {
  lsh::Table<true> table(4, 2, 3);

  const lsh::vector_t result = table.query(lsh::vector_t(std::string("1111")));

  EXPECT_TRUE(result.empty());
}

/**
 * This test asserts that we always get a result, even if none of the bits of
 * the inserted and query bitvectors match.
 *
 * If none of the bits match, they won't be put into the same buckets, so the
 * fallback strategy needs to kick in.
 */
TEST(LSHFallback, NotEmptyIfTableNotEmpty) {
  lsh::Table<true> table(4, 2, 3);
  const lsh::vector_t expected_result = lsh::vector_t(std::string("0000"));
  table.insert(expected_result);

  const lsh::vector_t result = table.query(lsh::vector_t(std::string("1111")));

  EXPECT_EQ(result, expected_result);
}

TEST(LSH, EmptyIfTableEmpty) {
  lsh::Table<false> table(4, 2, 3);

  const lsh::vector_t result = table.query(lsh::vector_t(std::string("1111")));

  EXPECT_TRUE(result.empty());
}

TEST(LSH, EmptyIfTableNotEmpty) {
  lsh::Table<false> table(4, 2, 3);
  const lsh::vector_t expected_result = lsh::vector_t(std::string("0000"));
  table.insert(expected_result);

  const lsh::vector_t result = table.query(lsh::vector_t(std::string("1111")));

  EXPECT_TRUE(result.empty());
}
