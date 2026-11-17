/**
 * @file pixmap.cpp
 * @brief Tests for `lh/gx/pixmap.h`.
 *
 * The pixmap is a view of memory, not a picture of its own: the bytes belong
 * to whoever provided them. What is checked here is that the shape follows
 * the format, that the row step is right, and that a picture which does not
 * fit in the bytes is refused.
 */

#include <gtest/gtest.h>

#include <lh/core/memory/bounds.h>
#include <lh/core/util/addr.h>
#include <lh/gx/pixmap.h>

namespace
{

class gx_pixmap : public ::testing::Test
{
  protected:
    lh_byte_t pixels[64 * 64 * 4];
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;

    void
    SetUp() override
    {
        lh_memory_bounds_init(lh_addr_of(bits), pixels, sizeof(pixels));
    }
};

TEST_F(gx_pixmap, a_picture_is_a_view_of_memory)
{
    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 64u, 64u, lh_gx_pixmap_format_argb8888);

    EXPECT_EQ(lh_gx_pixmap_get_width(lh_addr_of(pixmap)), 64u);
    EXPECT_EQ(lh_gx_pixmap_get_height(lh_addr_of(pixmap)), 64u);
    EXPECT_EQ(lh_gx_pixmap_get_format(lh_addr_of(pixmap)), lh_gx_pixmap_format_argb8888);
    EXPECT_EQ(lh_memory_bounds_get_begin(lh_gx_pixmap_get_bits(lh_addr_of(pixmap))), pixels);
}

TEST_F(gx_pixmap, the_row_step_follows_the_format)
{
    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 64u, 64u, lh_gx_pixmap_format_argb8888);
    EXPECT_EQ(lh_gx_pixmap_get_stride(lh_addr_of(pixmap)), 64u * 4u);

    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 64u, 64u, lh_gx_pixmap_format_rgb565);
    EXPECT_EQ(lh_gx_pixmap_get_stride(lh_addr_of(pixmap)), 64u * 2u);
}

TEST_F(gx_pixmap, a_padded_row_keeps_its_step)
{
    lh_gx_pixmap_init_stride(lh_addr_of(pixmap), lh_addr_of(bits), 64u, 64u, 256u,
                             lh_gx_pixmap_format_rgb565);

    EXPECT_EQ(lh_gx_pixmap_get_stride(lh_addr_of(pixmap)), 256u);
}

TEST_F(gx_pixmap, the_size_of_the_picture_is_the_rows)
{
    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 64u, 64u, lh_gx_pixmap_format_argb8888);

    EXPECT_EQ(lh_gx_pixmap_size_bytes(lh_addr_of(pixmap)), 64u * 64u * 4u);
}

TEST_F(gx_pixmap, a_picture_that_does_not_fit_is_refused)
{
    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 64u, 64u, lh_gx_pixmap_format_argb8888);
    EXPECT_EQ(lh_gx_pixmap_is_valid(lh_addr_of(pixmap)), lh_bool_true);

    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 128u, 128u, lh_gx_pixmap_format_argb8888);
    EXPECT_EQ(lh_gx_pixmap_is_valid(lh_addr_of(pixmap)), lh_bool_false);
}

TEST_F(gx_pixmap, an_empty_picture_is_refused)
{
    lh_gx_pixmap_init(lh_addr_of(pixmap), lh_addr_of(bits), 0u, 64u, lh_gx_pixmap_format_argb8888);

    EXPECT_EQ(lh_gx_pixmap_is_valid(lh_addr_of(pixmap)), lh_bool_false);
}

TEST(gx_pixmap_format, the_size_of_a_pixel)
{
    EXPECT_EQ(lh_gx_pixmap_bytes_per_pixel(lh_gx_pixmap_format_argb8888), 4u);
    EXPECT_EQ(lh_gx_pixmap_bytes_per_pixel(lh_gx_pixmap_format_rgb565), 2u);
    EXPECT_EQ(lh_gx_pixmap_bytes_per_pixel(lh_gx_pixmap_format_rgb565_swapped), 2u);
}

} // namespace
