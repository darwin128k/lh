#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/container.h>
#include <lh/ui/mask.h>
#include <lh/ui/paint.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/point.h>
#include <lh/ui/radius.h>
#include <lh/ui/rect.h>
#include <lh/ui/shadow.h>
#include <lh/ui/style.h>
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
const lh_ui_canvas_backend_t g_rect_only = {nullptr, nullptr, nullptr, nullptr, lh_ui_canvas_sw_fill_rect,
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

/* ── Rounded clip ────────────────────────────────────────────────────────── */

namespace
{

void
push_round_clip(sw_fixture &f, int x, int y, int w, int h, int radius)
{
    lh_ui_rect_t clip;
    lh_ui_point_t zero;
    lh_ui_rect_init(lh_addr_of(clip), x, y, w, h);
    lh_ui_point_init(lh_addr_of(zero), 0, 0);
    lh_ui_canvas_push_round(lh_addr_of(f.canvas), zero, lh_addr_of(clip), static_cast<lh_ui_scalar_t>(radius));
}

/* Draw every primitive kind on @p f under its current clip. */
void
draw_everything(sw_fixture &f)
{
    const lh_ui_color_t opaque = color_of(200, 100, 50, 255);
    const lh_ui_color_t half = color_of(10, 220, 90, 140);
    const lh_ui_rect_t all = rect_of(0, 0, side, side);
    const lh_ui_rect_t round = rect_of(2, 1, 19, 20);
    const lh_ui_mask_t mask = glyph();
    lh_ui_point_t at;

    lh_ui_point_init(lh_addr_of(at), 1, 2);
    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(all), lh_addr_of(opaque));
    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(all), lh_addr_of(half));
    lh_ui_canvas_fill_round_rect(lh_addr_of(f.canvas), lh_addr_of(round), lh_ui_scalar(6), lh_addr_of(half));
    lh_ui_canvas_fill_mask(lh_addr_of(f.canvas), lh_addr_of(mask), at, lh_addr_of(opaque));
}

} // namespace

TEST(ui_canvas_clip, coverage_is_the_product_of_the_rounds)
{
    lh_ui_canvas_clip_round_t rounds[2];
    lh_ui_canvas_clip_t clip;
    const lh_ui_rect_t a = rect_of(0, 0, 20, 20);
    const lh_ui_rect_t b = rect_of(2, 2, 20, 20);

    lh_ui_canvas_clip_round_init(rounds + 0, &a, lh_ui_scalar(8));
    lh_ui_canvas_clip_round_init(rounds + 1, &b, lh_ui_scalar(8));
    lh_ui_canvas_clip_init(lh_addr_of(clip), &a, rounds, 2U);
    for (int y = 0; y < 22; ++y)
    {
        for (int x = 0; x < 22; ++x)
        {
            const lh_byte_t want = lh_ui_radius_scale(lh_ui_radius_coverage(&a, lh_ui_scalar(8), x, y),
                                                      lh_ui_radius_coverage(&b, lh_ui_scalar(8), x, y));
            ASSERT_EQ(lh_ui_canvas_clip_coverage(lh_addr_of(clip), x, y), want) << x << "," << y;
        }
    }
}

/* The middle a row is split into must be wholly inside: coverage 255. */
TEST(ui_canvas_clip, split_row_middle_is_fully_covered)
{
    lh_ui_canvas_clip_round_t rounds[2];
    lh_ui_canvas_clip_t clip;
    const lh_ui_rect_t a = rect_of(1, 0, 20, 21);
    const lh_ui_rect_t b = rect_of(4, 3, 17, 15);

    lh_ui_canvas_clip_round_init(rounds + 0, &a, lh_ui_scalar(9));
    lh_ui_canvas_clip_round_init(rounds + 1, &b, LH_UI_RADIUS_CIRCLE);
    lh_ui_canvas_clip_init(lh_addr_of(clip), &b, rounds, 2U);
    for (int y = -1; y < 23; ++y)
    {
        lh_s32_t mid0;
        lh_s32_t mid1;
        lh_ui_canvas_clip_split_row(lh_addr_of(clip), -2, 24, y, &mid0, &mid1);
        ASSERT_LE(-2, mid0);
        ASSERT_LE(mid0, mid1);
        ASSERT_LE(mid1, 24);
        for (int x = mid0; x < mid1; ++x)
        {
            ASSERT_EQ(lh_ui_canvas_clip_coverage(lh_addr_of(clip), x, y), 255) << x << "," << y;
        }
    }
}

TEST(ui_canvas_sw, rounded_clip_alpha_is_the_clip_coverage)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(10, 20, 30, 255);
    const lh_ui_rect_t all = rect_of(0, 0, side, side);
    const lh_ui_rect_t clip = rect_of(3, 2, 18, 16);
    const lh_ui_scalar_t radius = lh_ui_radius_clamp(&clip, lh_ui_scalar(7));

    push_round_clip(f, 3, 2, 18, 16, 7);
    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(all), lh_addr_of(c));
    lh_ui_canvas_pop(lh_addr_of(f.canvas));

    for (int y = 0; y < side; ++y)
    {
        for (int x = 0; x < side; ++x)
        {
            const int cover = lh_ui_radius_coverage(&clip, radius, x, y);
            if (cover == 0)
            {
                EXPECT_EQ(f.at(x, y), sentinel) << x << "," << y;
                continue;
            }
            EXPECT_EQ(f.alpha_at(x, y), cover) << x << "," << y;
        }
    }
}

TEST(ui_canvas_sw, rounded_clip_matches_the_canvas_fallback_for_every_primitive)
{
    const lh_ui_pixmap_format_t formats[] = {lh_ui_pixmap_format_argb8888, lh_ui_pixmap_format_rgb565,
                                             lh_ui_pixmap_format_rgb565_swapped};
    for (lh_ui_pixmap_format_t format : formats)
    {
        sw_fixture sw(lh_addr_of(lh_ui_canvas_backend_sw), format);
        sw_fixture fallback(&g_rect_only, format);

        push_round_clip(sw, 2, 1, 20, 21, 8);
        push_round_clip(fallback, 2, 1, 20, 21, 8);
        draw_everything(sw);
        draw_everything(fallback);

        for (int y = 0; y < side; ++y)
        {
            for (int x = 0; x < side; ++x)
            {
                ASSERT_EQ(sw.at(x, y), fallback.at(x, y)) << format << " " << x << "," << y;
            }
        }
    }
}

TEST(ui_canvas_sw, nested_rounded_clips_match_the_canvas_fallback)
{
    sw_fixture sw;
    sw_fixture fallback(&g_rect_only);

    push_round_clip(sw, 0, 0, 20, 20, 9);
    push_round_clip(fallback, 0, 0, 20, 20, 9);
    push_round_clip(sw, 4, 3, 19, 18, 6);
    push_round_clip(fallback, 4, 3, 19, 18, 6);
    draw_everything(sw);
    draw_everything(fallback);

    expect_same_pixels(sw, fallback);
    /* Inside the inner clip, but in the bottom-right corner of the outer one: cut. */
    EXPECT_EQ(sw.at(19, 19), sentinel);
}

TEST(ui_canvas_sw, pop_drops_the_rounded_cut)
{
    sw_fixture f;
    const lh_ui_color_t c = color_of(1, 2, 3, 255);
    const lh_ui_rect_t corner = rect_of(3, 2, 1, 1);

    push_round_clip(f, 3, 2, 18, 16, 7);
    EXPECT_EQ(lh_ui_canvas_get_round_count(lh_addr_of(f.canvas)), 1U);
    lh_ui_canvas_pop(lh_addr_of(f.canvas));
    EXPECT_EQ(lh_ui_canvas_get_round_count(lh_addr_of(f.canvas)), 0U);
    EXPECT_FALSE(lh_ui_canvas_sw_is_rounded(lh_addr_of(f.sw)));

    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(corner), lh_addr_of(c));
    EXPECT_EQ(f.at(3, 2), 0xff010203u);
}

TEST(ui_canvas_sw, radius_zero_push_is_a_plain_rect_clip)
{
    sw_fixture f;

    push_round_clip(f, 3, 2, 18, 16, 0);
    EXPECT_EQ(lh_ui_canvas_get_round_count(lh_addr_of(f.canvas)), 0U);
    EXPECT_FALSE(lh_ui_canvas_sw_is_rounded(lh_addr_of(f.sw)));
    lh_ui_canvas_pop(lh_addr_of(f.canvas));
}

/* A container with a corner radius cuts its children along the same corner. */
TEST(ui_canvas_sw, container_cuts_its_children_along_its_corners)
{
    sw_fixture f;
    lh_ui_color_t c = color_of(9, 9, 9, 255);
    lh_ui_paint_t paint;
    lh_ui_style_t box_style;
    lh_ui_style_t child_style;
    lh_ui_container_t box;
    lh_ui_entity_t child;
    const lh_ui_rect_t box_rect = rect_of(2, 2, 20, 20);

    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(c));
    lh_ui_style_init(lh_addr_of(box_style));
    lh_ui_style_set_radius(lh_addr_of(box_style), lh_ui_scalar(8));
    lh_ui_style_init(lh_addr_of(child_style));
    lh_ui_style_set_fill(lh_addr_of(child_style), lh_addr_of(paint));
    lh_ui_container_init(lh_addr_of(box), box_rect);
    lh_ui_entity_set_style(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(box_style));
    lh_ui_entity_init(lh_addr_of(child), rect_of(0, 0, side, side));
    lh_ui_entity_set_style(lh_addr_of(child), lh_addr_of(child_style));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(child));

    lh_ui_entity_draw(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(f.canvas));

    EXPECT_EQ(f.at(2, 2), sentinel);     /* the cut corner */
    EXPECT_EQ(f.at(12, 2), 0xff090909u); /* the straight top edge */
    EXPECT_EQ(f.alpha_at(3, 5), lh_ui_radius_coverage(&box_rect, lh_ui_scalar(8), 3, 5));
}

/* ── Effects ───────────────────────────────────────────────────────────────── */

lh_ui_shadow_t
shadow_of(int peak, int spread, int dx, int dy)
{
    lh_ui_shadow_t shadow;
    lh_ui_color_t black;

    lh_ui_color_init(lh_addr_of(black), 0, 0, 0, static_cast<lh_u8_t>(peak));
    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), black);
    lh_ui_shadow_set_spread(lh_addr_of(shadow), lh_ui_scalar(spread));
    lh_ui_shadow_set_offset(lh_addr_of(shadow), lh_ui_scalar(dx), lh_ui_scalar(dy));
    return shadow;
}

TEST(ui_canvas_sw, a_shadow_reaches_out_of_its_box_and_is_gone_one_spread_past_it)
{
    sw_fixture f;
    const lh_ui_shadow_t shadow = shadow_of(255, 4, 0, 0);
    const lh_ui_rect_t box = rect_of(8, 8, 8, 8);
    const lh_ui_color_t white = color_of(255, 255, 255, 255);

    lh_ui_canvas_clear(lh_addr_of(f.canvas), lh_addr_of(white));
    EXPECT_TRUE(lh_ui_canvas_shadow(lh_addr_of(f.canvas), lh_addr_of(box), lh_ui_scalar(2), lh_addr_of(shadow)));

    EXPECT_EQ(f.at(12, 12), 0xffffffffu); /* inside the box: the fill owns it */
    EXPECT_LT(f.at(12, 16), 0xffffffffu); /* just past the edge: darkest */
    EXPECT_LT(f.at(12, 16), f.at(12, 18)); /* and lighter further out */
    EXPECT_EQ(f.at(12, 20), 0xffffffffu); /* one spread past the edge: gone */
    EXPECT_EQ(f.at(4, 12), f.at(12, 4));   /* the same all round */
}

/* The picture must not depend on the frame being drawn in one piece: an area that
 * clips the shadow draws its own slice of it, and the slices add up to the same
 * shadow. This is what a partial frame in strips does, every frame. */
TEST(ui_canvas_sw, a_shadow_cut_by_the_area_is_the_same_shadow)
{
    sw_fixture whole;
    sw_fixture cut;
    const lh_ui_shadow_t shadow = shadow_of(200, 5, 1, 2);
    const lh_ui_rect_t box = rect_of(9, 9, 6, 6);
    const lh_ui_color_t white = color_of(250, 250, 250, 255);

    lh_ui_canvas_clear(lh_addr_of(whole.canvas), lh_addr_of(white));
    lh_ui_canvas_shadow(lh_addr_of(whole.canvas), lh_addr_of(box), lh_ui_scalar(3), lh_addr_of(shadow));

    lh_ui_canvas_clear(lh_addr_of(cut.canvas), lh_addr_of(white));
    for (int y = 0; y < side; ++y)
    {
        cut.push_clip(0, y, side, 1);
        lh_ui_canvas_shadow(lh_addr_of(cut.canvas), lh_addr_of(box), lh_ui_scalar(3), lh_addr_of(shadow));
        lh_ui_canvas_pop(lh_addr_of(cut.canvas));
    }

    expect_same_pixels(whole, cut);
}

/* Without the slot the canvas draws it pixel by pixel, and it must be the very
 * same shadow: a backend that has no shadow renderer still gets this effect. */
TEST(ui_canvas_sw, the_shadow_without_a_slot_is_the_shadow_with_one)
{
    sw_fixture slot;
    sw_fixture fallback(lh_addr_of(g_rect_only));
    const lh_ui_shadow_t shadow = shadow_of(180, 5, 2, 3);
    const lh_ui_rect_t box = rect_of(7, 6, 9, 10);
    const lh_ui_rect_t everything = rect_of(0, 0, side, side);
    const lh_ui_color_t white = color_of(240, 240, 240, 255);

    /* ill_rect, not clear: g_rect_only has no clear slot, so the two
       backgrounds would not have been the same pixels to begin with. */
    lh_ui_canvas_fill_rect(lh_addr_of(slot.canvas), lh_addr_of(everything), lh_addr_of(white));
    lh_ui_canvas_shadow(lh_addr_of(slot.canvas), lh_addr_of(box), lh_ui_scalar(4), lh_addr_of(shadow));

    lh_ui_canvas_fill_rect(lh_addr_of(fallback.canvas), lh_addr_of(everything), lh_addr_of(white));
    lh_ui_canvas_shadow(lh_addr_of(fallback.canvas), lh_addr_of(box), lh_ui_scalar(4), lh_addr_of(shadow));

    expect_same_pixels(slot, fallback);
}

/* A shadow nowhere near the clip is not drawn at all, and says so, rather than
 * paying for a pixel loop that every pixel of which is cut away. */
TEST(ui_canvas_sw, a_shadow_the_clip_cannot_see_paints_nothing_and_says_so)
{
    sw_fixture f;
    const lh_ui_shadow_t shadow = shadow_of(255, 4, 0, 0);
    const lh_ui_rect_t box = rect_of(0, 0, 4, 4);

    f.push_clip(12, 12, 4, 4);
    EXPECT_FALSE(lh_ui_canvas_shadow(lh_addr_of(f.canvas), lh_addr_of(box), lh_ui_scalar(1), lh_addr_of(shadow)));
    lh_ui_canvas_pop(lh_addr_of(f.canvas));

    EXPECT_EQ(f.at(1, 1), sentinel);
}
/* A glass panel over a flat picture has to come out flat too: the blur is two
 * sliding windows over a picture that is one colour, so any column that comes out
 * different from its neighbour is the window reading something it should not. */
TEST(ui_canvas_sw, glass_over_a_flat_picture_stays_flat)
{
    sw_fixture f;
    const lh_ui_rect_t rect = rect_of(2, 2, 20, 20);
    const lh_ui_rect_t everything = rect_of(0, 0, side, side);
    const lh_ui_color_t dark = color_of(33, 37, 43, 255);
    const lh_ui_color_t row = color_of(224, 108, 117, 255);
    const lh_ui_color_t tint = color_of(236, 240, 248, 46);
    lh_u8_t scratch[side * side * 4];
    int odd = 0;

    lh_ui_canvas_set_scratch(lh_addr_of(f.canvas), scratch, sizeof(scratch));
    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(everything), lh_addr_of(dark));
    lh_ui_canvas_fill_rect(lh_addr_of(f.canvas), lh_addr_of(rect), lh_addr_of(row));
    EXPECT_TRUE(lh_ui_canvas_glass(lh_addr_of(f.canvas), lh_addr_of(rect), lh_ui_scalar(4), lh_ui_scalar(5),
                                   lh_addr_of(tint)));

    /* The middle of the panel is far from every edge, so only the blur is left. */
    const lh_u32_t middle = f.at(12, 12);
    for (int y = 8; y < 16; ++y)
    {
        for (int x = 8; x < 16; ++x)
        {
            if (f.at(x, y) != middle)
            {
                ++odd;
            }
        }
    }
    EXPECT_EQ(odd, 0);
    EXPECT_NE(middle, sentinel);
}
/* The picture must not depend on how much of it was invalidated. A glass panel
 * drawn while the canvas clips to its own rect has to be the very same panel as
 * one drawn over a whole-target frame: a frame clears and redraws what it
 * touches, so the blur reads the picture this frame drew, never one left over
 * from the last. */
TEST(ui_canvas_sw, glass_is_the_same_panel_in_a_clipped_frame_as_in_a_whole_one)
{
    sw_fixture whole;
    sw_fixture clipped;
    const lh_ui_rect_t rect = rect_of(4, 4, 16, 14);
    const lh_ui_color_t tint = color_of(236, 240, 248, 46);
    lh_u8_t scratch[side * side * 4];

    /* Something with edges in it: a blur of one flat colour is invisible. */
    for (int y = 0; y < side; ++y)
    {
        for (int x = 0; x < side; ++x)
        {
            const lh_ui_color_t c = color_of((x * 9) % 256, (y * 17) % 256, 90, 255);
            const lh_ui_rect_t one = rect_of(x, y, 1, 1);

            lh_ui_canvas_fill_rect(lh_addr_of(whole.canvas), lh_addr_of(one), lh_addr_of(c));
            lh_ui_canvas_fill_rect(lh_addr_of(clipped.canvas), lh_addr_of(one), lh_addr_of(c));
        }
    }
    lh_ui_canvas_set_scratch(lh_addr_of(whole.canvas), scratch, sizeof(scratch));
    EXPECT_TRUE(lh_ui_canvas_glass(lh_addr_of(whole.canvas), lh_addr_of(rect), lh_ui_scalar(4), lh_ui_scalar(4),
                                   lh_addr_of(tint)));

    lh_ui_canvas_set_scratch(lh_addr_of(clipped.canvas), scratch, sizeof(scratch));
    clipped.push_clip(4, 4, 16, 14);
    EXPECT_TRUE(lh_ui_canvas_glass(lh_addr_of(clipped.canvas), lh_addr_of(rect), lh_ui_scalar(4), lh_ui_scalar(4),
                                   lh_addr_of(tint)));
    lh_ui_canvas_pop(lh_addr_of(clipped.canvas));

    expect_same_pixels(whole, clipped);
}