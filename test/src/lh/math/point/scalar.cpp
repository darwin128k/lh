#include <gtest/gtest.h>
#include <type_traits>

#include <lh/config.h>
#include <lh/math/point.h>
#include <lh/math/point/scalar.h>
#include <lh/math/scalar.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_point_scalar, components_are_the_scalar)
{
    const lh_math_point_scalar_t p = lh_math_point_scalar_make(1, 2);
    using component = decltype(lh_math_point_scalar_get_x(lh_addr_of(p)));
    EXPECT_TRUE((std::is_same<component, lh_math_scalar_t>::value));
}

TEST(math_point_scalar, holds_x_y)
{
    const lh_math_point_scalar_t p = lh_math_point_scalar_make(10, 20);
    EXPECT_EQ(lh_math_point_scalar_get_x(lh_addr_of(p)), 10);
    EXPECT_EQ(lh_math_point_scalar_get_y(lh_addr_of(p)), 20);
}

TEST(math_point_scalar, zero_is_origin)
{
    const lh_math_point_scalar_t p = lh_math_point_scalar_make_empty();
    EXPECT_EQ(lh_math_point_scalar_get_x(lh_addr_of(p)), 0);
    EXPECT_EQ(lh_math_point_scalar_get_y(lh_addr_of(p)), 0);
}

TEST(math_point_scalar, setters)
{
    lh_math_point_scalar_t p = lh_math_point_scalar_make_empty();
    lh_math_point_scalar_set_x(lh_addr_of(p), 1);
    lh_math_point_scalar_set_y(lh_addr_of(p), 2);
    EXPECT_EQ(lh_math_point_scalar_get_x(lh_addr_of(p)), 1);
    EXPECT_EQ(lh_math_point_scalar_get_y(lh_addr_of(p)), 2);
}

TEST(math_point_scalar, eq)
{
    const lh_math_point_scalar_t a = lh_math_point_scalar_make(1, 2);
    const lh_math_point_scalar_t b = lh_math_point_scalar_make(1, 2);
    const lh_math_point_scalar_t c = lh_math_point_scalar_make(1, 3);
    EXPECT_TRUE(lh_math_point_scalar_eq(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_FALSE(lh_math_point_scalar_eq(lh_addr_of(a), lh_addr_of(c)));
}

TEST(math_point_scalar, roundtrip_through_the_screen_point)
{
    const lh_math_point_t pixel = lh_math_point_make(-8, 15);
    const lh_math_point_scalar_t scalar = lh_math_point_to_point_scalar(pixel);
    const lh_math_point_t back = lh_math_point_scalar_to_point(scalar);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(back)), -8);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(back)), 15);
}

#if LH_LIBRARY_OPTION_MATH_FPU
TEST(math_point_scalar, to_point_truncates_toward_zero)
{
    const lh_math_point_scalar_t scalar = lh_math_point_scalar_make(3.9f, -3.9f);
    const lh_math_point_t pixel = lh_math_point_scalar_to_point(scalar);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(pixel)), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(pixel)), -3);
}
#endif

} /* namespace */
