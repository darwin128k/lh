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

#include <lh/core/cpu/simd.h>
#include <lh/core/pixel/argb.h>
#include <lh/core/pixel/rgb565.h>
#include <lh/core/pixel/round.h>
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

/* A colour per pixel is each pixel laid over with its own colour by the one-colour
   kernel: opaque, clear and translucent, over opaque and translucent pixels. */
TEST(pixel_span, each_pixel_of_its_own_colour_is_the_one_colour_span_per_pixel)
{
    const lh_u32_t src[5] = {0xFF112233U, 0x00445566U, 0x80FF0000U, 0x4000FF00U, 0xC00000FFU};
    const lh_u32_t base[5] = {0xFF000000U, 0xFFFFFFFFU, 0xFF102030U, 0x80FFFFFFU, 0xFF808080U};
    const lh_byte_t cover[5] = {255U, 128U, 0U, 200U, 64U};
    lh_u32_t row[5];
    lh_u32_t covered[5];
    lh_u16_t row16[5];
    lh_u16_t covered16[5];

    for (int i = 0; i < 5; ++i)
    {
        row[i] = covered[i] = base[i];
        row16[i] = covered16[i] = static_cast<lh_u16_t>(0x1234U * (i + 1));
    }
    lh_pixel_span_blend_each_argb(row, src, 5U);
    lh_pixel_span_cover_each_argb(covered, cover, src, 5U);
    lh_pixel_span_blend_each_rgb565(row16, src, 5U, lh_bool_true);
    lh_pixel_span_cover_each_rgb565(covered16, cover, src, 5U, lh_bool_false);
    for (int i = 0; i < 5; ++i)
    {
        lh_u32_t one = base[i];
        lh_u32_t one_covered = base[i];
        lh_u16_t one16 = static_cast<lh_u16_t>(0x1234U * (i + 1));
        lh_u16_t one_covered16 = one16;

        lh_pixel_span_blend_argb(&one, 1U, src[i], src[i] >> 24);
        lh_pixel_span_cover_argb(&one_covered, &cover[i], 1U, src[i], src[i] >> 24);
        lh_pixel_span_blend_rgb565(&one16, 1U, src[i], src[i] >> 24, lh_bool_true);
        lh_pixel_span_cover_rgb565(&one_covered16, &cover[i], 1U, src[i], src[i] >> 24,
                                   lh_bool_false);
        EXPECT_EQ(row[i], one) << i;
        EXPECT_EQ(covered[i], one_covered) << i;
        EXPECT_EQ(row16[i], one16) << i;
        EXPECT_EQ(covered16[i], one_covered16) << i;
    }
}

/* What a word becomes under @p rgb at @p alpha: the per-pixel contract every alpha
   kernel answers to. */
lh_u32_t
over_reference(lh_u32_t dst, lh_u32_t rgb, lh_u32_t alpha)
{
    if (alpha == 0U)
    {
        return dst;
    }
    if (alpha == 255U)
    {
        return 0xFF000000U | rgb;
    }
    return lh_pixel_argb_over(dst, (alpha << 24) | rgb);
}

/* A row of alphas over a row of words, the dispatcher and the scalar kernel alike,
   against the per-pixel word: long runs of one alpha, short ones, 0 and 255, an
   opaque background with a translucent word in it, every length to 40 — so the
   const runs, the short-row path, the blocks and their tails are all crossed. */
TEST(pixel_span, a_row_of_alphas_is_every_pixel_laid_alone)
{
    const lh_u32_t rgb = 0x0033CCFFU;

    for (int count = 1; count <= 40; ++count)
    {
        for (int pattern = 0; pattern < 3; ++pattern)
        {
            lh_u32_t row[40];
            lh_u32_t scalar[40];
            lh_u32_t want[40];
            lh_byte_t alpha[40];

            for (int i = 0; i < count; ++i)
            {
                row[i] = (pattern == 2 && i % 7 == 3) ? 0x80FFFFFFU : 0xFF14181DU;
                alpha[i] = static_cast<lh_byte_t>(pattern == 0   ? i * 7
                                                  : pattern == 1 ? (i / 5) * 60
                                                                 : (i % 3 == 0 ? 0 : 255 - i));
                scalar[i] = row[i];
                want[i] = over_reference(row[i], rgb, alpha[i]);
            }
            lh_pixel_span_blend_alphas_argb(row, alpha, static_cast<lh_usize_t>(count), rgb);
            lh_pixel_span_blend_vary_argb(scalar, alpha, static_cast<lh_usize_t>(count), rgb);
            for (int i = 0; i < count; ++i)
            {
                ASSERT_EQ(row[i], want[i])
                    << "count " << count << " pattern " << pattern << " " << i;
                ASSERT_EQ(scalar[i], want[i]) << "scalar, count " << count << " " << i;
            }
        }
    }
}

/* A board has no SIMD, so a run of changing alphas over one background goes through
   the opaque run, two channels at a time: every alpha, 0 and 255 included. */
TEST(pixel_span, the_opaque_run_is_the_per_pixel_word_for_every_alpha)
{
    lh_u32_t at[256];
    lh_byte_t alpha[256];
    const lh_u32_t dst = 0xFF14181DU;
    const lh_u32_t rgb = 0x002E6BE6U;

    for (int i = 0; i < 256; ++i)
    {
        at[i] = dst;
        alpha[i] = static_cast<lh_byte_t>(i);
    }
    lh_pixel_span_over_opaque_argb(at, alpha, 256U, dst, rgb);
    for (int i = 0; i < 256; ++i)
    {
        EXPECT_EQ(at[i], over_reference(dst, rgb, static_cast<lh_u32_t>(i))) << i;
    }
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* The SIMD kernel trades three branches a pixel for one formula, so what is pinned is
   that it lands on the scalar word for every alpha — 0 and 255 included, which the
   formula reproduces rather than special-cases — and every run length to 40: the
   four-pixel loop, its tail, both. A translucent destination anywhere in a block
   sends the block through the "over"; its alpha walks the range, transparent too,
   where a zero alpha must leave the word. */
TEST(pixel_span, the_sse2_kernel_is_the_scalar_word_for_every_alpha_and_length)
{
    if (!lh_cpu_simd_has_sse2())
    {
        GTEST_SKIP() << "SSE2 not available on this CPU";
    }
    const lh_u32_t rgb = 0x0077AA33U;
    const lh_u32_t dest_alpha[] = {0xFF000000U, 0x00000000U, 0x01000000U,
                                   0x7F000000U, 0x80000000U, 0xFE000000U};

    for (lh_u32_t dst_alpha : dest_alpha)
    {
        for (int count = 1; count <= 40; ++count)
        {
            for (int hole = 0; hole < count; hole += 3)
            {
                lh_u32_t at[40];
                lh_u32_t want[40];
                lh_byte_t alpha[40];

                for (int i = 0; i < count; ++i)
                {
                    at[i] = (i == hole ? dst_alpha : 0xFF000000U) |
                            ((static_cast<lh_u32_t>(i) * 7U * 2654435761U) & 0x00FFFFFFU);
                    alpha[i] = static_cast<lh_byte_t>(i % 3 == 0 ? 0 : i * 13);
                    want[i] = over_reference(at[i], rgb, alpha[i]);
                }
                lh_pixel_span_blend_alphas_sse2_argb(at, alpha, static_cast<lh_usize_t>(count),
                                                     rgb);
                for (int i = 0; i < count; ++i)
                {
                    ASSERT_EQ(at[i], want[i]) << "dst alpha " << (dst_alpha >> 24) << " count "
                                              << count << " pixel " << i;
                }
            }
        }
    }
}

/* The general "over" divides in single precision, exact only because every numerator
   stays below 2^24 and every quotient that is not whole stays more than half an ulp
   from the next integer. The tightest cases: a large total with channels near 255,
   a tiny total with channels at the ends — every destination alpha and every source
   alpha against those channels, bit for bit with the scalar word. */
TEST(pixel_span, the_sse2_over_is_exact_for_every_alpha_pair)
{
    if (!lh_cpu_simd_has_sse2())
    {
        GTEST_SKIP() << "SSE2 not available on this CPU";
    }
    const lh_u32_t channels[] = {0U, 1U, 2U, 63U, 64U, 127U, 128U, 129U, 191U, 253U, 254U, 255U};
    lh_u32_t at[256];
    lh_u32_t want[256];
    lh_byte_t alpha[256];

    for (lh_u32_t dst_a = 0U; dst_a < 256U; ++dst_a)
    {
        for (const lh_u32_t s : channels)
        {
            for (const lh_u32_t d : channels)
            {
                const lh_u32_t rgb = (s << 16) | (d << 8) | (255U - s);
                const lh_u32_t dst = (dst_a << 24) | (d << 16) | (s << 8) | (255U - d);

                for (lh_u32_t a = 0U; a < 256U; ++a)
                {
                    at[a] = dst;
                    alpha[a] = static_cast<lh_byte_t>(a);
                    want[a] = a == 0U ? dst : lh_pixel_argb_over(dst, (a << 24) | rgb);
                }
                lh_pixel_span_over_alphas_sse2_argb(at, alpha, 256U, rgb);
                for (lh_u32_t a = 0U; a < 256U; ++a)
                {
                    ASSERT_EQ(at[a], want[a]) << "dst alpha " << dst_a << " alpha " << a;
                }
            }
        }
    }
}
#endif

/* A 10 by 10 rectangle at 0, 0 with a radius of 4, in fixed point. */
lh_byte_t
round_at(int x, int y)
{
    const lh_s64_t s = LH_PIXEL_ROUND_SUBPIXEL;

    return lh_pixel_round_coverage(0, 0, 10 * s, 10 * s, 4 * s, x, y);
}

TEST(pixel_round, the_straight_parts_are_whole_and_the_outside_is_empty)
{
    EXPECT_EQ(round_at(5, 5), 255U) << "the middle";
    EXPECT_EQ(round_at(5, 0), 255U) << "the top edge between the corners";
    EXPECT_EQ(round_at(0, 5), 255U) << "the left edge between the corners";
    EXPECT_EQ(round_at(10, 5), 0U) << "outside";
    EXPECT_EQ(round_at(-1, 5), 0U);
}

TEST(pixel_round, a_corner_is_cut_and_its_edge_is_a_ramp)
{
    EXPECT_EQ(round_at(0, 0), 0U) << "the corner pixel is outside the curve";
    EXPECT_EQ(round_at(9, 9), 0U);
    const lh_byte_t edge = round_at(1, 1);

    EXPECT_GT(edge, 0U);
    EXPECT_LT(edge, 255U);
    EXPECT_EQ(round_at(3, 3), 255U) << "inside the curve";
}

} // namespace
