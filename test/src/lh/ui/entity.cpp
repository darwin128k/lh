#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

TEST(entity, make_keeps_the_rect)
{
    const lh_ui_rect_t rect = lh_ui_rect_make(1, 2, 3, 4);
    lh_ui_entity_t entity;
    lh_ui_entity_init(lh_addr_of(entity), rect);
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(lh_addr_of(entity));

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_class(lh_addr_of(entity)), lh_addr_of(lh_ui_entity_class));
}

TEST(entity, init_starts_with_no_style)
{
    lh_ui_entity_t entity;
    lh_ui_entity_init(lh_addr_of(entity), lh_ui_rect_make(0, 0, 1, 1));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_style(lh_addr_of(entity))));
}

TEST(entity, set_style_keeps_the_pointer)
{
    lh_ui_entity_t entity;
    const lh_ui_style_t style = lh_ui_style_make_empty();
    lh_ui_entity_init(lh_addr_of(entity), lh_ui_rect_make(0, 0, 1, 1));
    lh_ui_entity_set_style(lh_addr_of(entity), lh_addr_of(style));
    EXPECT_EQ(lh_ui_entity_get_style(lh_addr_of(entity)), lh_addr_of(style));
    lh_ui_entity_set_style(lh_addr_of(entity), lh_ptr_rcast(const lh_ui_style_t, lh_null));
    EXPECT_TRUE(lh_null_eq(lh_ui_entity_get_style(lh_addr_of(entity))));
}
