#include <gtest/gtest.h>

#include <lh/ui/geom.h>

namespace
{

TEST(ui_rect, contains_is_half_open)
{
    const lh_ui_rect_t r = lh_ui_rect_make(10, 20, 5, 3);
    EXPECT_TRUE(lh_ui_rect_contains_point(&r, lh_ui_point_make(10, 20)));
    EXPECT_TRUE(lh_ui_rect_contains_point(&r, lh_ui_point_make(14, 22)));
    EXPECT_FALSE(lh_ui_rect_contains_point(&r, lh_ui_point_make(15, 20)));
    EXPECT_FALSE(lh_ui_rect_contains_point(&r, lh_ui_point_make(10, 23)));
}

TEST(ui_rect, intersection_and_union)
{
    const lh_ui_rect_t a = lh_ui_rect_make(0, 0, 10, 10);
    const lh_ui_rect_t b = lh_ui_rect_make(5, 5, 10, 10);
    const lh_ui_rect_t i = lh_ui_rect_intersection(&a, &b);
    const lh_ui_rect_t expected_i = lh_ui_rect_make(5, 5, 5, 5);
    EXPECT_TRUE(lh_ui_rect_eq(&i, &expected_i));
    EXPECT_TRUE(lh_ui_rect_intersects(&a, &b));

    const lh_ui_rect_t u = lh_ui_rect_union(&a, &b);
    const lh_ui_rect_t expected_u = lh_ui_rect_make(0, 0, 15, 15);
    EXPECT_TRUE(lh_ui_rect_eq(&u, &expected_u));

    const lh_ui_rect_t far = lh_ui_rect_make(100, 100, 1, 1);
    EXPECT_FALSE(lh_ui_rect_intersects(&a, &far));
    const lh_ui_rect_t none = lh_ui_rect_intersection(&a, &far);
    EXPECT_TRUE(lh_ui_rect_is_empty(&none));
}

TEST(ui_rect, offset_and_inset)
{
    const lh_ui_rect_t r = lh_ui_rect_make(0, 0, 10, 8);
    const lh_ui_rect_t moved = lh_ui_rect_offset(&r, 3, -2);
    EXPECT_EQ(moved.origin.x, 3);
    EXPECT_EQ(moved.origin.y, -2);
    EXPECT_EQ(lh_ui_rect_width(&moved), 10);

    const lh_ui_rect_t inner = lh_ui_rect_inset(&r, 1, 2);
    const lh_ui_rect_t expected = lh_ui_rect_make(1, 2, 8, 4);
    EXPECT_TRUE(lh_ui_rect_eq(&inner, &expected));
}

} // namespace
