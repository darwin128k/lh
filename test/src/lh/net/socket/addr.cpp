#include <gtest/gtest.h>

#include <string>

#include <lh/net/socket/addr.h>

namespace
{

TEST(net_socket_addr_make, roundtrip_via_getters)
{
    lh_net_ip4_t ip = lh_net_ip4_make(192, 168, 0, 1);
    lh_net_socket_addr_t addr = lh_net_socket_addr_make(&ip, 27015);

    lh_net_ip4_t got_ip = lh_net_socket_addr_get_ip(&addr);
    EXPECT_TRUE(lh_net_ip4_equals(&got_ip, &ip));
    EXPECT_EQ(lh_net_socket_addr_get_port(&addr), 27015U);
}

TEST(net_socket_addr_set_port, replaces_only_port)
{
    lh_net_ip4_t ip = lh_net_ip4_make(1, 2, 3, 4);
    lh_net_socket_addr_t addr = lh_net_socket_addr_make(&ip, 80);
    lh_net_socket_addr_set_port(&addr, 443);

    lh_net_ip4_t got_ip = lh_net_socket_addr_get_ip(&addr);
    EXPECT_TRUE(lh_net_ip4_equals(&got_ip, &ip));
    EXPECT_EQ(lh_net_socket_addr_get_port(&addr), 443U);
}

TEST(net_socket_addr_set_ip, replaces_only_ip)
{
    lh_net_ip4_t ip = lh_net_ip4_make(1, 2, 3, 4);
    lh_net_socket_addr_t addr = lh_net_socket_addr_make(&ip, 80);

    lh_net_ip4_t new_ip = lh_net_ip4_make(5, 6, 7, 8);
    lh_net_socket_addr_set_ip(&addr, &new_ip);

    lh_net_ip4_t got_ip = lh_net_socket_addr_get_ip(&addr);
    EXPECT_TRUE(lh_net_ip4_equals(&got_ip, &new_ip));
    EXPECT_EQ(lh_net_socket_addr_get_port(&addr), 80U);
}

TEST(net_socket_addr_assign, copies_ip_and_port)
{
    lh_net_ip4_t ip = lh_net_ip4_make(9, 9, 9, 9);
    lh_net_socket_addr_t src = lh_net_socket_addr_make(&ip, 1234);
    lh_net_socket_addr_t dst{};
    lh_net_socket_addr_assign(&dst, &src);
    EXPECT_TRUE(lh_net_socket_addr_equals(&dst, &src));
}

TEST(net_socket_addr_parse, parses_valid_address)
{
    lh_net_socket_addr_t addr{};
    ASSERT_TRUE(lh_net_socket_addr_parse("192.168.0.1:27015", 17, &addr));
    lh_net_ip4_t ip = lh_net_socket_addr_get_ip(&addr);
    EXPECT_EQ(lh_net_ip4_get_octet(&ip, 0), 192);
    EXPECT_EQ(lh_net_ip4_get_octet(&ip, 3), 1);
    EXPECT_EQ(lh_net_socket_addr_get_port(&addr), 27015U);
}

TEST(net_socket_addr_parse, rejects_missing_colon)
{
    lh_net_socket_addr_t addr{};
    EXPECT_FALSE(lh_net_socket_addr_parse("192.168.0.1", 11, &addr));
}

TEST(net_socket_addr_parse, rejects_invalid_ip)
{
    lh_net_socket_addr_t addr{};
    EXPECT_FALSE(lh_net_socket_addr_parse("256.168.0.1:80", 14, &addr));
}

TEST(net_socket_addr_parse, rejects_invalid_port)
{
    lh_net_socket_addr_t addr{};
    EXPECT_FALSE(lh_net_socket_addr_parse("192.168.0.1:99999", 17, &addr));
}

TEST(net_socket_addr_parse, rejects_empty_port)
{
    lh_net_socket_addr_t addr{};
    EXPECT_FALSE(lh_net_socket_addr_parse("192.168.0.1:", 12, &addr));
}

TEST(net_socket_addr_format, formats_roundtrip)
{
    lh_net_socket_addr_t addr{};
    ASSERT_TRUE(lh_net_socket_addr_parse("10.20.30.40:8080", 16, &addr));

    lh_char_t buf[LH_NET_SOCKET_ADDR_TEXT_MAX];
    lh_usize_t written = lh_net_socket_addr_format(&addr, buf, sizeof(buf));
    ASSERT_EQ(written, 16U);
    EXPECT_EQ(std::string(buf, written), "10.20.30.40:8080");
}

TEST(net_socket_addr_format, returns_zero_when_buffer_too_small)
{
    lh_net_socket_addr_t addr{};
    ASSERT_TRUE(lh_net_socket_addr_parse("255.255.255.255:65535", 21, &addr));

    lh_char_t buf[LH_NET_SOCKET_ADDR_TEXT_MAX];
    EXPECT_EQ(lh_net_socket_addr_format(&addr, buf, 16), 0U);
}

TEST(net_socket_addr_equals, compares_ip_and_port)
{
    lh_net_socket_addr_t a{};
    lh_net_socket_addr_t b{};
    ASSERT_TRUE(lh_net_socket_addr_parse("1.2.3.4:80", 10, &a));
    ASSERT_TRUE(lh_net_socket_addr_parse("1.2.3.4:80", 10, &b));
    EXPECT_TRUE(lh_net_socket_addr_equals(&a, &b));

    ASSERT_TRUE(lh_net_socket_addr_parse("1.2.3.4:81", 10, &b));
    EXPECT_FALSE(lh_net_socket_addr_equals(&a, &b));
}

} // namespace
