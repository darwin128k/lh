#include <gtest/gtest.h>
#include <type_traits>

#include <lh/math/fpoint.h>
#include <lh/math/fsize.h>
#include <lh/math/point.h>
#include <lh/math/fscalar.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_fpoint, components_are_the_scalar)
{
    const lh_math_fpoint_t p = lh_math_fpoint_make(1, 2);
    using component = decltype(lh_math_fpoint_get_x(lh_addr_of(p)));
    EXPECT_TRUE((std::is_same<component, lh_math_fscalar_t>::value));
}

TEST(math_fpoint, holds_x_y)
{
    const lh_math_fpoint_t p = lh_math_fpoint_make(10, 20);
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(p)), 10);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(p)), 20);
}

TEST(math_fpoint, zero_is_origin)
{
    const lh_math_fpoint_t p = lh_math_fpoint_make_empty();
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(p)), 0);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(p)), 0);
}

TEST(math_fpoint, setters)
{
    lh_math_fpoint_t p = lh_math_fpoint_make_empty();
    lh_math_fpoint_set_x(lh_addr_of(p), 1);
    lh_math_fpoint_set_y(lh_addr_of(p), 2);
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(p)), 1);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(p)), 2);
}

TEST(math_fpoint, eq)
{
    const lh_math_fpoint_t a = lh_math_fpoint_make(1, 2);
    const lh_math_fpoint_t b = lh_math_fpoint_make(1, 2);
    const lh_math_fpoint_t c = lh_math_fpoint_make(1, 3);
    EXPECT_TRUE(lh_math_fpoint_eq(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_FALSE(lh_math_fpoint_eq(lh_addr_of(a), lh_addr_of(c)));
}

TEST(math_fpoint, roundtrip_through_point)
{
    const lh_math_point_t pixel = lh_math_point_make(-8, 15);
    const lh_math_fpoint_t p = lh_math_point_to_fpoint(pixel);
    const lh_math_point_t back = lh_math_fpoint_to_point(p);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(back)), -8);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(back)), 15);
}

TEST(math_fpoint, to_point_truncates_toward_zero)
{
    const lh_math_fpoint_t p = lh_math_fpoint_make(3.9f, -3.9f);
    const lh_math_point_t pixel = lh_math_fpoint_to_point(p);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(pixel)), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(pixel)), -3);
}

TEST(math_fpoint, min_max_and_offset_size)
{
    const lh_math_fpoint_t a = lh_math_fpoint_make(1, 8);
    const lh_math_fpoint_t b = lh_math_fpoint_make(4, 2);
    const lh_math_fpoint_t lo = lh_math_fpoint_min(lh_addr_of(a), lh_addr_of(b));
    const lh_math_fpoint_t hi = lh_math_fpoint_max(lh_addr_of(a), lh_addr_of(b));
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(lo)), 1);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(lo)), 2);
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(hi)), 4);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(hi)), 8);

    const lh_math_fsize_t size = lh_math_fsize_make(5, 3);
    const lh_math_fpoint_t far = lh_math_fpoint_offset_size(lh_addr_of(a), lh_addr_of(size));
    EXPECT_EQ(lh_math_fpoint_get_x(lh_addr_of(far)), 6);
    EXPECT_EQ(lh_math_fpoint_get_y(lh_addr_of(far)), 11);
}

TEST(math_fpoint, in_extent_is_half_open)
{
    const lh_math_fpoint_t min = lh_math_fpoint_make(10, 20);
    const lh_math_fpoint_t max = lh_math_fpoint_make(15, 23);
    const lh_math_fpoint_t inside = lh_math_fpoint_make(14, 22);
    EXPECT_TRUE(lh_math_fpoint_in_extent(lh_addr_of(min), lh_addr_of(min), lh_addr_of(max)));
    EXPECT_TRUE(lh_math_fpoint_in_extent(lh_addr_of(inside), lh_addr_of(min), lh_addr_of(max)));
    EXPECT_FALSE(lh_math_fpoint_in_extent(lh_addr_of(max), lh_addr_of(min), lh_addr_of(max)));
}

} /* namespace */
