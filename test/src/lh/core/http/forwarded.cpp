#include <gtest/gtest.h>

#include <lh/core/http/forwarded.h>

#include <cstring>
#include <string>
#include <vector>

namespace
{

/* The client of a request from `proxy` with these X-Forwarded-For fields, trusting
   10.0.0.0/8, as text. */
std::string
client_of(const char *proxy, const std::vector<const char *> &fields)
{
    lh_net_cidr_t trusted;
    lh_net_ip4_t from;
    lh_net_ip4_t client;
    std::vector<lh_str_view_t> views(fields.size());
    std::vector<const lh_str_view_t *> values(fields.size());
    char text[16] = {0};

    EXPECT_TRUE(lh_net_cidr_try_parse("10.0.0.0/8", 10U, &trusted));
    EXPECT_TRUE(lh_net_ip4_parse(proxy, std::strlen(proxy), &from));
    for (std::size_t i = 0; i < fields.size(); ++i)
    {
        lh_str_view_init(&views[i], fields[i]);
        values[i] = &views[i];
    }
    lh_http_forwarded_copy_client(values.data(), values.size(), &from, &trusted, 1U, &client);
    lh_net_ip4_format(&client, text, sizeof(text) - 1U);
    return text;
}

} // namespace

TEST(lh_http_forwarded, the_rightmost_untrusted_hop_is_the_client)
{
    EXPECT_EQ(client_of("10.0.0.1", {"1.2.3.4, 10.0.0.2"}), "1.2.3.4");
    EXPECT_EQ(client_of("10.0.0.1", {"5.5.5.5, 1.2.3.4"}), "1.2.3.4");
    EXPECT_EQ(client_of("10.0.0.1", {"1.2.3.4", "10.0.0.3"}), "1.2.3.4");
}

TEST(lh_http_forwarded, no_header_leaves_the_proxy)
{
    EXPECT_EQ(client_of("10.0.0.1", {}), "10.0.0.1");
}

TEST(lh_http_forwarded, all_trusted_leaves_the_leftmost_hop)
{
    EXPECT_EQ(client_of("10.0.0.1", {"10.0.0.9, 10.0.0.2"}), "10.0.0.9");
}

TEST(lh_http_forwarded, a_malformed_entry_ends_the_walk)
{
    EXPECT_EQ(client_of("10.0.0.1", {"1.2.3.4, junk, 10.0.0.2"}), "10.0.0.2");
    EXPECT_EQ(client_of("10.0.0.1", {"junk"}), "10.0.0.1");
    EXPECT_EQ(client_of("10.0.0.1", {"1.2.3.4,"}), "10.0.0.1");
}
