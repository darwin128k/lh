#include <gtest/gtest.h>

#include <lh/test/ui/draw_log.h>
#include <lh/test/ui/fill_probe.h>

#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(entity_container, init_keeps_the_rect_and_container_class)
{
    lh_ui_rect_t rect;
    lh_ui_entity_container_t box;

    lh_ui_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_entity_t *entity = lh_ui_entity_container_as_entity(lh_addr_of(box));
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(entity);

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_entity_container_class));
}

TEST(entity_container, draw_keeps_the_base_class_fill)
{
    lh_ui_entity_container_t box;
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_canvas_t canvas;
    lh_test::fill_probe probe;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 4, 4);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_entity_set_style(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(style));
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_backend(), rect,
                             color);

    lh_ui_entity_draw(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(canvas));

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
    lh_ui_entity_container_t box;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_entity_t c;

    scroll_fixture()
    {
        lh_ui_entity_container_init(lh_addr_of(box), rect_of(10, 10, 50, 50));
        lh_ui_entity_init(lh_addr_of(a), rect_of(10, 10, 50, 40));
        lh_ui_entity_init(lh_addr_of(b), rect_of(20, 50, 30, 40));
        lh_ui_entity_init(lh_addr_of(c), rect_of(10, 100, 50, 50));
        lh_ui_entity_set_hidden(lh_addr_of(c), lh_bool_true);
        lh_ui_entity_add_child(entity(), lh_addr_of(a));
        lh_ui_entity_add_child(entity(), lh_addr_of(b));
        lh_ui_entity_add_child(entity(), lh_addr_of(c));
    }

    lh_ui_entity_t *
    entity()
    {
        return lh_ui_entity_container_as_entity(lh_addr_of(box));
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
    lh_ui_entity_container_t box;

    lh_ui_entity_container_init(lh_addr_of(box), rect_of(0, 0, 4, 4));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(box)), 0, 0));
}

TEST(entity_container, content_size_spans_the_visible_children_from_the_origin)
{
    scroll_fixture f;

    const lh_ui_size_t size = lh_ui_entity_container_get_content_size(lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(size)), lh_ui_scalar(50));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(size)), lh_ui_scalar(80));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll_max(lh_addr_of(f.box)), 0, 30));

    lh_ui_entity_set_hidden(lh_addr_of(f.c), lh_bool_false);
    const lh_ui_size_t grown = lh_ui_entity_container_get_content_size(lh_addr_of(f.box));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(grown)), lh_ui_scalar(140));
}

TEST(entity_container, content_size_without_children_is_empty)
{
    lh_ui_entity_container_t box;

    lh_ui_entity_container_init(lh_addr_of(box), rect_of(-10, -10, 4, 4));
    const lh_ui_size_t size = lh_ui_entity_container_get_content_size(lh_addr_of(box));
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(size)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(size)), lh_ui_scalar(0));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll_max(lh_addr_of(box)), 0, 0));
}

TEST(entity_container, set_scroll_clamps_to_zero_and_max)
{
    scroll_fixture f;

    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(5, 12));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 12));
    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(-5, 99));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 30));
    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(0, -1));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 0));
}

TEST(entity_container, scroll_by_moves_and_clamps)
{
    scroll_fixture f;

    lh_ui_entity_container_scroll_by(lh_addr_of(f.box), lh_ui_scalar(0), lh_ui_scalar(20));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 20));
    lh_ui_entity_container_scroll_by(lh_addr_of(f.box), lh_ui_scalar(0), lh_ui_scalar(20));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 30));
    lh_ui_entity_container_scroll_by(lh_addr_of(f.box), lh_ui_scalar(0), lh_ui_scalar(-100));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 0));
}

TEST(entity_container, answers_children_with_minus_scroll_and_a_clip)
{
    scroll_fixture f;
    lh_ui_point_t offset;

    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(0, 25));
    EXPECT_EQ(lh_ui_entity_get_children_transform(f.entity(), lh_addr_of(offset)), lh_bool_true);
    EXPECT_TRUE(point_is(offset, 0, -25));
}

TEST(entity_container, children_are_drawn_moved_and_cut_to_the_viewport)
{
    scroll_fixture f;
    lh_test::draw_log log;
    lh_ui_canvas_t canvas;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;

    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_set_style(lh_addr_of(f.a), lh_addr_of(style));
    lh_ui_entity_set_style(lh_addr_of(f.b), lh_addr_of(style));
    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(0, 30));
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
    lh_ui_entity_container_t box;

    lh_ui_entity_container_init(lh_addr_of(box), rect_of(5, 6, 30, 40));
    const lh_ui_size_t viewport = lh_ui_entity_container_get_viewport_size(lh_addr_of(box));
    EXPECT_EQ(lh_ui_size_get_width(lh_addr_of(viewport)), lh_ui_scalar(30));
    EXPECT_EQ(lh_ui_size_get_height(lh_addr_of(viewport)), lh_ui_scalar(40));
    EXPECT_TRUE(point_is(lh_ui_entity_container_get_content_far(lh_addr_of(box)), 5, 6));
}

TEST(entity_container, content_far_is_the_far_corner_of_the_children)
{
    scroll_fixture f;

    EXPECT_TRUE(point_is(lh_ui_entity_container_get_content_far(lh_addr_of(f.box)), 60, 90));
}

TEST(entity_container, clamp_scroll_keeps_each_axis_in_zero_to_max)
{
    scroll_fixture f;

    EXPECT_TRUE(point_is(lh_ui_entity_container_clamp_scroll(lh_addr_of(f.box), point_of(-3, 50)), 0, 30));
    EXPECT_TRUE(point_is(lh_ui_entity_container_clamp_scroll(lh_addr_of(f.box), point_of(7, 12)), 0, 12));
}

TEST(entity_container, get_scroll_is_clamped_when_the_content_shrinks)
{
    scroll_fixture f;
    lh_ui_entity_transform_t transform;

    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(0, 30));
    /* b leaves the content: it now ends at a, 40 tall in a 50 viewport. */
    lh_ui_entity_set_hidden(lh_addr_of(f.b), lh_bool_true);

    EXPECT_TRUE(point_is(lh_ui_entity_container_get_scroll(lh_addr_of(f.box)), 0, 0));
    lh_ui_entity_transform_init(lh_addr_of(transform));
    lh_ui_entity_container_place_children(lh_addr_of(f.box), lh_addr_of(transform));
    EXPECT_TRUE(point_is(lh_ui_entity_transform_get_offset(lh_addr_of(transform)), 0, 0));
    EXPECT_EQ(lh_ui_entity_transform_is_clip(lh_addr_of(transform)), lh_bool_true);
}

TEST(entity_container, event_is_public_and_answers_children)
{
    scroll_fixture f;
    lh_ui_entity_transform_t transform;
    lh_ui_entity_event_t event;

    lh_ui_entity_container_set_scroll(lh_addr_of(f.box), point_of(0, 10));
    lh_ui_entity_transform_init(lh_addr_of(transform));
    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_children, lh_addr_of(transform));

    lh_ui_entity_container_event(f.entity(), lh_addr_of(event));

    EXPECT_TRUE(point_is(lh_ui_entity_transform_get_offset(lh_addr_of(transform)), 0, -10));
}
