#include <gtest/gtest.h>

#include <lh/numeric/big.h>

namespace
{

TEST(numeric_big, shifts_multiplies_compares_and_subtracts_across_words)
{
    lh_numeric_big_t a;
    lh_numeric_big_t b;

    lh_numeric_big_set_u32(&a, 3U);
    lh_numeric_big_shl(&a, 40); // 3 << 40: word 1 holds 3 << 8
    EXPECT_EQ(lh_numeric_big_get_word(&a, 0), 0U);
    EXPECT_EQ(lh_numeric_big_get_word(&a, 1), 3U << 8);

    lh_numeric_big_set_u32(&b, 0xFFFFFFFFU);
    lh_numeric_big_mul_u32(&b, 0x100U); // carries into word 1
    EXPECT_EQ(lh_numeric_big_get_word(&b, 0), 0xFFFFFF00U);
    EXPECT_EQ(lh_numeric_big_get_word(&b, 1), 0xFFU);

    EXPECT_GT(lh_numeric_big_compare(&a, &b), 0);
    EXPECT_LT(lh_numeric_big_compare(&b, &a), 0);
    EXPECT_EQ(lh_numeric_big_compare(&a, &a), 0);

    lh_numeric_big_sub(&a, &b); // 0x30000000000 - 0xFFFFFFFF00 = 0x2000000100, with a borrow
    EXPECT_EQ(lh_numeric_big_get_word(&a, 0), 0x100U);
    EXPECT_EQ(lh_numeric_big_get_word(&a, 1), 0x200U);
}

} // namespace
