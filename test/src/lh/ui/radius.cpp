#include <gtest/gtest.h>

#include <lh/ui/radius.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>

namespace
{

TEST(ui_radius, clamp_keeps_a_small_radius)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 6);
    EXPECT_EQ(lh_ui_radius_clamp(lh_addr_of(rect), lh_ui_scalar(2)), lh_ui_scalar(2));
}

TEST(ui_radius, clamp_limits_to_half_the_short_side)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 6);
    EXPECT_EQ(lh_ui_radius_clamp(lh_addr_of(rect), lh_ui_scalar(100)), lh_ui_scalar(3));
    EXPECT_EQ(lh_ui_radius_clamp(lh_addr_of(rect), LH_UI_RADIUS_CIRCLE), lh_ui_scalar(3));
}

TEST(ui_radius, clamp_turns_negative_into_zero)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 6);
    EXPECT_EQ(lh_ui_radius_clamp(lh_addr_of(rect), lh_ui_scalar(-1)), lh_ui_scalar(0));
}

TEST(ui_radius, coverage_inside_outside_and_square)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);

    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 5, 5), 255);
    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 0, 5), 255);
    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), -1, 5), 0);
    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 10, 5), 0);
    /* The very corner pixel lies outside the arc. */
    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 0, 0), 0);
    /* Square corners cover the corner pixel fully. */
    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(0), 0, 0), 255);
}

TEST(ui_radius, coverage_is_partial_on_the_arc)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);

    /* Pixel (0, 2): center (0.5, 2.5), 3.81 px from the corner center (4, 4). */
    const int on_arc = lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 0, 2);
    EXPECT_GT(on_arc, 0);
    EXPECT_LT(on_arc, 255);
}

TEST(ui_radius, coverage_is_the_same_in_every_corner)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            const int tl = lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), x, y);
            EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 9 - x, y), tl);
            EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), x, 9 - y), tl);
            EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 9 - x, 9 - y), tl);
        }
    }
}

TEST(ui_radius, coverage_follows_a_moved_rect)
{
    lh_ui_rect_t rect;
    lh_ui_rect_t moved;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_rect_init(lh_addr_of(moved), -20, -30, 10, 10);

    EXPECT_EQ(lh_ui_radius_coverage(lh_addr_of(moved), lh_ui_scalar(4), -20, -28),
              lh_ui_radius_coverage(lh_addr_of(rect), lh_ui_scalar(4), 0, 2));
}

} // namespace
