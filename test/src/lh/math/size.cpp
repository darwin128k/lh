#include <gtest/gtest.h>

#include <lh/math/point.h>
#include <lh/math/size.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_size, is_empty_for_zero_or_negative)
{
    lh_math_size_t zero;

    lh_math_size_init_empty(lh_addr_of(zero));
    lh_math_size_t neg;

    lh_math_size_init(lh_addr_of(neg), -1, 4);
    lh_math_size_t ok;

    lh_math_size_init(lh_addr_of(ok), 2, 3);
    EXPECT_TRUE(lh_math_size_is_empty(lh_addr_of(zero)));
    EXPECT_TRUE(lh_math_size_is_empty(lh_addr_of(neg)));
    EXPECT_FALSE(lh_math_size_is_empty(lh_addr_of(ok)));
}

TEST(math_size, from_extent_is_delta)
{
    lh_math_point_t min;

    lh_math_point_init(lh_addr_of(min), 1, 2);
    lh_math_point_t max;

    lh_math_point_init(lh_addr_of(max), 6, 5);
    const lh_math_size_t size = lh_math_size_from_extent(lh_addr_of(min), lh_addr_of(max));
    EXPECT_EQ(lh_math_size_get_width(lh_addr_of(size)), 5);
    EXPECT_EQ(lh_math_size_get_height(lh_addr_of(size)), 3);
}

TEST(math_size, inset_shrinks_both_sides)
{
    lh_math_size_t size;

    lh_math_size_init(lh_addr_of(size), 10, 8);
    const lh_math_size_t inner = lh_math_size_inset(lh_addr_of(size), 1, 2);
    EXPECT_EQ(lh_math_size_get_width(lh_addr_of(inner)), 8);
    EXPECT_EQ(lh_math_size_get_height(lh_addr_of(inner)), 4);
}

TEST(math_size, compare_suite_is_lexicographic)
{
    lh_math_size_t a;
    lh_math_size_t b;
    lh_math_size_t c;

    lh_math_size_init(lh_addr_of(a), 2, 9);
    lh_math_size_init(lh_addr_of(b), 2, 10);
    lh_math_size_init(lh_addr_of(c), 3, 1);

    EXPECT_EQ(lh_math_size_equals(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_math_size_is_less(lh_addr_of(a), lh_addr_of(b)), lh_bool_true);
    EXPECT_EQ(lh_math_size_is_less(lh_addr_of(a), lh_addr_of(c)), lh_bool_true);
    EXPECT_EQ(lh_math_size_is_greater(lh_addr_of(c), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_math_size_is_at_least(lh_addr_of(b), lh_addr_of(a)), lh_bool_true);
}

} // namespace
