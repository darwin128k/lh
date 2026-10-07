#include <gtest/gtest.h>
#include <lh/bool.h>
#include <lh/ui/point.h>

#include <lh/math/isqrt.h>
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

TEST(ui_radius, cover_from_square_matches_the_square_root_everywhere)
{
    const lh_s64_t radii[] = {0, 100, 128, 129, 256, 1000, 4096, 16 * 256};
    for (lh_s64_t r : radii)
    {
        for (lh_s64_t d = 0; d <= r + 3 * LH_UI_RADIUS_SUBPIXEL; d += 7)
        {
            for (lh_s64_t extra = 0; extra < 3; ++extra)
            {
                const lh_s64_t d2 = d * d + extra * d;
                const lh_byte_t want = lh_ui_radius_cover_from_distance(
                    r, static_cast<lh_s64_t>(lh_math_isqrt_u64(static_cast<lh_u64_t>(d2))));
                ASSERT_EQ(lh_ui_radius_cover_from_square(r, d2), want) << r << " " << d2;
            }
        }
    }
}

TEST(ui_radius, contains_follows_the_rounded_shape)
{
    lh_ui_rect_t rect;
    lh_ui_point_t point;

    lh_ui_rect_init(&rect, 10, 10, 40, 30);
    lh_ui_point_init(&point, 10, 10); /* the cut corner pixel */
    EXPECT_EQ(lh_ui_radius_contains(&rect, lh_ui_scalar(10), point), lh_bool_false);
    EXPECT_EQ(lh_ui_radius_contains(&rect, lh_ui_scalar(0), point), lh_bool_true);
    lh_ui_point_init(&point, 30, 10); /* the straight top edge */
    EXPECT_EQ(lh_ui_radius_contains(&rect, lh_ui_scalar(10), point), lh_bool_true);
    lh_ui_point_init(&point, 13, 13); /* inside the arc */
    EXPECT_EQ(lh_ui_radius_contains(&rect, lh_ui_scalar(10), point), lh_bool_true);
    lh_ui_point_init(&point, 50, 20); /* past the right edge */
    EXPECT_EQ(lh_ui_radius_contains(&rect, lh_ui_scalar(10), point), lh_bool_false);
    /* Every pixel: inside exactly when its coverage is at least half. */
    for (int y = 8; y < 42; ++y)
    {
        for (int x = 8; x < 52; ++x)
        {
            lh_ui_point_init(&point, x, y);
            const bool want = lh_ui_radius_coverage(&rect, lh_ui_scalar(10), x, y) >= LH_UI_RADIUS_HIT_COVERAGE;
            ASSERT_EQ(lh_ui_radius_contains(&rect, lh_ui_scalar(10), point) != 0, want) << x << "," << y;
        }
    }
}
