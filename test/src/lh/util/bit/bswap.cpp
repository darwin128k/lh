#include <gtest/gtest.h>

#include <lh/util/bit/bswap.h>

namespace
{

TEST(util_bit_bswap, u16_reverses_bytes)
{
    EXPECT_EQ(lh_bit_bswap_u16(0x1234u), 0x3412u);
}

TEST(util_bit_bswap, u32_reverses_bytes)
{
    EXPECT_EQ(lh_bit_bswap_u32(0x12345678u), 0x78563412u);
}

TEST(util_bit_bswap, u64_reverses_bytes)
{
    EXPECT_EQ(lh_bit_bswap_u64(0x0123456789ABCDEFULL), 0xEFCDAB8967452301ULL);
}

TEST(util_bit_bswap, is_its_own_inverse)
{
    EXPECT_EQ(lh_bit_bswap_u16(lh_bit_bswap_u16(0xABCDu)), 0xABCDu);
    EXPECT_EQ(lh_bit_bswap_u32(lh_bit_bswap_u32(0xAABBCCDDu)), 0xAABBCCDDu);
    EXPECT_EQ(lh_bit_bswap_u64(lh_bit_bswap_u64(0x1122334455667788ULL)), 0x1122334455667788ULL);
}

} // namespace
