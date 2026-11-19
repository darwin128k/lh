/**
 * @file mask.cpp
 * @brief Tests for the row read of `lh/core/pixel/mask.h`.
 *
 * ::lh_pixel_mask_get_row_coverage is the per-pixel coverage a row at a time, so
 * it is checked against ::lh_pixel_mask_get_coverage at every depth a mask has,
 * over every column — those outside the mask included — and every row.
 */

#include <gtest/gtest.h>

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

} // namespace
