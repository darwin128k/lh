#include <gtest/gtest.h>

#include <lh/math/irect.h>

namespace
{

TEST(math_irect, contains_is_half_open)
{
    const lh_math_irect_t r = lh_math_irect_make(10, 20, 5, 3);
    EXPECT_TRUE(lh_math_irect_contains_point(&r, lh_math_ipoint_make(10, 20)));
    EXPECT_TRUE(lh_math_irect_contains_point(&r, lh_math_ipoint_make(14, 22)));
    EXPECT_FALSE(lh_math_irect_contains_point(&r, lh_math_ipoint_make(15, 20)));
    EXPECT_FALSE(lh_math_irect_contains_point(&r, lh_math_ipoint_make(10, 23)));
}

TEST(math_irect, intersection_and_union)
{
    const lh_math_irect_t a = lh_math_irect_make(0, 0, 10, 10);
    const lh_math_irect_t b = lh_math_irect_make(5, 5, 10, 10);
    const lh_math_irect_t i = lh_math_irect_intersection(&a, &b);
    const lh_math_irect_t expected_i = lh_math_irect_make(5, 5, 5, 5);
    EXPECT_TRUE(lh_math_irect_eq(&i, &expected_i));
    EXPECT_TRUE(lh_math_irect_intersects(&a, &b));

    const lh_math_irect_t u = lh_math_irect_union(&a, &b);
    const lh_math_irect_t expected_u = lh_math_irect_make(0, 0, 15, 15);
    EXPECT_TRUE(lh_math_irect_eq(&u, &expected_u));

    const lh_math_irect_t far = lh_math_irect_make(100, 100, 1, 1);
    EXPECT_FALSE(lh_math_irect_intersects(&a, &far));
    const lh_math_irect_t none = lh_math_irect_intersection(&a, &far);
    EXPECT_TRUE(lh_math_irect_is_empty(&none));
}

TEST(math_irect, offset_and_inset)
{
    const lh_math_irect_t r = lh_math_irect_make(0, 0, 10, 8);
    const lh_math_irect_t moved = lh_math_irect_offset(&r, 3, -2);
    EXPECT_EQ(lh_math_irect_get_x(&moved), 3);
    EXPECT_EQ(lh_math_irect_get_y(&moved), -2);
    EXPECT_EQ(lh_math_irect_get_width(&moved), 10);

    const lh_math_irect_t inner = lh_math_irect_inset(&r, 1, 2);
    const lh_math_irect_t expected = lh_math_irect_make(1, 2, 8, 4);
    EXPECT_TRUE(lh_math_irect_eq(&inner, &expected));
}

} // namespace