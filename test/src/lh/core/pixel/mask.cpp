/**
 * @file mask.cpp
 * @brief Tests for the row read of `lh/core/pixel/mask.h`.
 *
 * ::lh_pixel_mask_get_row_coverage is the per-pixel coverage a row at a time, so
 * it is checked against ::lh_pixel_mask_get_coverage at every depth a mask has,
 * over every column — those outside the mask included — and every row.
 */

#include <gtest/gtest.h>

#include <lh/core/cpu/simd.h>
#include <lh/core/pixel/mask.h>
#include <lh/core/size.h>

namespace
{

TEST(pixel_mask, a_row_read_is_the_per_pixel_read_at_every_depth)
{
    /* Three rows of seven pixels, the bytes counting up so every sample differs. */
    lh_byte_t bits[3 * 7];

    for (lh_usize_t i = 0U; i < sizeof(bits); ++i)
    {
        bits[i] = static_cast<lh_byte_t>(i * 37U + 11U);
    }
    for (lh_byte_t bpp : {static_cast<lh_byte_t>(1U), static_cast<lh_byte_t>(2U),
                          static_cast<lh_byte_t>(4U), static_cast<lh_byte_t>(8U)})
    {
        lh_pixel_mask_t mask;

        lh_pixel_mask_init(&mask, bits, 7, 3, 7, bpp);
        for (lh_s32_t y = -1; y <= 3; ++y)
        {
            lh_byte_t row[11];

            lh_pixel_mask_get_row_coverage(&mask, y, -2, 11, row);
            for (lh_s32_t i = 0; i < 11; ++i)
            {
                ASSERT_EQ(row[i], lh_pixel_mask_get_coverage(&mask, i - 2, y))
                    << "bpp " << int(bpp) << " row " << y << " column " << i - 2;
            }
        }
    }
}

/* The eight-pixel step of a four-bit row: every start from before the mask to past
   it and every length up to three steps, odd widths with a set padding nibble, so a
   whole step, a short last step and the scalar head are each the per-pixel read. */
TEST(pixel_mask, a_four_bit_row_read_eight_a_step_is_the_per_pixel_read)
{
    for (lh_s32_t width = 1; width <= 19; ++width)
    {
        const lh_s32_t row_bytes = (width + 1) / 2;
        lh_byte_t bits[2 * 10];

        for (lh_usize_t i = 0U; i < sizeof(bits); ++i)
        {
            bits[i] = static_cast<lh_byte_t>((i * 53U + 7U) | 0x01U);
        }
        lh_pixel_mask_t mask;

        lh_pixel_mask_init(&mask, bits, width, 2, row_bytes, 4U);
        for (lh_s32_t x = -3; x <= width + 1; ++x)
        {
            for (lh_s32_t count = 0; count <= 24; ++count)
            {
                lh_byte_t row[24];

                lh_pixel_mask_get_row_coverage(&mask, 1, x, count, row);
                for (lh_s32_t i = 0; i < count; ++i)
                {
                    ASSERT_EQ(row[i], lh_pixel_mask_get_coverage(&mask, x + i, 1))
                        << "width " << width << " x " << x << " count " << count << " i " << i;
                }
            }
        }
    }
}

TEST(pixel_mask, a_full_sample_is_full_coverage)
{
    const lh_byte_t bits[1] = {0xFFU};
    lh_pixel_mask_t mask;

    lh_pixel_mask_init(&mask, bits, 2, 1, 1, 4U);
    lh_byte_t row[2];
    lh_pixel_mask_get_row_coverage(&mask, 0, 0, 2, row);
    EXPECT_EQ(row[0], 255U);
    EXPECT_EQ(row[1], 255U);
}

/* Eight coverages in a register: byte i is pixel x + i, 0 outside the mask, for
   every start on and off it, odd widths (a padding nibble, set) and rows packed
   back to back -- in every depth, the four-bit one read eight at a time. */
TEST(pixel_mask, coverage_8_is_eight_per_pixel_coverages)
{
    for (int bpp : {1, 2, 4, 8})
    {
        for (int width = 1; width <= 17; ++width)
        {
            const int row_bytes = (width * bpp + 7) / 8;
            lh_byte_t bits[3 * 17];
            lh_pixel_mask_t mask;

            for (int i = 0; i < 3 * 17; ++i)
            {
                bits[i] = static_cast<lh_byte_t>((i * 53 + 7) | 0x01);
            }
            lh_pixel_mask_init(lh_addr_of(mask), bits, width, 3, row_bytes,
                               static_cast<lh_byte_t>(bpp));
            for (int y = -1; y <= 3; ++y)
            {
                for (int x = -9; x <= width + 1; ++x)
                {
                    const lh_u64_t cover = lh_pixel_mask_get_coverage_8(lh_addr_of(mask), x, y);

                    for (int i = 0; i < 8; ++i)
                    {
                        ASSERT_EQ(static_cast<lh_byte_t>(cover >> (8 * i)),
                                  lh_pixel_mask_get_coverage(lh_addr_of(mask), x + i, y))
                            << "bpp " << bpp << " width " << width << " y " << y << " x " << x
                            << " i " << i;
                    }
                }
            }
        }
    }
}

TEST(pixel_mask, unpack_4_words_rescales_each_nibble_in_pixel_order)
{
    /* Nibbles 1 2 3 4 5 6 7 8 9 A from the high nibble of the lowest byte up. */
    const lh_u64_t bytes = 0x9A78563412ULL;
    const lh_u64_t even = lh_pixel_mask_unpack_4_words(bytes, lh_bool_false);
    const lh_u64_t odd = lh_pixel_mask_unpack_4_words(bytes, lh_bool_true);
    for (unsigned i = 0; i < 8U; ++i)
    {
        EXPECT_EQ((even >> (8U * i)) & 0xFFU, (i + 1U) * 17U) << "pixel " << i;
        EXPECT_EQ((odd >> (8U * i)) & 0xFFU, (i + 2U) * 17U) << "pixel " << i;
    }
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
TEST(pixel_mask, unpack_4_words_matches_the_sse2_unpack)
{
    lh_u64_t state = 0x243F6A8885A308D3ULL;
    for (int round = 0; round < 4096; ++round)
    {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        for (lh_bool_t odd : {lh_bool_false, lh_bool_true})
        {
            ASSERT_EQ(lh_pixel_mask_unpack_4_words(state, odd),
                      lh_pixel_mask_unpack_4_sse2(state, odd))
                << std::hex << state << " odd " << odd;
        }
    }
}
#endif

} // namespace
