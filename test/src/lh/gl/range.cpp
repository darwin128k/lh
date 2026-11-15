#include <gtest/gtest.h>

#include <lh/gl/range.h>

TEST(ui_range, window_length_is_the_seen_share_of_the_track)
{
    EXPECT_EQ(lh_gl_range_window_length(lh_gl_scalar(200), lh_gl_scalar(100), lh_gl_scalar(400), lh_gl_scalar(16)),
              lh_gl_scalar(50));
}

TEST(ui_range, window_length_is_the_whole_track_when_all_is_seen)
{
    EXPECT_EQ(lh_gl_range_window_length(lh_gl_scalar(200), lh_gl_scalar(100), lh_gl_scalar(80), lh_gl_scalar(16)),
              lh_gl_scalar(200));
}

TEST(ui_range, window_length_keeps_the_minimum_but_not_past_the_track)
{
    EXPECT_EQ(lh_gl_range_window_length(lh_gl_scalar(200), lh_gl_scalar(1), lh_gl_scalar(10000), lh_gl_scalar(16)),
              lh_gl_scalar(16));
    EXPECT_EQ(lh_gl_range_window_length(lh_gl_scalar(10), lh_gl_scalar(1), lh_gl_scalar(10000), lh_gl_scalar(16)),
              lh_gl_scalar(10));
}

TEST(ui_range, window_start_moves_from_zero_to_the_track_end)
{
    EXPECT_EQ(lh_gl_range_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(0), lh_gl_scalar(300)),
              lh_gl_scalar(0));
    EXPECT_EQ(lh_gl_range_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(150), lh_gl_scalar(300)),
              lh_gl_scalar(75));
    EXPECT_EQ(lh_gl_range_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(300), lh_gl_scalar(300)),
              lh_gl_scalar(150));
    EXPECT_EQ(lh_gl_range_window_start(lh_gl_scalar(200), lh_gl_scalar(200), lh_gl_scalar(0), lh_gl_scalar(0)),
              lh_gl_scalar(0));
}

TEST(ui_range, offset_from_window_start_is_the_inverse)
{
    EXPECT_EQ(lh_gl_range_offset_from_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(0),
                                                   lh_gl_scalar(300)),
              lh_gl_scalar(0));
    EXPECT_EQ(lh_gl_range_offset_from_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(75),
                                                   lh_gl_scalar(300)),
              lh_gl_scalar(150));
    EXPECT_EQ(lh_gl_range_offset_from_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(150),
                                                   lh_gl_scalar(300)),
              lh_gl_scalar(300));
    EXPECT_EQ(lh_gl_range_offset_from_window_start(lh_gl_scalar(200), lh_gl_scalar(50), lh_gl_scalar(200),
                                                   lh_gl_scalar(300)),
              lh_gl_scalar(300));
    EXPECT_EQ(lh_gl_range_offset_from_window_start(lh_gl_scalar(200), lh_gl_scalar(200), lh_gl_scalar(0),
                                                   lh_gl_scalar(0)),
              lh_gl_scalar(0));
}
