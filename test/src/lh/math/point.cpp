#include <gtest/gtest.h>

#include <lh/math/point.h>
#include <lh/math/size.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_point, min_max_are_element_wise)
{
    lh_math_point_t a;

    lh_math_point_init(lh_addr_of(a), 1, 8);
    lh_math_point_t b;

    lh_math_point_init(lh_addr_of(b), 4, 2);
    const lh_math_point_t lo = lh_math_point_min(lh_addr_of(a), lh_addr_of(b));
    const lh_math_point_t hi = lh_math_point_max(lh_addr_of(a), lh_addr_of(b));
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(lo)), 1);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(lo)), 2);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(hi)), 4);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(hi)), 8);
}

TEST(math_point, offset_size_is_far_corner)
{
    lh_math_point_t origin;

    lh_math_point_init(lh_addr_of(origin), 10, 20);
    lh_math_size_t size;

    lh_math_size_init(lh_addr_of(size), 5, 3);
    const lh_math_point_t far = lh_math_point_offset_size(lh_addr_of(origin), lh_addr_of(size));
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(far)), 15);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(far)), 23);
}

TEST(math_point, compare_suite_is_lexicographic)
{
    lh_math_point_t a;
    lh_math_point_t b;
    lh_math_point_t c;

    lh_math_point_init(lh_addr_of(a), 1, 8);
    lh_math_point_init(lh_addr_of(b), 1, 9);
    lh_math_point_init(lh_addr_of(c), 2, 0);

    EXPECT_EQ(lh_math_point_equals(lh_addr_of(a), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_math_point_equals(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
    EXPECT_EQ(lh_math_point_is_less(lh_addr_of(a), lh_addr_of(b)), lh_bool_true);
    EXPECT_EQ(lh_math_point_is_less(lh_addr_of(a), lh_addr_of(c)), lh_bool_true);
    EXPECT_EQ(lh_math_point_is_greater(lh_addr_of(c), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_math_point_is_at_least(lh_addr_of(b), lh_addr_of(a)), lh_bool_true);
    EXPECT_EQ(lh_math_point_is_at_least(lh_addr_of(a), lh_addr_of(b)), lh_bool_false);
}

TEST(math_point, in_extent_is_half_open)
{
    lh_math_point_t min;

    lh_math_point_init(lh_addr_of(min), 10, 20);
    lh_math_point_t max;

    lh_math_point_init(lh_addr_of(max), 15, 23);
    lh_math_point_t inside;

    lh_math_point_init(lh_addr_of(inside), 14, 22);
    lh_math_point_t on_bottom;

    lh_math_point_init(lh_addr_of(on_bottom), 10, 23);
    EXPECT_TRUE(lh_math_point_in_extent(lh_addr_of(min), lh_addr_of(min), lh_addr_of(max)));
    EXPECT_TRUE(lh_math_point_in_extent(lh_addr_of(inside), lh_addr_of(min), lh_addr_of(max)));
    EXPECT_FALSE(lh_math_point_in_extent(lh_addr_of(max), lh_addr_of(min), lh_addr_of(max)));
    EXPECT_FALSE(lh_math_point_in_extent(lh_addr_of(on_bottom), lh_addr_of(min), lh_addr_of(max)));
}

} // namespace
