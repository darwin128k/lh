#include <gtest/gtest.h>

#include <string>

#include <lh/net/socket/addr.h>

namespace
{

TEST(net_socket_addr_make_ip4, tags_as_ip4)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 192, 168, 0, 1);
    lh_net_ip4_socket_addr_t ip4_addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(ip4_addr), &ip, 27015);

    lh_net_socket_addr_t addr;


    lh_net_socket_addr_init_ip4(lh_addr_of(addr), &ip4_addr);
    EXPECT_EQ(lh_net_socket_addr_get_family(&addr), lh_net_socket_addr_family_ip4);
}

TEST(net_socket_addr_get_ip4, roundtrips_wrapped_address)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 10, 0, 0, 1);
    lh_net_ip4_socket_addr_t ip4_addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(ip4_addr), &ip, 8080);

    lh_net_socket_addr_t addr;


    lh_net_socket_addr_init_ip4(lh_addr_of(addr), &ip4_addr);
    lh_net_ip4_socket_addr_t got = lh_net_socket_addr_get_ip4(&addr);
    EXPECT_TRUE(lh_net_ip4_socket_addr_equals(&got, &ip4_addr));
}

TEST(net_socket_addr_format, dispatches_to_ip4)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 1, 2, 3, 4);
    lh_net_ip4_socket_addr_t ip4_addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(ip4_addr), &ip, 80);
    lh_net_socket_addr_t addr;

    lh_net_socket_addr_init_ip4(lh_addr_of(addr), &ip4_addr);

    lh_char_t buf[LH_NET_IP4_SOCKET_ADDR_TEXT_MAX];
    lh_usize_t written = lh_net_socket_addr_format(&addr, buf, sizeof(buf));
    ASSERT_EQ(written, 10U);
    EXPECT_EQ(std::string(buf, written), "1.2.3.4:80");
}

TEST(net_socket_addr_equals, compares_family_and_address)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 1, 2, 3, 4);
    lh_net_ip4_socket_addr_t ip4_addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(ip4_addr), &ip, 80);
    lh_net_socket_addr_t a;

    lh_net_socket_addr_init_ip4(lh_addr_of(a), &ip4_addr);
    lh_net_socket_addr_t b;

    lh_net_socket_addr_init_ip4(lh_addr_of(b), &ip4_addr);
    EXPECT_TRUE(lh_net_socket_addr_equals(&a, &b));

    lh_net_ip4_t other_ip;


    lh_net_ip4_init(lh_addr_of(other_ip), 1, 2, 3, 5);
    lh_net_ip4_socket_addr_t other_ip4_addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(other_ip4_addr), &other_ip, 80);
    lh_net_socket_addr_t c;

    lh_net_socket_addr_init_ip4(lh_addr_of(c), &other_ip4_addr);
    EXPECT_FALSE(lh_net_socket_addr_equals(&a, &c));
}

} // namespace
