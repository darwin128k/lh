#include <gtest/gtest.h>

#include <string>

#include <lh/str/format/uint.h>

namespace
{

TEST(str_ptr_format_uint, formats_zero)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_uint(0U, buf, sizeof(buf));
    ASSERT_EQ(written, 1U);
    EXPECT_EQ(buf[0], '0');
}

TEST(str_ptr_format_uint, formats_multi_digit)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_uint(255U, buf, sizeof(buf));
    ASSERT_EQ(written, 3U);
    EXPECT_EQ(std::string(buf, written), "255");
}

TEST(str_ptr_format_uint, returns_zero_when_buffer_too_small)
{
    lh_char_t buf[2];
    EXPECT_EQ(lh_str_ptr_format_uint(255U, buf, 2), 0U);
}

TEST(str_ptr_format_uint, exact_fit_buffer_succeeds)
{
    lh_char_t buf[3];
    lh_usize_t written = lh_str_ptr_format_uint(255U, buf, sizeof(buf));
    ASSERT_EQ(written, 3U);
    EXPECT_EQ(std::string(buf, written), "255");
}

} // namespace
