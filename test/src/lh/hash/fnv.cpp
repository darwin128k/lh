#include <gtest/gtest.h>

#include <lh/hash/fnv.h>
#include <lh/null.h>

/* Reference values from the FNV authors' test suite (FNV-1a, 32-bit). */
TEST(hash_fnv1a_32, known_vectors)
{
    EXPECT_EQ(lh_hash_fnv1a_32(lh_null, 0U), 0x811C9DC5U);
    EXPECT_EQ(lh_hash_fnv1a_32(const_cast<char *>("a"), 1U), 0xE40C292CU);
    EXPECT_EQ(lh_hash_fnv1a_32(const_cast<char *>("foobar"), 6U), 0xBF9CF968U);
}

TEST(hash_fnv1a_32, empty_is_the_offset_basis)
{
    EXPECT_EQ(lh_hash_fnv1a_32(lh_null, 0U), LH_HASH_FNV1A_32_OFFSET);
    EXPECT_EQ(lh_hash_fnv1a_32_update(123U, lh_null, 0U), 123U);
}

TEST(hash_fnv1a_32, update_continues_where_it_stopped)
{
    const lh_u32_t head = lh_hash_fnv1a_32(const_cast<char *>("foo"), 3U);

    EXPECT_EQ(lh_hash_fnv1a_32_update(head, const_cast<char *>("bar"), 3U),
              lh_hash_fnv1a_32(const_cast<char *>("foobar"), 6U));
}

TEST(hash_fnv1a_32, byte_is_one_step)
{
    EXPECT_EQ(lh_hash_fnv1a_32_byte(LH_HASH_FNV1A_32_OFFSET, 'a'),
              lh_hash_fnv1a_32(const_cast<char *>("a"), 1U));
}
