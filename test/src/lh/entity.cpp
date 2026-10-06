#include <gtest/gtest.h>

#include <lh/entity.h>
#include <lh/math/rect.h>
#include <lh/util/addr.h>

TEST(entity, make_keeps_the_rect)
{
    const lh_math_rect_t rect = lh_math_rect_make(1, 2, 3, 4);
    lh_entity_t entity;
    lh_entity_init(lh_addr_of(entity), rect);
    const lh_math_rect_t stored = lh_entity_get_rect(lh_addr_of(entity));

    EXPECT_EQ(lh_math_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_entity_get_class(lh_addr_of(entity)), lh_addr_of(lh_entity_class));
}
