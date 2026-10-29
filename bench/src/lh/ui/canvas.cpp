/*
 * The software canvas, one primitive at a time: what a box, a corner, a gradient, a
 * shadow, a blur or a caption costs before any entity or layout is involved.
 *
 * Every benchmark takes the pixel format as its first argument (0 ARGB8888,
 * 1 RGB565) and reports pixels per second where a primitive has an area.
 */

#include <benchmark/benchmark.h>

#include "fixture.h"

#include <lh/ui/canvas/box.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/text.h>

#include <string>

using namespace lh_bench_ui;

static const char *const paragraph =
    "The renderer draws every primitive itself, row by row, into the pixmap: straight runs as "
    "one span, anti-aliased edges per pixel with the coverage of the shape. A caption is a "
    "row of glyph masks, each blended in the colour of the text.";

static void
BM_ui_canvas_clear(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_color_t bg = rgb(20, 24, 29);

    for (auto _ : state)
    {
        lh_ui_canvas_clear(&s.canvas, &bg);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * width * height);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_clear)->Arg(0)->Arg(1);

static void
BM_ui_canvas_fill_rect(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const int side = static_cast<int>(state.range(1));
    const lh_ui_rect_t r = rect(0, 0, side, side < height ? side : height);
    const lh_ui_color_t fill = rgb(46, 107, 230);

    for (auto _ : state)
    {
        lh_ui_canvas_fill_rect(&s.canvas, &r, &fill);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * side * (side < height ? side : height));
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_fill_rect)->ArgsProduct({{0, 1}, {16, 72, 320}});

static void
BM_ui_canvas_fill_rect_translucent(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_rect_t r = rect(8, 8, 72, 66);
    const lh_ui_color_t fill = rgb(46, 107, 230, 128);

    for (auto _ : state)
    {
        lh_ui_canvas_fill_rect(&s.canvas, &r, &fill);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * 72 * 66);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_fill_rect_translucent)->Arg(0)->Arg(1);

/** A 72 x 66 card, the size bench/lvgl draws, rounded by the second argument. */
static void
BM_ui_canvas_fill_round_rect(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_rect_t r = rect(8, 8, 72, 66);
    const lh_ui_color_t fill = rgb(46, 107, 230);
    const lh_ui_scalar_t radius = lh_ui_scalar(static_cast<int>(state.range(1)));

    for (auto _ : state)
    {
        lh_ui_canvas_fill_round_rect(&s.canvas, &r, radius, &fill);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * 72 * 66);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_fill_round_rect)->ArgsProduct({{0, 1}, {0, 8, 24}});

/** The same card filled with a two-stop gradient: 0 down, 1 corner to corner, 2 radial. */
static void
BM_ui_canvas_fill_box_gradient(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_rect_t r = rect(8, 8, 72, 66);
    lh_ui_color_t stops[2] = {rgb(46, 107, 230), rgb(155, 77, 230)};
    lh_ui_gradient_t gradient;
    lh_ui_paint_t paint;
    lh_ui_radii_t radii;

    lh_ui_gradient_init(&gradient);
    lh_ui_gradient_init_stops(&gradient, stops, nullptr, 2);
    switch (state.range(1))
    {
    case 0:
        lh_ui_gradient_set_linear(&gradient, 180);
        break;
    case 1:
        lh_ui_gradient_set_linear(&gradient, 135);
        break;
    default:
        lh_ui_gradient_set_radial(&gradient);
        break;
    }
    lh_ui_paint_init_gradient(&paint, &gradient);
    lh_ui_radii_init_all(&radii, lh_ui_scalar(8));
    for (auto _ : state)
    {
        lh_ui_canvas_fill_box(&s.canvas, &r, &radii, &paint);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * 72 * 66);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_fill_box_gradient)->ArgsProduct({{0, 1}, {0, 1, 2}});

/** A card's soft shadow, faded over the second argument's pixels. */
static void
BM_ui_canvas_shadow(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_rect_t r = rect(40, 40, 72, 66);
    lh_ui_shadow_t shadow;

    lh_ui_shadow_init(&shadow);
    lh_ui_shadow_set_color(&shadow, rgb(0, 0, 0, 128));
    lh_ui_shadow_set_spread_v(&shadow, lh_ui_scalar(static_cast<int>(state.range(1))));
    lh_ui_shadow_set_offset(&shadow, lh_ui_scalar(0), lh_ui_scalar(4));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_ui_canvas_shadow(&s.canvas, &r, lh_ui_scalar(8), &shadow));
        benchmark::ClobberMemory();
    }
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_shadow)->ArgsProduct({{0, 1}, {4, 12, 24}});

/** A 160 x 120 panel blurred by the second argument. */
static void
BM_ui_canvas_blur(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_rect_t r = rect(40, 40, 160, 120);
    const lh_ui_scalar_t radius = lh_ui_scalar(static_cast<int>(state.range(1)));
    lh_bool_t ok = lh_bool_true;

    for (auto _ : state)
    {
        ok = lh_ui_canvas_blur(&s.canvas, &r, radius);
        benchmark::ClobberMemory();
    }
    if (!ok)
    {
        state.SkipWithError("blur refused (scratch too small?)");
    }
    state.SetItemsProcessed(state.iterations() * 160 * 120);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_blur)->ArgsProduct({{0, 1}, {2, 8}});

static void
BM_ui_canvas_glass(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_rect_t r = rect(40, 40, 160, 120);
    const lh_ui_color_t tint = rgb(255, 255, 255, 64);
    lh_bool_t ok = lh_bool_true;

    for (auto _ : state)
    {
        ok = lh_ui_canvas_glass(&s.canvas, &r, lh_ui_scalar(12), lh_ui_scalar(8), &tint);
        benchmark::ClobberMemory();
    }
    if (!ok)
    {
        state.SkipWithError("glass refused (scratch too small?)");
    }
    state.SetItemsProcessed(state.iterations() * 160 * 120);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_glass)->Arg(0)->Arg(1);

/** A clip pushed and popped around a box: the price of a scrolling container. */
static void
BM_ui_canvas_push_pop(benchmark::State &state)
{
    surface s(0);
    const lh_ui_rect_t clip = rect(8, 8, 200, 120);
    lh_ui_point_t delta;

    lh_ui_point_init(&delta, lh_ui_scalar(4), lh_ui_scalar(4));
    for (auto _ : state)
    {
        lh_ui_canvas_push(&s.canvas, delta, &clip);
        lh_ui_canvas_pop(&s.canvas);
    }
}
BENCHMARK(BM_ui_canvas_push_pop);

/* -- Text ------------------------------------------------------------------ */

static void
BM_ui_text_draw(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_color_t ink = rgb(255, 255, 255);
    const char *text = state.range(1) == 0 ? "Card 12" : "The quick brown fox jumps over the lazy dog";
    lh_ui_point_t at;

    lh_ui_point_init(&at, lh_ui_scalar(16), lh_ui_scalar(16));
    for (auto _ : state)
    {
        lh_ui_text_draw(&s.canvas, lh_ui_font_get_default(), text, at, &ink);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(std::char_traits<char>::length(text)));
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_text_draw)->ArgsProduct({{0, 1}, {0, 1}});

static void
BM_ui_text_draw_cyrillic(benchmark::State &state)
{
    surface s(0);
    const lh_ui_color_t ink = rgb(255, 255, 255);
    const char *text = "\xd0\xa1\xd1\x8a\xd0\xb5\xd1\x88\xd1\x8c \xd0\xb6\xd0\xb5 \xd0\xb5\xd1\x89\xd1\x91";
    lh_ui_point_t at;

    lh_ui_point_init(&at, lh_ui_scalar(16), lh_ui_scalar(16));
    for (auto _ : state)
    {
        lh_ui_text_draw(&s.canvas, lh_ui_font_get_default(), text, at, &ink);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ui_text_draw_cyrillic);

/* One glyph mask, the unit a caption is made of: second argument 0 straight into
   the backend slot, 1 through the canvas (transform, clip, damage) on top of it. */
static void
BM_ui_canvas_fill_glyph(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_font_t *font = lh_ui_font_get_default();
    const lh_ui_color_t ink = rgb(255, 255, 255);
    lh_u32_t index = 0U;
    const lh_ui_mask_t *glyph;
    lh_ui_point_t at;

    lh_ui_font_find_index(font, static_cast<lh_u32_t>('g'), &index);
    glyph = lh_ui_font_get_glyph_at(font, index);
    lh_ui_point_init(&at, lh_ui_scalar(16), lh_ui_scalar(16));
    for (auto _ : state)
    {
        if (state.range(1) == 0)
        {
            lh_ui_canvas_sw_fill_mask(&s.sw, &at, glyph, &ink);
        }
        else
        {
            lh_ui_canvas_fill_mask(&s.canvas, glyph, at, &ink);
        }
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * lh_ui_mask_get_width(glyph) * lh_ui_mask_get_height(glyph));
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_canvas_fill_glyph)->ArgsProduct({{0, 1}, {0, 1}});

static void
BM_ui_text_draw_wrapped(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_color_t ink = rgb(255, 255, 255);
    lh_ui_point_t at;

    lh_ui_point_init(&at, lh_ui_scalar(8), lh_ui_scalar(8));
    for (auto _ : state)
    {
        lh_ui_text_draw_wrapped(&s.canvas, lh_ui_font_get_default(), paragraph, at, &ink, lh_ui_scalar(228),
                                lh_ui_text_align_h_left);
        benchmark::ClobberMemory();
    }
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_text_draw_wrapped)->Arg(0)->Arg(1);

static void
BM_ui_text_get_size(benchmark::State &state)
{
    const char *text = "The quick brown fox jumps over the lazy dog";

    for (auto _ : state)
    {
        lh_ui_size_t size = lh_ui_text_get_size(lh_ui_font_get_default(), text);
        benchmark::DoNotOptimize(size);
    }
}
BENCHMARK(BM_ui_text_get_size);

static void
BM_ui_text_get_size_wrapped(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_ui_size_t size = lh_ui_text_get_size_wrapped(lh_ui_font_get_default(), paragraph, lh_ui_scalar(228));
        benchmark::DoNotOptimize(size);
    }
}
BENCHMARK(BM_ui_text_get_size_wrapped);

/** One line laid out: its end, width and ink in one walk (::lh_ui_text_line_measure). */
static void
BM_ui_text_line_measure(benchmark::State &state)
{
    const char *text = "The quick brown fox jumps over the lazy dog";
    lh_ui_text_line_t line;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_ui_text_line_measure(&line, lh_ui_font_get_default(), text, lh_ui_scalar(0)));
        benchmark::DoNotOptimize(line);
    }
    state.SetItemsProcessed(state.iterations() * 43);
}
BENCHMARK(BM_ui_text_line_measure);

/** The width of the same line alone: one advance per letter. */
static void
BM_ui_text_get_line_width(benchmark::State &state)
{
    const char *text = "The quick brown fox jumps over the lazy dog";

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_ui_text_get_line_width(lh_ui_font_get_default(), text));
    }
    state.SetItemsProcessed(state.iterations() * 43);
}
BENCHMARK(BM_ui_text_get_line_width);

static void
BM_ui_font_get_glyph(benchmark::State &state)
{
    const lh_ui_font_t *font = lh_ui_font_get_default();
    lh_u32_t code = 'A';
    lh_ui_mask_t mask;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_ui_font_get_glyph(font, code, &mask));
        benchmark::DoNotOptimize(mask);
        code = code == 'z' ? 'A' : code + 1;
    }
}
BENCHMARK(BM_ui_font_get_glyph);

static void
BM_ui_text_next_code(benchmark::State &state)
{
    const char *text = "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82, world";

    for (auto _ : state)
    {
        const char *cursor = text;
        lh_u32_t sum = 0;
        while (*cursor != '\0')
        {
            sum += lh_ui_text_next_code(&cursor);
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_ui_text_next_code);

/* -- Pixmap and colour kernels --------------------------------------------- */

static void
BM_ui_pixmap_fill_span(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_color_t fill = rgb(46, 107, 230);
    lh_s32_t y = 0;

    for (auto _ : state)
    {
        lh_ui_pixmap_fill_span(&s.pixmap, 0, width, y, &fill);
        y = (y + 1) % height;
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * width);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_pixmap_fill_span)->Arg(0)->Arg(1);

static void
BM_ui_pixmap_blend_span(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    const lh_ui_color_t fill = rgb(46, 107, 230, 128);
    lh_s32_t y = 0;

    for (auto _ : state)
    {
        lh_ui_pixmap_blend_span(&s.pixmap, 0, width, y, &fill);
        y = (y + 1) % height;
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * width);
    state.SetLabel(format_name(static_cast<int>(state.range(0))));
}
BENCHMARK(BM_ui_pixmap_blend_span)->Arg(0)->Arg(1);

/** A run of per-pixel alpha (an anti-aliased edge or a glyph row) over ::LH_UI_PIXMAP_RUN
    pixels. The second argument is the destination: 0 opaque (a cleared window), 1
    transparent (an off-screen layer, ARGB only -- RGB565 has no alpha to be transparent). */
static void
BM_ui_pixmap_blend_alpha_span(benchmark::State &state)
{
    surface s(static_cast<int>(state.range(0)));
    lh_byte_t alpha[LH_UI_PIXMAP_RUN];
    lh_s32_t y = 0;
    const lh_ui_color_t under = state.range(1) == 0 ? rgb(20, 24, 29) : rgb(20, 24, 29, 0);

    lh_ui_pixmap_clear(&s.pixmap, &under);
    for (int i = 0; i < LH_UI_PIXMAP_RUN; ++i)
    {
        alpha[i] = static_cast<lh_byte_t>(i * 4);
    }
    for (auto _ : state)
    {
        lh_ui_pixmap_blend_alpha_span(&s.pixmap, 0, LH_UI_PIXMAP_RUN, y, 0x2E6BE6u, alpha);
        y = (y + 1) % height;
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * LH_UI_PIXMAP_RUN);
    std::string label = format_name(static_cast<int>(state.range(0)));
    label += state.range(1) == 0 ? "/opaque" : "/transparent";
    state.SetLabel(label);
}
BENCHMARK(BM_ui_pixmap_blend_alpha_span)->Args({0, 0})->Args({0, 1})->Args({1, 0});

static void
BM_ui_color_over(benchmark::State &state)
{
    lh_ui_color_t dst = rgb(20, 24, 29);
    const lh_ui_color_t src = rgb(46, 107, 230, 128);

    for (auto _ : state)
    {
        dst = lh_ui_color_over(&dst, &src);
        benchmark::DoNotOptimize(dst);
    }
}
BENCHMARK(BM_ui_color_over);

static void
BM_ui_color_rgb565_roundtrip(benchmark::State &state)
{
    lh_ui_color_t c = rgb(46, 107, 230);
    lh_u32_t word = 0;

    for (auto _ : state)
    {
        word = lh_ui_color_get_rgb565(&c);
        lh_ui_color_init_rgb565(&c, word + 1u);
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(BM_ui_color_rgb565_roundtrip);
