/**
 * @file pixel.cpp
 * @brief Tests for `lh/core/pixel/channel.h`, `argb.h`, `rgb565.h` and `span.h`.
 *
 * The pixel arithmetic every renderer of LH shares. The rounding divide is
 * checked against the real divide over its whole range, the blends at their
 * ends (nothing, everything, half), and the spans against the per-pixel blend
 * they speed up.
 */

#include <gtest/gtest.h>

#include <lh/core/pixel/rgb565.h>
#include <lh/core/pixel/span.h>

namespace
{

TEST(pixel_channel, the_rounding_divide_is_the_real_one_over_its_whole_range)
{
    for (lh_u32_t x = 0U; x <= 255U * 255U; ++x)
    {
        ASSERT_EQ(lh_pixel_channel_div255_round(x), (x + 127U) / 255U) << x;
    }
}

TEST(pixel_channel, the_pair_divide_is_two_divides_in_one_word)
{
    for (lh_u32_t x = 0U; x <= 255U * 255U; x += 97U)
    {
        const lh_u32_t y = 255U * 255U - x;

        ASSERT_EQ(lh_pixel_channel_div255_round_pair((x << 16) | y),
                  (lh_pixel_channel_div255_round(x) << 16) | lh_pixel_channel_div255_round(y));
    }
}

TEST(pixel_channel, widening_keeps_the_ends)
{
    EXPECT_EQ(lh_pixel_channel_expand_bits(0U, 5U), 0U);
    EXPECT_EQ(lh_pixel_channel_expand_bits(31U, 5U), 255U);
    EXPECT_EQ(lh_pixel_channel_expand_bits(63U, 6U), 255U);
}

TEST(pixel_argb, laying_a_colour_over_an_opaque_pixel)
{
    EXPECT_EQ(lh_pixel_argb_over_opaque(0xFF102030U, 0xFFA0B0C0U), 0xFFA0B0C0U) << "opaque wins";
    EXPECT_EQ(lh_pixel_argb_over_opaque(0xFF102030U, 0x00A0B0C0U), 0xFF102030U)
        << "clear is nothing";
    EXPECT_EQ(lh_pixel_argb_over_opaque(0xFF000000U, 0x80FFFFFFU), 0xFF808080U);
    EXPECT_EQ(lh_pixel_argb_make(1U, 2U, 3U, 4U), 0x04010203U);
    EXPECT_EQ(lh_pixel_argb_get_channel(0x04010203U, 16), 1U);
}

TEST(pixel_argb, laying_a_colour_over_a_translucent_pixel)
{
    EXPECT_EQ(lh_pixel_argb_over_translucent(0x00000000U, 0x00FFFFFFU), 0U)
        << "nothing over nothing";
    EXPECT_EQ(lh_pixel_argb_over_translucent(0x00000000U, 0x80FF0000U), 0x80FF0000U);
    EXPECT_EQ(lh_pixel_argb_over(0xFF000000U, 0x80FFFFFFU),
              lh_pixel_argb_over_opaque(0xFF000000U, 0x80FFFFFFU));
}

TEST(pixel_rgb565, packing_and_widening_keep_the_ends)
{
    EXPECT_EQ(lh_pixel_rgb565_from_argb(0xFFFFFFFFU), 0xFFFFU);
    EXPECT_EQ(lh_pixel_rgb565_from_argb(0xFFFF0000U), 0xF800U);
    EXPECT_EQ(lh_pixel_rgb565_to_argb(0xF800U), 0xFFFF0000U);
    EXPECT_EQ(lh_pixel_rgb565_to_argb(0x07E0U), 0xFF00FF00U);
    EXPECT_EQ(lh_pixel_rgb565_to_argb(0x001FU), 0xFF0000FFU);
    EXPECT_EQ(lh_pixel_rgb565_order(0x1234U, lh_bool_true), 0x3412U);
    EXPECT_EQ(lh_pixel_rgb565_order(0x1234U, lh_bool_false), 0x1234U);
}

TEST(pixel_rgb565, a_full_mix_is_the_colour_and_none_is_the_pixel)
{
    EXPECT_EQ(lh_pixel_rgb565_mix(0x0000U, 0xFFFFFFU, 255U), 0xFFFFU);
    EXPECT_EQ(lh_pixel_rgb565_mix(0x1234U, 0xFFFFFFU, 0U), 0x1234U);
}

/* A run of equal words is mixed once; the result is the per-pixel blend all the same. */
TEST(pixel_span, a_translucent_blend_is_the_per_pixel_blend)
{
    lh_u32_t row[6] = {0xFF000000U, 0xFF000000U, 0xFF000000U,
                       0xFFFFFFFFU, 0xFFFFFFFFU, 0xFF102030U};
    lh_u32_t want[6];

    for (int i = 0; i < 6; ++i)
    {
        want[i] = lh_pixel_argb_over(row[i], 0x40336699U);
    }
    lh_pixel_span_blend_argb(row, 6U, 0x336699U, 0x40U);
    for (int i = 0; i < 6; ++i)
    {
        EXPECT_EQ(row[i], want[i]) << i;
    }
}

TEST(pixel_span, an_opaque_blend_stores_and_a_clear_one_does_nothing)
{
    lh_u32_t row[3] = {1U, 2U, 3U};

    lh_pixel_span_blend_argb(row, 3U, 0x112233U, 0U);
    EXPECT_EQ(row[1], 2U);
    lh_pixel_span_blend_argb(row, 3U, 0x112233U, 255U);
    EXPECT_EQ(row[0], 0xFF112233U);
    EXPECT_EQ(row[2], 0xFF112233U);
}

TEST(pixel_span, a_swapped_565_row_stays_swapped)
{
    lh_u16_t row[2] = {0U, 0U};

    lh_pixel_span_blend_rgb565(row, 2U, 0xFF0000U, 255U, lh_bool_true);
    EXPECT_EQ(row[0], 0x00F8U) << "0xF800 with its bytes swapped";
    lh_pixel_span_blend_rgb565(row, 2U, 0x0000FFU, 255U, lh_bool_false);
    EXPECT_EQ(row[1], 0x001FU);
}

TEST(pixel_span, a_mask_scales_the_colour_pixel_by_pixel)
{
    lh_u32_t row[3] = {0xFF000000U, 0xFF000000U, 0xFF000000U};
    const lh_byte_t cover[3] = {0U, 128U, 255U};

    lh_pixel_span_cover_argb(row, cover, 3U, 0xFFFFFFU, 255U);
    EXPECT_EQ(row[0], 0xFF000000U) << "no coverage, no change";
    EXPECT_EQ(row[1], 0xFF808080U);
    EXPECT_EQ(row[2], 0xFFFFFFFFU);
    lh_u16_t row16[2] = {0U, 0U};
    const lh_byte_t cover16[2] = {0U, 255U};

    lh_pixel_span_cover_rgb565(row16, cover16, 2U, 0xFFFFFFU, 255U, lh_bool_false);
    EXPECT_EQ(row16[0], 0U);
    EXPECT_EQ(row16[1], 0xFFFFU);
}

} // namespace
