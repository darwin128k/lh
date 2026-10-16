#include <gtest/gtest.h>

#include <string>

#include <lh/numeric/limits.h>
#include <lh/str/format/sint.h>

namespace
{

TEST(str_ptr_format_sint, formats_zero)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_sint(0, buf, sizeof(buf));
    ASSERT_EQ(written, 1U);
    EXPECT_EQ(buf[0], '0');
}

TEST(str_ptr_format_sint, formats_positive)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_sint(255, buf, sizeof(buf));
    ASSERT_EQ(written, 3U);
    EXPECT_EQ(std::string(buf, written), "255");
}

TEST(str_ptr_format_sint, formats_negative)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_sint(-255, buf, sizeof(buf));
    ASSERT_EQ(written, 4U);
    EXPECT_EQ(std::string(buf, written), "-255");
}

TEST(str_ptr_format_sint, formats_sint_min_without_overflow)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_sint(LH_SINT_T_MIN, buf, sizeof(buf));
    ASSERT_EQ(written, 11U);
    EXPECT_EQ(std::string(buf, written), "-2147483648");
}

TEST(str_ptr_format_sint, returns_zero_when_buffer_too_small_for_sign)
{
    lh_char_t buf[1];
    EXPECT_EQ(lh_str_ptr_format_sint(-5, buf, 0), 0U);
}

TEST(str_ptr_format_sint, returns_zero_when_buffer_too_small_for_digits)
{
    lh_char_t buf[16];
    EXPECT_EQ(lh_str_ptr_format_sint(-255, buf, 2), 0U);
}

} // namespace
