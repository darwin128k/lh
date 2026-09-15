#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/str/format/text.h>
#include <lh/str/parse/text.h>

namespace
{

lh_usize_t
parse(lh_str_cptr str, lh_str_cptr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    lh_usize_t consumed = lh_str_ptr_parse_text_v(str, std::strlen(str), fmt, args);
    va_end(args);
    return consumed;
}

TEST(str_ptr_parse_text, parses_unsigned_dotted)
{
    lh_uint_t a = 0;
    lh_uint_t b = 0;
    lh_uint_t c = 0;
    lh_uint_t d = 0;
    EXPECT_EQ(parse("192.168.0.1", "%u.%u.%u.%u", &a, &b, &c, &d), 11U);
    EXPECT_EQ(a, 192U);
    EXPECT_EQ(b, 168U);
    EXPECT_EQ(c, 0U);
    EXPECT_EQ(d, 1U);
}

TEST(str_ptr_parse_text, parses_signed_and_hex)
{
    lh_sint_t s = 0;
    lh_uint_t h = 0;
    EXPECT_EQ(parse("-42 0xAB", "%d 0x%X", &s, &h), 8U);
    EXPECT_EQ(s, -42);
    EXPECT_EQ(h, 0xABU);
}

TEST(str_ptr_parse_text, parses_char)
{
    lh_char_t ch = 0;
    EXPECT_EQ(parse("x", "%c", &ch), 1U);
    EXPECT_EQ(ch, 'x');
}

TEST(str_ptr_parse_text, parses_string_until_literal)
{
    lh_char_t buf[16];
    lh_uint_t port = 0;
    EXPECT_EQ(parse("id:7", "%s:%u", buf, sizeof(buf), &port), 4U);
    EXPECT_EQ(std::string(buf, 2), "id");
    EXPECT_EQ(port, 7U);
}

TEST(str_ptr_parse_text, parses_percent_literal)
{
    lh_uint_t n = 0;
    EXPECT_EQ(parse("100%", "100%%"), 4U);
    EXPECT_EQ(parse("42%", "%u%%", &n), 3U);
    EXPECT_EQ(n, 42U);
}

TEST(str_ptr_parse_text, rejects_leading_zeros_on_integers)
{
    lh_uint_t n = 0;
    EXPECT_EQ(parse("00042", "%u", &n), 0U);
}

TEST(str_ptr_parse_text, rejects_mismatched_literal)
{
    lh_uint_t n = 0;
    EXPECT_EQ(parse("192:168", "%u.%u", &n, &n), 0U);
}

TEST(str_ptr_parse_text, rejects_unknown_conversion)
{
    lh_uint_t n = 0;
    EXPECT_EQ(parse("1", "%q", &n), 0U);
}

TEST(str_ptr_parse_text, roundtrip_with_format)
{
    lh_char_t formatted[32];
    lh_usize_t written =
        lh_str_ptr_format_text(formatted, sizeof(formatted), "%s:%u", "host", 27015U);
    ASSERT_GT(written, 0U);

    lh_char_t host[16];
    lh_uint_t port = 0;
    EXPECT_EQ(lh_str_ptr_parse_text(formatted, written, "%s:%u", host, sizeof(host), &port),
              written);
    EXPECT_EQ(std::string(host, 4), "host");
    EXPECT_EQ(port, 27015U);
}

} // namespace
