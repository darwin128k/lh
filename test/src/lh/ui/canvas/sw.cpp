#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/point.h>
#include <lh/ui/radius.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

const int side = 24;
const lh_u32_t sentinel = 0x00123456u; /* transparent, so blends over it see no dst */

/* A side x side pixmap, a software context over it, and a canvas on it. */
struct sw_fixture
{
    lh_u32_t words[side * side];
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;

    explicit sw_fixture(const lh_ui_canvas_backend_t *backend = lh_addr_of(lh_ui_canvas_backend_sw),
                        lh_ui_pixmap_format_t format = lh_ui_pixmap_format_argb8888)
    {
        for (lh_u32_t &w : words)
        {
            w = sentinel;
        }
        lh_ui_pixmap_init(lh_addr_of(pixmap), lh_ptr_rcast(lh_byte_t, words), side, side,
                          side * lh_ui_pixmap_format_get_bytes(format), format);
        lh_ui_canvas_sw_init(lh_addr_of(sw));
        lh_ui_canvas_sw_set_pixmap(lh_addr_of(sw), lh_addr_of(pixmap));
        lh_ui_canvas_init(lh_addr_of(canvas), backend, lh_addr_of(sw));
    }

    lh_u32_t
    at(int x, int y) const
    {
        return lh_ui_pixmap_read_word(lh_addr_of(pixmap), x, y);
    }

    int
    alpha_at(int x, int y) const
    {
        return static_cast<int>(at(x, y) >> 24);
    }

    void
    push_clip(int x, int y, int w, int h)
    {
        lh_ui_rect_t clip;
        lh_ui_point_t zero;
        lh_ui_rect_init(lh_addr_of(clip), x, y, w, h);
        lh_ui_point_init(lh_addr_of(zero), 0, 0);
        lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(clip));
    }
};

/* Only fill_rect, through the same software context: the canvas draws
 * round rects and masks itself (its fallback), one pixel box at a time. */
const lh_ui_canvas_backend_t g_rect_only = {nullptr, nullptr, nullptr, lh_ui_canvas_sw_fill_rect,
                                              nullptr, nullptr, nullptr};

lh_ui_rect_t
rect_of(int x, int y, int w, int h)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), x, y, w, h);
    return rect;
}

lh_ui_color_t
color_of(int r, int g, int b, int a)
{
    lh_ui_color_t c;
    lh_ui_color_init(lh_addr_of(c), static_cast<lh_u8_t>(r), static_cast<lh_u8_t>(g), static_cast<lh_u8_t>(b),
                     static_cast<lh_u8_t>(a));
    return c;
}

/* 8bpp 5x4 glyph with every coverage step the tests look at. */
const lh_byte_t g_glyph_bits[] = {
    0, 40, 80, 120, 160, 200, 255, 0, 255, 200, 160, 120, 80, 40, 0, 7, 255, 255, 255, 1,
};

lh_ui_mask_t
glyph()
{
    lh_ui_mask_t mask;
    lh_ui_mask_init(lh_addr_of(mask), g_glyph_bits, 5, 4, 5, 8);
    return mask;
}

void
expect_same_pixels(const sw_fixture &a, const sw_fixture &b)
{
    for (int y = 0; y < side; ++y)
    {
        for (int x = 0; x < side; ++x)
        {
            EXPECT_EQ(a.at(x, y), b.at(x, y)) << x << "," << y;
        }
    }
}

} // namespace

TEST(ui_canvas_sw, fill_rect_stores_the_box_and_nothing_else)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_rect_t rect = rect_of(2, 3, 4, 5);

    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(rect), lh_addr_of(c));

    for (int y = 0; y < side; ++y)
    {
        for (int x = 0; x < side; ++x)
        {
            const bool inside = x >= 2 && x < 6 && y >= 3 && y < 8;
            EXPECT_EQ(f.at(x, y), inside ? 0xff010203u : sentinel) << x << "," << y;
        }
    }
}

TEST(ui_canvas_sw, fill_rect_off_the_pixmap_is_cut_to_it)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_rect_t rect = rect_of(-5, side - 2, 8, 10);

    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(rect), lh_addr_of(c));

    EXPECT_EQ(f.at(0, side - 1), 0xff010203u);
    EXPECT_EQ(f.at(2, side - 2), 0xff010203u);
    EXPECT_EQ(f.at(3, side - 2), sentinel);
    EXPECT_EQ(f.at(0, side - 3), sentinel);
}

TEST(ui_canvas_sw, clip_cuts_and_pop_restores)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_rect_t rect = rect_of(0, 0, side, side);

    f.push_clip(4, 4, 2, 3);
    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(rect), lh_addr_of(c));
    lh_ui_canvas_pop(lh_addr_of(f.canvas));

    EXPECT_EQ(f.at(4, 4), 0xff010203u);
    EXPECT_EQ(f.at(5, 6), 0xff010203u);
    EXPECT_EQ(f.at(6, 6), sentinel);
    EXPECT_EQ(f.at(4, 7), sentinel);
    const lh_ui_rect_t limit = lh_ui_canvas_sw_get_limit(lh_addr_of(f.sw));
    EXPECT_TRUE(lh_ui_rect_eq(lh_addr_of(limit), &rect));
}

TEST(ui_canvas_sw, clear_fills_everything_even_under_a_clip)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(5, 6, 7, 255);

    f.push_clip(1, 1, 2, 2);
    lh_ui_canvas_clear(lh_addr_of(f.canvas), lh_addr_of(c));
    lh_ui_canvas_pop(lh_addr_of(f.canvas));

    EXPECT_EQ(f.at(0, 0), 0xff050607u);
    EXPECT_EQ(f.at(side - 1, side - 1), 0xff050607u);
}

/* On a transparent pixmap a pixel's alpha is exactly its coverage. */
TEST(ui_canvas_sw, round_rect_alpha_is_the_radius_coverage)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(10, 20, 30, 255);
    const lh_ui_rect_t rect = rect_of(2, 3, 17, 13);
    const lh_ui_scalar_t radius = lh_ui_radius_clamp(lh_addr_of(rect), lh_ui_scalar(5));

    lh_ui_canvas_fill_round_rect(lh_addr_of(f.canvas), lh_addr_of(rect), lh_ui_scalar(5), lh_addr_of(c));

    for (int y = 0; y < side; ++y)
    {
        for (int x = 0; x < side; ++x)
        {
            const int cover = lh_ui_radius_coverage(lh_addr_of(rect), radius, x, y);
            if (cover == 0)
            {
                EXPECT_EQ(f.at(x, y), sentinel) << x << "," << y;
                continue;
            }
            EXPECT_EQ(f.alpha_at(x, y), cover) << x << "," << y;
            EXPECT_EQ(f.at(x, y) & 0x00ffffffu, 0x000a141eu) << x << "," << y;
        }
    }
}

TEST(ui_canvas_sw, round_rect_matches_the_canvas_fallback_pixel_for_pixel)
{
    sw_fixture sw;
    sw_fixture fallback(&g_rect_only);
    const lh_ui_color_t c = color_of(200, 100, 50, 255);
    const lh_ui_rect_t rect = rect_of(1, 2, 21, 19);

    lh_ui_canvas_fill_round_rect(lh_addr_of(sw.canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE, lh_addr_of(c));
    lh_ui_canvas_fill_round_rect(lh_addr_of(fallback.canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE,
                                 lh_addr_of(c));

    expect_same_pixels(sw, fallback);
}

TEST(ui_canvas_sw, clipped_round_rect_matches_the_canvas_fallback)
{
    sw_fixture sw;
    sw_fixture fallback(&g_rect_only);
    const lh_ui_color_t c = color_of(200, 100, 50, 180);
    const lh_ui_rect_t rect = rect_of(1, 2, 21, 19);

    sw.push_clip(3, 0, 9, 7);
    fallback.push_clip(3, 0, 9, 7);
    lh_ui_canvas_fill_round_rect(lh_addr_of(sw.canvas), lh_addr_of(rect), lh_ui_scalar(8), lh_addr_of(c));
    lh_ui_canvas_fill_round_rect(lh_addr_of(fallback.canvas), lh_addr_of(rect), lh_ui_scalar(8), lh_addr_of(c));

    expect_same_pixels(sw, fallback);
    EXPECT_EQ(sw.at(2, 5), sentinel);
    EXPECT_EQ(sw.at(12, 5), sentinel);
}

TEST(ui_canvas_sw, mask_alpha_is_the_mask_coverage)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_mask_t mask = glyph();
    lh_ui_point_t at;

    lh_ui_point_init(lh_addr_of(at), 6, 9);
    lh_ui_canvas_fill_mask(lh_addr_of(f.canvas), lh_addr_of(mask), at, lh_addr_of(c));

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            const int cover = lh_ui_mask_get_coverage(lh_addr_of(mask), x, y);
            if (cover == 0)
            {
                EXPECT_EQ(f.at(6 + x, 9 + y), sentinel) << x << "," << y;
                continue;
            }
            EXPECT_EQ(f.alpha_at(6 + x, 9 + y), cover) << x << "," << y;
        }
    }
    EXPECT_EQ(f.at(5, 9), sentinel);
}

TEST(ui_canvas_sw, clipped_mask_matches_the_canvas_fallback)
{
    sw_fixture sw;
    sw_fixture fallback(&g_rect_only);
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_mask_t mask = glyph();
    lh_ui_point_t at;

    lh_ui_point_init(lh_addr_of(at), 6, 9);
    sw.push_clip(7, 10, 2, 2);
    fallback.push_clip(7, 10, 2, 2);
    lh_ui_canvas_fill_mask(lh_addr_of(sw.canvas), lh_addr_of(mask), at, lh_addr_of(c));
    lh_ui_canvas_fill_mask(lh_addr_of(fallback.canvas), lh_addr_of(mask), at, lh_addr_of(c));

    expect_same_pixels(sw, fallback);
    EXPECT_NE(sw.at(7, 10), sentinel);
    EXPECT_EQ(sw.at(6, 9), sentinel);
}

TEST(ui_canvas_sw, mask_off_the_pixmap_is_cut_to_it)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_mask_t mask = glyph();
    lh_ui_point_t at;

    lh_ui_point_init(lh_addr_of(at), side - 2, -1);
    lh_ui_canvas_fill_mask(lh_addr_of(f.canvas), lh_addr_of(mask), at, lh_addr_of(c));

    /* Glyph row 1 starts 200, 255 and lands on pixmap row 0; the rest is cut. */
    EXPECT_EQ(f.alpha_at(side - 2, 0), 200);
    EXPECT_EQ(f.alpha_at(side - 1, 0), 255);
    EXPECT_EQ(f.at(side - 2, 3), sentinel);
}

TEST(ui_canvas_sw, an_empty_context_cuts_every_write_away)
{
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_rect_t rect = rect_of(0, 0, 10, 10);

    lh_ui_canvas_sw_init(lh_addr_of(sw));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_sw), lh_addr_of(sw));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(c));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_ui_scalar(3), lh_addr_of(c));
    SUCCEED();
}

/* RGB565: the same pixels as the canvas fallback on an RGB565 pixmap. */
TEST(ui_canvas_sw, rgb565_round_rect_and_mask_match_the_canvas_fallback)
{
    sw_fixture sw(lh_addr_of(lh_ui_canvas_backend_sw), lh_ui_pixmap_format_rgb565);
    sw_fixture fallback(&g_rect_only, lh_ui_pixmap_format_rgb565);
    const lh_ui_color_t c = color_of(200, 100, 50, 255);
    const lh_ui_color_t text = color_of(250, 250, 250, 255);
    const lh_ui_rect_t rect = rect_of(1, 2, 21, 19);
    const lh_ui_mask_t mask = glyph();
    lh_ui_point_t at;

    lh_ui_point_init(lh_addr_of(at), 6, 9);
    lh_ui_canvas_clear(lh_addr_of(sw.canvas), lh_addr_of(text));
    lh_ui_canvas_fill_rect(lh_addr_of(fallback.canvas), &rect, lh_addr_of(text));
    lh_ui_canvas_fill_round_rect(lh_addr_of(sw.canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE, lh_addr_of(c));
    lh_ui_canvas_fill_round_rect(lh_addr_of(fallback.canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE,
                                 lh_addr_of(c));
    lh_ui_canvas_fill_mask(lh_addr_of(sw.canvas), lh_addr_of(mask), at, lh_addr_of(text));
    lh_ui_canvas_fill_mask(lh_addr_of(fallback.canvas), lh_addr_of(mask), at, lh_addr_of(text));

    /* The clear covers everything in sw; compare inside the rect only. */
    for (int y = 2; y < 21; ++y)
    {
        for (int x = 1; x < 22; ++x)
        {
            EXPECT_EQ(sw.at(x, y), fallback.at(x, y)) << x << "," << y;
        }
    }
    EXPECT_EQ(sw.at(11, 11) >> 16, 0u); /* 16-bit words */
}
