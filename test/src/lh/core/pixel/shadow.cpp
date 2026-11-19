/**
 * @file shadow.cpp
 * @brief Tests for `lh/core/pixel/round/box.h`, `lh/core/pixel/shadow.h` and
 *        `lh/core/pixel/shadow/ramp.h`.
 *
 * The rounded box answers what the coverage kernel answers, and its full run is
 * exactly the pixels at coverage 255. The shadow's run is every pixel asked alone,
 * over boxes whole and fractional, radii, spreads and shifts; the ramp gives the
 * fade of the root without taking it; a shadow with no spread or no box casts
 * nothing.
 */

#include <gtest/gtest.h>

#include <lh/core/math/isqrt.h>
#include <lh/core/pixel/shadow/ramp.h>

namespace
{

constexpr lh_s64_t one = LH_PIXEL_ROUND_SUBPIXEL;

lh_pixel_round_box_t
box_of(lh_s64_t x, lh_s64_t y, lh_s64_t w, lh_s64_t h, lh_s64_t r)
{
    lh_pixel_round_box_t box;

    lh_pixel_round_box_init(&box, x, y, x + w, y + h, r);
    return box;
}

TEST(pixel_round_box, it_reads_back_and_knows_the_pixels_it_touches)
{
    const lh_pixel_round_box_t whole = box_of(3 * one, 4 * one, 10 * one, 6 * one, 2 * one);
    const lh_pixel_round_box_t part = box_of(-one / 2, one / 4, 3 * one, 2 * one, 0);

    EXPECT_EQ(lh_pixel_round_box_get_lo_x(&whole), 3 * one);
    EXPECT_EQ(lh_pixel_round_box_get_hi_y(&whole), 10 * one);
    EXPECT_EQ(lh_pixel_round_box_get_r(&whole), 2 * one);
    EXPECT_EQ(lh_pixel_round_box_get_left(&whole), 3);
    EXPECT_EQ(lh_pixel_round_box_get_right(&whole), 13);
    EXPECT_EQ(lh_pixel_round_box_get_top(&whole), 4);
    EXPECT_EQ(lh_pixel_round_box_get_bottom(&whole), 10);
    EXPECT_EQ(lh_pixel_round_box_get_left(&part), -1) << "the pixel a fractional edge is in";
    EXPECT_EQ(lh_pixel_round_box_get_right(&part), 3) << "2.5 rounded up";
    EXPECT_EQ(lh_pixel_round_box_get_top(&part), 0);
    EXPECT_EQ(lh_pixel_round_box_get_bottom(&part), 3) << "2.25 rounded up";
    EXPECT_EQ(lh_pixel_round_box_get_coverage(&whole, 8, 7),
              lh_pixel_round_coverage(3 * one, 4 * one, 13 * one, 10 * one, 2 * one, 8, 7));
}

TEST(pixel_round_box, the_full_run_is_the_pixels_it_covers_wholly)
{
    const lh_s64_t radii[5] = {0, 3 * one, 8 * one, 15 * one, 5 * one + 9};
    /* On whole pixels, and at fractional places on either side (a box's right end
       is its own, not its left one mirrored). */
    const lh_s64_t places[3][4] = {{4 * one, 2 * one, 30 * one, 30 * one},
                                   {12 * one + 77, 9 * one + 13, 33 * one + 40, 21 * one + 5},
                                   {-one / 3, one / 2 + 1, 31 * one + 200, 30 * one + 100}};

    for (const auto &p : places)
    {
        for (const lh_s64_t r : radii)
        {
            const lh_pixel_round_box_t box = box_of(p[0], p[1], p[2], p[3], r);

            for (lh_s32_t y = -2; y < 36; ++y)
            {
                lh_s32_t x0 = 0;
                lh_s32_t x1 = 0;

                lh_pixel_round_box_get_full_span(&box, y, &x0, &x1);
                for (lh_s32_t x = -2; x < 50; ++x)
                {
                    const bool full = lh_pixel_round_box_get_coverage(&box, x, y) == 255U;

                    ASSERT_EQ(full, x >= x0 && x < x1)
                        << "box " << p[0] << " radius " << r << " at " << x << "," << y;
                }
            }
        }
    }
}

TEST(pixel_shadow, nothing_is_cast_without_a_spread_or_a_box)
{
    const lh_pixel_round_box_t box = box_of(0, 0, 10 * one, 10 * one, 0);
    const lh_pixel_round_box_t empty = box_of(0, 0, 0, 10 * one, 0);
    lh_pixel_shadow_t shadow;

    lh_pixel_shadow_init(&shadow, &box, 0, 0, 0, 200);
    EXPECT_EQ(lh_pixel_shadow_get_peak(&shadow), 0);
    lh_pixel_shadow_init(&shadow, &empty, 0, 0, 4 * one, 200);
    EXPECT_EQ(lh_pixel_shadow_get_peak(&shadow), 0);
    lh_pixel_shadow_init(&shadow, &box, 0, 0, 4 * one, 200);
    EXPECT_EQ(lh_pixel_shadow_get_peak(&shadow), 200);
    EXPECT_EQ(lh_pixel_shadow_get_fade(&shadow, -4 * one), 200) << "a spread inside: the peak";
    EXPECT_EQ(lh_pixel_shadow_get_fade(&shadow, 4 * one), 0) << "a spread outside: nothing";
    EXPECT_NEAR(lh_pixel_shadow_get_fade(&shadow, 0), 100, 1) << "on the edge: half";
}

/* The run is every pixel asked alone: the box's own pixels left alone, the straight
   parts, the corners read off the ramp. */
TEST(pixel_shadow, a_run_is_every_pixel_asked_alone)
{
    struct Case
    {
        lh_s64_t x, y, w, h, r, ox, oy, spread;
    };

    const Case cases[] = {
        {10 * one, 8 * one, 40 * one, 30 * one, 8 * one, 0, 4 * one, 12 * one},
        {10 * one, 8 * one, 40 * one, 30 * one, 0, 3 * one, -2 * one, 6 * one},
        {12 * one + 77, 9 * one + 13, 33 * one + 40, 21 * one + 5, 5 * one + 9, one / 3, one,
         9 * one},
        {20 * one, 20 * one, 4 * one, 4 * one, 2 * one, 0, 0, 20 * one},
    };

    for (const Case &c : cases)
    {
        const lh_pixel_round_box_t box = box_of(c.x, c.y, c.w, c.h, c.r);
        lh_pixel_shadow_t shadow;
        lh_pixel_shadow_ramp_t ramp;
        lh_byte_t row[80];

        lh_pixel_shadow_init(&shadow, &box, c.ox, c.oy, c.spread, 180);
        lh_pixel_shadow_ramp_init(&ramp, &shadow);
        for (lh_s32_t y = -4; y < 70; ++y)
        {
            lh_pixel_shadow_get_span(&shadow, &ramp, y, -5, 75, row);
            for (lh_s32_t x = -5; x < 75; ++x)
            {
                ASSERT_EQ(row[x + 5], lh_pixel_shadow_get_alpha(
                                          &shadow, lh_pixel_shadow_get_row(&shadow, y), x, y))
                    << "box " << c.x << " at " << x << "," << y;
            }
        }
    }
}

TEST(pixel_shadow_ramp, a_level_is_the_fade_of_the_root)
{
    const lh_pixel_round_box_t box = box_of(0, 0, 40 * one, 40 * one, 6 * one);
    lh_pixel_shadow_t shadow;
    lh_pixel_shadow_ramp_t ramp;
    lh_s32_t level = 0;

    lh_pixel_shadow_init(&shadow, &box, 0, 0, 10 * one, 230);
    lh_pixel_shadow_ramp_init(&ramp, &shadow);
    EXPECT_EQ(lh_pixel_shadow_ramp_get_count(&ramp), 230);
    for (lh_u64_t square = 0U; square < 40000000U; square += 9973U)
    {
        const lh_s32_t root = static_cast<lh_s32_t>(lh_math_isqrt_u64(square));

        level = lh_pixel_shadow_ramp_get_level(&ramp, square, level);
        ASSERT_EQ(level, lh_pixel_shadow_get_fade(&shadow, root - lh_pixel_shadow_get_r(&shadow)))
            << square;
    }
}

TEST(pixel_shadow, two_shadows_are_equal_when_every_number_is)
{
    const lh_pixel_round_box_t box = box_of(0, 0, 10 * one, 8 * one, 2 * one);
    const lh_pixel_round_box_t other = box_of(0, 0, 10 * one, 8 * one, 3 * one);
    lh_pixel_shadow_t a;
    lh_pixel_shadow_t b;

    EXPECT_TRUE(lh_pixel_round_box_is_equal(&box, &box));
    EXPECT_FALSE(lh_pixel_round_box_is_equal(&box, &other));
    lh_pixel_shadow_init(&a, &box, 0, one, 4 * one, 100);
    lh_pixel_shadow_init(&b, &box, 0, one, 4 * one, 100);
    EXPECT_TRUE(lh_pixel_shadow_is_equal(&a, &b));
    lh_pixel_shadow_init(&b, &box, 0, 2 * one, 4 * one, 100);
    EXPECT_FALSE(lh_pixel_shadow_is_equal(&a, &b)) << "the shift";
    lh_pixel_shadow_init(&b, &box, 0, one, 5 * one, 100);
    EXPECT_FALSE(lh_pixel_shadow_is_equal(&a, &b)) << "the spread";
    lh_pixel_shadow_init(&b, &box, 0, one, 4 * one, 101);
    EXPECT_FALSE(lh_pixel_shadow_is_equal(&a, &b)) << "the peak";
    lh_pixel_shadow_init(&b, &other, 0, one, 4 * one, 100);
    EXPECT_FALSE(lh_pixel_shadow_is_equal(&a, &b)) << "the box";
}

} // namespace
