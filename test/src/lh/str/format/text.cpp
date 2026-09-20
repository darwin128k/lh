#include <gtest/gtest.h>

#include <string>

#include <lh/str/format/text.h>
#include <lh/util/str/ptr.h>

namespace
{

std::string
format(lh_str_cptr fmt, ...)
{
    lh_char_t buf[128];
    va_list args;
    va_start(args, fmt);
    lh_usize_t written = lh_str_ptr_format_text_v(buf, sizeof(buf), fmt, args);
    va_end(args);
    return std::string(buf, written);
}

TEST(str_ptr_format_text, copies_plain_text)
{
    EXPECT_EQ(format("hello"), "hello");
}

TEST(str_ptr_format_text, formats_percent_literal)
{
    EXPECT_EQ(format("100%%"), "100%");
}

TEST(str_ptr_format_text, formats_char)
{
    EXPECT_EQ(format("[%c]", 'x'), "[x]");
}

TEST(str_ptr_format_text, formats_string)
{
    EXPECT_EQ(format("hi %s!", "there"), "hi there!");
}

TEST(str_ptr_format_text, formats_string_with_precision)
{
    EXPECT_EQ(format("%.3s", "abcdef"), "abc");
}

TEST(str_ptr_format_text, formats_unsigned)
{
    EXPECT_EQ(format("%u", 27015U), "27015");
}

TEST(str_ptr_format_text, formats_signed_negative)
{
    EXPECT_EQ(format("%d", -42), "-42");
}

TEST(str_ptr_format_text, formats_hex_lower_and_upper)
{
    EXPECT_EQ(format("%x %X", 0xABu, 0xABu), "ab AB");
}

TEST(str_ptr_format_text, zero_pads_width)
{
    EXPECT_EQ(format("%05u", 42U), "00042");
}

TEST(str_ptr_format_text, zero_pads_negative_keeps_sign_first)
{
    EXPECT_EQ(format("%05d", -42), "-0042");
}

TEST(str_ptr_format_text, left_justifies_with_spaces)
{
    EXPECT_EQ(format("[%-5d]", 42), "[42   ]");
}

TEST(str_ptr_format_text, right_justifies_with_spaces_by_default)
{
    EXPECT_EQ(format("[%5d]", 42), "[   42]");
}

TEST(str_ptr_format_text, combines_multiple_conversions)
{
    EXPECT_EQ(format("%s:%u (0x%X)", "192.168.0.1", 27015U, 27015U), "192.168.0.1:27015 (0x6987)");
}

TEST(str_ptr_format_text, returns_zero_on_buffer_too_small)
{
    lh_char_t buf[3];
    lh_usize_t written = lh_str_ptr_format_text(buf, sizeof(buf), "%u", 12345U);
    EXPECT_EQ(written, 0U);
}

TEST(str_ptr_format_text, returns_zero_on_width_overflow)
{
    lh_char_t buf[64];
    lh_usize_t written =
        lh_str_ptr_format_text(buf, sizeof(buf), "%99999999999999999999d", 1);
    EXPECT_EQ(written, 0U);
}

TEST(str_ptr_format_text, returns_zero_on_precision_overflow)
{
    lh_char_t buf[64];
    lh_usize_t written =
        lh_str_ptr_format_text(buf, sizeof(buf), "%.99999999999999999999s", "hi");
    EXPECT_EQ(written, 0U);
}

TEST(str_ptr_format_text, returns_zero_on_unknown_conversion)
{
    lh_char_t buf[16];
    lh_usize_t written = lh_str_ptr_format_text(buf, sizeof(buf), "%q", 1);
    EXPECT_EQ(written, 0U);
}

TEST(str_ptr_format_text, variadic_wrapper_matches_v)
{
    lh_char_t buf[32];
    lh_usize_t written = lh_str_ptr_format_text(buf, sizeof(buf), "%s-%u", "id", 7U);
    ASSERT_EQ(written, 4U);
    EXPECT_EQ(std::string(buf, written), "id-7");
}

TEST(str_ptr_format_text_size, counts_without_writing)
{
    EXPECT_EQ(lh_str_ptr_format_text_size("%s-%u", "id", 7U), 4U);
    EXPECT_EQ(lh_str_ptr_format_text_size(""), 0U);
    EXPECT_EQ(lh_str_ptr_format_text_size("%q", 1), LH_STR_PTR_INVALID);
}

} // namespace
