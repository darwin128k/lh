#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/ui/mask.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

/* 3 x 2 at 4 bpp, rows of 2 bytes: 0 F 8 | 1 0 F */
const lh_byte_t g_bits4[] = {0x0F, 0x80, 0x10, 0xF0};

TEST(ui_mask, init_keeps_the_layout)
{
    lh_ui_mask_t mask;

    lh_ui_mask_init(lh_addr_of(mask), g_bits4, 3, 2, 2, 4);
    EXPECT_EQ(lh_ui_mask_get_width(lh_addr_of(mask)), 3);
    EXPECT_EQ(lh_ui_mask_get_height(lh_addr_of(mask)), 2);
    EXPECT_EQ(lh_ui_mask_get_bpp(lh_addr_of(mask)), 4);
    EXPECT_EQ(lh_ui_mask_get_row(lh_addr_of(mask), 1), g_bits4 + 2);
}

TEST(ui_mask, is_bpp_takes_one_two_four_eight)
{
    EXPECT_EQ(lh_ui_mask_is_bpp(1U), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_is_bpp(8U), lh_bool_true);
    EXPECT_EQ(lh_ui_mask_is_bpp(3U), lh_bool_false);
    EXPECT_EQ(lh_ui_mask_is_bpp(16U), lh_bool_false);
}

TEST(ui_mask, sample_and_coverage_read_packed_pixels)
{
    lh_ui_mask_t mask;

    lh_ui_mask_init(lh_addr_of(mask), g_bits4, 3, 2, 2, 4);
    EXPECT_EQ(lh_ui_mask_get_sample(lh_addr_of(mask), 0, 0), 0U);
    EXPECT_EQ(lh_ui_mask_get_sample(lh_addr_of(mask), 1, 0), 15U);
    EXPECT_EQ(lh_ui_mask_get_sample(lh_addr_of(mask), 2, 0), 8U);
    EXPECT_EQ(lh_ui_mask_get_sample(lh_addr_of(mask), 0, 1), 1U);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 1, 0), 255);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 2, 0), 136);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 1), 17);
}

TEST(ui_mask, outside_is_zero)
{
    lh_ui_mask_t mask;

    lh_ui_mask_init(lh_addr_of(mask), g_bits4, 3, 2, 2, 4);
    EXPECT_EQ(lh_ui_mask_has_pixel(lh_addr_of(mask), 3, 0), lh_bool_false);
    EXPECT_EQ(lh_ui_mask_has_pixel(lh_addr_of(mask), -1, 0), lh_bool_false);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 2), 0);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 5, 5), 0);
}

TEST(ui_mask, one_bit_pixels_are_off_or_full)
{
    const lh_byte_t bits[] = {0xA0};
    lh_ui_mask_t mask;

    lh_ui_mask_init(lh_addr_of(mask), bits, 3, 1, 1, 1);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 0), 255);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 1, 0), 0);
    EXPECT_EQ(lh_ui_mask_get_coverage(lh_addr_of(mask), 2, 0), 255);
}

TEST(ui_mask, rect_starts_at_the_origin)
{
    lh_ui_mask_t mask;
    lh_ui_point_t origin;

    lh_ui_mask_init(lh_addr_of(mask), g_bits4, 3, 2, 2, 4);
    lh_ui_point_init(lh_addr_of(origin), 5, 6);
    EXPECT_TRUE(rect_is(lh_ui_mask_get_rect(lh_addr_of(mask), origin), rect_of(5, 6, 3, 2)));
}

/* The 4 bpp run has an SSE2 pass that unpacks eight nibbles at once, and every
   way of being wrong about that — starting on an odd pixel, stopping short of
   eight bytes, running off the end of the row — shows up as one wrong coverage.
   The answer to compare against is the per-pixel one the loop ends up being. */
TEST(ui_mask, coverage_run_is_the_per_pixel_coverage_for_every_start_and_length)
{
    lh_byte_t bits[4 * 12];
    lh_ui_mask_t mask;
    const int width = 20;
    const int row_bytes = (width * 4) / 8;

    /* Two rows of distinct nibbles, no two alike, so a swapped or shifted sample
       cannot land on the value it should have. */
    for (int i = 0; i < 4 * 12; ++i)
    {
        bits[i] = static_cast<lh_byte_t>((i * 37 + 11) & 0xFF);
    }
    lh_ui_mask_init(lh_addr_of(mask), bits, width, 2, row_bytes, 4);

    for (int y = -1; y < 3; ++y)
    {
        for (int x0 = -2; x0 < width; ++x0)
        {
            for (int count = 0; count <= 12; ++count)
            {
                lh_byte_t run[12];
                const int x1 = x0 + count;

                for (int i = 0; i < count; ++i)
                {
                    run[i] = lh_ui_mask_get_coverage(lh_addr_of(mask), x0 + i, y);
                }
                lh_ui_mask_coverage_run(lh_addr_of(mask), x0, x1, y, run);
                for (int i = 0; i < count; ++i)
                {
                    ASSERT_EQ(run[i], lh_ui_mask_get_coverage(lh_addr_of(mask), x0 + i, y))
                        << "y " << y << " x0 " << x0 << " i " << i;
                }
            }
        }
    }
}

} // namespace
