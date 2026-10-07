#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

/* width x 3 pixels of @p format, rows one spare pixel longer. */
struct pixmap_fixture
{
    static const int capacity = 3 * 41;
    lh_u32_t words[capacity]; /* u32 storage keeps any format aligned */
    lh_ui_pixmap_t pixmap;
    int width;
    int bytes;

    pixmap_fixture(lh_u32_t fill, lh_ui_pixmap_format_t format = lh_ui_pixmap_format_argb8888, int w = 4)
        : width(w), bytes(lh_ui_pixmap_format_get_bytes(format))
    {
        for (lh_u32_t &word : words)
        {
            word = fill;
        }
        lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), w, 3, (w + 1) * bytes, format);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return lh_ui_pixmap_read_word(lh_addr_of(pixmap), x, y);
    }

    /* The spare pixel past the width of row @p y. */
    lh_u32_t
    spare(int y) const
    {
        return at(width, y);
    }
};

lh_ui_color_t
color_of(int r, int g, int b, int a)
{
    lh_ui_color_t c;
    lh_ui_color_init(lh_addr_of(c), static_cast<lh_u8_t>(r), static_cast<lh_u8_t>(g), static_cast<lh_u8_t>(b),
                     static_cast<lh_u8_t>(a));
    return c;
}

} // namespace

TEST(ui_pixmap, init_keeps_shape_and_rows_follow_the_stride)
{
    pixmap_fixture f(0u);

    EXPECT_EQ(lh_ui_pixmap_get_width(lh_addr_of(f.pixmap)), 4);
    EXPECT_EQ(lh_ui_pixmap_get_height(lh_addr_of(f.pixmap)), 3);
    EXPECT_EQ(lh_ui_pixmap_get_format(lh_addr_of(f.pixmap)), lh_ui_pixmap_format_argb8888);
    EXPECT_EQ(lh_ui_pixmap_get_row(lh_addr_of(f.pixmap), 2), lh_ptr_rcast(lh_byte_t, f.words) + 2 * 20);
    EXPECT_EQ(lh_ui_pixmap_get_address(lh_addr_of(f.pixmap), 3, 1), lh_ptr_rcast(lh_byte_t, f.words) + 20 + 12);
    const lh_ui_rect_t bounds = lh_ui_pixmap_get_bounds(lh_addr_of(f.pixmap));
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(bounds))), 4);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(bounds))), 3);
}

TEST(ui_pixmap, format_sizes)
{
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_argb8888), 4);
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_rgb565), 2);
}

TEST(ui_pixmap, set_and_get_pixel_round_trip_through_argb)
{
    pixmap_fixture f(0u);
    const lh_ui_color_t c = color_of(1, 2, 3, 4);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 3, 1, lh_addr_of(c));
    EXPECT_EQ(f.at(3, 1), 0x04010203u);
    const lh_ui_color_t back = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 3, 1);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(back), lh_addr_of(c)));
}

TEST(ui_pixmap, rgb565_stores_16_bit_words_and_reads_opaque)
{
    pixmap_fixture f(0u, lh_ui_pixmap_format_rgb565);
    const lh_ui_color_t c = color_of(255, 0, 255, 77);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 1, 0, lh_addr_of(c));
    EXPECT_EQ(f.at(1, 0), 0xF81Fu);
    /* Only the two bytes of that pixel changed. */
    EXPECT_EQ(f.at(0, 0), 0u);
    EXPECT_EQ(f.at(2, 0), 0u);
    const lh_ui_color_t back = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 1, 0);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(back)), 255);
    EXPECT_EQ(lh_ui_color_get_g(lh_addr_of(back)), 0);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(back)), 255);
}

TEST(ui_pixmap, fill_span_stores_an_opaque_color_and_leaves_the_rest)
{
    pixmap_fixture f(0x11111111u);
    const lh_ui_color_t c = color_of(10, 20, 30, 255);

    lh_ui_pixmap_fill_span(lh_addr_of(f.pixmap), 1, 3, 2, lh_addr_of(c));
    EXPECT_EQ(f.at(0, 2), 0x11111111u);
    EXPECT_EQ(f.at(1, 2), 0xff0a141eu);
    EXPECT_EQ(f.at(2, 2), 0xff0a141eu);
    EXPECT_EQ(f.at(3, 2), 0x11111111u);
}

/* Spans of every length (unrolled and tail): every pixel, and not one more. */
TEST(ui_pixmap, long_store_span_fills_exactly_its_pixels_in_both_formats)
{
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_argb8888, lh_ui_pixmap_format_rgb565};
    for (lh_ui_pixmap_format_t format : formats)
    {
        for (int x1 = 3; x1 <= 40; ++x1)
        {
            pixmap_fixture f(0u, format, 40);
            lh_ui_pixmap_store_span(lh_addr_of(f.pixmap), 2, x1, 1, 0xABCDu);
            for (int x = 0; x <= 40; ++x)
            {
                const lh_u32_t want = x >= 2 && x < x1 ? 0xABCDu : 0u;
                ASSERT_EQ(f.at(x, 1), want) << format << " " << x1 << " " << x;
            }
            ASSERT_EQ(f.at(0, 0) | f.at(39, 0) | f.at(0, 2) | f.at(39, 2), 0u);
        }
    }
}

TEST(ui_pixmap, fill_span_blends_a_translucent_color)
{
    pixmap_fixture f(0xff000000u);
    const lh_ui_color_t c = color_of(200, 100, 0, 128);
    const lh_ui_color_t black = color_of(0, 0, 0, 255);

    lh_ui_pixmap_fill_span(lh_addr_of(f.pixmap), 0, 1, 0, lh_addr_of(c));
    const lh_ui_color_t want = lh_ui_color_over(lh_addr_of(black), lh_addr_of(c));
    const lh_ui_color_t got = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 0, 0);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(got), lh_addr_of(want)));
}

TEST(ui_pixmap, rgb565_blend_works_on_the_unpacked_pixel)
{
    pixmap_fixture f(0u, lh_ui_pixmap_format_rgb565);
    const lh_ui_color_t white = color_of(255, 255, 255, 255);
    const lh_ui_color_t half = color_of(0, 0, 0, 128);

    lh_ui_pixmap_set_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(white));
    lh_ui_pixmap_blend_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(half));
    const lh_ui_color_t over = lh_ui_color_over(lh_addr_of(white), lh_addr_of(half));
    EXPECT_EQ(f.at(0, 0), lh_ui_color_get_rgb565(lh_addr_of(over)));
}

TEST(ui_pixmap, cover_pixel_scales_alpha_skips_zero_and_stores_full)
{
    pixmap_fixture f(0u);
    const lh_ui_color_t c = color_of(9, 8, 7, 255);

    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c), 0);
    EXPECT_EQ(f.at(0, 0), 0u);
    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 0, 0, lh_addr_of(c), 100);
    const lh_ui_color_t got = lh_ui_pixmap_get_pixel(lh_addr_of(f.pixmap), 0, 0);
    EXPECT_EQ(lh_ui_color_get_a(lh_addr_of(got)), 100);
    EXPECT_EQ(lh_ui_color_get_r(lh_addr_of(got)), 9);
    lh_ui_pixmap_cover_pixel(lh_addr_of(f.pixmap), 1, 0, lh_addr_of(c), 255);
    EXPECT_EQ(f.at(1, 0), 0xff090807u);
}

TEST(ui_pixmap, fill_box_and_clear_stay_inside_the_width)
{
    pixmap_fixture f(0x12345678u, lh_ui_pixmap_format_argb8888, 20);
    const lh_ui_color_t c = color_of(0, 0, 0, 255);

    lh_ui_pixmap_fill_box(lh_addr_of(f.pixmap), 1, 1, 19, 3, lh_addr_of(c));
    EXPECT_EQ(f.at(1, 1), 0xff000000u);
    EXPECT_EQ(f.at(18, 2), 0xff000000u);
    EXPECT_EQ(f.at(19, 2), 0x12345678u);
    EXPECT_EQ(f.at(1, 0), 0x12345678u);

    lh_ui_pixmap_clear(lh_addr_of(f.pixmap), lh_addr_of(c));
    EXPECT_EQ(f.at(19, 2), 0xff000000u);
    EXPECT_EQ(f.at(0, 0), 0xff000000u);
    EXPECT_EQ(f.spare(0), 0x12345678u);
    EXPECT_EQ(f.spare(2), 0x12345678u);
}

TEST(ui_pixmap, translucent_box_blends_every_row)
{
    pixmap_fixture f(0xff000000u, lh_ui_pixmap_format_argb8888, 20);
    const lh_ui_color_t c = color_of(255, 255, 255, 128);

    lh_ui_pixmap_fill_box(lh_addr_of(f.pixmap), 0, 0, 20, 3, lh_addr_of(c));
    EXPECT_EQ(f.at(0, 0), f.at(19, 2));
    EXPECT_NE(f.at(0, 0), 0xff000000u);
    EXPECT_NE(f.at(0, 0), 0xffffffffu);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(ui_pixmap_death, stride_shorter_than_a_row)
{
    lh_u32_t words[4];
    lh_ui_pixmap_t pixmap;

    LH_EXPECT_DEATH(lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), 4, 1, 12,
                                      lh_ui_pixmap_format_argb8888));
}

TEST(ui_pixmap_death, stride_not_a_whole_number_of_pixels)
{
    lh_u32_t words[4];
    lh_ui_pixmap_t pixmap;

    LH_EXPECT_DEATH(lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), 2, 1, 5,
                                      lh_ui_pixmap_format_rgb565));
}

#endif
