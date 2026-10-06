#include <gtest/gtest.h>

#include <lh/net/port.h>

namespace
{

TEST(net_port_parse, parses_valid_port)
{
    lh_net_port_t port = 0;
    ASSERT_TRUE(lh_net_port_parse("27015", 5, &port));
    EXPECT_EQ(port, 27015U);
}

TEST(net_port_parse, parses_zero_and_max)
{
    lh_net_port_t port = 0;
    ASSERT_TRUE(lh_net_port_parse("0", 1, &port));
    EXPECT_EQ(port, 0U);

    ASSERT_TRUE(lh_net_port_parse("65535", 5, &port));
    EXPECT_EQ(port, 65535U);
}

TEST(net_port_parse, rejects_above_max)
{
    lh_net_port_t port = 0;
    EXPECT_FALSE(lh_net_port_parse("65536", 5, &port));
}

TEST(net_port_parse, rejects_leading_zero)
{
    lh_net_port_t port = 0;
    EXPECT_FALSE(lh_net_port_parse("0080", 4, &port));
}

TEST(net_port_parse, rejects_empty_input)
{
    lh_net_port_t port = 0;
    EXPECT_FALSE(lh_net_port_parse("", 0, &port));
}

TEST(net_port_compare, suite)
{
    EXPECT_EQ(lh_net_port_equals(80, 80), lh_bool_true);
    EXPECT_EQ(lh_net_port_equals(80, 81), lh_bool_false);
    EXPECT_EQ(lh_net_port_is_at_least(81, 80), lh_bool_true);
    EXPECT_EQ(lh_net_port_is_less(80, 81), lh_bool_true);
    EXPECT_EQ(lh_net_port_is_greater(81, 80), lh_bool_true);
}

TEST(net_port_parse, rejects_non_digit)
{
    lh_net_port_t port = 0;
    EXPECT_FALSE(lh_net_port_parse("80a", 3, &port));
}

} // namespace
