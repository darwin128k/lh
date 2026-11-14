#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/fill_probe.h>

#include <lh/ui/container.h>
#include <lh/ui/button.h>
#include <lh/test/out.h>

TEST(entity_container, init_keeps_the_rect_and_container_class)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    lh_ui_container_t box;
    lh_ui_container_init(lh_addr_of(box), rect);
    lh_ui_entity_t *entity = lh_ui_container_as_entity(lh_addr_of(box));
    const lh_ui_rect_t stored = lh_test::out(lh_ui_entity_copy_rect, entity);

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_container_class));
}

TEST(entity_container, draw_keeps_the_base_class_fill)
{
    lh_ui_rect_t rect;
    lh_test::fill_probe probe;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 4, 4);
    lh_ui_color_t color;
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_t paint;
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_t style;
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_assign_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_container_t box;
    lh_ui_container_init(lh_addr_of(box), rect);
    lh_ui_entity_set_style_v(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(style));
    lh_ui_canvas_t canvas;
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_backend(),
                             rect, color);

    lh_ui_entity_draw(lh_ui_container_as_entity(lh_addr_of(box)), lh_addr_of(canvas));

    EXPECT_EQ(probe.matches, 1);
}

namespace
{
using lh_test::rect_is;
using lh_test::rect_of;

/* A container at (10, 10), 50x50, with children a (10,10) 50x40, b (20,50) 30x40
 * and a hidden c (10,100) 50x50. Content: 50 wide, 80 tall. */
struct scroll_fixture
{
    lh_ui_container_t box;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_entity_t c;

    scroll_fixture()
    {
        lh_ui_container_init(lh_addr_of(box), rect_of(10, 10, 50, 50));
        lh_ui_entity_init(lh_addr_of(a), rect_of(10, 10, 50, 40));
        lh_ui_entity_init(lh_addr_of(b), rect_of(20, 50, 30, 40));
        lh_ui_entity_init(lh_addr_of(c), rect_of(10, 100, 50, 50));
        lh_ui_entity_set_hidden_v(lh_addr_of(c), lh_bool_true);
        lh_ui_entity_add_child(entity(), lh_addr_of(a));
        lh_ui_entity_add_child(entity(), lh_addr_of(b));
        lh_ui_entity_add_child(entity(), lh_addr_of(c));
    }

    lh_ui_entity_t *
    entity()
    {
        return lh_ui_container_as_entity(lh_addr_of(box));
    }
};

lh_ui_point_t
point_of(int x, int y)
{
    lh_ui_point_t point;
    lh_ui_point_init(lh_addr_of(point), x, y);
    return point;
}

bool
point_is(lh_ui_point_t point, int x, int y)
{
    return lh_ui_point_get_x(lh_addr_of(point)) == static_cast<lh_ui_scalar_t>(x) &&
           lh_ui_point_get_y(lh_addr_of(point)) == static_cast<lh_ui_scalar_t>(y);
}
} // namespace

TEST(entity_container, init_starts_unscrolled)
{
    lh_ui_container_t box;

    lh_ui_container_init(lh_addr_of(box), rect_of(0, 0, 4, 4));
    EXPECT_TRUE(point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(box)), 0, 0));
}

TEST(entity_container, content_size_spans_the_visible_children_from_the_origin)
{
    scroll_fixture f;

    const lh_ui_size_t size = lh_test::out(lh_ui_container_get_content_size, lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(size)), lh_ui_scalar(50));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(size)), lh_ui_scalar(80));
    EXPECT_TRUE(point_is(lh_test::out(lh_ui_container_get_scroll_max, lh_addr_of(f.box)), 0, 30));

    lh_ui_entity_set_hidden_v(lh_addr_of(f.c), lh_bool_false);
    const lh_ui_size_t grown = lh_test::out(lh_ui_container_get_content_size, lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(grown)), lh_ui_scalar(140));
}

TEST(entity_container, content_size_without_children_is_empty)
{
    lh_ui_container_t box;

    lh_ui_container_init(lh_addr_of(box), rect_of(-10, -10, 4, 4));
    const lh_ui_size_t size = lh_test::out(lh_ui_container_get_content_size, lh_addr_of(box));
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(size)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(size)), lh_ui_scalar(0));
    EXPECT_TRUE(point_is(lh_test::out(lh_ui_container_get_scroll_max, lh_addr_of(box)), 0, 0));
}

TEST(entity_container, set_scroll_clamps_to_zero_and_max)
{
    scroll_fixture f;

    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(5, 12));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 12));
    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(-5, 99));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 30));
    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(0, -1));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 0));
}

TEST(entity_container, scroll_by_moves_and_clamps)
{
    scroll_fixture f;

    lh_ui_container_scroll_by(lh_addr_of(f.box), lh_ui_scalar(0), lh_ui_scalar(20));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 20));
    lh_ui_container_scroll_by(lh_addr_of(f.box), lh_ui_scalar(0), lh_ui_scalar(20));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 30));
    lh_ui_container_scroll_by(lh_addr_of(f.box), lh_ui_scalar(0), lh_ui_scalar(-100));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 0));
}

TEST(entity_container, answers_children_with_minus_scroll_and_a_clip)
{
    scroll_fixture f;

    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(0, 25));
    lh_ui_point_t offset;
    EXPECT_EQ(lh_ui_entity_get_children_transform(f.entity(), lh_addr_of(offset)), lh_bool_true);
    EXPECT_TRUE(point_is(offset, 0, -25));
}

TEST(entity_container, children_are_drawn_moved_and_cut_to_the_viewport)
{
    lh_test::draw_log log;
    lh_ui_color_t color;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_t paint;
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_t style;
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_assign_fill(lh_addr_of(style), lh_addr_of(paint));
    scroll_fixture f;
    lh_ui_entity_set_style_v(lh_addr_of(f.a), lh_addr_of(style));
    lh_ui_entity_set_style_v(lh_addr_of(f.b), lh_addr_of(style));
    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(0, 30));
    lh_ui_canvas_t canvas;
    lh_test::draw_log_init(lh_addr_of(log), lh_addr_of(canvas), false, false);

    lh_ui_entity_draw(f.entity(), lh_addr_of(canvas));

    /* a: (10,-20) 50x40 cut to (10,10) 50x50 -> (10,10) 50x10.
     * b: (20,20) 30x40 cut -> (20,20) 30x40. */
    ASSERT_EQ(log.fill_count, 2);
    EXPECT_TRUE(rect_is(log.fills[0], rect_of(10, 10, 50, 10)));
    EXPECT_TRUE(rect_is(log.fills[1], rect_of(20, 20, 30, 40)));
    /* The stack is balanced after the walk. */
    EXPECT_TRUE(lh_null_eq(lh_ui_canvas_get_clip(lh_addr_of(canvas))));
}

TEST(entity_container, viewport_is_the_rect_size_and_content_far_falls_back_to_the_origin)
{
    lh_ui_container_t box;

    lh_ui_container_init(lh_addr_of(box), rect_of(5, 6, 30, 40));
    const lh_ui_size_t viewport = lh_test::out(lh_ui_container_get_viewport_size, lh_addr_of(box));
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(viewport)), lh_ui_scalar(30));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(viewport)), lh_ui_scalar(40));
    EXPECT_TRUE(point_is(lh_test::out(lh_ui_container_get_content_far, lh_addr_of(box)), 5, 6));
}

TEST(entity_container, content_far_is_the_far_corner_of_the_children)
{
    scroll_fixture f;

    EXPECT_TRUE(point_is(lh_test::out(lh_ui_container_get_content_far, lh_addr_of(f.box)), 60, 90));
}

TEST(entity_container, clamp_scroll_keeps_each_axis_in_zero_to_max)
{
    scroll_fixture f;

    EXPECT_TRUE(point_is(
        lh_test::out(lh_ui_container_clamp_scroll, lh_addr_of(f.box), point_of(-3, 50)), 0, 30));
    EXPECT_TRUE(point_is(
        lh_test::out(lh_ui_container_clamp_scroll, lh_addr_of(f.box), point_of(7, 12)), 0, 12));
}

TEST(entity_container, get_scroll_is_clamped_when_the_content_shrinks)
{
    scroll_fixture f;

    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(0, 30));
    /* b leaves the content: it now ends at a, 40 tall in a 50 viewport. */
    lh_ui_entity_set_hidden_v(lh_addr_of(f.b), lh_bool_true);

    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_container_get_clamped_scroll, lh_addr_of(f.box)), 0, 0));
    lh_ui_entity_transform_t transform;
    lh_ui_entity_transform_init(lh_addr_of(transform));
    lh_ui_container_place_children(lh_addr_of(f.box), lh_addr_of(transform));
    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_entity_transform_copy_offset, lh_addr_of(transform)), 0, 0));
    EXPECT_EQ(lh_ui_entity_transform_is_clip(lh_addr_of(transform)), lh_bool_true);
}

TEST(entity_container, event_is_public_and_answers_children)
{
    scroll_fixture f;

    lh_ui_container_set_scroll_v(lh_addr_of(f.box), point_of(0, 10));
    lh_ui_entity_transform_t transform;
    lh_ui_entity_transform_init(lh_addr_of(transform));
    lh_ui_entity_event_t event;
    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_children, lh_addr_of(transform));

    lh_ui_container_event(f.entity(), lh_addr_of(event));

    EXPECT_TRUE(
        point_is(lh_test::out(lh_ui_entity_transform_copy_offset, lh_addr_of(transform)), 0, -10));
}

/* Padding is room after the last child: it adds to the content size (so the
 * scroll reaches it), but an empty container stays empty. */
TEST(entity_container, padding_extends_the_content_past_the_last_child)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(&rect, 0, 0, 100, 100);
    lh_ui_container_t box;
    lh_ui_container_init(&box, rect);
    lh_ui_style_t style;
    lh_ui_style_init(&style);
    lh_ui_style_set_padding_all(&style, lh_ui_scalar(8));
    lh_ui_entity_set_style_v(lh_ui_container_as_entity(&box), &style);

    lh_ui_size_t size = lh_test::out(lh_ui_container_get_content_size, &box);
    EXPECT_EQ(lh_ui_size_get_height(&size), lh_ui_scalar(0));

    lh_ui_rect_init(&rect, 0, 0, 50, 150);
    lh_ui_entity_t child;
    lh_ui_entity_init(&child, rect);
    lh_ui_entity_add_child(lh_ui_container_as_entity(&box), &child);
    size = lh_test::out(lh_ui_container_get_content_size, &box);
    EXPECT_EQ(lh_ui_size_get_width(&size), lh_ui_scalar(58));
    EXPECT_EQ(lh_ui_size_get_height(&size), lh_ui_scalar(158));
    const lh_ui_point_t max = lh_test::out(lh_ui_container_get_scroll_max, &box);
    EXPECT_EQ(lh_ui_point_get_y(&max), lh_ui_scalar(58));
}

TEST(entity_container, scrollability_uses_content_overflow_on_either_axis)
{
    lh_ui_container_t box;
    lh_ui_container_init(&box, lh_test::rect_of(0, 0, 100, 60));
    EXPECT_FALSE(lh_ui_container_can_scroll(&box));
    lh_ui_entity_t child;
    lh_ui_entity_init(&child, lh_test::rect_of(0, 0, 100, 60));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&box), &child);
    EXPECT_FALSE(lh_ui_container_can_scroll(&box));
    lh_ui_entity_set_rect_v(&child, lh_test::rect_of(0, 0, 101, 60));
    EXPECT_TRUE(lh_ui_container_can_scroll(&box));
    lh_ui_entity_set_rect_v(&child, lh_test::rect_of(0, 0, 100, 61));
    EXPECT_TRUE(lh_ui_container_can_scroll(&box));
    lh_ui_entity_set_hidden_v(&child, lh_bool_true);
    EXPECT_FALSE(lh_ui_container_can_scroll(&box));
}

TEST(entity_container, nearest_scrollable_skips_fitting_buttons_and_empty_containers)
{
    lh_ui_container_t outer;
    lh_ui_container_init(&outer, lh_test::rect_of(0, 0, 100, 60));
    lh_ui_container_t inner;
    lh_ui_container_init(&inner, lh_test::rect_of(0, 0, 100, 200));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&outer), lh_ui_container_as_entity(&inner));
    lh_ui_button_t button;
    lh_ui_button_init(&button, lh_test::rect_of(0, 0, 100, 200));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&inner), lh_ui_button_as_entity(&button));
    EXPECT_EQ(lh_ui_entity_find_container(lh_ui_button_as_entity(&button)),
              lh_ui_button_get_container_mut(&button));
    EXPECT_EQ(lh_ui_entity_find_scrollable(lh_ui_button_as_entity(&button)), &outer);
    lh_ui_entity_t caption;
    lh_ui_entity_init(&caption, lh_test::rect_of(0, 0, 10, 10));
    lh_ui_entity_add_child(lh_ui_button_as_entity(&button), &caption);
    EXPECT_EQ(lh_ui_entity_find_scrollable(&caption), &outer);
    lh_ui_entity_t overflow;
    lh_ui_entity_init(&overflow, lh_test::rect_of(0, 200, 100, 100));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&inner), &overflow);
    EXPECT_EQ(lh_ui_entity_find_scrollable(&caption), &inner);
    EXPECT_EQ(lh_ui_entity_find_scrollable(nullptr), nullptr);
    lh_ui_entity_t unattached;
    lh_ui_entity_init(&unattached, lh_test::rect_of(0, 0, 10, 10));
    EXPECT_EQ(lh_ui_entity_find_scrollable(&unattached), nullptr);
}

TEST(entity_container, a_barrier_on_a_plain_entity_blocks_ancestor_selection)
{
    lh_ui_container_t outer;
    lh_ui_container_init(&outer, lh_test::rect_of(0, 0, 100, 60));
    lh_ui_entity_t wrapper;
    lh_ui_entity_init(&wrapper, lh_test::rect_of(0, 0, 500, 60));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&outer), &wrapper);
    lh_ui_button_t button;
    lh_ui_button_init(&button, lh_test::rect_of(0, 0, 50, 50));
    lh_ui_entity_add_child(&wrapper, lh_ui_button_as_entity(&button));
    EXPECT_TRUE(lh_ui_entity_is_scroll_chain_enabled(&wrapper));
    EXPECT_EQ(lh_ui_entity_find_scrollable(lh_ui_button_as_entity(&button)), &outer);
    lh_ui_entity_set_scroll_chain(&wrapper, lh_bool_false);
    EXPECT_EQ(lh_ui_entity_find_scrollable(lh_ui_button_as_entity(&button)), nullptr);
    lh_ui_entity_set_scroll_chain(&wrapper, lh_bool_true);
    EXPECT_EQ(lh_ui_entity_find_scrollable(lh_ui_button_as_entity(&button)), &outer);
}

TEST(entity_container, chain_permission_does_not_disable_the_containers_own_scroll)
{
    lh_ui_container_t box;
    lh_ui_container_init(&box, lh_test::rect_of(0, 0, 100, 60));
    lh_ui_entity_t content;
    lh_ui_entity_init(&content, lh_test::rect_of(0, 0, 500, 60));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&box), &content);
    lh_ui_entity_set_scroll_chain(lh_ui_container_as_entity(&box), lh_bool_false);
    EXPECT_TRUE(lh_ui_container_can_scroll(&box));
    EXPECT_EQ(lh_ui_entity_find_scrollable(&content), &box);
}

TEST(entity_container, buttons_do_not_scroll_their_caption_unless_explicitly_enabled)
{
    lh_ui_container_t outer;
    lh_ui_container_init(&outer, lh_test::rect_of(0, 0, 100, 60));
    lh_ui_button_t button;
    lh_ui_button_init(&button, lh_test::rect_of(0, 0, 500, 60));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&outer), lh_ui_button_as_entity(&button));
    lh_ui_entity_t caption;
    lh_ui_entity_init(&caption, lh_test::rect_of(0, 0, 500, 61));
    lh_ui_entity_add_child(lh_ui_button_as_entity(&button), &caption);
    EXPECT_FALSE(lh_ui_container_is_scroll_enabled(lh_ui_button_get_container(&button)));
    EXPECT_FALSE(lh_ui_container_can_scroll(lh_ui_button_get_container(&button)));
    EXPECT_EQ(lh_ui_entity_find_scrollable(&caption), &outer);
    lh_ui_container_set_scroll_enabled(lh_ui_button_get_container_mut(&button), lh_bool_true);
    EXPECT_TRUE(lh_ui_container_can_scroll(lh_ui_button_get_container(&button)));
    EXPECT_EQ(lh_ui_entity_find_scrollable(&caption), lh_ui_button_get_container_mut(&button));
    lh_ui_container_set_scroll_enabled(&outer, lh_bool_false);
    EXPECT_FALSE(lh_ui_container_can_scroll(&outer));
    lh_ui_point_t position;
    lh_ui_point_init(&position, 20, 0);
    lh_ui_container_set_scroll_v(&outer, position);
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_container_get_scroll(&outer)), 20);
}
