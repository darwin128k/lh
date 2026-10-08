#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/container.h>
#include <lh/ui/scrollbar.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

/* root 200x200 > container (0,0) 100x100 with one content child, and a
 * vertical scrollbar beside it: track (100,0) 10x200. */
struct bar_fixture
{
    lh_ui_entity_t root;
    lh_ui_container_t box;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t bar;

    explicit bar_fixture(int content_height)
    {
        lh_ui_entity_init(lh_addr_of(root), rect_of(0, 0, 200, 200));
        lh_ui_container_init(lh_addr_of(box), rect_of(0, 0, 100, 100));
        lh_ui_entity_init(lh_addr_of(content), rect_of(0, 0, 100, content_height));
        lh_ui_scrollbar_init(lh_addr_of(bar), rect_of(100, 0, 10, 200),
                                    lh_ui_axis_vertical, lh_addr_of(box));
        lh_ui_entity_add_child(box_entity(), lh_addr_of(content));
        lh_ui_entity_add_child(lh_addr_of(root), box_entity());
        lh_ui_entity_add_child(lh_addr_of(root), bar_entity());
    }

    lh_ui_entity_t *
    box_entity()
    {
        return lh_ui_container_as_entity(lh_addr_of(box));
    }

    lh_ui_entity_t *
    bar_entity()
    {
        return lh_ui_scrollbar_as_entity(lh_addr_of(bar));
    }

    void
    scroll_to(int y)
    {
        lh_ui_point_t point;
        lh_ui_point_init(lh_addr_of(point), 0, y);
        lh_ui_container_set_scroll(lh_addr_of(box), point);
    }

    int
    scroll_y()
    {
        const lh_ui_point_t scroll = lh_ui_container_get_scroll(lh_addr_of(box));
        return static_cast<int>(lh_ui_point_get_y(lh_addr_of(scroll)));
    }

    lh_ui_entity_t *
    click(int x, int y)
    {
        lh_ui_point_t point;
        lh_ui_point_init(lh_addr_of(point), x, y);
        return lh_ui_entity_click(lh_addr_of(root), point);
    }
};

lh_ui_rect_t
thumb_of(const bar_fixture &f)
{
    return lh_ui_scrollbar_get_thumb_rect(lh_addr_of(f.bar));
}
} // namespace

TEST(entity_scrollbar, init_keeps_axis_container_and_defaults)
{
    bar_fixture f(100);

    EXPECT_EQ(f.bar.axis, lh_ui_axis_vertical);
    EXPECT_EQ(f.bar.container, lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_scrollbar_get_mode(lh_addr_of(f.bar)), lh_ui_scrollbar_mode_auto);
    EXPECT_TRUE(lh_null_eq(lh_ui_scrollbar_get_thumb_style(lh_addr_of(f.bar))));
    EXPECT_EQ(lh_ui_entity_get_class(f.bar_entity()), lh_addr_of(lh_ui_scrollbar_class));
}

TEST(entity_scrollbar, thumb_is_the_whole_track_with_nothing_to_scroll)
{
    bar_fixture f(80);

    EXPECT_TRUE(rect_is(thumb_of(f), rect_of(100, 0, 10, 200)));
}

/* Content 400, viewport 100, track 200: thumb 50 long, max scroll 300. */
TEST(entity_scrollbar, thumb_length_and_position_follow_the_scroll)
{
    bar_fixture f(400);

    EXPECT_TRUE(rect_is(thumb_of(f), rect_of(100, 0, 10, 50)));
    f.scroll_to(150);
    EXPECT_TRUE(rect_is(thumb_of(f), rect_of(100, 75, 10, 50)));
    f.scroll_to(300);
    EXPECT_TRUE(rect_is(thumb_of(f), rect_of(100, 150, 10, 50)));
}

TEST(entity_scrollbar, thumb_never_gets_shorter_than_the_minimum)
{
    bar_fixture f(10000);

    const lh_ui_rect_t thumb = thumb_of(f);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(thumb))),
              LH_UI_SCROLLBAR_THUMB_MIN);
}

TEST(entity_scrollbar, horizontal_thumb_runs_along_x)
{
    lh_ui_container_t box;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t bar;
    lh_ui_point_t scroll;

    lh_ui_container_init(lh_addr_of(box), rect_of(0, 0, 100, 100));
    lh_ui_entity_init(lh_addr_of(content), rect_of(0, 0, 400, 100));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(content));
    lh_ui_scrollbar_init(lh_addr_of(bar), rect_of(0, 100, 200, 10),
                                lh_ui_axis_horizontal, lh_addr_of(box));
    lh_ui_point_init(lh_addr_of(scroll), 300, 0);
    lh_ui_container_set_scroll(lh_addr_of(box), scroll);

    EXPECT_TRUE(rect_is(lh_ui_scrollbar_get_thumb_rect(lh_addr_of(bar)), rect_of(150, 100, 50, 10)));
}

TEST(entity_scrollbar, click_off_the_thumb_pages_toward_it)
{
    bar_fixture f(400);

    /* Thumb at y 0..50: below it pages down by the viewport (100). */
    EXPECT_EQ(f.click(105, 180), f.bar_entity());
    EXPECT_EQ(f.scroll_y(), 100);
    /* Thumb now at y 50..100: on it, nothing moves. */
    f.click(105, 60);
    EXPECT_EQ(f.scroll_y(), 100);
    /* Above it pages up. */
    f.click(105, 10);
    EXPECT_EQ(f.scroll_y(), 0);
    /* Paging stops at max. */
    f.click(105, 190);
    f.click(105, 190);
    f.click(105, 190);
    f.click(105, 190);
    EXPECT_EQ(f.scroll_y(), 300);
}

TEST(entity_scrollbar, draw_fills_the_track_then_the_thumb)
{
    bar_fixture f(400);
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t track_color;
    lh_ui_color_t thumb_color;
    lh_ui_paint_t track_paint;
    lh_ui_paint_t thumb_paint;
    lh_ui_style_t track_style;
    lh_ui_style_t thumb_style;

    lh_ui_color_init(lh_addr_of(track_color), 1, 1, 1, 255);
    lh_ui_color_init(lh_addr_of(thumb_color), 2, 2, 2, 255);
    lh_ui_paint_init_color(lh_addr_of(track_paint), lh_addr_of(track_color));
    lh_ui_paint_init_color(lh_addr_of(thumb_paint), lh_addr_of(thumb_color));
    lh_ui_style_init(lh_addr_of(track_style));
    lh_ui_style_init(lh_addr_of(thumb_style));
    lh_ui_style_set_fill(lh_addr_of(track_style), lh_addr_of(track_paint));
    lh_ui_style_set_fill(lh_addr_of(thumb_style), lh_addr_of(thumb_paint));
    lh_ui_style_set_radius(lh_addr_of(thumb_style), LH_UI_RADIUS_CIRCLE);
    lh_ui_entity_set_style(f.bar_entity(), lh_addr_of(track_style));
    lh_ui_scrollbar_set_thumb_style(lh_addr_of(f.bar), lh_addr_of(thumb_style));
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), true, false);

    lh_ui_entity_draw(f.bar_entity(), lh_addr_of(canvas));

    ASSERT_EQ(log.fill_count, 1);
    EXPECT_TRUE(rect_is(log.fills[0], rect_of(100, 0, 10, 200)));
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(log.fill_colors[0]), lh_addr_of(track_color)));
    ASSERT_EQ(log.round_count, 1);
    EXPECT_TRUE(rect_is(log.round_fills[0], rect_of(100, 0, 10, 50)));
    EXPECT_EQ(log.round_radii[0], lh_ui_scalar(5));
}

/* ── Show modes ──────────────────────────────────────────────────────────── */

namespace
{
struct mode_case
{
    lh_ui_scrollbar_mode_t mode;
    int content_height;
    bool shown;
};

class entity_scrollbar_mode : public ::testing::TestWithParam<mode_case>
{
};
} // namespace

TEST_P(entity_scrollbar_mode, decides_draw_and_click)
{
    const mode_case c = GetParam();
    bar_fixture f(c.content_height);
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 1, 1, 1, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_set_style(f.bar_entity(), lh_addr_of(style));
    lh_ui_scrollbar_set_mode(lh_addr_of(f.bar), c.mode);
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), true, false);

    EXPECT_EQ(lh_ui_entity_is_shown(f.bar_entity()) != 0, c.shown);
    lh_ui_entity_draw(lh_addr_of(f.root), lh_addr_of(canvas));
    EXPECT_EQ(log.fill_count, c.shown ? 1 : 0);
    EXPECT_EQ(f.click(105, 180), c.shown ? f.bar_entity() : lh_addr_of(f.root));

    /* The hidden flag wins over every mode. */
    lh_ui_entity_set_hidden(f.bar_entity(), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_shown(f.bar_entity()), lh_bool_false);
    EXPECT_EQ(f.click(105, 180), lh_addr_of(f.root));
}

INSTANTIATE_TEST_SUITE_P(
    modes_by_overflow, entity_scrollbar_mode,
    ::testing::Values(mode_case{lh_ui_scrollbar_mode_hidden, 400, false},
                      mode_case{lh_ui_scrollbar_mode_hidden, 80, false},
                      mode_case{lh_ui_scrollbar_mode_auto, 400, true},
                      mode_case{lh_ui_scrollbar_mode_auto, 80, false},
                      mode_case{lh_ui_scrollbar_mode_always, 400, true},
                      mode_case{lh_ui_scrollbar_mode_always, 80, true}));

TEST(entity_scrollbar, hidden_mode_still_lets_the_container_scroll)
{
    bar_fixture f(400);

    lh_ui_scrollbar_set_mode(lh_addr_of(f.bar), lh_ui_scrollbar_mode_hidden);
    f.scroll_to(120);
    EXPECT_EQ(f.scroll_y(), 120);
}

/* ── Public parts ────────────────────────────────────────────────────────── */

TEST(entity_scrollbar, reads_the_container_on_its_axis)
{
    bar_fixture f(400);

    f.scroll_to(120);
    EXPECT_EQ(lh_ui_scrollbar_get_axis(lh_addr_of(f.bar)), lh_ui_axis_vertical);
    EXPECT_EQ(lh_ui_scrollbar_get_scroll(lh_addr_of(f.bar)), lh_ui_scalar(120));
    EXPECT_EQ(lh_ui_scrollbar_get_scroll_max(lh_addr_of(f.bar)), lh_ui_scalar(300));
    EXPECT_EQ(lh_ui_scrollbar_get_viewport_length(lh_addr_of(f.bar)), lh_ui_scalar(100));
    EXPECT_EQ(lh_ui_scrollbar_get_content_length(lh_addr_of(f.bar)), lh_ui_scalar(400));
    EXPECT_EQ(lh_ui_scrollbar_get_track_length(lh_addr_of(f.bar)), lh_ui_scalar(200));
}

TEST(entity_scrollbar, is_needed_only_on_overflow)
{
    bar_fixture fits(80);
    bar_fixture overflows(400);

    EXPECT_EQ(lh_ui_scrollbar_is_needed(lh_addr_of(fits.bar)), lh_bool_false);
    EXPECT_EQ(lh_ui_scrollbar_is_needed(lh_addr_of(overflows.bar)), lh_bool_true);
}

TEST(entity_scrollbar, thumb_length_and_start_separately)
{
    bar_fixture f(400);

    EXPECT_EQ(lh_ui_scrollbar_get_thumb_length(lh_addr_of(f.bar)), lh_ui_scalar(50));
    EXPECT_EQ(lh_ui_scrollbar_get_thumb_start(lh_addr_of(f.bar)), lh_ui_scalar(0));
    f.scroll_to(300);
    EXPECT_EQ(lh_ui_scrollbar_get_thumb_start(lh_addr_of(f.bar)), lh_ui_scalar(150));

    bar_fixture fits(80);
    EXPECT_EQ(lh_ui_scrollbar_get_thumb_length(lh_addr_of(fits.bar)), lh_ui_scalar(200));
    EXPECT_EQ(lh_ui_scrollbar_get_thumb_start(lh_addr_of(fits.bar)), lh_ui_scalar(0));
}

TEST(entity_scrollbar, page_toward_without_a_click)
{
    bar_fixture f(400);
    lh_ui_point_t below;
    lh_ui_point_t on;

    lh_ui_point_init(lh_addr_of(below), 105, 180);
    lh_ui_point_init(lh_addr_of(on), 105, 10);
    EXPECT_EQ(lh_ui_scrollbar_get_page_toward(lh_addr_of(f.bar), below), lh_ui_scalar(100));
    EXPECT_EQ(lh_ui_scrollbar_get_page_toward(lh_addr_of(f.bar), on), lh_ui_scalar(0));

    lh_ui_scrollbar_page_toward(lh_addr_of(f.bar), below);
    EXPECT_EQ(f.scroll_y(), 100);
}

TEST(entity_scrollbar, contains_thumb_and_set_scroll_at)
{
    bar_fixture f(400);
    lh_ui_point_t on_thumb;
    lh_ui_point_t off_thumb;
    lh_ui_point_t at_mid;

    lh_ui_point_init(lh_addr_of(on_thumb), 105, 10);
    lh_ui_point_init(lh_addr_of(off_thumb), 105, 180);
    EXPECT_EQ(lh_ui_scrollbar_contains_thumb(lh_addr_of(f.bar), on_thumb), lh_bool_true);
    EXPECT_EQ(lh_ui_scrollbar_contains_thumb(lh_addr_of(f.bar), off_thumb), lh_bool_false);

    EXPECT_EQ(lh_ui_scrollbar_get_thumb_start_at(lh_addr_of(f.bar), on_thumb), lh_ui_scalar(10));

    /* Thumb start at y 75 → scroll 150 (track 200, thumb 50, max 300). */
    lh_ui_point_init(lh_addr_of(at_mid), 105, 75);
    lh_ui_scrollbar_set_scroll_at(lh_addr_of(f.bar), at_mid);
    EXPECT_EQ(f.scroll_y(), 150);
    EXPECT_TRUE(rect_is(thumb_of(f), rect_of(100, 75, 10, 50)));
}

TEST(entity_scrollbar, as_scrollbar_matches_the_class)
{
    bar_fixture f(400);

    EXPECT_EQ(lh_ui_entity_as_scrollbar(f.bar_entity()), lh_addr_of(f.bar));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_as_scrollbar(f.box_entity())));
}

TEST(entity_scrollbar, is_visible_follows_the_mode)
{
    bar_fixture f(80);

    EXPECT_EQ(lh_ui_scrollbar_is_visible(lh_addr_of(f.bar)), lh_bool_false);
    lh_ui_scrollbar_set_mode(lh_addr_of(f.bar), lh_ui_scrollbar_mode_always);
    EXPECT_EQ(lh_ui_scrollbar_is_visible(lh_addr_of(f.bar)), lh_bool_true);
}

TEST(entity_scrollbar, draw_thumb_needs_a_canvas_and_a_thumb_style)
{
    bar_fixture f(400);
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;

    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), true, false);
    lh_ui_scrollbar_draw_thumb(lh_addr_of(f.bar), nullptr);
    lh_ui_scrollbar_draw_thumb(lh_addr_of(f.bar), lh_addr_of(canvas));
    EXPECT_EQ(log.round_count, 0);
    EXPECT_EQ(log.fill_count, 0);
}

TEST(entity_scrollbar, thumb_within_a_known_max_matches_the_measured_thumb)
{
    bar_fixture f(400);
    lh_ui_point_t max;

    f.scroll_to(150);
    max = lh_ui_container_get_scroll_max(lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_scrollbar_get_thumb_length_within(lh_addr_of(f.bar), max),
              lh_ui_scrollbar_get_thumb_length(lh_addr_of(f.bar)));
    EXPECT_EQ(lh_ui_scrollbar_get_thumb_start_within(lh_addr_of(f.bar), max), lh_ui_scalar(75));
}

TEST(entity_scrollbar, driven_and_scrolled_containers)
{
    bar_fixture f(400);

    EXPECT_EQ(lh_ui_scrollbar_get_driven(f.bar_entity()), lh_addr_of(f.box));
    EXPECT_TRUE(lh_null_eq(lh_ui_scrollbar_get_driven(f.box_entity())));
    EXPECT_TRUE(lh_null_eq(lh_ui_scrollbar_get_driven(nullptr)));
    EXPECT_EQ(lh_ui_scrollbar_find_scrolled(f.bar_entity()), lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_scrollbar_find_scrolled(lh_addr_of(f.content)), lh_addr_of(f.box));
    EXPECT_TRUE(lh_null_eq(lh_ui_scrollbar_find_scrolled(lh_addr_of(f.root))));
}

TEST(entity_scrollbar, get_bound_needs_the_same_container)
{
    bar_fixture f(400);
    lh_ui_container_t other;

    lh_ui_container_init(lh_addr_of(other), rect_of(0, 0, 10, 10));
    EXPECT_EQ(lh_ui_scrollbar_get_bound(f.bar_entity(), lh_addr_of(f.box)), lh_addr_of(f.bar));
    EXPECT_TRUE(lh_null_eq(lh_ui_scrollbar_get_bound(f.bar_entity(), lh_addr_of(other))));
    EXPECT_TRUE(lh_null_eq(lh_ui_scrollbar_get_bound(f.box_entity(), lh_addr_of(f.box))));
}

/* A horizontal bar linked before the vertical one: scroll damage still holds
 * the vertical track, so its thumb cannot stay drawn where it was. */
TEST(entity_scrollbar, scroll_damage_covers_the_viewport_and_every_bound_track)
{
    bar_fixture f(400);
    lh_ui_scrollbar_t across;
    lh_ui_canvas_t canvas;
    const lh_ui_rect_t *damage;

    lh_ui_entity_remove_child(lh_addr_of(f.root), f.bar_entity());
    lh_ui_scrollbar_init(lh_addr_of(across), rect_of(0, 100, 100, 10), lh_ui_axis_horizontal,
                                lh_addr_of(f.box));
    lh_ui_entity_add_child(lh_addr_of(f.root), lh_ui_scrollbar_as_entity(lh_addr_of(across)));
    lh_ui_entity_add_child(lh_addr_of(f.root), f.bar_entity());
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);

    lh_ui_scrollbar_add_scroll_damage(lh_addr_of(f.box), lh_addr_of(canvas));

    damage = lh_ui_canvas_get_damage(lh_addr_of(canvas));
    ASSERT_NE(damage, nullptr);
    EXPECT_TRUE(rect_is(*damage, rect_of(0, 0, 110, 200)));
}
