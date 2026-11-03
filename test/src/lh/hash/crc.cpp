/**
 * @file crc.cpp
 * @brief Reflected CRC and gzip CRC-32 test vectors.
 */

#include <gtest/gtest.h>
#include <lh/hash/crc.h>

TEST(hash_crc32, standard_vectors_and_incremental_updates)
{
    const char text[] = "123456789";
    EXPECT_EQ(lh_hash_crc32(nullptr, 0U), 0U);
    EXPECT_EQ(lh_hash_crc32(text, 9U), 0xCBF43926U);
    const lh_u32_t first = lh_hash_crc32_update(0U, text, 4U);
    EXPECT_EQ(lh_hash_crc32_update(first, text + 4, 5U), 0xCBF43926U);
    EXPECT_EQ(lh_hash_crc32_update(first, nullptr, 0U), first);
}

TEST(hash_crc_reflected, modbus_check_vector_and_byte_step)
{
    const char text[] = "123456789";
    EXPECT_EQ(lh_hash_crc_reflected_update(0xFFFFU, text, 9U, 0xA001U), 0x4B37U);
    EXPECT_EQ(lh_hash_crc_reflected_byte(0xFFFFU, '1', 0xA001U),
              lh_hash_crc_reflected_update(0xFFFFU, text, 1U, 0xA001U));
}
