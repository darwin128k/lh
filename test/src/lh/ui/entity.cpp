#include <gtest/gtest.h>

#include <lh/test/ui/fill_probe.h>

#include <lh/bool.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/paint.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/void.h>

namespace
{
const lh_ui_entity_t *g_draw_order[8];
int g_draw_order_n;

const lh_ui_entity_t *g_click_target;
lh_ui_point_t g_click_point;
int g_click_count;

/* Records draws only; draw also asks a parent about its children first. */
lh_void
record_draw(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    if (lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw)
    {
        return;
    }
    ASSERT_LT(g_draw_order_n, 8);
    g_draw_order[g_draw_order_n] = self;
    ++g_draw_order_n;
}

lh_void
record_click(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    if (lh_ui_entity_event_get_code(event) != lh_ui_entity_event_click)
    {
        return;
    }
    g_click_target = self;
    g_click_point = lh_ui_entity_event_get_point(event);
    ++g_click_count;
}

const lh_ui_entity_class_t g_record_class = {record_draw,
                                            lh_ptr_rcast(const lh_ui_entity_class_t, lh_null)};
const lh_ui_entity_class_t g_click_class = {record_click,
                                           lh_ptr_rcast(const lh_ui_entity_class_t, lh_null)};
struct fill_log
{
    int count;
    lh_ui_rect_t rect;
    lh_ui_color_t color;
};

lh_void
log_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    fill_log *log = lh_ptr_rcast(fill_log, context);
    ++log->count;
    log->rect = *rect;
    log->color = *color;
}

const lh_ui_canvas_backend_t g_fill_log_backend = {nullptr, nullptr, nullptr, log_fill_rect, nullptr,
                                                     nullptr, nullptr};

/* Derived classes over lh_ui_entity_class: one skips the base, one calls it. */
lh_void
skip_base_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    (void)self;
    (void)event;
}

extern const lh_ui_entity_class_t g_call_base_class;

/* Name the own class, as label.c does: lh_ui_entity_get_class(self) would be
 * the most-derived class and recurse in a deeper chain. */
lh_void
call_base_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    lh_ui_entity_class_event_base(lh_addr_of(g_call_base_class), self, event);
}

const lh_ui_entity_class_t g_skip_base_class = {skip_base_event, lh_addr_of(lh_ui_entity_class)};
const lh_ui_entity_class_t g_call_base_class = {call_base_event, lh_addr_of(lh_ui_entity_class)};

/* Draw one entity of @p klass with a solid style fill; return the fill count. */
int
fills_for_class(const lh_ui_entity_class_t *klass)
{
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_entity_t entity;
    lh_ui_canvas_t canvas;
    fill_log log{};

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 4, 4);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_entity_set_style(lh_addr_of(entity), lh_addr_of(style));
    lh_ui_entity_set_class(lh_addr_of(entity), klass);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_fill_log_backend), lh_addr_of(log));
    lh_ui_entity_draw(lh_addr_of(entity), lh_addr_of(canvas));
    return log.count;
}

int g_walk_n;

lh_bool_t
count_until_two(const struct lh_ui_entity *entity, lh_ptr context)
{
    (void)entity;
    (void)context;
    ++g_walk_n;
    return g_walk_n < 2 ? lh_bool_true : lh_bool_false;
}

lh_bool_t
count_all(const struct lh_ui_entity *entity, lh_ptr context)
{
    (void)entity;
    ++*lh_ptr_rcast(int, context);
    return lh_bool_true;
}
} // namespace

TEST(entity, init_keeps_the_rect)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    lh_ui_entity_t entity;
    lh_ui_entity_init(lh_addr_of(entity), rect);
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(lh_addr_of(entity));

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_class(lh_addr_of(entity)), lh_addr_of(lh_ui_entity_class));
}

TEST(entity, init_starts_with_no_style)
{
    lh_ui_entity_t entity;
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_style(lh_addr_of(entity))));
}

TEST(entity, init_starts_with_no_children)
{
    lh_ui_entity_t entity;
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_first_child(lh_addr_of(entity))));
}

TEST(entity, set_style_keeps_the_pointer)
{
    lh_ui_entity_t entity;
    lh_ui_style_t style;

    lh_ui_style_init(lh_addr_of(style));
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_entity_set_style(lh_addr_of(entity), lh_addr_of(style));
    EXPECT_EQ(lh_ui_entity_get_style(lh_addr_of(entity)), lh_addr_of(style));
    lh_ui_entity_set_style(lh_addr_of(entity), lh_ptr_rcast(const lh_ui_style_t, lh_null));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_style(lh_addr_of(entity))));
}

TEST(entity, add_child_links_in_order)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t parent;
    lh_ui_entity_t first;
    lh_ui_entity_t second;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(first), rect);
    lh_ui_entity_init(lh_addr_of(second), rect);

    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(first));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(second));

    EXPECT_EQ(lh_ui_entity_get_first_child(lh_addr_of(parent)), lh_addr_of(first));
    EXPECT_EQ(lh_ui_entity_get_next_child(lh_addr_of(parent), lh_addr_of(first)), lh_addr_of(second));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_next_child(lh_addr_of(parent), lh_addr_of(second))));
}

TEST(entity, remove_child_unlinks)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t parent;
    lh_ui_entity_t child;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));

    lh_ui_entity_remove_child(lh_addr_of(parent), lh_addr_of(child));

    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_first_child(lh_addr_of(parent))));
}

TEST(entity, draw_visits_parent_then_children)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t parent;
    lh_ui_entity_t first;
    lh_ui_entity_t second;
    lh_ui_canvas_t canvas;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(first), rect);
    lh_ui_entity_init(lh_addr_of(second), rect);
    lh_ui_entity_set_class(lh_addr_of(parent), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(first), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(second), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(first));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(second));

    g_draw_order_n = 0;
    lh_ui_entity_draw(lh_addr_of(parent), lh_addr_of(canvas));

    ASSERT_EQ(g_draw_order_n, 3);
    EXPECT_EQ(g_draw_order[0], lh_addr_of(parent));
    EXPECT_EQ(g_draw_order[1], lh_addr_of(first));
    EXPECT_EQ(g_draw_order[2], lh_addr_of(second));
}

TEST(entity, init_starts_visible)
{
    lh_ui_entity_t entity;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    EXPECT_EQ(lh_ui_entity_is_hidden(lh_addr_of(entity)), lh_bool_false);
}

TEST(entity, set_hidden_toggles_flag)
{
    lh_ui_entity_t entity;
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_entity_set_hidden(lh_addr_of(entity), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_hidden(lh_addr_of(entity)), lh_bool_true);
    lh_ui_entity_set_hidden(lh_addr_of(entity), lh_bool_false);
    EXPECT_EQ(lh_ui_entity_is_hidden(lh_addr_of(entity)), lh_bool_false);
}

TEST(entity, draw_skips_hidden_subtree)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t parent;
    lh_ui_entity_t child;
    lh_ui_canvas_t canvas;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_set_class(lh_addr_of(parent), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(child), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));
    lh_ui_entity_set_hidden(lh_addr_of(parent), lh_bool_true);

    g_draw_order_n = 0;
    lh_ui_entity_draw(lh_addr_of(parent), lh_addr_of(canvas));
    EXPECT_EQ(g_draw_order_n, 0);
}

TEST(entity, find_at_prefers_later_child)
{
    lh_ui_rect_t root_rect;
    lh_ui_rect_t child_rect;
    lh_ui_entity_t root;
    lh_ui_entity_t first;
    lh_ui_entity_t second;
    lh_ui_point_t point;

    lh_ui_rect_init(lh_addr_of(root_rect), 0, 0, 100, 100);
    lh_ui_rect_init(lh_addr_of(child_rect), 10, 10, 20, 20);
    lh_ui_entity_init(lh_addr_of(root), root_rect);
    lh_ui_entity_init(lh_addr_of(first), child_rect);
    lh_ui_entity_init(lh_addr_of(second), child_rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(first));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(second));
    lh_ui_point_init(lh_addr_of(point), 15, 15);

    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(second));
}

TEST(entity, find_at_skips_hidden)
{
    lh_ui_rect_t root_rect;
    lh_ui_rect_t child_rect;
    lh_ui_entity_t root;
    lh_ui_entity_t child;
    lh_ui_point_t point;

    lh_ui_rect_init(lh_addr_of(root_rect), 0, 0, 100, 100);
    lh_ui_rect_init(lh_addr_of(child_rect), 10, 10, 20, 20);
    lh_ui_entity_init(lh_addr_of(root), root_rect);
    lh_ui_entity_init(lh_addr_of(child), child_rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(child));
    lh_ui_entity_set_hidden(lh_addr_of(child), lh_bool_true);
    lh_ui_point_init(lh_addr_of(point), 15, 15);

    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(root));
}

TEST(entity, click_sends_event_to_hit)
{
    lh_ui_rect_t root_rect;
    lh_ui_rect_t child_rect;
    lh_ui_entity_t root;
    lh_ui_entity_t child;
    lh_ui_point_t point;
    lh_ui_entity_t *hit;

    lh_ui_rect_init(lh_addr_of(root_rect), 0, 0, 100, 100);
    lh_ui_rect_init(lh_addr_of(child_rect), 10, 10, 20, 20);
    lh_ui_entity_init(lh_addr_of(root), root_rect);
    lh_ui_entity_init(lh_addr_of(child), child_rect);
    lh_ui_entity_set_class(lh_addr_of(child), lh_addr_of(g_click_class));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(child));
    lh_ui_point_init(lh_addr_of(point), 12, 14);

    g_click_count = 0;
    g_click_target = nullptr;
    hit = lh_ui_entity_click(lh_addr_of(root), point);

    EXPECT_EQ(hit, lh_addr_of(child));
    EXPECT_EQ(g_click_count, 1);
    EXPECT_EQ(g_click_target, lh_addr_of(child));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(g_click_point)), 12);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(g_click_point)), 14);
}

TEST(entity, add_child_sets_parent_and_remove_clears_it)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t parent;
    lh_ui_entity_t child;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_parent(lh_addr_of(child))));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));
    EXPECT_EQ(lh_ui_entity_get_parent(lh_addr_of(child)), lh_addr_of(parent));
    lh_ui_entity_remove_child(lh_addr_of(parent), lh_addr_of(child));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_parent(lh_addr_of(child))));

    /* Free again: it can join another parent. */
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));
    EXPECT_EQ(lh_ui_entity_get_first_child(lh_addr_of(parent)), lh_addr_of(child));
}

TEST(entity, walk_visits_every_node_and_can_stop)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    int n;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(a));
    lh_ui_entity_add_child(lh_addr_of(a), lh_addr_of(b));

    n = 0;
    EXPECT_EQ(lh_ui_entity_walk(lh_addr_of(root), count_all, lh_addr_of(n)), lh_bool_true);
    EXPECT_EQ(n, 3);

    g_walk_n = 0;
    EXPECT_EQ(lh_ui_entity_walk(lh_addr_of(root), count_until_two, nullptr), lh_bool_false);
    EXPECT_EQ(g_walk_n, 2);
}

TEST(entity, base_class_fills_the_style_color_on_the_canvas)
{
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_entity_t entity;
    lh_ui_canvas_t canvas;
    fill_log log{};

    lh_ui_rect_init(lh_addr_of(rect), 5, 6, 7, 8);
    lh_ui_color_init(lh_addr_of(color), 9, 10, 11, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_entity_set_style(lh_addr_of(entity), lh_addr_of(style));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_fill_log_backend), lh_addr_of(log));

    EXPECT_TRUE(lh_ui_color_equals(lh_ui_entity_get_fill_color(lh_addr_of(entity)), lh_addr_of(color)));
    lh_ui_entity_draw(lh_addr_of(entity), lh_addr_of(canvas));

    EXPECT_EQ(log.count, 1);
    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(log.rect), lh_addr_of(rect)), lh_bool_true);
    EXPECT_TRUE(lh_ui_color_equals(lh_addr_of(log.color), lh_addr_of(color)));
}

TEST(entity, base_class_without_style_paints_nothing)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t entity;
    lh_ui_canvas_t canvas;
    fill_log log{};

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_fill_log_backend), lh_addr_of(log));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_fill_color(lh_addr_of(entity))));
    lh_ui_entity_draw(lh_addr_of(entity), lh_addr_of(canvas));
    EXPECT_EQ(log.count, 0);
}

/* Contract: the fill belongs to the base class. A derived class keeps it only
 * by calling lh_ui_entity_class_event_base; skipping that drops it on purpose. */
TEST(entity, derived_class_keeps_fill_only_through_the_base)
{
    EXPECT_EQ(fills_for_class(lh_addr_of(g_call_base_class)), 1);
    EXPECT_EQ(fills_for_class(lh_addr_of(g_skip_base_class)), 0);
}

TEST(entity, base_class_passes_the_style_radius_to_the_canvas)
{
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_entity_t entity;
    lh_ui_canvas_t canvas;
    lh_test::fill_probe probe;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 20, 10);
    lh_ui_color_init(lh_addr_of(color), 9, 10, 11, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_style_set_radius(lh_addr_of(style), lh_ui_scalar(3));
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_entity_set_style(lh_addr_of(entity), lh_addr_of(style));
    lh_test::fill_probe_init(lh_addr_of(probe), lh_addr_of(canvas), lh_test::fill_probe_round_backend(),
                             rect, color);

    lh_ui_entity_draw(lh_addr_of(entity), lh_addr_of(canvas));

    EXPECT_EQ(probe.round_matches, 1);
    EXPECT_EQ(probe.matches, 0);
    EXPECT_EQ(probe.radius, lh_ui_scalar(3));
}

TEST(entity, base_class_places_children_with_no_offset_and_no_clip)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t entity;
    lh_ui_point_t offset;

    lh_ui_rect_init(lh_addr_of(rect), 3, 4, 5, 6);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_point_init(lh_addr_of(offset), 9, 9);

    EXPECT_EQ(lh_ui_entity_get_children_transform(lh_addr_of(entity), lh_addr_of(offset)), lh_bool_false);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(offset)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(offset)), lh_ui_scalar(0));
}

TEST(entity, base_class_is_shown_unless_hidden)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t entity;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    EXPECT_EQ(lh_ui_entity_is_shown(lh_addr_of(entity)), lh_bool_true);
    lh_ui_entity_set_hidden(lh_addr_of(entity), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_shown(lh_addr_of(entity)), lh_bool_false);
}

/* root 100x100 > container (10,10) 50x50 > a (10,10) 50x40, b (10,50) 50x40.
 * Content is 80 tall, so the container scrolls up to 30. */
TEST(entity, find_at_and_click_follow_a_scrolled_container)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_container_t box;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_point_t point;
    lh_ui_point_t scroll;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 50, 50);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 50, 40);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 50, 50, 40);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_entity_set_class(lh_addr_of(b), lh_addr_of(g_click_class));
    lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_container_as_entity(lh_addr_of(box)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(a));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(b));

    lh_ui_point_init(lh_addr_of(point), 20, 30);
    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(a));

    lh_ui_point_init(lh_addr_of(scroll), 0, 30);
    lh_ui_entity_container_set_scroll(lh_addr_of(box), scroll);
    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(b));

    /* The click reaches b in its own (content) space: y 30 + scroll 30. */
    g_click_count = 0;
    EXPECT_EQ(lh_ui_entity_click(lh_addr_of(root), point), lh_addr_of(b));
    EXPECT_EQ(g_click_count, 1);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(g_click_point)), lh_ui_scalar(20));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(g_click_point)), lh_ui_scalar(60));

    /* Below the viewport b would be there, but it is cut away: the root is hit. */
    lh_ui_point_init(lh_addr_of(point), 20, 70);
    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(root));
}

TEST(entity, send_reaches_the_class_with_the_code_and_context)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t entity;
    lh_ui_point_t point;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_entity_set_class(lh_addr_of(entity), lh_addr_of(g_click_class));
    lh_ui_point_init(lh_addr_of(point), 4, 5);
    g_click_count = 0;

    lh_ui_entity_send(lh_addr_of(entity), lh_ui_entity_event_click, lh_addr_of(point));

    EXPECT_EQ(g_click_count, 1);
    EXPECT_EQ(g_click_target, lh_addr_of(entity));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(g_click_point)), lh_ui_scalar(5));
}

TEST(entity, is_ancestor_follows_parents)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t child;
    lh_ui_entity_t other;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_init(lh_addr_of(other), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(child));

    EXPECT_EQ(lh_ui_entity_is_ancestor(lh_addr_of(root), lh_addr_of(child)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_ancestor(lh_addr_of(child), lh_addr_of(child)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_ancestor(lh_addr_of(child), lh_addr_of(root)), lh_bool_false);
    EXPECT_EQ(lh_ui_entity_is_ancestor(lh_addr_of(other), lh_addr_of(child)), lh_bool_false);
}

TEST(entity, walk_children_skips_self)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    int count = 0;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(a));
    lh_ui_entity_add_child(lh_addr_of(a), lh_addr_of(b));

    EXPECT_EQ(lh_ui_entity_walk_children(lh_addr_of(root), count_all, lh_addr_of(count)), lh_bool_true);
    EXPECT_EQ(count, 2);
}

TEST(entity, children_bounds_unite_the_children_that_are_not_hidden)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_entity_t c;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    const lh_ui_rect_t none = lh_ui_entity_get_children_bounds(lh_addr_of(root));
    EXPECT_EQ(lh_ui_rect_is_empty(lh_addr_of(none)), lh_bool_true);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 5, 5);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_rect_init(lh_addr_of(rect), 20, 30, 5, 5);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_rect_init(lh_addr_of(rect), 90, 90, 5, 5);
    lh_ui_entity_init(lh_addr_of(c), rect);
    lh_ui_entity_set_hidden(lh_addr_of(c), lh_bool_true);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(a));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(b));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(c));

    const lh_ui_rect_t bounds = lh_ui_entity_get_children_bounds(lh_addr_of(root));
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 15, 25);
    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(bounds), lh_addr_of(rect)), lh_bool_true);
}

TEST(entity, is_hit_needs_the_rect_and_shown)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t entity;
    lh_ui_point_t inside;
    lh_ui_point_t outside;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_entity_init(lh_addr_of(entity), rect);
    lh_ui_point_init(lh_addr_of(inside), 5, 5);
    lh_ui_point_init(lh_addr_of(outside), 15, 5);

    EXPECT_EQ(lh_ui_entity_is_hit(lh_addr_of(entity), inside), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_hit(lh_addr_of(entity), outside), lh_bool_false);
    lh_ui_entity_set_hidden(lh_addr_of(entity), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_is_hit(lh_addr_of(entity), inside), lh_bool_false);
}

TEST(entity, push_children_only_when_the_transform_does_something)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t plain;
    lh_ui_entity_container_t box;
    lh_ui_canvas_t canvas;
    lh_ui_point_t point;

    lh_ui_rect_init(lh_addr_of(rect), 2, 3, 10, 10);
    lh_ui_entity_init(lh_addr_of(plain), rect);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_canvas_init(lh_addr_of(canvas), nullptr, nullptr);

    EXPECT_EQ(lh_ui_entity_push_children(lh_addr_of(plain), lh_addr_of(canvas)), lh_bool_false);
    EXPECT_EQ(lh_ui_entity_push_children(lh_ui_entity_container_as_entity(lh_addr_of(box)), nullptr),
              lh_bool_false);
    ASSERT_EQ(lh_ui_entity_push_children(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(canvas)),
              lh_bool_true);
    EXPECT_EQ(lh_ui_rect_eq(lh_ui_canvas_get_clip(lh_addr_of(canvas)), lh_addr_of(rect)), lh_bool_true);
    lh_ui_canvas_pop(lh_addr_of(canvas));

    lh_ui_point_init(lh_addr_of(point), 1, 1);
    point = lh_ui_entity_to_children_space(lh_addr_of(plain), point);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(point)), lh_ui_scalar(1));
}

TEST(entity, find_child_at_searches_children_last_first)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_point_t point;
    lh_ui_point_t local;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 50, 50);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(a));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(b));
    lh_ui_point_init(lh_addr_of(point), 10, 10);

    EXPECT_EQ(lh_ui_entity_find_child_at(lh_addr_of(root), point, lh_addr_of(local)), lh_addr_of(b));
    lh_ui_point_init(lh_addr_of(point), 70, 70);
    EXPECT_EQ(lh_ui_entity_find_child_at(lh_addr_of(root), point, lh_addr_of(local)), nullptr);
    EXPECT_EQ(lh_ui_entity_find_at_local(lh_addr_of(root), point, lh_addr_of(local)), lh_addr_of(root));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(local)), lh_ui_scalar(70));
}

TEST(entity, draw_children_draws_each_child_not_self)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_canvas_t canvas;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_entity_set_class(lh_addr_of(root), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(a), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(b), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(a));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(b));
    g_draw_order_n = 0;

    lh_ui_entity_draw_children(lh_addr_of(root), lh_addr_of(canvas));

    ASSERT_EQ(g_draw_order_n, 2);
    EXPECT_EQ(g_draw_order[0], lh_addr_of(a));
    EXPECT_EQ(g_draw_order[1], lh_addr_of(b));
}

TEST(entity, draw_keeps_a_child_outside_a_plain_parent_that_is_culled)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t parent;
    lh_ui_entity_t child;
    lh_ui_canvas_t canvas;
    lh_ui_point_t zero;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_rect_init(lh_addr_of(rect), 50, 50, 10, 10);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_set_class(lh_addr_of(parent), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(child), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_point_init(lh_addr_of(zero), 0, 0);
    lh_ui_rect_init(lh_addr_of(rect), 40, 40, 30, 30);
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(rect));
    g_draw_order_n = 0;

    lh_ui_entity_draw(lh_addr_of(parent), lh_addr_of(canvas));

    /* The parent is off the clip, its child is on it: only the child draws. */
    ASSERT_EQ(g_draw_order_n, 1);
    EXPECT_EQ(g_draw_order[0], lh_addr_of(child));
}

TEST(entity, draw_skips_the_children_of_a_clipping_parent_that_is_culled)
{
    lh_ui_rect_t rect;
    lh_ui_entity_container_t box;
    lh_ui_entity_t child;
    lh_ui_canvas_t canvas;
    lh_ui_point_t zero;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_rect_init(lh_addr_of(rect), 50, 50, 10, 10);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_set_class(lh_addr_of(child), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(child));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);
    lh_ui_point_init(lh_addr_of(zero), 0, 0);
    lh_ui_rect_init(lh_addr_of(rect), 40, 40, 30, 30);
    lh_ui_canvas_push(lh_addr_of(canvas), zero, lh_addr_of(rect));
    g_draw_order_n = 0;

    lh_ui_entity_draw(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(canvas));

    EXPECT_EQ(g_draw_order_n, 0);
}

TEST(entity, draw_without_a_canvas_still_sends_every_draw_event)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t child;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_set_class(lh_addr_of(root), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(child), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(child));
    g_draw_order_n = 0;

    lh_ui_entity_draw(lh_addr_of(root), nullptr);

    ASSERT_EQ(g_draw_order_n, 2);
    EXPECT_EQ(g_draw_order[0], lh_addr_of(root));
    EXPECT_EQ(g_draw_order[1], lh_addr_of(child));
}

TEST(entity, root_offset_and_local_follow_every_scrolled_ancestor)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_container_t box;
    lh_ui_entity_t leaf;
    lh_ui_entity_t tall;
    lh_ui_point_t scroll;
    lh_ui_point_t point;
    lh_ui_point_t offset;
    lh_ui_point_t local;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 50, 50);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 50, 50, 40);
    lh_ui_entity_init(lh_addr_of(leaf), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 50, 200);
    lh_ui_entity_init(lh_addr_of(tall), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_container_as_entity(lh_addr_of(box)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(tall));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(leaf));
    lh_ui_point_init(lh_addr_of(scroll), 0, 30);
    lh_ui_entity_container_set_scroll(lh_addr_of(box), scroll);

    offset = lh_ui_entity_get_root_offset(lh_addr_of(leaf));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(offset)), lh_ui_scalar(0));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(offset)), lh_ui_scalar(-30));
    offset = lh_ui_entity_get_root_offset(lh_addr_of(root));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(offset)), lh_ui_scalar(0));

    /* Same local point the hit test gives. */
    lh_ui_point_init(lh_addr_of(point), 20, 30);
    local = lh_ui_entity_to_local(lh_addr_of(leaf), point);
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(local)), lh_ui_scalar(20));
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(local)), lh_ui_scalar(60));
    point = lh_ui_entity_add_children_offset(lh_ui_entity_container_as_entity(lh_addr_of(box)), local);
    EXPECT_EQ(lh_ui_point_get_y(lh_addr_of(point)), lh_ui_scalar(30));

    rect = lh_ui_entity_get_root_rect(lh_addr_of(leaf));
    EXPECT_EQ(lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(rect))), lh_ui_scalar(20));
}

TEST(entity, add_damage_puts_the_rect_in_the_root_space)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_container_t box;
    lh_ui_entity_t leaf;
    lh_ui_entity_t tall;
    lh_ui_point_t scroll;
    lh_ui_canvas_t canvas;
    const lh_ui_rect_t *damage;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 50, 50);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 50, 50, 40);
    lh_ui_entity_init(lh_addr_of(leaf), rect);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 50, 200);
    lh_ui_entity_init(lh_addr_of(tall), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_container_as_entity(lh_addr_of(box)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(tall));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(leaf));
    lh_ui_point_init(lh_addr_of(scroll), 0, 30);
    lh_ui_entity_container_set_scroll(lh_addr_of(box), scroll);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_null), lh_null);

    lh_ui_entity_add_damage(lh_addr_of(leaf), lh_addr_of(canvas));

    damage = lh_ui_canvas_get_damage(lh_addr_of(canvas));
    ASSERT_NE(damage, nullptr);
    lh_ui_rect_init(lh_addr_of(rect), 10, 20, 50, 40);
    EXPECT_TRUE(lh_ui_rect_eq(damage, lh_addr_of(rect)));
}

TEST(entity, find_at_reaches_a_child_outside_a_plain_parent)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t parent;
    lh_ui_entity_t child;
    lh_ui_point_t point;
    lh_ui_point_t local;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_rect_init(lh_addr_of(rect), 50, 50, 10, 10);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(parent));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));

    lh_ui_point_init(lh_addr_of(point), 55, 55);
    EXPECT_EQ(lh_ui_entity_find_at_local(lh_addr_of(root), point, lh_addr_of(local)), lh_addr_of(child));
    EXPECT_EQ(lh_ui_point_get_x(lh_addr_of(local)), lh_ui_scalar(55));

    /* Off every rect, the parent is not hit just for being searched. */
    lh_ui_point_init(lh_addr_of(point), 30, 30);
    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(root));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_find_at(lh_addr_of(parent), point)));

    /* Hidden still hides the whole subtree. */
    lh_ui_entity_set_hidden(lh_addr_of(parent), lh_bool_true);
    lh_ui_point_init(lh_addr_of(point), 55, 55);
    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(root));
}

TEST(entity, find_at_does_not_reach_a_child_a_container_cuts_away)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_container_t box;
    lh_ui_entity_t child;
    lh_ui_point_t point;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 100, 100);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_entity_container_init(lh_addr_of(box), rect);
    lh_ui_rect_init(lh_addr_of(rect), 50, 50, 10, 10);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_ui_entity_container_as_entity(lh_addr_of(box)));
    lh_ui_entity_add_child(lh_ui_entity_container_as_entity(lh_addr_of(box)), lh_addr_of(child));

    lh_ui_point_init(lh_addr_of(point), 55, 55);
    EXPECT_EQ(lh_ui_entity_find_at(lh_addr_of(root), point), lh_addr_of(root));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(entity_death, add_ancestor_as_child_is_a_cycle)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t root;
    lh_ui_entity_t child;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(root), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_add_child(lh_addr_of(root), lh_addr_of(child));
    LH_EXPECT_DEATH(lh_ui_entity_add_child(lh_addr_of(child), lh_addr_of(root)));
}

TEST(entity_death, remove_someone_elses_child)
{
    lh_ui_rect_t rect;
    lh_ui_entity_t a;
    lh_ui_entity_t b;
    lh_ui_entity_t child;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(a), rect);
    lh_ui_entity_init(lh_addr_of(b), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_add_child(lh_addr_of(a), lh_addr_of(child));
    LH_EXPECT_DEATH(lh_ui_entity_remove_child(lh_addr_of(b), lh_addr_of(child)));
}

#endif
