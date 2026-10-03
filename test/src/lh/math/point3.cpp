#include <gtest/gtest.h>

#include <lh/math/point.h>
#include <lh/math/point3.h>
#include <lh/util/addr.h>

namespace
{

TEST(math_point3, holds_x_y_z)
{
    const lh_math_point3_t p = lh_math_point3_make(10, 20, 30);
    EXPECT_EQ(lh_math_point3_get_x(lh_addr_of(p)), 10);
    EXPECT_EQ(lh_math_point3_get_y(lh_addr_of(p)), 20);
    EXPECT_EQ(lh_math_point3_get_z(lh_addr_of(p)), 30);
}

TEST(math_point3, zero_is_origin)
{
    const lh_math_point3_t p = lh_math_point3_make_empty();
    EXPECT_EQ(lh_math_point3_get_x(lh_addr_of(p)), 0);
    EXPECT_EQ(lh_math_point3_get_y(lh_addr_of(p)), 0);
    EXPECT_EQ(lh_math_point3_get_z(lh_addr_of(p)), 0);
}

TEST(math_point3, embeds_2d_point)
{
    /* A point3 IS a 2D point (its `point` field) plus z. The 3D x/y must
     * match a 2D point constructed with the same x/y — that is what "2D is
     * the core of 3D" means. The z is an additional field. */
    const lh_math_point3_t p = lh_math_point3_make(7, 11, 19);
    const lh_math_point_t two_d = lh_math_point_make(7, 11);
    EXPECT_EQ(lh_math_point_get_x(lh_addr_of(two_d)), lh_math_point3_get_x(lh_addr_of(p)));
    EXPECT_EQ(lh_math_point_get_y(lh_addr_of(two_d)), lh_math_point3_get_y(lh_addr_of(p)));
    EXPECT_EQ(lh_math_point3_get_z(lh_addr_of(p)), 19);
}

TEST(math_point3, setters)
{
    lh_math_point3_t p = lh_math_point3_make_empty();
    lh_math_point3_set_x(lh_addr_of(p), 1);
    lh_math_point3_set_y(lh_addr_of(p), 2);
    lh_math_point3_set_z(lh_addr_of(p), 3);
    EXPECT_EQ(lh_math_point3_get_x(lh_addr_of(p)), 1);
    EXPECT_EQ(lh_math_point3_get_y(lh_addr_of(p)), 2);
    EXPECT_EQ(lh_math_point3_get_z(lh_addr_of(p)), 3);
}

TEST(math_point3, eq)
{
    const lh_math_point3_t a = lh_math_point3_make(1, 2, 3);
    const lh_math_point3_t b = lh_math_point3_make(1, 2, 3);
    const lh_math_point3_t c = lh_math_point3_make(1, 2, 4);
    EXPECT_TRUE(lh_math_point3_eq(lh_addr_of(a), lh_addr_of(b)));
    EXPECT_FALSE(lh_math_point3_eq(lh_addr_of(a), lh_addr_of(c)));
}

TEST(math_point3, offset)
{
    const lh_math_point3_t p = lh_math_point3_make(1, 1, 1);
    const lh_math_point3_t q = lh_math_point3_offset(lh_addr_of(p), 4, 5, 6);
    EXPECT_EQ(lh_math_point3_get_x(lh_addr_of(q)), 5);
    EXPECT_EQ(lh_math_point3_get_y(lh_addr_of(q)), 6);
    EXPECT_EQ(lh_math_point3_get_z(lh_addr_of(q)), 7);
}

} // namespace