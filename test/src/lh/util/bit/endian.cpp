#include <gtest/gtest.h>

#include <lh/util/bit/endian.h>

namespace
{

TEST(util_bit_endian, pack_be16_writes_high_byte_first)
{
    lh_uchar_t bytes[2];
    lh_bit_pack_be16(0x1234u, bytes);
    EXPECT_EQ(bytes[0], 0x12u);
    EXPECT_EQ(bytes[1], 0x34u);
}

TEST(util_bit_endian, pack_be16_handles_zero_and_max)
{
    lh_uchar_t bytes[2];

    lh_bit_pack_be16(0x0000u, bytes);
    EXPECT_EQ(bytes[0], 0x00u);
    EXPECT_EQ(bytes[1], 0x00u);

    lh_bit_pack_be16(0xFFFFu, bytes);
    EXPECT_EQ(bytes[0], 0xFFu);
    EXPECT_EQ(bytes[1], 0xFFu);
}

TEST(util_bit_endian, unpack_be16_reads_high_byte_first)
{
    const lh_uchar_t bytes[2] = {0x12u, 0x34u};
    EXPECT_EQ(lh_bit_unpack_be16(bytes), 0x1234u);
}

TEST(util_bit_endian, pack_and_unpack_be16_round_trip)
{
    for (lh_u32_t v = 0U; v <= 0xFFFFU; v += 4099U)
    {
        lh_uchar_t bytes[2];
        const lh_u16_t value = static_cast<lh_u16_t>(v);

        lh_bit_pack_be16(value, bytes);
        EXPECT_EQ(lh_bit_unpack_be16(bytes), value);
    }
}

TEST(util_bit_endian, pack_and_unpack_le16_round_trip)
{
    lh_uchar_t bytes[2];
    lh_bit_pack_le16(0x1234u, bytes);
    EXPECT_EQ(bytes[0], 0x34u);
    EXPECT_EQ(bytes[1], 0x12u);
    EXPECT_EQ(lh_bit_unpack_le16(bytes), 0x1234u);
}

TEST(util_bit_endian, pack_and_unpack_be32_round_trip)
{
    lh_uchar_t bytes[4];
    lh_bit_pack_be32(0x12345678u, bytes);
    EXPECT_EQ(bytes[0], 0x12u);
    EXPECT_EQ(bytes[1], 0x34u);
    EXPECT_EQ(bytes[2], 0x56u);
    EXPECT_EQ(bytes[3], 0x78u);
    EXPECT_EQ(lh_bit_unpack_be32(bytes), 0x12345678u);
}

TEST(util_bit_endian, pack_and_unpack_le32_round_trip)
{
    lh_uchar_t bytes[4];
    lh_bit_pack_le32(0x12345678u, bytes);
    EXPECT_EQ(bytes[0], 0x78u);
    EXPECT_EQ(bytes[1], 0x56u);
    EXPECT_EQ(bytes[2], 0x34u);
    EXPECT_EQ(bytes[3], 0x12u);
    EXPECT_EQ(lh_bit_unpack_le32(bytes), 0x12345678u);
}

TEST(util_bit_endian, pack_and_unpack_be64_round_trip)
{
    lh_uchar_t bytes[8];
    const lh_u64_t value = 0x0123456789ABCDEFULL;
    lh_bit_pack_be64(value, bytes);
    EXPECT_EQ(bytes[0], 0x01u);
    EXPECT_EQ(bytes[7], 0xEFu);
    EXPECT_EQ(lh_bit_unpack_be64(bytes), value);
}

TEST(util_bit_endian, pack_and_unpack_le64_round_trip)
{
    lh_uchar_t bytes[8];
    const lh_u64_t value = 0x0123456789ABCDEFULL;
    lh_bit_pack_le64(value, bytes);
    EXPECT_EQ(bytes[0], 0xEFu);
    EXPECT_EQ(bytes[7], 0x01u);
    EXPECT_EQ(lh_bit_unpack_le64(bytes), value);
}

} // namespace
