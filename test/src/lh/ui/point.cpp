#include <gtest/gtest.h>
#include <type_traits>

#include <lh/config.h>
#include <lh/math/point.h>
#include <lh/ui/point.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>

namespace
{

TEST(ui_point, components_are_the_scalar)
{
    lh_ui_point_t p;

    lh_ui_point_init(lh_addr_of(p), 1, 2);
    using component = decltype(lh_ui_point_get_x(lh_addr_of(p)));
    EXPECT_TRUE((std::is_same<component, lh_ui_scalar_t>::value));
}

TEST(ui_point, holds_x_y)
{
    lh_ui_point_t p;

    lh_ui_point_init(lh_addr_of(p), 10, 20);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(p)), 10);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(p)), 20);
}

TEST(ui_point, zero_is_origin)
{
    lh_ui_point_t p;

    lh_ui_point_init_empty(lh_addr_of(p));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(p)), 0);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(p)), 0);
}

TEST(ui_point, setters)
{
    lh_ui_point_t p;

    lh_ui_point_init_empty(lh_addr_of(p));
    lh_ui_point_set_x(lh_addr_of(p), 1);
    lh_ui_point_set_y(lh_addr_of(p), 2);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(p)), 1);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(p)), 2);
}

TEST(ui_point, eq)
{
    lh_ui_point_t a;

    lh_ui_point_init(lh_addr_of(a), 1, 2);
    lh_ui_point_t b;

    lh_ui_point_init(lh_addr_of(b), 1, 2);
    lh_ui_point_t c;

    lh_ui_point_init(lh_addr_of(c), 1, 3);
    EXPECT_TRUE(lh_ui_point_eq(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_FALSE(lh_ui_point_eq(lh_addr_of(a), lh_addr_of(c)));
}

TEST(ui_point, roundtrip_through_the_screen_point)
{
    lh_math_point_t pixel;

    lh_math_point_init(lh_addr_of(pixel), -8, 15);
    const lh_ui_point_t p = lh_ui_point_from_point(pixel);
    const lh_math_point_t back = lh_ui_point_to_point(p);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(back)), -8);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(back)), 15);
}

#if LH_LIBRARY_OPTION_MATH_FPU
TEST(ui_point, to_point_truncates_toward_zero)
{
    lh_ui_point_t p;

    lh_ui_point_init(lh_addr_of(p), 3.9f, -3.9f);
    const lh_math_point_t pixel = lh_ui_point_to_point(p);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(pixel)), 3);
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(pixel)), -3);
}
#endif

} /* namespace */
