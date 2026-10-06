#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
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
} // namespace

TEST(entity, make_keeps_the_rect)
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
