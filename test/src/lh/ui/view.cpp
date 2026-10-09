#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/null.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/button.h>
#include <lh/ui/container.h>
#include <lh/ui/label.h>
#include <lh/ui/scrollbar.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

/* Test helper: a backend with `begin_area`, drawing each strip where it belongs on
 * the whole target.
 *
 * A partial test needs this twice: the strip lands on a real pixel buffer, so a
 * frame drawn strip by strip can be compared with the same frame drawn in one go,
 * pixel for pixel. It records every area it was given, and whether any primitive
 * ever left the buffer.
 *
 * The buffer here is the whole target, which is what a real partial backend avoids
 * by keeping only the strip. That does not change where the pixels go: the canvas
 * hands the backend buffer-space coordinates, so the probe puts them back at the
 * area corner and the picture is the same either way. */
struct partial_probe
{
    static const int width = 160;
    static const int height = 120;
    static const int capacity = 32;
    static const lh_u32_t sentinel = 0x00123456u; /* transparent, so blends over it see no dst */

    lh_u32_t words[width * height];
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t area; /* the strip being drawn, or empty outside a frame */
    lh_ui_rect_t areas[capacity];
    int area_count;
    int begin_count;
    int end_count;
    bool escaped; /* a primitive reached past the buffer of its own frame */
};

bool
partial_probe_is_inside(const partial_probe *probe, const lh_ui_rect_t *buffer)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(lh_addr_of(probe->area));
    lh_ui_rect_t window;
    lh_ui_rect_t part;

    lh_ui_rect_init(lh_addr_of(window), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_size_get_width(lh_addr_of(*size)),
                    lh_ui_size_get_height(lh_addr_of(*size)));
    part = lh_ui_rect_intersection(lh_addr_of(window), buffer);
    return lh_ui_rect_equals(lh_addr_of(part), buffer);
}

void
partial_probe_place(lh_ui_rect_t *placed, const lh_ui_rect_t *buffer, partial_probe *probe)
{
    const lh_ui_point_t *at = lh_ui_rect_get_origin_as_const(lh_addr_of(probe->area));

    if (!partial_probe_is_inside(probe, buffer))
    {
        probe->escaped = true;
    }
    *placed = lh_ui_rect_offset(buffer, lh_ui_point_get_x(at), lh_ui_point_get_y(at));
}

void
partial_probe_begin_area(lh_ptr context, const lh_ui_rect_t *area)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    if (probe->area_count < partial_probe::capacity)
    {
        probe->areas[probe->area_count] = *area;
    }
    ++probe->area_count;
    probe->area = *area;
}

void
partial_probe_begin(lh_ptr context)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    ++probe->begin_count;
    lh_ui_rect_init_empty(lh_addr_of(probe->area));
}

void
partial_probe_end(lh_ptr context, const lh_ui_rects_t *drawn)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    (void)drawn;
    ++probe->end_count;
}

lh_void
partial_probe_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    lh_ui_rect_t placed;

    partial_probe_place(lh_addr_of(placed), rect, probe);
    lh_ui_canvas_sw_fill_rect(lh_addr_of(probe->sw), lh_addr_of(placed), color);
}

lh_bool_t
partial_probe_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                              const lh_ui_color_t *color)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    lh_ui_rect_t placed;

    partial_probe_place(lh_addr_of(placed), rect, probe);
    return lh_ui_canvas_sw_fill_round_rect(lh_addr_of(probe->sw), lh_addr_of(placed), radius, color);
}

lh_void
partial_probe_clear(lh_ptr context, const lh_ui_color_t *color)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    partial_probe_fill_rect(context, lh_addr_of(probe->area), color);
}

/* The backend: `begin_area`, software drawing, no `set_clip` so the canvas cuts
 * every primitive to the strip itself and the probe only places it. */
const lh_ui_canvas_backend_t *
partial_probe_backend()
{
    static const lh_ui_canvas_backend_t backend = {
        partial_probe_begin,   partial_probe_begin_area, partial_probe_end,   partial_probe_clear,
        partial_probe_fill_rect, partial_probe_fill_round_rect, nullptr,     nullptr};
    return &backend;
}

void
partial_probe_init(partial_probe *probe)
{
    lh_ui_size_t size;

    *probe = partial_probe{};
    for (lh_u32_t &w : probe->words)
    {
        w = partial_probe::sentinel;
    }
    lh_ui_rect_init_empty(lh_addr_of(probe->area));
    lh_ui_pixmap_init(lh_addr_of(probe->pixmap), lh_ptr_rcast(lh_byte_t, probe->words), partial_probe::width,
                      partial_probe::height, partial_probe::width * 4, lh_ui_pixmap_format_argb8888);
    lh_ui_canvas_sw_init(lh_addr_of(probe->sw));
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(probe->sw), lh_addr_of(probe->pixmap));
    lh_ui_canvas_init(lh_addr_of(probe->canvas), partial_probe_backend(), probe);
    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(partial_probe::width), lh_ui_scalar(partial_probe::height));
    lh_ui_canvas_set_size(lh_addr_of(probe->canvas), size);
}

/** Pixels of @p a and @p b that differ over the whole target. */
int
partial_probe_diff(const partial_probe &a, const partial_probe &b)
{
    int different = 0;
    for (int i = 0; i < partial_probe::width * partial_probe::height; ++i)
    {
        if (a.words[i] != b.words[i])
        {
            ++different;
        }
    }
    return different;
}

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;
    lh_ui_point_init(lh_addr_of(point), x, y);
    return point;
}

int
scroll_y_of(const lh_ui_container_t &box)
{
    const lh_ui_point_t scroll = lh_ui_container_get_scroll(lh_addr_of(box));
    return static_cast<int>(lh_ui_point_get_y(lh_addr_of(scroll)));
}

/* root 200x200 > container (0,0) 100x100 over content 100x400, a horizontal
 * bar (0,100) 100x10 linked first, then a vertical bar (100,0) 10x200. */
struct view_fixture
{
    lh_ui_entity_t root;
    lh_ui_container_t box;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t across;
    lh_ui_scrollbar_t down;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;

    view_fixture()
    {
        lh_ui_entity_init(lh_addr_of(root), rect_of(0, 0, 200, 200));
        lh_ui_container_init(lh_addr_of(box), rect_of(0, 0, 100, 100));
        lh_ui_entity_init(lh_addr_of(content), rect_of(0, 0, 100, 400));
        lh_ui_scrollbar_init(lh_addr_of(across), rect_of(0, 100, 100, 10), lh_ui_axis_horizontal,
                                    lh_addr_of(box));
        lh_ui_scrollbar_set_mode(lh_addr_of(across), lh_ui_scrollbar_mode_always);
        lh_ui_scrollbar_init(lh_addr_of(down), rect_of(100, 0, 10, 200), lh_ui_axis_vertical,
                                    lh_addr_of(box));
        lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(content));
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_container_as_entity(lh_addr_of(box)));
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_scrollbar_as_entity(lh_addr_of(across)));
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_scrollbar_as_entity(lh_addr_of(down)));
        lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
        lh_ui_view_init(lh_addr_of(view));
        lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
        lh_ui_view_set_root(lh_addr_of(view), lh_addr_of(root));
    }

    const lh_ui_rect_t *
    damage() const
    {
        return lh_ui_canvas_get_damage(lh_addr_of(canvas));
    }
};
} // namespace

TEST(view, wheel_over_the_content_damages_the_vertical_track)
{
    view_fixture f;

    EXPECT_EQ(lh_ui_view_wheel(lh_addr_of(f.view), point_of(50, 50), lh_ui_scalar(0), lh_ui_scalar(40)), lh_bool_true);
    EXPECT_EQ(scroll_y_of(f.box), 40);
    ASSERT_NE(f.damage(), nullptr);
    EXPECT_TRUE(rect_is(*f.damage(), rect_of(0, 0, 110, 200)));
}

TEST(view, wheel_over_the_horizontal_bar_scrolls_its_container)
{
    view_fixture f;

    EXPECT_EQ(lh_ui_view_wheel(lh_addr_of(f.view), point_of(50, 105), lh_ui_scalar(0), lh_ui_scalar(40)), lh_bool_true);
    EXPECT_EQ(scroll_y_of(f.box), 40);
    ASSERT_NE(f.damage(), nullptr);
    EXPECT_TRUE(rect_is(*f.damage(), rect_of(0, 0, 110, 200)));
}

TEST(view, wheel_that_does_not_scroll_reports_no_change)
{
    view_fixture f;

    EXPECT_EQ(lh_ui_view_wheel(lh_addr_of(f.view), point_of(50, 50), lh_ui_scalar(0), lh_ui_scalar(-40)), lh_bool_false);
    EXPECT_EQ(lh_ui_view_wheel(lh_addr_of(f.view), point_of(150, 150), lh_ui_scalar(0), lh_ui_scalar(40)), lh_bool_false);
    EXPECT_EQ(f.damage(), nullptr);
}

/* Track 200, thumb 50 at 0: a click at y 150 pages down by the viewport. */
TEST(view, release_without_drag_pages_and_records_scroll_damage)
{
    view_fixture f;

    lh_ui_view_press(lh_addr_of(f.view), point_of(105, 150));
    EXPECT_EQ(lh_ui_view_release(lh_addr_of(f.view), point_of(105, 150)),
              lh_ui_scrollbar_as_entity(lh_addr_of(f.down)));
    EXPECT_EQ(scroll_y_of(f.box), 100);
    ASSERT_NE(f.damage(), nullptr);
    EXPECT_TRUE(rect_is(*f.damage(), rect_of(0, 0, 110, 200)));
}

TEST(view, drag_moves_the_thumb_and_does_not_click)
{
    view_fixture f;

    lh_ui_view_press(lh_addr_of(f.view), point_of(105, 10));
    EXPECT_EQ(lh_ui_view_move(lh_addr_of(f.view), point_of(105, 85)), lh_bool_true);
    EXPECT_EQ(scroll_y_of(f.box), 150);
    EXPECT_TRUE(lh_null_eq(lh_ui_view_release(lh_addr_of(f.view), point_of(105, 85))));
    EXPECT_EQ(scroll_y_of(f.box), 150);
}

/* A list (inner box + bar) inside an outer container scrolled by 50: the bar
 * sits 50 higher on screen, and the drag must grab it there. */
TEST(view, drag_finds_the_thumb_of_a_bar_inside_a_scrolled_container)
{
    lh_ui_entity_t root;
    lh_ui_container_t outer;
    lh_ui_entity_t filler;
    lh_ui_container_t inner;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t bar;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;

    lh_ui_entity_init(lh_addr_of(root), rect_of(0, 0, 300, 300));
    lh_ui_container_init(lh_addr_of(outer), rect_of(0, 0, 200, 100));
    lh_ui_entity_init(lh_addr_of(filler), rect_of(0, 0, 10, 400));
    lh_ui_container_init(lh_addr_of(inner), rect_of(0, 60, 100, 100));
    lh_ui_entity_init(lh_addr_of(content), rect_of(0, 60, 100, 400));
    lh_ui_scrollbar_init(lh_addr_of(bar), rect_of(100, 60, 10, 200), lh_ui_axis_vertical,
                                lh_addr_of(inner));
    lh_ui_entity_add_child(lh_addr_of(root), lh_ui_container_as_entity(lh_addr_of(outer)));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(outer)), lh_addr_of(filler));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(outer)),
                           lh_ui_container_as_entity(lh_addr_of(inner)));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(outer)),
                           lh_ui_scrollbar_as_entity(lh_addr_of(bar)));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(inner)), lh_addr_of(content));
    lh_ui_container_set_scroll(lh_addr_of(outer), point_of(0, 50));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_view_init(lh_addr_of(view));
    lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
    lh_ui_view_set_root(lh_addr_of(view), lh_addr_of(root));

    /* Thumb: track y 60..110 in outer content space, 10..60 on screen. */
    lh_ui_view_press(lh_addr_of(view), point_of(105, 20));
    EXPECT_EQ(view.grab, lh_addr_of(bar));
    EXPECT_EQ(lh_ui_view_move(lh_addr_of(view), point_of(105, 95)), lh_bool_true);
    /* Thumb start 75 of travel 150 → scroll 150 of max 300. */
    EXPECT_EQ(scroll_y_of(inner), 150);
    /* Damage is in the root space: inner (0,10) 100x100 ∪ bar track (100,10) 10x200. */
    ASSERT_NE(lh_ui_canvas_get_damage(lh_addr_of(canvas)), nullptr);
    EXPECT_TRUE(rect_is(*lh_ui_canvas_get_damage(lh_addr_of(canvas)), rect_of(0, 10, 110, 200)));
}

TEST(view, wheel_sideways_scrolls_a_wide_container_horizontally)
{
    view_fixture f;

    lh_ui_entity_set_rect(lh_addr_of(f.content), lh_test::rect_of(0, 0, 300, 400));
    EXPECT_EQ(lh_ui_view_wheel(lh_addr_of(f.view), point_of(50, 50), lh_ui_scalar(50), lh_ui_scalar(0)),
              lh_bool_true);
    const lh_ui_point_t scroll = lh_ui_container_get_scroll(lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(scroll)), lh_ui_scalar(50));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(scroll)), lh_ui_scalar(0));
}

/* ── Partial frames ───────────────────────────────────────────────────────── */

/* A scene with everything that can be wrong in a strip: a rounded panel, a
   scrolling box with a round thumb, and text (masks). Its own styles are a
   child struct, so one scene can be drawn twice into two pixel buffers. */
struct scene
{
    lh_ui_style_t panel_style;
    lh_ui_style_t box_style;
    lh_ui_style_t thumb_style;
    lh_ui_style_t label_style;
    lh_ui_paint_t panel_paint;
    lh_ui_paint_t box_paint;
    lh_ui_paint_t thumb_paint;
    lh_ui_color_t panel_color;
    lh_ui_color_t box_color;
    lh_ui_color_t thumb_color;
    lh_ui_entity_t panel;
    lh_ui_container_t box;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t bar;
    lh_ui_label_t label;
};

void
build_scene(scene &s)
{
    lh_ui_color_init(lh_addr_of(s.panel_color), 30, 60, 90, 255);
    lh_ui_color_init(lh_addr_of(s.box_color), 200, 120, 40, 255);
    lh_ui_color_init(lh_addr_of(s.thumb_color), 250, 240, 200, 255);
    lh_ui_paint_init_color(lh_addr_of(s.panel_paint), lh_addr_of(s.panel_color));
    lh_ui_paint_init_color(lh_addr_of(s.box_paint), lh_addr_of(s.box_color));
    lh_ui_paint_init_color(lh_addr_of(s.thumb_paint), lh_addr_of(s.thumb_color));
    lh_ui_style_init(lh_addr_of(s.panel_style));
    lh_ui_style_set_fill(lh_addr_of(s.panel_style), lh_addr_of(s.panel_paint));
    lh_ui_style_set_radius(lh_addr_of(s.panel_style), lh_ui_scalar(12));
    lh_ui_style_init(lh_addr_of(s.box_style));
    lh_ui_style_set_fill(lh_addr_of(s.box_style), lh_addr_of(s.box_paint));
    lh_ui_style_set_radius(lh_addr_of(s.box_style), lh_ui_scalar(6));
    lh_ui_style_init(lh_addr_of(s.thumb_style));
    lh_ui_style_set_fill(lh_addr_of(s.thumb_style), lh_addr_of(s.thumb_paint));
    lh_ui_style_set_radius(lh_addr_of(s.thumb_style), LH_UI_RADIUS_CIRCLE);
    lh_ui_style_init(lh_addr_of(s.label_style));
    lh_ui_style_set_padding(lh_addr_of(s.label_style), lh_ui_scalar(2));

    lh_ui_entity_init(lh_addr_of(s.panel), lh_test::rect_of(0, 0, 160, 120));
    lh_ui_entity_set_style(lh_addr_of(s.panel), lh_addr_of(s.panel_style));
    lh_ui_container_init(lh_addr_of(s.box), lh_test::rect_of(10, 10, 100, 60));
    lh_ui_entity_set_style(lh_ui_container_as_entity(lh_addr_of(s.box)), lh_addr_of(s.box_style));
    lh_ui_entity_init(lh_addr_of(s.content), lh_test::rect_of(10, 10, 100, 300));
    lh_ui_scrollbar_init(lh_addr_of(s.bar), lh_test::rect_of(112, 10, 8, 60), lh_ui_axis_vertical,
                                lh_addr_of(s.box));
    lh_ui_entity_set_style(lh_ui_scrollbar_as_entity(lh_addr_of(s.bar)), lh_addr_of(s.thumb_style));
    lh_ui_scrollbar_set_mode(lh_addr_of(s.bar), lh_ui_scrollbar_mode_always);
    lh_ui_scrollbar_set_thumb_style(lh_addr_of(s.bar), lh_addr_of(s.thumb_style));
    lh_ui_label_init(lh_addr_of(s.label), lh_test::rect_of(20, 20, 80, 40), "Paladin");
    lh_ui_entity_set_style(lh_ui_label_as_entity(lh_addr_of(s.label)), lh_addr_of(s.label_style));

    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(s.box)), lh_addr_of(s.content));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(s.box)),
                           lh_ui_label_as_entity(lh_addr_of(s.label)));
    lh_ui_entity_add_child(lh_addr_of(s.panel), lh_ui_container_as_entity(lh_addr_of(s.box)));
    lh_ui_entity_add_child(lh_addr_of(s.panel), lh_ui_scrollbar_as_entity(lh_addr_of(s.bar)));
}

TEST(view, strips_draw_the_same_picture_as_one_whole_frame)
{
    scene s;
    partial_probe whole;
    partial_probe strips;
    lh_ui_view_t one;
    lh_ui_view_t many;

    build_scene(s);
    partial_probe_init(lh_addr_of(whole));
    partial_probe_init(lh_addr_of(strips));
    lh_ui_view_init(lh_addr_of(one));
    lh_ui_view_init(lh_addr_of(many));
    lh_ui_view_set_canvas(lh_addr_of(one), lh_addr_of(whole.canvas));
    lh_ui_view_set_canvas(lh_addr_of(many), lh_addr_of(strips.canvas));
    lh_ui_view_set_root(lh_addr_of(one), lh_addr_of(s.panel));
    lh_ui_view_set_root(lh_addr_of(many), lh_addr_of(s.panel));
    lh_ui_view_set_strip_height(lh_addr_of(many), lh_ui_scalar(32));

    lh_ui_view_paint(lh_addr_of(one));
    lh_ui_view_paint(lh_addr_of(many));

    EXPECT_EQ(lh_ui_view_is_stripped(lh_addr_of(one)), lh_bool_false);
    EXPECT_EQ(lh_ui_view_is_stripped(lh_addr_of(many)), lh_bool_true);
    EXPECT_EQ(whole.begin_count, 1);
    EXPECT_EQ(whole.area_count, 0);
    /* 120 rows in 32: 32, 32, 32, 24, and the last one pulled up to 32 tall so
       the backend keeps one buffer. */
    EXPECT_EQ(strips.area_count, 4);
    EXPECT_EQ(strips.begin_count, 0);
    EXPECT_EQ(strips.end_count, 4);
    EXPECT_FALSE(strips.escaped) << "a primitive left the strip it was drawn for";
    EXPECT_EQ(partial_probe_diff(whole, strips), 0);
}

TEST(view, strips_greater_than_the_target_draw_it_as_one)
{
    scene s;
    partial_probe whole;
    partial_probe strips;
    lh_ui_view_t one;
    lh_ui_view_t many;

    build_scene(s);
    partial_probe_init(lh_addr_of(whole));
    partial_probe_init(lh_addr_of(strips));
    lh_ui_view_init(lh_addr_of(one));
    lh_ui_view_init(lh_addr_of(many));
    lh_ui_view_set_canvas(lh_addr_of(one), lh_addr_of(whole.canvas));
    lh_ui_view_set_canvas(lh_addr_of(many), lh_addr_of(strips.canvas));
    lh_ui_view_set_root(lh_addr_of(one), lh_addr_of(s.panel));
    lh_ui_view_set_root(lh_addr_of(many), lh_addr_of(s.panel));
    lh_ui_view_set_strip_height(lh_addr_of(many), lh_ui_scalar(500));

    lh_ui_view_paint(lh_addr_of(one));
    lh_ui_view_paint(lh_addr_of(many));

    EXPECT_EQ(strips.area_count, 1);
    EXPECT_TRUE(rect_is(strips.areas[0], lh_test::rect_of(0, 0, 160, 120)));
    EXPECT_EQ(partial_probe_diff(whole, strips), 0);
}

TEST(view, damage_skips_the_strips_it_does_not_reach)
{
    scene s;
    partial_probe one;
    partial_probe strips;
    lh_ui_view_t whole_view;
    lh_ui_view_t strip_view;
    const lh_ui_rect_t damage = lh_test::rect_of(0, 64, 160, 56);

    build_scene(s);
    partial_probe_init(lh_addr_of(one));
    partial_probe_init(lh_addr_of(strips));
    lh_ui_view_init(lh_addr_of(whole_view));
    lh_ui_view_init(lh_addr_of(strip_view));
    lh_ui_view_set_canvas(lh_addr_of(whole_view), lh_addr_of(one.canvas));
    lh_ui_view_set_canvas(lh_addr_of(strip_view), lh_addr_of(strips.canvas));
    lh_ui_view_set_root(lh_addr_of(whole_view), lh_addr_of(s.panel));
    lh_ui_view_set_root(lh_addr_of(strip_view), lh_addr_of(s.panel));
    lh_ui_view_set_strip_height(lh_addr_of(strip_view), lh_ui_scalar(32));

    /* The lower half of the panel, the way a scroll hands it over. */
    lh_ui_view_draw(lh_addr_of(whole_view), lh_addr_of(damage));
    lh_ui_view_draw(lh_addr_of(strip_view), lh_addr_of(damage));

    /* Strips at y 0 and 32 hold nothing of it, so only the last two run. */
    EXPECT_EQ(one.area_count, 0);
    EXPECT_EQ(strips.area_count, 2);
    EXPECT_FALSE(strips.escaped);
    EXPECT_EQ(partial_probe_diff(one, strips), 0);
}

TEST(view, a_whole_area_makes_the_strip_that_reaches_it_wide_and_the_ones_inside_it_are_not_repeated)
{
    scene s;
    partial_probe whole;
    partial_probe strips;
    lh_ui_view_t one;
    lh_ui_view_t many;
    /* Rows 20..80: crosses two strip lines, so no strip holds all of it. */
    const lh_ui_rect_t region = lh_test::rect_of(0, 20, 160, 60);

    build_scene(s);
    partial_probe_init(lh_addr_of(whole));
    partial_probe_init(lh_addr_of(strips));
    lh_ui_view_init(lh_addr_of(one));
    lh_ui_view_init(lh_addr_of(many));
    lh_ui_view_set_canvas(lh_addr_of(one), lh_addr_of(whole.canvas));
    lh_ui_view_set_canvas(lh_addr_of(many), lh_addr_of(strips.canvas));
    lh_ui_view_set_root(lh_addr_of(one), lh_addr_of(s.panel));
    lh_ui_view_set_root(lh_addr_of(many), lh_addr_of(s.panel));
    lh_ui_view_set_strip_height(lh_addr_of(many), lh_ui_scalar(32));
    EXPECT_TRUE(lh_ui_view_add_whole_area(lh_addr_of(many), lh_addr_of(region)));

    lh_ui_view_paint(lh_addr_of(one));
    lh_ui_view_paint(lh_addr_of(many));

    /* The first strip reaches into the region and is drawn 80 rows tall; the
       strip at y 32 is inside that area, so it is neither buffered nor
       presented again; the strip at y 64 has grown past the region, and the one
       at y 88 reaches nothing. */
    EXPECT_EQ(strips.area_count, 3);
    EXPECT_TRUE(rect_is(strips.areas[0], lh_test::rect_of(0, 0, 160, 80)));
    EXPECT_TRUE(rect_is(strips.areas[1], lh_test::rect_of(0, 20, 160, 76)));
    EXPECT_TRUE(rect_is(strips.areas[2], lh_test::rect_of(0, 88, 160, 32)));
    EXPECT_FALSE(strips.escaped) << "a primitive left the area it was drawn for";
    /* The picture is the whole frame's, overlap and all: a grown area draws the
       rows it shares with the last one again, and they come out the same. */
    EXPECT_EQ(partial_probe_diff(whole, strips), 0);
}

TEST(view, a_whole_area_the_view_has_no_room_for_is_refused)
{
    lh_ui_view_t view;
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;
    lh_ui_rect_t at;

    lh_ui_size_init(lh_addr_of(size), 160, 120);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_view_init(lh_addr_of(view));
    lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));

    /* An empty region is not a region. */
    lh_ui_rect_init(lh_addr_of(at), 0, 0, 0, 40);
    EXPECT_FALSE(lh_ui_view_add_whole_area(lh_addr_of(view), lh_addr_of(at)));
    for (int i = 0; i < LH_UI_VIEW_WHOLE_MAX; ++i)
    {
        lh_ui_rect_init(lh_addr_of(at), 0, i * 4, 10, 4);
        EXPECT_TRUE(lh_ui_view_add_whole_area(lh_addr_of(view), lh_addr_of(at))) << "region " << i;
    }
    lh_ui_rect_init(lh_addr_of(at), 0, 100, 10, 4);
    EXPECT_FALSE(lh_ui_view_add_whole_area(lh_addr_of(view), lh_addr_of(at)));

    /* Forgetting them gives the room back. */
    lh_ui_view_clear_whole_areas(lh_addr_of(view));
    EXPECT_TRUE(lh_ui_view_add_whole_area(lh_addr_of(view), lh_addr_of(at)));

    /* A region that leaves the target is cut to it, not refused: the picture is
       what the target can hold, and an effect that needs the rest is told it
       cannot be drawn. */
    lh_ui_rect_init(lh_addr_of(at), 0, 100, 160, 40);
    EXPECT_TRUE(lh_ui_view_add_whole_area(lh_addr_of(view), lh_addr_of(at)));
    const lh_ui_rect_t last = lh_test::rect_of(0, 96, 160, 24);
    EXPECT_TRUE(rect_is(lh_ui_view_area_whole(lh_addr_of(view), lh_addr_of(last), lh_addr_of(last)),
                        lh_test::rect_of(0, 96, 160, 24)));
}

/* ── Pressing ──────────────────────────────────────────────────────────────── */

/* The panel of the scene above, given a pressed look: the same card in another
   colour with a shadow under it, so a press has something to say. */
struct pressed_panel
{
    lh_ui_style_t style;
    lh_ui_style_t hot;
    lh_ui_shadow_t shadow;
};

void
build_pressed_panel(pressed_panel &p)
{
    lh_ui_paint_t paint;
    lh_ui_color_t normal;
    lh_ui_color_t hot;

    lh_ui_style_init(lh_addr_of(p.style));
    lh_ui_color_init(lh_addr_of(normal), 30, 60, 90, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(normal));
    lh_ui_style_set_fill(lh_addr_of(p.style), lh_addr_of(paint));
    lh_ui_shadow_init(lh_addr_of(p.shadow));
    lh_ui_shadow_set_color(lh_addr_of(p.shadow), lh_ui_color_t{0, 0, 0, 200});
    lh_ui_shadow_set_spread(lh_addr_of(p.shadow), lh_ui_scalar(6));
    lh_ui_shadow_set_offset(lh_addr_of(p.shadow), lh_ui_scalar(0), lh_ui_scalar(3));
    lh_ui_style_set_shadow(lh_addr_of(p.style), lh_addr_of(p.shadow));
    lh_ui_style_init(lh_addr_of(p.hot));
    lh_ui_color_init(lh_addr_of(hot), 200, 60, 90, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(hot));
    lh_ui_style_set_fill(lh_addr_of(p.hot), lh_addr_of(paint));
    lh_ui_shadow_set_spread(lh_addr_of(p.shadow), lh_ui_scalar(6));
    lh_ui_style_set_shadow(lh_addr_of(p.hot), lh_addr_of(p.shadow));
    lh_ui_style_set_pressed(lh_addr_of(p.style), lh_addr_of(p.hot));
}

/* A press is a look and nothing else: press it, let go, and the frame has to come
   back pixel for pixel. A flag that is not cleared, or a pressed style that
   outlasts the press, leaves a difference here that no value assertion would
   have caught. */
TEST(view, a_press_and_a_release_leave_the_picture_as_it_was)
{
    scene s;
    partial_probe before;
    partial_probe after;
    pressed_panel p;
    lh_ui_view_t plain;
    lh_ui_view_t pressed;
    /* Where only the panel is: its box ends at x 110 and its label at y 60. */
    const lh_ui_point_t on_panel = point_of(140, 100);

    build_scene(s);
    build_pressed_panel(p);
    lh_ui_entity_set_style(lh_addr_of(s.panel), lh_addr_of(p.style));
    partial_probe_init(lh_addr_of(before));
    partial_probe_init(lh_addr_of(after));
    lh_ui_view_init(lh_addr_of(plain));
    lh_ui_view_init(lh_addr_of(pressed));
    lh_ui_view_set_canvas(lh_addr_of(plain), lh_addr_of(before.canvas));
    lh_ui_view_set_canvas(lh_addr_of(pressed), lh_addr_of(after.canvas));
    lh_ui_view_set_root(lh_addr_of(plain), lh_addr_of(s.panel));
    lh_ui_view_set_root(lh_addr_of(pressed), lh_addr_of(s.panel));

    /* One frame of a scene nobody has touched. */
    lh_ui_view_paint(lh_addr_of(plain));
    ASSERT_EQ(lh_ui_view_hit_test(lh_addr_of(pressed), on_panel), lh_addr_of(s.panel));

    /* Down on the panel, a look of its own, a release, and a repaint. */
    lh_ui_view_press(lh_addr_of(pressed), on_panel);
    EXPECT_TRUE(lh_ui_entity_is_pressed(lh_addr_of(s.panel)));
    lh_ui_view_paint(lh_addr_of(pressed));
    lh_ui_view_release(lh_addr_of(pressed), on_panel);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_addr_of(s.panel)));
    lh_ui_view_paint(lh_addr_of(pressed));

    EXPECT_EQ(partial_probe_diff(before, after), 0);
}

/* The damage of a press is what both looks painted, and a shadow reaches past
   its own box: less than that and the frame keeps the fringe of the look that is
   gone. */
TEST(view, a_press_damages_what_both_looks_painted)
{
    scene s;
    pressed_panel p;
    lh_ui_view_t view;
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;
    const lh_ui_rect_t *damage;

    build_scene(s);
    build_pressed_panel(p);
    lh_ui_entity_set_style(lh_addr_of(s.panel), lh_addr_of(p.style));
    lh_ui_size_init(lh_addr_of(size), 160, 120);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_view_init(lh_addr_of(view));
    lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
    lh_ui_view_set_root(lh_addr_of(view), lh_addr_of(s.panel));

    lh_ui_view_press(lh_addr_of(view), point_of(140, 100));
    damage = lh_ui_canvas_get_damage(lh_addr_of(canvas));
    ASSERT_NE(damage, nullptr);
    /* The panel is (0,0) 160x120 and its shadow reaches 9 rows past it: 6 of
       spread and 3 of shift. */
    EXPECT_TRUE(rect_is(*damage, lh_test::rect_of(-9, -9, 178, 138)));
}

/* One press at a time. A second pointer down while the first is still open lets
   go of it rather than leaving a pressed look under nothing — the one way a
   pressed look could stick. */
TEST(view, a_press_still_open_when_the_next_one_starts_is_let_go)
{
    scene s;
    pressed_panel p;
    lh_ui_view_t view;
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;

    build_scene(s);
    build_pressed_panel(p);
    lh_ui_entity_set_style(lh_addr_of(s.panel), lh_addr_of(p.style));
    lh_ui_size_init(lh_addr_of(size), 160, 120);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_view_init(lh_addr_of(view));
    lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
    lh_ui_view_set_root(lh_addr_of(view), lh_addr_of(s.panel));

    lh_ui_view_press(lh_addr_of(view), point_of(140, 100));
    EXPECT_TRUE(lh_ui_entity_is_pressed(lh_addr_of(s.panel)));
    /* No release in between, and the second press lands elsewhere: the app went
       away with the pointer still down on the panel. */
    const lh_ui_point_t on_box = point_of(80, 60);
    lh_ui_entity_t *second = lh_ui_view_hit_test(lh_addr_of(view), on_box);
    ASSERT_NE(second, nullptr);
    ASSERT_NE(second, lh_addr_of(s.panel));
    lh_ui_view_press(lh_addr_of(view), on_box);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_addr_of(s.panel))) << "the first press stuck";
    EXPECT_TRUE(lh_ui_entity_is_pressed(second)) << "the second press did not take";
}

/* Hover is the same rule as a press over a different pair of looks: the button
   is a plain entity with two styles, and changing what the pointer is on has to
   damage both of them — the one leaving casts its shadow past its own box too. */
TEST(view, a_hover_damages_what_both_looks_painted)
{
    lh_ui_view_t view;
    lh_ui_canvas_t canvas;
    lh_ui_button_t button;
    lh_ui_style_t rest;
    lh_ui_style_t hot;
    lh_ui_shadow_t small;
    lh_ui_shadow_t wide;
    lh_ui_paint_t paint;
    lh_ui_color_t color;
    lh_ui_size_t size;
    const lh_ui_rect_t *damage;

    lh_ui_shadow_init(lh_addr_of(small));
    lh_ui_shadow_set_color(lh_addr_of(small), lh_ui_color_t{0, 0, 0, 200});
    lh_ui_shadow_set_spread(lh_addr_of(small), lh_ui_scalar(6));
    lh_ui_shadow_set_offset(lh_addr_of(small), lh_ui_scalar(0), lh_ui_scalar(3));
    lh_ui_shadow_init(lh_addr_of(wide));
    lh_ui_shadow_set_color(lh_addr_of(wide), lh_ui_color_t{0, 0, 0, 200});
    lh_ui_shadow_set_spread(lh_addr_of(wide), lh_ui_scalar(12));

    lh_ui_style_init(lh_addr_of(rest));
    lh_ui_color_init(&color, 30, 60, 90, 255);
    lh_ui_paint_init_color(&paint, &color);
    lh_ui_style_set_fill(lh_addr_of(rest), &paint);
    lh_ui_style_set_shadow(lh_addr_of(rest), lh_addr_of(small));
    lh_ui_style_init(lh_addr_of(hot));
    lh_ui_style_set_shadow(lh_addr_of(hot), lh_addr_of(wide));
    lh_ui_style_set_hit_radius(lh_addr_of(rest), LH_UI_RADIUS_CIRCLE);

    lh_ui_button_init(lh_addr_of(button), lh_test::rect_of(20, 10, 40, 24));
    lh_ui_button_set_style(lh_addr_of(button), lh_addr_of(rest));
    lh_ui_button_set_hot_style(lh_addr_of(button), lh_addr_of(hot));

    lh_ui_size_init(lh_addr_of(size), 160, 120);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_view_init(lh_addr_of(view));
    lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
    lh_ui_view_set_root(lh_addr_of(view), lh_ui_button_as_entity(lh_addr_of(button)));
    /* Nothing drawn and nothing damaged yet, so what the hover adds is what is
       left to read: a test that clears first would be measuring the clear. */
    lh_ui_canvas_reset_damage(lh_addr_of(canvas));

    lh_ui_view_set_hot(lh_addr_of(view), lh_addr_of(button), lh_bool_true);
    damage = lh_ui_canvas_get_damage(lh_addr_of(canvas));
    ASSERT_NE(damage, nullptr);
    EXPECT_TRUE(lh_ui_button_get_hot(lh_addr_of(button)));
    /* The button is (20,10) 40x24 and the hot shadow reaches 12 past it, the
       resting one only 9: the damage is the larger, or the fringe of the look
       being left stays on the surface. */
    EXPECT_TRUE(rect_is(*damage, lh_test::rect_of(8, -2, 64, 48)));
}

TEST(view, a_strip_height_is_the_buffer_and_not_the_step)
{
    lh_ui_view_t view;
    lh_ui_canvas_t canvas;
    lh_ui_size_t size;
    lh_ui_rect_t strip;

    lh_ui_size_init(lh_addr_of(size), 160, 120);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_view_init(lh_addr_of(view));
    lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
    lh_ui_view_set_strip_height(lh_addr_of(view), lh_ui_scalar(32));

    /* Every strip is the same height, the last pulled up: the backend sizes its
       buffer once instead of once per strip. */
    strip = lh_ui_view_get_strip(lh_addr_of(view), 0);
    EXPECT_TRUE(rect_is(strip, lh_test::rect_of(0, 0, 160, 32)));
    strip = lh_ui_view_get_strip(lh_addr_of(view), 1);
    EXPECT_TRUE(rect_is(strip, lh_test::rect_of(0, 32, 160, 32)));
    strip = lh_ui_view_get_strip(lh_addr_of(view), 2);
    EXPECT_TRUE(rect_is(strip, lh_test::rect_of(0, 64, 160, 32)));
    strip = lh_ui_view_get_strip(lh_addr_of(view), 3);
    EXPECT_TRUE(rect_is(strip, lh_test::rect_of(0, 88, 160, 32)));
    strip = lh_ui_view_get_strip(lh_addr_of(view), 4);
    EXPECT_TRUE(lh_ui_rect_is_empty(lh_addr_of(strip)));
    lh_ui_view_set_strip_height(lh_addr_of(view), lh_ui_scalar(0));
    strip = lh_ui_view_get_strip(lh_addr_of(view), 0);
    EXPECT_TRUE(lh_ui_rect_is_empty(lh_addr_of(strip)));
}

/* root 200x200 with a button (10,10) 120x30 and its caption at (20,20) 60x10. */
struct press_fixture
{
    lh_ui_entity_t root;
    lh_ui_button_t button;
    lh_ui_label_t caption;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;

    press_fixture()
    {
        lh_ui_entity_init(lh_addr_of(root), rect_of(0, 0, 200, 200));
        lh_ui_button_init(lh_addr_of(button), rect_of(10, 10, 120, 30));
        lh_ui_label_init(lh_addr_of(caption), rect_of(20, 20, 60, 10), "Hi");
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_button_as_entity(lh_addr_of(button)));
        lh_ui_entity_add_child(lh_ui_container_as_entity(lh_ui_button_as_container(lh_addr_of(button))),
                               lh_ui_label_as_entity(lh_addr_of(caption)));
        lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
        lh_ui_view_init(lh_addr_of(view));
        lh_ui_view_set_canvas(lh_addr_of(view), lh_addr_of(canvas));
        lh_ui_view_set_root(lh_addr_of(view), lh_addr_of(root));
    }
};

/* The pressed look belongs to the button, so a press over its own caption has to
   press the button: otherwise a captioned button looks like nothing is happening
   while the pointer is down on it. */
TEST(view, a_press_on_a_caption_presses_the_button_it_is_in)
{
    press_fixture f;

    lh_ui_view_press(lh_addr_of(f.view), point_of(30, 24));

    EXPECT_EQ(lh_ui_entity_is_pressed(lh_ui_button_as_entity(lh_addr_of(f.button))), lh_bool_true)
        << "the caption took the press away from the button";
    EXPECT_EQ(lh_ui_entity_is_pressed(lh_ui_label_as_entity(lh_addr_of(f.caption))), lh_bool_false);

    lh_ui_view_release(lh_addr_of(f.view), point_of(30, 24));
    EXPECT_EQ(lh_ui_entity_is_pressed(lh_ui_button_as_entity(lh_addr_of(f.button))), lh_bool_false);
}