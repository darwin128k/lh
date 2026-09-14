#include <gtest/gtest.h>

#include <string>

#include <lh/net/ip.h>

namespace
{

TEST(net_ip4_pack, updates_only_provided_octets)
{
    lh_net_ip4_t addr = lh_net_ip4_make(1, 2, 3, 4);
    lh_u8_t new_octet1 = 99;
    lh_net_ip4_pack(&addr, nullptr, &new_octet1, nullptr, nullptr);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_0), 1);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_1), 99);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_2), 3);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_3), 4);
}

TEST(net_ip4_unpack, skips_null_outputs)
{
    lh_net_ip4_t addr = lh_net_ip4_make(10, 20, 30, 40);
    lh_u8_t octet2 = 0;
    lh_net_ip4_unpack(&addr, nullptr, nullptr, &octet2, nullptr);
    EXPECT_EQ(octet2, 30);
}

TEST(net_ip4_unpack, matches_pack_with_all_pointers)
{
    lh_net_ip4_t a = lh_net_ip4_make(5, 6, 7, 8);
    lh_net_ip4_t b{};
    lh_u8_t o0, o1, o2, o3;
    lh_net_ip4_unpack(&a, &o0, &o1, &o2, &o3);
    lh_net_ip4_pack(&b, &o0, &o1, &o2, &o3);
    EXPECT_TRUE(lh_net_ip4_equals(&a, &b));
}

TEST(net_ip4_make, roundtrip_via_get_octet)
{
    lh_net_ip4_t addr = lh_net_ip4_make(192, 168, 0, 1);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_0), 192);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_1), 168);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_2), 0);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_3), 1);
}

TEST(net_ip4_set_octet, replaces_single_octet)
{
    lh_net_ip4_t addr = lh_net_ip4_make(1, 2, 3, 4);
    lh_net_ip4_set_octet(&addr, LH_NET_IP4_OCTET_INDEX_2, 99);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_0), 1);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_2), 99);
}

TEST(net_ip4_assign, copies_all_octets)
{
    lh_net_ip4_t src = lh_net_ip4_make(10, 20, 30, 40);
    lh_net_ip4_t dst = lh_net_ip4_make(0, 0, 0, 0);
    lh_net_ip4_assign(&dst, &src);
    EXPECT_TRUE(lh_net_ip4_equals(&dst, &src));
}

TEST(net_ip4_parse, parses_valid_address)
{
    lh_net_ip4_t addr{};
    ASSERT_TRUE(lh_net_ip4_parse("192.168.0.1", 11, &addr));
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_0), 192);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_1), 168);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_2), 0);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_3), 1);
}

TEST(net_ip4_parse, parses_all_zero_and_all_max)
{
    lh_net_ip4_t addr{};
    ASSERT_TRUE(lh_net_ip4_parse("0.0.0.0", 7, &addr));
    EXPECT_TRUE(lh_net_ip4_equals(&addr, &addr));

    ASSERT_TRUE(lh_net_ip4_parse("255.255.255.255", 15, &addr));
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_0), 255);
    EXPECT_EQ(lh_net_ip4_get_octet(&addr, LH_NET_IP4_OCTET_INDEX_3), 255);
}

TEST(net_ip4_parse, rejects_octet_above_255)
{
    lh_net_ip4_t addr{};
    EXPECT_FALSE(lh_net_ip4_parse("256.0.0.1", 9, &addr));
}

TEST(net_ip4_parse, rejects_leading_zero_octet)
{
    lh_net_ip4_t addr{};
    EXPECT_FALSE(lh_net_ip4_parse("192.168.001.1", 13, &addr));
}

TEST(net_ip4_parse, rejects_missing_octet)
{
    lh_net_ip4_t addr{};
    EXPECT_FALSE(lh_net_ip4_parse("192.168.1", 9, &addr));
}

TEST(net_ip4_parse, rejects_trailing_garbage)
{
    lh_net_ip4_t addr{};
    EXPECT_FALSE(lh_net_ip4_parse("192.168.0.1x", 12, &addr));
}

TEST(net_ip4_parse, rejects_empty_octet)
{
    lh_net_ip4_t addr{};
    EXPECT_FALSE(lh_net_ip4_parse("192..0.1", 8, &addr));
}

TEST(net_ip4_format, formats_roundtrip)
{
    lh_net_ip4_t addr{};
    ASSERT_TRUE(lh_net_ip4_parse("10.20.30.40", 11, &addr));

    lh_char_t buf[LH_NET_IP4_TEXT_MAX];
    lh_usize_t written = lh_net_ip4_format(&addr, buf, sizeof(buf));
    ASSERT_EQ(written, 11U);
    EXPECT_EQ(std::string(buf, written), "10.20.30.40");
}

TEST(net_ip4_format, returns_zero_when_buffer_too_small)
{
    lh_net_ip4_t addr{};
    ASSERT_TRUE(lh_net_ip4_parse("255.255.255.255", 15, &addr));

    lh_char_t buf[LH_NET_IP4_TEXT_MAX];
    EXPECT_EQ(lh_net_ip4_format(&addr, buf, 5), 0U);
}

TEST(net_ip4_equals, compares_octets)
{
    lh_net_ip4_t a{};
    lh_net_ip4_t b{};
    ASSERT_TRUE(lh_net_ip4_parse("1.2.3.4", 7, &a));
    ASSERT_TRUE(lh_net_ip4_parse("1.2.3.4", 7, &b));
    EXPECT_TRUE(lh_net_ip4_equals(&a, &b));

    ASSERT_TRUE(lh_net_ip4_parse("1.2.3.5", 7, &b));
    EXPECT_FALSE(lh_net_ip4_equals(&a, &b));
}

TEST(net_ip4_is_loopback, matches_127_net)
{
    lh_net_ip4_t loop = lh_net_ip4_make(127, 0, 0, 1);
    lh_net_ip4_t public_addr = lh_net_ip4_make(8, 8, 8, 8);
    EXPECT_TRUE(lh_net_ip4_is_loopback(&loop));
    EXPECT_FALSE(lh_net_ip4_is_loopback(&public_addr));
}

TEST(net_ip4_is_private, matches_rfc1918)
{
    lh_net_ip4_t ten = lh_net_ip4_make(10, 1, 2, 3);
    lh_net_ip4_t one_seventy_two = lh_net_ip4_make(172, 16, 0, 1);
    lh_net_ip4_t one_ninety_two = lh_net_ip4_make(192, 168, 1, 1);
    lh_net_ip4_t public_addr = lh_net_ip4_make(8, 8, 8, 8);
    lh_net_ip4_t loop = lh_net_ip4_make(127, 0, 0, 1);

    EXPECT_TRUE(lh_net_ip4_is_private(&ten));
    EXPECT_TRUE(lh_net_ip4_is_private(&one_seventy_two));
    EXPECT_TRUE(lh_net_ip4_is_private(&one_ninety_two));
    EXPECT_FALSE(lh_net_ip4_is_private(&public_addr));
    EXPECT_FALSE(lh_net_ip4_is_private(&loop));
}

} // namespace
