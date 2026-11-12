#include <gtest/gtest.h>

#include <lh/core/net/cidr.h>

#include <cstring>

namespace
{

bool
parse(const char *text, lh_net_cidr_t *out)
{
    return lh_net_cidr_try_parse(text, std::strlen(text), out) != lh_bool_false;
}

bool
contains(const lh_net_cidr_t *block, lh_u8_t a, lh_u8_t b, lh_u8_t c, lh_u8_t d)
{
    lh_net_ip4_t address;

    lh_net_ip4_init(&address, a, b, c, d);
    return lh_net_cidr_contains(block, &address) != lh_bool_false;
}

} // namespace

TEST(lh_net_cidr, a_block_holds_its_addresses)
{
    lh_net_cidr_t block;

    ASSERT_TRUE(parse("10.1.2.3/8", &block));
    EXPECT_EQ(lh_net_cidr_get_net(&block), 0x0A000000U);
    EXPECT_EQ(lh_net_cidr_get_mask(&block), 0xFF000000U);
    EXPECT_TRUE(contains(&block, 10, 200, 0, 1));
    EXPECT_FALSE(contains(&block, 11, 0, 0, 1));
}

TEST(lh_net_cidr, a_bare_address_is_one_address)
{
    lh_net_cidr_t block;

    ASSERT_TRUE(parse("192.168.0.7", &block));
    EXPECT_TRUE(contains(&block, 192, 168, 0, 7));
    EXPECT_FALSE(contains(&block, 192, 168, 0, 8));
}

TEST(lh_net_cidr, slash_zero_holds_everything)
{
    lh_net_cidr_t block;

    ASSERT_TRUE(parse("0.0.0.0/0", &block));
    EXPECT_TRUE(contains(&block, 8, 8, 8, 8));
    EXPECT_TRUE(contains(&block, 255, 255, 255, 255));
}

TEST(lh_net_cidr, malformed_text_is_refused)
{
    lh_net_cidr_t block;

    EXPECT_FALSE(parse("", &block));
    EXPECT_FALSE(parse("/8", &block));
    EXPECT_FALSE(parse("10.0.0.0/", &block));
    EXPECT_FALSE(parse("10.0.0.0/33", &block));
    EXPECT_FALSE(parse("10.0.0.0/8x", &block));
    EXPECT_FALSE(parse("10.0.0/8", &block));
    EXPECT_FALSE(parse("host/8", &block));
}

TEST(lh_net_cidr, a_list_holds_what_any_block_holds)
{
    lh_net_cidr_t list[2];

    ASSERT_TRUE(parse("10.0.0.0/8", &list[0]));
    ASSERT_TRUE(parse("127.0.0.1", &list[1]));
    lh_net_ip4_t address;
    lh_net_ip4_init(&address, 127, 0, 0, 1);
    EXPECT_TRUE(lh_net_cidr_list_contains(list, 2U, &address));
    lh_net_ip4_init(&address, 127, 0, 0, 2);
    EXPECT_FALSE(lh_net_cidr_list_contains(list, 2U, &address));
    EXPECT_FALSE(lh_net_cidr_list_contains(lh_null, 0U, &address));
}
