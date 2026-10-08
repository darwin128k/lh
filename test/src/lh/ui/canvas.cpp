#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/fill_probe.h>

#include <lh/config.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/radius.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{

struct call_log
{
    int begin;
    int end;
    int clear;
    int fill_rect;
};

lh_void
log_begin(lh_ptr context)
{
    ++lh_ptr_rcast(call_log, context)->begin;
}

lh_void
log_end(lh_ptr context)
{
    ++lh_ptr_rcast(call_log, context)->end;
}

lh_void
log_clear(lh_ptr context, const lh_ui_color_t *color)
{
    (void)color;
    ++lh_ptr_rcast(call_log, context)->clear;
}

lh_void
log_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    (void)rect;
    (void)color;
    ++lh_ptr_rcast(call_log, context)->fill_rect;
}

const lh_ui_canvas_backend_t g_log_backend = {log_begin, nullptr, log_end, log_clear, log_fill_rect, nullptr,
                                              nullptr,   nullptr};

/* A backend that clips (it has `set_clip`), so the canvas hands it whole rects
   and lets it cut them — the shape a desktop backend has. */
struct effect_log
{
    int glass;
    int round;
    int areas;
    lh_ui_rect_t at;
    lh_ui_rect_t area;
};

lh_void
effect_begin_area(lh_ptr context, const lh_ui_rect_t *area)
{
    effect_log *log = lh_ptr_rcast(effect_log, context);

    log->area = *area;
    ++log->areas;
}

lh_void
effect_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    (void)context;
    (void)clip;
}

lh_bool_t
effect_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner, lh_ui_scalar_t blur_radius,
             const lh_ui_color_t *tint, lh_u8_t *scratch, lh_usize_t bytes)
{
    effect_log *log = lh_ptr_rcast(effect_log, context);

    (void)corner;
    (void)blur_radius;
    (void)tint;
    (void)scratch;
    (void)bytes;
    log->at = *rect;
    ++log->glass;
    return lh_bool_true;
}

lh_bool_t
effect_round(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, const lh_ui_color_t *color)
{
    effect_log *log = lh_ptr_rcast(effect_log, context);

    (void)radius;
    (void)color;
    log->at = *rect;
    ++log->round;
    return lh_bool_true;
}

const lh_ui_canvas_backend_t g_effect_backend = {nullptr,         effect_begin_area, nullptr,
                                                  nullptr,         nullptr,          effect_round,
                                                  effect_set_clip, nullptr,          nullptr,
                                                  nullptr,         effect_glass};

/* A strip frame: the buffer is the strip, so the space the backend draws in starts
   at the strip, and a sheet 40 rows tall crosses the line between two of them. This
   is the case the whole-area mechanism exists for, and it is decided here and not
   in the backend: with the clip holding only the strip there are no pixels behind
   the sheet to read, so the canvas refuses rather than blurs whatever rows it
   happens to have; with the part grown to the sheet there are, and it draws. */
TEST(ui_canvas, a_sheet_across_the_strip_line_is_refused_until_the_part_holds_it)
{
    effect_log log{};
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;
    lh_ui_point_t zero;
    lh_ui_rect_t area;  /* the strip: rows 32..64 */
    lh_ui_rect_t part;  /* what the damage leaves of it: the same rows */
    lh_ui_rect_t whole; /* the strip grown to the sheet: rows 32..88 */
    lh_ui_rect_t sheet;
    lh_ui_color_t tint;
    lh_u8_t scratch[64];

    lh_ui_size_init(lh_addr_of(size), 800, 600);
    lh_ui_color_init(lh_addr_of(tint), 1, 2, 3, 46);
    lh_ui_point_init(lh_addr_of(zero), 0, 0);
    lh_ui_rect_init(lh_addr_of(area), 0, 32, 800, 32);
    lh_ui_rect_init(lh_addr_of(part), 0, 32, 800, 32);
    lh_ui_rect_init(lh_addr_of(whole), 0, 32, 800, 56);
    lh_ui_rect_init(lh_addr_of(sheet), 360, 48, 240, 40);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_effect_backend), lh_addr_of(log));
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_canvas_set_scratch(lh_addr_of(canvas), scratch, sizeof(scratch));

    lh_ui_canvas_begin_area(lh_addr_of(canvas), lh_addr_of(area));
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(part));
    EXPECT_FALSE(lh_ui_canvas_glass(lh_addr_of(canvas), lh_addr_of(sheet), lh_ui_scalar(4), lh_ui_scalar(5),
                                    lh_addr_of(tint)));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
    EXPECT_EQ(log.glass, 0);

    lh_ui_canvas_begin_area(lh_addr_of(canvas), lh_addr_of(area));
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(whole));
    EXPECT_TRUE(lh_ui_canvas_glass(lh_addr_of(canvas), lh_addr_of(sheet), lh_ui_scalar(4), lh_ui_scalar(5),
                                   lh_addr_of(tint)));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
    /* And the sheet reaches the slot whole, in the buffer's own space. */
    EXPECT_EQ(log.glass, 1);
    EXPECT_TRUE(lh_test::rect_is(log.at, lh_test::rect_of(360, 16, 240, 40)))
        << "at " << (int)lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(log.at))) << ","
        << (int)lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(log.at))) << " size "
        << (int)lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(log.at))) << "x"
        << (int)lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(log.at)));
}

/* Two kinds of slot, one clip. A rounded box is a shape: the backend may have it
   whole and cut it. Glass reads the pixels behind it, so a clip that cuts it
   means they are not in the buffer at all — and a backend with a `set_clip` slot
   is not exempt from that, or a strip frame blurs whatever rows it happened to
   get and calls it a panel. */
TEST(ui_canvas, a_clip_that_cuts_a_shape_still_cuts_the_effect_that_reads_it)
{
    effect_log log{};
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;
    lh_ui_point_t zero;
    lh_ui_rect_t inside;
    lh_ui_rect_t across;
    lh_ui_rect_t panel;
    lh_ui_rect_t cut_panel;
    lh_ui_color_t tint;
    lh_u8_t scratch[64];

    lh_ui_size_init(lh_addr_of(size), 160, 120);
    lh_ui_color_init(lh_addr_of(tint), 1, 2, 3, 46);
    lh_ui_point_init(lh_addr_of(zero), 0, 0);
    lh_ui_rect_init(lh_addr_of(inside), 0, 0, 160, 120);
    lh_ui_rect_init(lh_addr_of(across), 0, 40, 160, 20);
    lh_ui_rect_init(lh_addr_of(panel), 20, 40, 40, 20);
    lh_ui_rect_init(lh_addr_of(cut_panel), 20, 55, 40, 20);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_effect_backend), lh_addr_of(log));
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_canvas_set_scratch(lh_addr_of(canvas), scratch, sizeof(scratch));

    /* The clip holds the panel: the shape and the effect both go to the slots. */
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(inside));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(panel), lh_ui_scalar(4), lh_addr_of(tint));
    EXPECT_TRUE(lh_ui_canvas_glass(lh_addr_of(canvas), lh_addr_of(panel), lh_ui_scalar(4), lh_ui_scalar(5),
                                   lh_addr_of(tint)));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
    EXPECT_EQ(log.round, 1);
    EXPECT_EQ(log.glass, 1);

    /* The clip cuts both: the shape is still the backend's (it clips), the effect
       is not sent at all — there is nothing honest to draw in its place. */
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(across));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(cut_panel), lh_ui_scalar(4), lh_addr_of(tint));
    EXPECT_FALSE(lh_ui_canvas_glass(lh_addr_of(canvas), lh_addr_of(cut_panel), lh_ui_scalar(4), lh_ui_scalar(5),
                                    lh_addr_of(tint)));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
    EXPECT_EQ(log.round, 2);
    EXPECT_EQ(log.glass, 1);
}

TEST(ui_canvas, dispatches_every_call_with_the_context)
{
    call_log log{};
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t rect;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_log_backend), lh_addr_of(log));
    EXPECT_EQ(lh_ui_canvas_get_backend(lh_addr_of(canvas)), lh_addr_of(g_log_backend));
    EXPECT_EQ(lh_ui_canvas_get_context(lh_addr_of(canvas)), lh_addr_of(log));

    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_clear(lh_addr_of(canvas), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_end(lh_addr_of(canvas));

    EXPECT_EQ(log.begin, 1);
    EXPECT_EQ(log.clear, 1);
    EXPECT_EQ(log.fill_rect, 2);
    EXPECT_EQ(log.end, 1);
}

TEST(ui_canvas, null_backend_and_null_slots_are_no_ops)
{
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t rect;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);

    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), nullptr);
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_clear(lh_addr_of(canvas), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_end(lh_addr_of(canvas));

    lh_ui_canvas_deinit(lh_addr_of(canvas));
    EXPECT_TRUE(lh_null_eq(lh_ui_canvas_get_backend(lh_addr_of(canvas))));
    lh_ui_canvas_begin(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));
}

/* A 16x16 alpha target: records every fill_rect pixel and how often it was hit. */
struct alpha_target
{
    int alpha[16][16];
    int writes[16][16];
};

lh_void
target_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    alpha_target *target = lh_ptr_rcast(alpha_target, context);
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const int x0 = static_cast<int>(lh_ui_point_get_x(origin));
    const int y0 = static_cast<int>(lh_ui_point_get_y(origin));
    const int w = static_cast<int>(lh_ui_size_get_width(size));
    const int h = static_cast<int>(lh_ui_size_get_height(size));
    for (int y = y0; y < y0 + h; ++y)
    {
        for (int x = x0; x < x0 + w; ++x)
        {
            target->alpha[y][x] = lh_ui_color_get_a(color);
            ++target->writes[y][x];
        }
    }
}

const lh_ui_canvas_backend_t g_target_backend = {nullptr, nullptr, nullptr, nullptr, target_fill_rect, nullptr,
                                                   nullptr, nullptr};

TEST(ui_canvas, round_rect_slot_gets_the_clamped_radius)
{
    lh_test::fill_probe probe;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t color;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 6);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_round_backend(),
                             rect, color);

    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_ui_scalar(100),
                                 lh_addr_of(color));

    EXPECT_EQ(probe.round_matches, 1);
    EXPECT_EQ(probe.matches, 0);
    EXPECT_EQ(probe.radius, lh_ui_scalar(3));
}

TEST(ui_canvas, round_rect_with_zero_radius_is_a_plain_fill)
{
    lh_test::fill_probe probe;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t color;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 6);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_round_backend(),
                             rect, color);

    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_ui_scalar(0), lh_addr_of(color));

    EXPECT_EQ(probe.matches, 1);
    EXPECT_EQ(probe.round_matches, 0);
}

TEST(ui_canvas, round_rect_fallback_is_anti_aliased_and_draws_each_pixel_once)
{
    alpha_target target{};
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t color;

    lh_ui_rect_init(lh_addr_of(rect), 2, 3, 11, 9);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_target_backend), lh_addr_of(target));

    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE,
                                 lh_addr_of(color));

    const lh_ui_scalar_t radius = lh_ui_radius_clamp(lh_addr_of(rect), LH_UI_RADIUS_CIRCLE);
    int partial = 0;
    for (int y = 0; y < 16; ++y)
    {
        for (int x = 0; x < 16; ++x)
        {
            const int cover = lh_ui_radius_coverage(lh_addr_of(rect), radius, x, y);
            EXPECT_LE(target.writes[y][x], 1) << x << "," << y;
            EXPECT_EQ(target.alpha[y][x], cover) << x << "," << y;
            partial += cover > 0 && cover < 255 ? 1 : 0;
        }
    }
    EXPECT_GT(partial, 0);
}

/* ── Frames ───────────────────────────────────────────────────────────────── */

TEST(ui_canvas, begin_area_moves_the_primitives_and_the_clip_into_the_buffer)
{
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t area;
    lh_ui_rect_t clip;
    lh_ui_rect_t rect;
    lh_ui_point_t zero;

    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, true);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_point_init(lh_addr_of(zero), 0, 0);
    lh_ui_rect_init(lh_addr_of(area), 0, 64, 160, 32);
    lh_ui_rect_init(lh_addr_of(clip), 0, 70, 160, 20);
    lh_ui_rect_init(lh_addr_of(rect), 10, 75, 20, 5);

    lh_ui_canvas_begin_area(lh_addr_of(canvas), lh_addr_of(area));
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(clip));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_end(lh_addr_of(canvas));

    /* The strip starts at y 64, so everything the backend sees is 64 rows up:
       the area is handed over untouched, the clip and the fill are the target
       ones minus that, and neither leaves the 160x32 buffer. */
    ASSERT_EQ(log.area_count, 1);
    EXPECT_TRUE(lh_test::rect_is(log.areas[0], lh_test::rect_of(0, 64, 160, 32)));
    ASSERT_EQ(log.clip_count, 1);
    EXPECT_TRUE(lh_test::rect_is(log.clip, lh_test::rect_of(0, 6, 160, 20)));
    ASSERT_EQ(log.fill_count, 1);
    EXPECT_TRUE(lh_test::rect_is(log.fills[0], lh_test::rect_of(10, 11, 20, 5)));
}

TEST(ui_canvas, begin_area_without_the_slot_draws_the_whole_target)
{
    lh_test::draw_log log;
    lh_ui_canvas_backend_t whole = *lh_test::draw_log_backend(false, true);
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t area;
    lh_ui_rect_t rect;

    /* The fallback is the whole-target frame: the same drawing, no smaller
       buffer and nothing gained. */
    whole.begin_area = nullptr;
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, true);
    lh_ui_canvas_set_backend(lh_addr_of(canvas), lh_addr_of(whole));
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(area), 0, 64, 160, 32);
    lh_ui_rect_init(lh_addr_of(rect), 10, 75, 20, 5);

    lh_ui_canvas_begin_area(lh_addr_of(canvas), lh_addr_of(area));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_end(lh_addr_of(canvas));

    EXPECT_EQ(log.area_count, 0);
    ASSERT_EQ(log.fill_count, 1);
    EXPECT_TRUE(lh_test::rect_is(log.fills[0], lh_test::rect_of(10, 75, 20, 5)));
}

TEST(ui_canvas, damage_of_an_area_frame_stays_in_target_space)
{
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_rect_t area;
    lh_ui_rect_t rect;

    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, true);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(area), 0, 64, 160, 32);
    lh_ui_rect_init(lh_addr_of(rect), 10, 75, 20, 5);

    lh_ui_canvas_begin_area(lh_addr_of(canvas), lh_addr_of(area));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_end(lh_addr_of(canvas));

    /* The buffer moved, the damage did not: a caller invalidates a window
       rectangle, not a strip offset. */
    ASSERT_NE(lh_ui_canvas_get_damage(lh_addr_of(canvas)), nullptr);
    EXPECT_TRUE(lh_test::rect_is(*lh_ui_canvas_get_damage(lh_addr_of(canvas)), lh_test::rect_of(10, 75, 20, 5)));
}

TEST(canvas, damage_in_an_area_is_the_damage_clipped_to_it)
{
    const lh_ui_rect_t area = lh_test::rect_of(0, 64, 160, 32);
    const lh_ui_rect_t cut = lh_test::rect_of(20, 70, 40, 10);
    const lh_ui_rect_t touching = lh_test::rect_of(150, 90, 40, 10);
    const lh_ui_rect_t below = lh_test::rect_of(0, 200, 160, 10);
    /* One row short of the area: touching is dirty, one row lower is not. */
    const lh_ui_rect_t under = lh_test::rect_of(0, 97, 160, 3);
    const lh_ui_rect_t all = lh_ui_canvas_damage_in(lh_addr_of(area), nullptr);
    const lh_ui_rect_t part = lh_ui_canvas_damage_in(lh_addr_of(area), lh_addr_of(cut));
    const lh_ui_rect_t edge = lh_ui_canvas_damage_in(lh_addr_of(area), lh_addr_of(touching));
    const lh_ui_rect_t none = lh_ui_canvas_damage_in(lh_addr_of(area), lh_addr_of(below));
    const lh_ui_rect_t short_of = lh_ui_canvas_damage_in(lh_addr_of(area), lh_addr_of(under));

    /* Nothing said means everything still holds, so the area answers itself. */
    EXPECT_TRUE(lh_test::rect_is(all, area));
    /* Partly over it: the part, which is what the frame may touch. */
    EXPECT_TRUE(lh_test::rect_is(part, lh_test::rect_of(20, 70, 40, 10)));
    /* Touching an edge is not empty: the row on it is still dirty. */
    EXPECT_TRUE(lh_test::rect_is(edge, lh_test::rect_of(150, 90, 10, 6)));
    /* And a strip the damage never reaches is empty, which is how a partial frame
       keeps the pixels already on screen. */
    EXPECT_TRUE(lh_ui_rect_is_empty(lh_addr_of(none)));
    EXPECT_TRUE(lh_ui_rect_is_empty(lh_addr_of(short_of)));
}

/* ── Offset and clip ─────────────────────────────────────────────────────── */

using lh_test::draw_log;
using lh_test::rect_is;
using lh_test::rect_of;

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;
    lh_ui_point_init(lh_addr_of(point), x, y);
    return point;
}

TEST(ui_canvas, push_moves_every_primitive_and_pop_restores)
{
    draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_rect_t rect = rect_of(1, 1, 2, 2);

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), true, false);

    lh_ui_canvas_push(lh_addr_of(canvas), point_of(5, 7), nullptr);
    lh_ui_canvas_push(lh_addr_of(canvas), point_of(1, 1), nullptr);
    const lh_ui_point_t offset = lh_ui_canvas_get_offset(lh_addr_of(canvas));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(offset)), lh_ui_scalar(6));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(offset)), lh_ui_scalar(8));
    EXPECT_TRUE(lh_null_eq(lh_ui_canvas_get_clip(lh_addr_of(canvas))));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_ui_scalar(1), lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(color));

    ASSERT_EQ(log.fill_count, 2);
    ASSERT_EQ(log.round_count, 1);
    EXPECT_TRUE(rect_is(log.fills[0], rect_of(7, 9, 2, 2)));
    EXPECT_TRUE(rect_is(log.round_fills[0], rect_of(7, 9, 2, 2)));
    EXPECT_TRUE(rect_is(log.fills[1], rect));
}

TEST(ui_canvas, push_clip_is_moved_by_the_old_offset_and_intersected)
{
    draw_log log;
    lh_ui_canvas_t canvas;
    const lh_ui_rect_t outer = rect_of(0, 0, 20, 20);
    const lh_ui_rect_t inner = rect_of(5, 5, 30, 30);

    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, false);

    lh_ui_canvas_push(lh_addr_of(canvas), point_of(10, 0), lh_addr_of(outer));
    ASSERT_TRUE(lh_null_ne(lh_ui_canvas_get_clip(lh_addr_of(canvas))));
    EXPECT_TRUE(rect_is(*lh_ui_canvas_get_clip(lh_addr_of(canvas)), outer));

    /* inner lands at (15, 5) after the offset 10 pushed before it. */
    lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), lh_addr_of(inner));
    EXPECT_TRUE(rect_is(*lh_ui_canvas_get_clip(lh_addr_of(canvas)), rect_of(15, 5, 5, 15)));

    lh_ui_canvas_pop(lh_addr_of(canvas));
    EXPECT_TRUE(rect_is(*lh_ui_canvas_get_clip(lh_addr_of(canvas)), outer));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    EXPECT_TRUE(lh_null_eq(lh_ui_canvas_get_clip(lh_addr_of(canvas))));
}

TEST(ui_canvas, without_set_clip_the_canvas_cuts_fill_rect)
{
    draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_rect_t clip = rect_of(0, 0, 10, 10);
    const lh_ui_rect_t half_out = rect_of(5, 5, 10, 10);
    const lh_ui_rect_t all_out = rect_of(20, 20, 4, 4);

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, false);

    lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), lh_addr_of(clip));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(half_out), lh_addr_of(color));
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(all_out), lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));

    ASSERT_EQ(log.fill_count, 1);
    EXPECT_TRUE(rect_is(log.fills[0], rect_of(5, 5, 5, 5)));
}

TEST(ui_canvas, set_clip_slot_gets_each_change_and_primitives_arrive_uncut)
{
    draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_rect_t clip = rect_of(0, 0, 10, 10);
    const lh_ui_rect_t half_out = rect_of(5, 5, 10, 10);

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), true, true);

    lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), lh_addr_of(clip));
    EXPECT_EQ(log.clip_count, 1);
    EXPECT_TRUE(rect_is(log.clip, clip));

    /* A push without a clip rect keeps the clip: no new call. */
    lh_ui_canvas_push(lh_addr_of(canvas), point_of(3, 0), nullptr);
    EXPECT_EQ(log.clip_count, 1);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(half_out), lh_addr_of(color));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(half_out), lh_ui_scalar(2), lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));
    EXPECT_EQ(log.unclip_count, 0);

    lh_ui_canvas_pop(lh_addr_of(canvas));
    EXPECT_EQ(log.unclip_count, 1);

    ASSERT_EQ(log.fill_count, 1);
    ASSERT_EQ(log.round_count, 1);
    EXPECT_TRUE(rect_is(log.fills[0], rect_of(8, 5, 10, 10)));
    EXPECT_TRUE(rect_is(log.round_fills[0], rect_of(8, 5, 10, 10)));
}

TEST(ui_canvas, round_rect_inside_the_clip_still_reaches_the_slot)
{
    draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_rect_t clip = rect_of(0, 0, 20, 20);
    const lh_ui_rect_t inside = rect_of(2, 2, 10, 10);

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), true, false);

    lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), lh_addr_of(clip));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(inside), lh_ui_scalar(3), lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));

    EXPECT_EQ(log.round_count, 1);
    EXPECT_EQ(log.fill_count, 0);
}

TEST(ui_canvas, round_rect_across_the_clip_goes_through_the_cut_fallback)
{
    alpha_target target{};
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    /* Slot present but no set_clip: the canvas must cut, so it falls back. */
    const lh_ui_canvas_backend_t backend = {nullptr, nullptr, nullptr, nullptr, target_fill_rect,
                                            lh_test::draw_log_fill_round_rect, nullptr, nullptr};
    const lh_ui_rect_t clip = rect_of(0, 0, 8, 16);
    const lh_ui_rect_t rect = rect_of(2, 3, 11, 9);

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(backend), lh_addr_of(target));

    lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), lh_addr_of(clip));
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE, lh_addr_of(color));
    lh_ui_canvas_pop(lh_addr_of(canvas));

    const lh_ui_scalar_t radius = lh_ui_radius_clamp(lh_addr_of(rect), LH_UI_RADIUS_CIRCLE);
    for (int y = 0; y < 16; ++y)
    {
        for (int x = 0; x < 16; ++x)
        {
            const int cover = x < 8 ? lh_ui_radius_coverage(lh_addr_of(rect), radius, x, y) : 0;
            EXPECT_LE(target.writes[y][x], 1) << x << "," << y;
            EXPECT_EQ(target.alpha[y][x], cover) << x << "," << y;
        }
    }
}

int g_declined;

lh_bool_t
decline_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, const lh_ui_color_t *color)
{
    (void)context;
    (void)rect;
    (void)radius;
    (void)color;
    ++g_declined;
    return lh_bool_false;
}

lh_bool_t
decline_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask, const lh_ui_color_t *color)
{
    (void)context;
    (void)origin;
    (void)mask;
    (void)color;
    ++g_declined;
    return lh_bool_false;
}

/* Both GDI+-like slots present, both draw nothing this frame. */
const lh_ui_canvas_backend_t g_declining_backend = {nullptr, nullptr, nullptr, nullptr, target_fill_rect,
                                                      decline_round_rect, nullptr, decline_mask};

TEST(ui_canvas, round_rect_the_slot_declines_is_drawn_by_the_canvas)
{
    alpha_target target{};
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    const lh_ui_rect_t rect = rect_of(2, 3, 11, 9);
    const lh_ui_scalar_t radius = lh_ui_radius_clamp(lh_addr_of(rect), LH_UI_RADIUS_CIRCLE);

    g_declined = 0;
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_declining_backend), lh_addr_of(target));

    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), LH_UI_RADIUS_CIRCLE, lh_addr_of(color));

    EXPECT_EQ(g_declined, 1);
    for (int y = 0; y < 16; ++y)
    {
        for (int x = 0; x < 16; ++x)
        {
            EXPECT_LE(target.writes[y][x], 1) << x << "," << y;
            EXPECT_EQ(target.alpha[y][x], lh_ui_radius_coverage(lh_addr_of(rect), radius, x, y)) << x << "," << y;
        }
    }
    ASSERT_NE(lh_ui_canvas_get_damage(lh_addr_of(canvas)), nullptr);
}

TEST(ui_canvas, mask_the_slot_declines_is_painted_by_the_canvas)
{
    static const lh_byte_t bits[] = {0x00, 0xff, 0x80, 0x00};
    alpha_target target{};
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_mask_t mask;

    g_declined = 0;
    lh_ui_mask_init(lh_addr_of(mask), bits, 2, 2, 2, 8);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_declining_backend), lh_addr_of(target));

    lh_ui_canvas_fill_mask(lh_addr_of(canvas), lh_addr_of(mask), point_of(4, 5), lh_addr_of(color));

    EXPECT_EQ(g_declined, 1);
    EXPECT_EQ(target.writes[5][4], 0);
    EXPECT_EQ(target.alpha[5][5], 255);
    EXPECT_EQ(target.alpha[6][4], lh_ui_mask_get_coverage(lh_addr_of(mask), 0, 1));
    EXPECT_EQ(target.writes[6][5], 0);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(ui_canvas_death, push_past_the_depth)
{
    lh_ui_canvas_t canvas;

    lh_ui_canvas_init(lh_addr_of(canvas), nullptr, nullptr);
    for (int i = 0; i < LH_LIBRARY_OPTION_UI_CANVAS_DEPTH; ++i)
    {
        lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), nullptr);
    }
    LH_EXPECT_DEATH(lh_ui_canvas_push(lh_addr_of(canvas), point_of(0, 0), nullptr));
}

TEST(ui_canvas_death, pop_with_nothing_pushed)
{
    lh_ui_canvas_t canvas;

    lh_ui_canvas_init(lh_addr_of(canvas), nullptr, nullptr);
    LH_EXPECT_DEATH(lh_ui_canvas_pop(lh_addr_of(canvas)));
}

#endif

} // namespace
