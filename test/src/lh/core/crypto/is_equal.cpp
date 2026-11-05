#include <gtest/gtest.h>

#include <lh/core/crypto/is_equal.h>

namespace
{

TEST(crypto_is_equal, same_bytes_are_equal)
{
    const lh_byte_t a[5] = {1, 2, 3, 4, 5};
    const lh_byte_t b[5] = {1, 2, 3, 4, 5};
    EXPECT_TRUE(lh_crypto_is_equal(a, b, sizeof(a)));
}

TEST(crypto_is_equal, any_one_different_byte_or_bit_makes_them_different)
{
    const lh_byte_t a[32] = {0};
    for (int i = 0; i < 32; ++i)
    {
        for (int bit = 0; bit < 8; ++bit)
        {
            lh_byte_t b[32] = {0};
            b[i] = static_cast<lh_byte_t>(1u << bit);
            EXPECT_FALSE(lh_crypto_is_equal(a, b, sizeof(a))) << i << ":" << bit;
        }
    }
}

TEST(crypto_is_equal, nothing_to_compare_is_equal)
{
    EXPECT_TRUE(lh_crypto_is_equal(nullptr, nullptr, 0u));
}

} // namespace
