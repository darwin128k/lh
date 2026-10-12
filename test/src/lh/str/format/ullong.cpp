#include <gtest/gtest.h>

#include <string>

#include <lh/str/format/ullong.h>

namespace
{

TEST(str_ptr_format_ullong, formats_zero)
{
    lh_char_t buf[24];
    lh_usize_t written = lh_str_ptr_format_ullong(0ULL, buf, sizeof(buf));
    ASSERT_EQ(written, 1U);
    EXPECT_EQ(buf[0], '0');
}

TEST(str_ptr_format_ullong, delegates_small_values_to_uint)
{
    lh_char_t buf[24];
    lh_usize_t written = lh_str_ptr_format_ullong(255ULL, buf, sizeof(buf));
    ASSERT_EQ(written, 3U);
    EXPECT_EQ(std::string(buf, written), "255");
}

TEST(str_ptr_format_ullong, pads_lower_nine_digit_chunks)
{
    lh_char_t buf[24];
    lh_usize_t written = lh_str_ptr_format_ullong(1000000000ULL, buf, sizeof(buf));
    ASSERT_EQ(written, 10U);
    EXPECT_EQ(std::string(buf, written), "1000000000");
}

TEST(str_ptr_format_ullong, formats_full_64_bit_range)
{
    lh_char_t buf[24];
    lh_usize_t written = lh_str_ptr_format_ullong(18446744073709551615ULL, buf, sizeof(buf));
    ASSERT_EQ(written, 20U);
    EXPECT_EQ(std::string(buf, written), "18446744073709551615");
}

TEST(str_ptr_format_ullong, returns_zero_when_buffer_too_small)
{
    lh_char_t buf[4];
    EXPECT_EQ(lh_str_ptr_format_ullong(1000000000ULL, buf, sizeof(buf)), 0U);
}

} // namespace
