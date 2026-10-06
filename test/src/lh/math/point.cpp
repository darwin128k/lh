#include <gtest/gtest.h>
#include <type_traits>

#include <lh/math/ipoint.h>
#include <lh/math/point.h>
#include <lh/math/scalar.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_point, components_are_the_scalar)
{
    const lh_math_point_t p = lh_math_point_make(1, 2);
    using component = decltype(lh_math_point_get_x(lh_addr_of(p)));
    EXPECT_TRUE((std::is_same<component, lh_math_scalar_t>::value));
}

TEST(math_point, holds_x_y)
{
    const lh_math_point_t p = lh_math_point_make(10, 20);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(p)), 10);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(p)), 20);
}

TEST(math_point, zero_is_origin)
{
    const lh_math_point_t p = lh_math_point_make_empty();
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(p)), 0);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(p)), 0);
}

TEST(math_point, setters)
{
    lh_math_point_t p = lh_math_point_make_empty();
    lh_math_point_set_x(lh_addr_of(p), 1);
    lh_math_point_set_y(lh_addr_of(p), 2);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(p)), 1);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(p)), 2);
}

TEST(math_point, eq)
{
    const lh_math_point_t a = lh_math_point_make(1, 2);
    const lh_math_point_t b = lh_math_point_make(1, 2);
    const lh_math_point_t c = lh_math_point_make(1, 3);
    EXPECT_TRUE(lh_math_point_eq(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_FALSE(lh_math_point_eq(lh_addr_of(a), lh_addr_of(c)));
}

TEST(math_point, roundtrip_through_ipoint)
{
    const lh_math_ipoint_t pixel = lh_math_ipoint_make(-8, 15);
    const lh_math_point_t p = lh_math_ipoint_to_point(pixel);
    const lh_math_ipoint_t back = lh_math_point_to_ipoint(p);
    EXPECT_EQ(lh_math_ipoint_get_x(lh_addr_of(back)), -8);
    EXPECT_EQ(lh_math_ipoint_get_y(lh_addr_of(back)), 15);
}

TEST(math_point, to_ipoint_truncates_toward_zero)
{
    const lh_math_point_t p = lh_math_point_make(3.9f, -3.9f);
    const lh_math_ipoint_t pixel = lh_math_point_to_ipoint(p);
    EXPECT_EQ(lh_math_ipoint_get_x(lh_addr_of(pixel)), 3);
    EXPECT_EQ(lh_math_ipoint_get_y(lh_addr_of(pixel)), -3);
}

} /* namespace */
