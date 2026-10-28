#include <gtest/gtest.h>

#include <lh/http/version.h>
#include <lh/str/view/initializer.h>

#include <cstring>

namespace
{

lh_bool_t
parse(const char *text, lh_u8_t *minor)
{
    return lh_http_parse_version(lh_str_view_initializer(text, std::strlen(text)), minor);
}

TEST(http_parse_version, reads_the_minor_digit)
{
    lh_u8_t minor = 9;
    ASSERT_TRUE(parse("HTTP/1.0", &minor));
    EXPECT_EQ(minor, 0);
    ASSERT_TRUE(parse("HTTP/1.1", &minor));
    EXPECT_EQ(minor, 1);
}

TEST(http_parse_version, rejects_anything_else_and_keeps_the_output)
{
    lh_u8_t minor = 7;
    for (const char *bad : {"HTTP/2.0", "HTTP/1.x", "http/1.1", "HTTP/1.10", "HTTP/1.", "HTTP 1.1"})
    {
        EXPECT_FALSE(parse(bad, &minor)) << bad;
    }
    EXPECT_EQ(minor, 7);
}

} // namespace
