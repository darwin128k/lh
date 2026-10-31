#include <gtest/gtest.h>

#include <string>

#include <lh/str/format/hex.h>

namespace
{

TEST(str_ptr_format_hex, formats_zero)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_hex(0U, lh_bool_false, buf, sizeof(buf));
    ASSERT_EQ(written, 1U);
    EXPECT_EQ(buf[0], '0');
}

TEST(str_ptr_format_hex, formats_lowercase)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_hex(0xDEADBEEFU, lh_bool_false, buf, sizeof(buf));
    ASSERT_EQ(written, 8U);
    EXPECT_EQ(std::string(buf, written), "deadbeef");
}

TEST(str_ptr_format_hex, formats_uppercase)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_hex(0xDEADBEEFU, lh_bool_true, buf, sizeof(buf));
    ASSERT_EQ(written, 8U);
    EXPECT_EQ(std::string(buf, written), "DEADBEEF");
}

TEST(str_ptr_format_hex, returns_zero_when_buffer_too_small)
{
    lh_char_t buf[16];
    EXPECT_EQ(lh_str_ptr_format_hex(256U, lh_bool_false, buf, 2), 0U);
}

TEST(str_ptr_format_hex_bytes, writes_two_characters_per_byte)
{
    const lh_byte_t bytes[3] = {0x0aU, 0x1bU, 0x2cU};
    lh_char_t buf[8];

    EXPECT_EQ(lh_str_ptr_format_hex_bytes(bytes, sizeof bytes, buf, sizeof buf, lh_bool_false), 6U);
    EXPECT_EQ(std::string(buf), "0a1b2c");
}

TEST(str_ptr_format_hex_bytes, writes_uppercase_digits)
{
    const lh_byte_t bytes[2] = {0xdeU, 0xadU};
    lh_char_t buf[8];

    EXPECT_EQ(lh_str_ptr_format_hex_bytes(bytes, sizeof bytes, buf, sizeof buf, lh_bool_true), 4U);
    EXPECT_EQ(std::string(buf), "DEAD");
}

TEST(str_ptr_format_hex_bytes, cuts_at_a_byte_boundary)
{
    const lh_byte_t bytes[2] = {0xabU, 0xcdU};
    lh_char_t buf[4];

    /* Room for one byte and the terminator: the second byte does not fit. */
    EXPECT_EQ(lh_str_ptr_format_hex_bytes(bytes, sizeof bytes, buf, sizeof buf, lh_bool_false), 2U);
    EXPECT_EQ(std::string(buf), "ab");
}

TEST(str_ptr_format_hex_bytes, returns_zero_for_nothing_to_write)
{
    const lh_byte_t bytes[1] = {0x00U};
    lh_char_t buf[4];

    EXPECT_EQ(lh_str_ptr_format_hex_bytes(bytes, 0U, buf, sizeof buf, lh_bool_false), 0U);
    EXPECT_EQ(lh_str_ptr_format_hex_bytes(lh_null, 1U, buf, sizeof buf, lh_bool_false), 0U);
}

} // namespace
