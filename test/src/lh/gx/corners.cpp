/**
 * @file corners.cpp
 * @brief Tests for `lh/gx/corners.h` and `lh/gx/corners/initializer.h`.
 *
 * Four lengths, clockwise from the top left: each reads back what was stored
 * in it alone, the same length goes to all four, and a constant takes them in
 * the same order.
 */

#include <gtest/gtest.h>

#include <lh/gx/corners.h>
#include <lh/gx/corners/initializer.h>

namespace
{

TEST(gx_corners, each_corner_reads_back_what_was_stored_in_it_alone)
{
    lh_gx_corners_t corners;

    lh_gx_corners_init_all(&corners, 0);
    lh_gx_corners_set_top_left(&corners, 1);
    lh_gx_corners_set_top_right(&corners, 2);
    lh_gx_corners_set_bottom_right(&corners, 3);
    lh_gx_corners_set_bottom_left(&corners, 4);
    EXPECT_EQ(lh_gx_corners_get_top_left(&corners), 1);
    EXPECT_EQ(lh_gx_corners_get_top_right(&corners), 2);
    EXPECT_EQ(lh_gx_corners_get_bottom_right(&corners), 3);
    EXPECT_EQ(lh_gx_corners_get_bottom_left(&corners), 4);
}

TEST(gx_corners, init_goes_clockwise_from_the_top_left_and_init_all_is_one_length)
{
    lh_gx_corners_t corners;

    lh_gx_corners_init(&corners, 5, 6, 7, 8);
    EXPECT_EQ(lh_gx_corners_get_top_left(&corners), 5);
    EXPECT_EQ(lh_gx_corners_get_top_right(&corners), 6);
    EXPECT_EQ(lh_gx_corners_get_bottom_right(&corners), 7);
    EXPECT_EQ(lh_gx_corners_get_bottom_left(&corners), 8);
    lh_gx_corners_init_all(&corners, 9);
    EXPECT_EQ(lh_gx_corners_get_top_left(&corners), 9);
    EXPECT_EQ(lh_gx_corners_get_top_right(&corners), 9);
    EXPECT_EQ(lh_gx_corners_get_bottom_right(&corners), 9);
    EXPECT_EQ(lh_gx_corners_get_bottom_left(&corners), 9);
}

TEST(gx_corners, a_constant_takes_the_same_order)
{
    static const lh_gx_corners_t tab = lh_gx_corners_initializer(6, 6, 0, 0);
    static const lh_gx_corners_t pill = lh_gx_corners_initializer_all(12);

    EXPECT_EQ(lh_gx_corners_get_top_right(&tab), 6);
    EXPECT_EQ(lh_gx_corners_get_bottom_right(&tab), 0);
    EXPECT_EQ(lh_gx_corners_get_bottom_left(&pill), 12);
}

} // namespace
