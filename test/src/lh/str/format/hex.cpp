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

} // namespace
