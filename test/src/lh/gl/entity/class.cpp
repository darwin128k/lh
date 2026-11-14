#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/core/null.h>
#include <lh/gl/entity/class.h>
#include <lh/gl/entity/class/initializer.h>

namespace
{

lh_void
draw_nothing(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    (void)entity;
    (void)canvas;
}

TEST(gl_entity_class, init_stores_kind_and_draw_call)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_layer, draw_nothing);
    EXPECT_EQ(lh_gl_entity_class_get_kind(lh_addr_of(cls)), lh_gl_entity_class_kind_layer);
    EXPECT_EQ(lh_gl_entity_class_get_draw_cb(lh_addr_of(cls)), draw_nothing);
}

TEST(gl_entity_class, setters_replace_the_fields)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_class_set_kind(lh_addr_of(cls), lh_gl_entity_class_kind_layer);
    lh_gl_entity_class_set_draw_cb(lh_addr_of(cls), draw_nothing);
    EXPECT_EQ(lh_gl_entity_class_get_kind(lh_addr_of(cls)), lh_gl_entity_class_kind_layer);
    EXPECT_EQ(lh_gl_entity_class_get_draw_cb(lh_addr_of(cls)), draw_nothing);
}

TEST(gl_entity_class, is_layer_follows_the_kind)
{
    lh_gl_entity_class_t entity_class;
    lh_gl_entity_class_t layer_class;

    lh_gl_entity_class_init(lh_addr_of(entity_class), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_class_init(lh_addr_of(layer_class), lh_gl_entity_class_kind_layer, lh_null);
    EXPECT_FALSE(lh_gl_entity_class_is_layer(lh_addr_of(entity_class)));
    EXPECT_TRUE(lh_gl_entity_class_is_layer(lh_addr_of(layer_class)));
}

TEST(gl_entity_class, initializer_fills_fields_in_order)
{
    const lh_gl_entity_class_t cls =
        lh_gl_entity_class_initializer(lh_gl_entity_class_kind_layer, draw_nothing);

    EXPECT_EQ(lh_gl_entity_class_get_kind(lh_addr_of(cls)), lh_gl_entity_class_kind_layer);
    EXPECT_EQ(lh_gl_entity_class_get_draw_cb(lh_addr_of(cls)), draw_nothing);
}

} // namespace