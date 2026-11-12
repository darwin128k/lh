#include <gtest/gtest.h>

#include <lh/core/net/authority.h>

#include <cstring>
#include <string>

namespace
{

/* "host|port" or "fail", for one call. */
std::string
split(const char *text)
{
    lh_net_port_t port = 7U;
    lh_bool_t has_port = lh_bool_false;

    lh_str_view_t host;
    if (!lh_net_authority_try_split(text, std::strlen(text), &host, &port, &has_port))
    {
        return "fail";
    }
    return std::string(lh_str_view_get_data(&host), lh_str_view_get_size(&host)) + "|" +
           (has_port ? std::to_string(port) : std::string("-"));
}

} // namespace

TEST(net_authority, splits_the_forms_an_address_takes)
{
    EXPECT_EQ(split("192.0.2.10:27015"), "192.0.2.10|27015");
    EXPECT_EQ(split("example.org"), "example.org|-");
    EXPECT_EQ(split("example.org:80"), "example.org|80");
    EXPECT_EQ(split("2001:db8::1"), "2001:db8::1|-");
    EXPECT_EQ(split("[2001:db8::1]:443"), "2001:db8::1|443");
    EXPECT_EQ(split("[::1]"), "::1|-");
}

TEST(net_authority, refuses_malformed_text)
{
    EXPECT_EQ(split(""), "fail");
    EXPECT_EQ(split("[::1"), "fail");
    EXPECT_EQ(split("[::1]x"), "fail");
    EXPECT_EQ(split("[]:80"), "fail");
    EXPECT_EQ(split(":80"), "fail");
    EXPECT_EQ(split("a:"), "fail");
    EXPECT_EQ(split("a:65536"), "fail");
    EXPECT_EQ(split("a:abc"), "fail");
    EXPECT_EQ(split("a b:80"), "fail");
    EXPECT_EQ(split("a\r\nb"), "fail");
}
