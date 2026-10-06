#include <gtest/gtest.h>

#include <lh/ui/entity.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>

TEST(entity, make_keeps_the_rect)
{
    const lh_ui_rect_t rect = lh_ui_rect_make(1, 2, 3, 4);
    lh_ui_entity_t entity;
    lh_ui_entity_init(lh_addr_of(entity), rect);
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(lh_addr_of(entity));

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_get_class(lh_addr_of(entity)), lh_addr_of(lh_ui_entity_class));
}
