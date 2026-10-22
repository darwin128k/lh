#include <gtest/gtest.h>

#include <random>

#include <lh/numeric/mul.h>

namespace
{

TEST(numeric_mul_u64_full, small_and_extreme_products)
{
    lh_u64_t high = 7U;

    EXPECT_EQ(lh_numeric_mul_u64_full(0U, ~0ULL, &high), 0U);
    EXPECT_EQ(high, 0U);
    EXPECT_EQ(lh_numeric_mul_u64_full(3U, 5U, &high), 15U);
    EXPECT_EQ(high, 0U);
    /* (2^64 - 1)^2 = 2^128 - 2^65 + 1 */
    EXPECT_EQ(lh_numeric_mul_u64_full(~0ULL, ~0ULL, &high), 1U);
    EXPECT_EQ(high, ~0ULL - 1U);
    EXPECT_EQ(lh_numeric_mul_u64_full(1ULL << 63, 2U, &high), 0U);
    EXPECT_EQ(high, 1U);
}

TEST(numeric_mul_u64_full, halves_agree_with_the_fast_path)
{
    std::mt19937_64 rng(20261022U);

    for (int i = 0; i < 100000; ++i)
    {
        const lh_u64_t a = rng() >> (rng() % 64U);
        const lh_u64_t b = rng() >> (rng() % 64U);
        lh_u64_t high_fast = 0U;
        lh_u64_t high_halves = 0U;
        const lh_u64_t low_fast = lh_numeric_mul_u64_full(a, b, &high_fast);
        const lh_u64_t low_halves = lh_numeric_mul_u64_full_halves(a, b, &high_halves);

        ASSERT_EQ(low_fast, low_halves) << a << " * " << b;
        ASSERT_EQ(high_fast, high_halves) << a << " * " << b;
#if defined(__SIZEOF_INT128__)
        const unsigned __int128 product = static_cast<unsigned __int128>(a) * b;
        ASSERT_EQ(high_halves, static_cast<lh_u64_t>(product >> 64));
        ASSERT_EQ(low_halves, static_cast<lh_u64_t>(product));
#endif
    }
}

} // namespace
