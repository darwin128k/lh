#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/void.h>

namespace
{
const lh_ui_entity_t *g_draw_order[8];
int g_draw_order_n;

lh_void
record_draw(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    EXPECT_EQ(lh_ui_entity_event_get_code(event), lh_ui_entity_event_draw);
    ASSERT_LT(g_draw_order_n, 8);
    g_draw_order[g_draw_order_n] = self;
    ++g_draw_order_n;
}

const lh_ui_entity_class_t g_record_class = {record_draw,
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
    lh_ui_entity_draw(lh_addr_of(parent));

    ASSERT_EQ(g_draw_order_n, 3);
    EXPECT_EQ(g_draw_order[0], lh_addr_of(parent));
    EXPECT_EQ(g_draw_order[1], lh_addr_of(first));
    EXPECT_EQ(g_draw_order[2], lh_addr_of(second));
}
