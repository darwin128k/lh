#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/partial_probe.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/label.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/view.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;
    lh_ui_point_init(lh_addr_of(point), x, y);
    return point;
}

int
scroll_y_of(const lh_ui_entity_container_t &box)
{
    const lh_ui_point_t scroll = lh_ui_entity_container_get_scroll(lh_addr_of(box));
    return static_cast<int>(lh_ui_point_get_y(lh_addr_of(scroll)));
}

/* root 200x200 > container (0,0) 100x100 over content 100x400, a horizontal
 * bar (0,100) 100x10 linked first, then a vertical bar (100,0) 10x200. */
struct view_fixture
{
    lh_ui_entity_t root;
    lh_ui_entity_container_t box;
    lh_ui_entity_t content;
    lh_ui_entity_scrollbar_t across;
    lh_ui_entity_scrollbar_t down;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;

    view_fixture()
    {
        lh_ui_entity_init(lh_addr_of(root), rect_of(0, 0, 200, 200));
        lh_ui_entity_container_init(lh_addr_of(box), rect_of(0, 0, 100, 100));
        lh_ui_entity_init(lh_addr_of(content), rect_of(0, 0, 100, 400));
        lh_ui_entity_scrollbar_init(lh_addr_of(across), rect_of(0, 100, 100, 10), lh_ui_axis_horizontal,
                                    lh_addr_of(box));
        lh_ui_entity_scrollbar_set_mode(lh_addr_of(across), lh_ui_entity_scrollbar_mode_always);
        lh_ui_entity_scrollbar_init(lh_addr_of(down), rect_of(100, 0, 10, 200), lh_ui_axis_vertical,
                                    lh_addr_of(box));
        lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(content));
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_container_as_entity(lh_addr_of(box)));
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_scrollbar_as_entity(lh_addr_of(across)));
        lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_scrollbar_as_entity(lh_addr_of(down)));
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
              lh_ui_entity_scrollbar_as_entity(lh_addr_of(f.down)));
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
    lh_ui_entity_container_t outer;
    lh_ui_entity_t filler;
    lh_ui_entity_container_t inner;
    lh_ui_entity_t content;
    lh_ui_entity_scrollbar_t bar;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;

    lh_ui_entity_init(lh_addr_of(root), rect_of(0, 0, 300, 300));
    lh_ui_entity_container_init(lh_addr_of(outer), rect_of(0, 0, 200, 100));
    lh_ui_entity_init(lh_addr_of(filler), rect_of(0, 0, 10, 400));
    lh_ui_entity_container_init(lh_addr_of(inner), rect_of(0, 60, 100, 100));
    lh_ui_entity_init(lh_addr_of(content), rect_of(0, 60, 100, 400));
    lh_ui_entity_scrollbar_init(lh_addr_of(bar), rect_of(100, 60, 10, 200), lh_ui_axis_vertical,
                                lh_addr_of(inner));
    lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_container_as_entity(lh_addr_of(outer)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(outer)), lh_addr_of(filler));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(outer)),
                           lh_ui_entity_container_as_entity(lh_addr_of(inner)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(outer)),
                           lh_ui_entity_scrollbar_as_entity(lh_addr_of(bar)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(inner)), lh_addr_of(content));
    lh_ui_entity_container_set_scroll(lh_addr_of(outer), point_of(0, 50));
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
    const lh_ui_point_t scroll = lh_ui_entity_container_get_scroll(lh_addr_of(f.box));
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
    lh_ui_entity_container_t box;
    lh_ui_entity_t content;
    lh_ui_entity_scrollbar_t bar;
    lh_ui_entity_label_t label;
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
    lh_ui_entity_container_init(lh_addr_of(s.box), lh_test::rect_of(10, 10, 100, 60));
    lh_ui_entity_set_style(lh_ui_entity_container_as_entity(lh_addr_of(s.box)), lh_addr_of(s.box_style));
    lh_ui_entity_init(lh_addr_of(s.content), lh_test::rect_of(10, 10, 100, 300));
    lh_ui_entity_scrollbar_init(lh_addr_of(s.bar), lh_test::rect_of(112, 10, 8, 60), lh_ui_axis_vertical,
                                lh_addr_of(s.box));
    lh_ui_entity_set_style(lh_ui_entity_scrollbar_as_entity(lh_addr_of(s.bar)), lh_addr_of(s.thumb_style));
    lh_ui_entity_scrollbar_set_mode(lh_addr_of(s.bar), lh_ui_entity_scrollbar_mode_always);
    lh_ui_entity_scrollbar_set_thumb_style(lh_addr_of(s.bar), lh_addr_of(s.thumb_style));
    lh_ui_entity_label_init(lh_addr_of(s.label), lh_test::rect_of(20, 20, 80, 40), "Paladin");
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(lh_addr_of(s.label)), lh_addr_of(s.label_style));

    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(s.box)), lh_addr_of(s.content));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(s.box)),
                           lh_ui_entity_label_as_entity(lh_addr_of(s.label)));
    lh_ui_entity_add_child(lh_addr_of(s.panel), lh_ui_entity_container_as_entity(lh_addr_of(s.box)));
    lh_ui_entity_add_child(lh_addr_of(s.panel), lh_ui_entity_scrollbar_as_entity(lh_addr_of(s.bar)));
}

TEST(view, strips_draw_the_same_picture_as_one_whole_frame)
{
    scene s;
    lh_test::partial_probe whole;
    lh_test::partial_probe strips;
    lh_ui_view_t one;
    lh_ui_view_t many;

    build_scene(s);
    lh_test::partial_probe_init(lh_addr_of(whole));
    lh_test::partial_probe_init(lh_addr_of(strips));
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
    EXPECT_EQ(lh_test::partial_probe_diff(whole, strips), 0);
}

TEST(view, strips_greater_than_the_target_draw_it_as_one)
{
    scene s;
    lh_test::partial_probe whole;
    lh_test::partial_probe strips;
    lh_ui_view_t one;
    lh_ui_view_t many;

    build_scene(s);
    lh_test::partial_probe_init(lh_addr_of(whole));
    lh_test::partial_probe_init(lh_addr_of(strips));
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
    EXPECT_EQ(lh_test::partial_probe_diff(whole, strips), 0);
}

TEST(view, damage_skips_the_strips_it_does_not_reach)
{
    scene s;
    lh_test::partial_probe one;
    lh_test::partial_probe strips;
    lh_ui_view_t whole_view;
    lh_ui_view_t strip_view;
    const lh_ui_rect_t damage = lh_test::rect_of(0, 64, 160, 56);

    build_scene(s);
    lh_test::partial_probe_init(lh_addr_of(one));
    lh_test::partial_probe_init(lh_addr_of(strips));
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
    EXPECT_EQ(lh_test::partial_probe_diff(one, strips), 0);
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