#include <gtest/gtest.h>

#include <climits>
#include <lh/core/math/fit.h>
#include <lh/expect/death.h>

TEST(math_fit, centres_and_preserves_the_source_aspect)
{
    lh_math_rect_t rect;
    ASSERT_TRUE(lh_math_fit_try_rect(&rect, 480, 320, 1000, 640));
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin(&rect)), 20);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin(&rect)), 0);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size(&rect)), 960);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(&rect)), 640);
    lh_math_fit_rect(&rect, 480, 320, 480, 640);
    EXPECT_EQ(lh_math_point_get_y(lh_math_rect_get_origin(&rect)), 160);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(&rect)), 320);
}

TEST(math_fit, mapping_matches_nearest_sampling_and_allows_captured_drags_outside)
{
    lh_math_rect_t rect;
    lh_math_fit_rect(&rect, 480, 320, 1000, 640);
    lh_math_point_t point, out;
    lh_math_point_init(&point, 220, 100);
    lh_math_fit_map_point(&out, &rect, 480, 320, point);
    EXPECT_EQ(lh_math_point_get_x(&out), 100);
    EXPECT_EQ(lh_math_point_get_y(&out), 50);
    lh_math_point_init(&point, 10, -10);
    ASSERT_TRUE(lh_math_fit_try_map_point(&out, &rect, 480, 320, point));
    EXPECT_EQ(lh_math_point_get_x(&out), -5);
    EXPECT_EQ(lh_math_point_get_y(&out), -5);
    EXPECT_FALSE(lh_math_rect_contains_point(&rect, point));
}

TEST(math_fit, handles_fractional_scale_tiny_and_large_extents)
{
    lh_math_rect_t rect;
    lh_math_fit_rect(&rect, 480, 320, 750, 500);
    EXPECT_EQ(lh_math_size_get_width(lh_math_rect_get_size(&rect)), 750);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(&rect)), 500);
    lh_math_fit_rect(&rect, INT_MAX, 1, 1, 1);
    EXPECT_EQ(lh_math_size_get_height(lh_math_rect_get_size(&rect)), 1);
    lh_math_fit_rect(&rect, 480, 320, INT_MAX, INT_MAX);
    EXPECT_GT(lh_math_size_get_height(lh_math_rect_get_size(&rect)), 0);
}

TEST(math_fit, invalid_inputs_leave_outputs_untouched)
{
    lh_math_rect_t rect;
    lh_math_rect_init(&rect, 7, 8, 9, 10);
    EXPECT_FALSE(lh_math_fit_try_rect(&rect, 0, 320, 1000, 640));
    EXPECT_FALSE(lh_math_fit_try_rect(nullptr, 480, 320, 1000, 640));
    EXPECT_EQ(lh_math_point_get_x(lh_math_rect_get_origin(&rect)), 7);
    lh_math_point_t point, out;
    lh_math_point_init(&point, INT_MAX, 0);
    lh_math_point_init(&out, 5, 6);
    EXPECT_FALSE(lh_math_fit_try_map_point(&out, &rect, INT_MAX, 1, point));
    EXPECT_EQ(lh_math_point_get_x(&out), 5);
    EXPECT_FALSE(lh_math_fit_try_map_point(nullptr, &rect, 1, 1, point));
    LH_EXPECT_CHECK_DEATH(lh_math_fit_rect(&rect, 0, 1, 1, 1));
    LH_EXPECT_CHECK_DEATH(lh_math_fit_map_point(&out, nullptr, 1, 1, point));
}
