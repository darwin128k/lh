#include <gtest/gtest.h>

#include <lh/util/swap.h>

namespace
{

TEST(util_swap, exchanges_two_values)
{
    int a = 10;
    int b = 20;
    lh_swap(int, a, b);
    EXPECT_EQ(a, 20);
    EXPECT_EQ(b, 10);
}

TEST(util_swap, swap_no_temp_exchanges_two_values)
{
    unsigned a = 1U;
    unsigned b = 2U;
    lh_swap_no_temp(a, b);
    EXPECT_EQ(a, 2U);
    EXPECT_EQ(b, 1U);
}

} // namespace
