#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
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

lh_void
record_draw(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    EXPECT_EQ(lh_ui_entity_event_get_code(event), lh_ui_entity_event_draw);
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

const lh_ui_canvas_backend_t g_fill_log_backend = {nullptr, nullptr, nullptr, log_fill_rect};

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

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(first), rect);
    lh_ui_entity_init(lh_addr_of(second), rect);
    lh_ui_entity_set_class(lh_addr_of(parent), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(first), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(second), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(first));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(second));

    g_draw_order_n = 0;
    lh_ui_entity_draw(lh_addr_of(parent), static_cast<lh_ui_canvas_t *>(nullptr));

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

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_init(lh_addr_of(parent), rect);
    lh_ui_entity_init(lh_addr_of(child), rect);
    lh_ui_entity_set_class(lh_addr_of(parent), lh_addr_of(g_record_class));
    lh_ui_entity_set_class(lh_addr_of(child), lh_addr_of(g_record_class));
    lh_ui_entity_add_child(lh_addr_of(parent), lh_addr_of(child));
    lh_ui_entity_set_hidden(lh_addr_of(parent), lh_bool_true);

    g_draw_order_n = 0;
    lh_ui_entity_draw(lh_addr_of(parent), static_cast<lh_ui_canvas_t *>(nullptr));
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
