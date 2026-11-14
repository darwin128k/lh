#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/core/null.h>
#include <lh/gl/entity.h>
#include <lh/gl/entity/initializer.h>
#include <lh/gl/layer.h>
#include <lh/gl/point/initializer.h>
#include <lh/gl/rect/initializer.h>
#include <lh/gl/size/initializer.h>

struct lh_gl_canvas
{
    int tag;
};

namespace
{

const lh_gl_entity_t *drawn_entity = nullptr;
lh_gl_canvas_t *drawn_canvas = nullptr;
int draw_calls = 0;

lh_void
record_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    drawn_entity = entity;
    drawn_canvas = canvas;
    ++draw_calls;
}

lh_gl_rect_t
rect_of(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

bool
has_rect(const lh_gl_entity_t *entity, lh_math_scalar_t x, lh_math_scalar_t y,
         lh_math_scalar_t width, lh_math_scalar_t height)
{
    const lh_gl_rect_t expected = rect_of(x, y, width, height);

    return lh_gl_rect_eq(lh_gl_entity_get_rect(entity), lh_addr_of(expected));
}

TEST(gl_entity, init_stores_rect_class_and_no_parent)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(1, 2, 30, 40), lh_addr_of(cls));
    EXPECT_TRUE(has_rect(lh_addr_of(entity), 1, 2, 30, 40));
    EXPECT_EQ(lh_gl_entity_get_class(lh_addr_of(entity)), lh_addr_of(cls));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(entity)), nullptr);
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(entity)));
    EXPECT_FALSE(lh_list_node_is_linked(lh_gl_entity_get_node(lh_addr_of(entity))));
}

TEST(gl_entity, set_rect_replaces_the_area)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_set_rect(lh_addr_of(entity), rect_of(5, 6, 7, 8));
    EXPECT_TRUE(has_rect(lh_addr_of(entity), 5, 6, 7, 8));
}

TEST(gl_entity, get_rect_mut_edits_in_place)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 10, 10), lh_addr_of(cls));
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 3, 4);
    lh_gl_rect_set_size(lh_gl_entity_get_rect_mut(lh_addr_of(entity)), size);
    EXPECT_TRUE(has_rect(lh_addr_of(entity), 0, 0, 3, 4));
}

TEST(gl_entity, parent_is_stored_and_cleared)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 100, 100));
    lh_gl_entity_set_parent(lh_addr_of(entity), lh_addr_of(layer));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(entity)), lh_addr_of(layer));
    EXPECT_FALSE(lh_gl_entity_is_root(lh_addr_of(entity)));
    lh_gl_entity_set_parent(lh_addr_of(entity), lh_null);
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(entity)));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_entity, set_class_replaces_the_class)
{
    lh_gl_entity_class_t first;
    lh_gl_entity_class_t second;

    lh_gl_entity_class_init(lh_addr_of(first), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_class_init(lh_addr_of(second), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(first));
    lh_gl_entity_set_class(lh_addr_of(entity), lh_addr_of(second));
    EXPECT_EQ(lh_gl_entity_get_class(lh_addr_of(entity)), lh_addr_of(second));
}

TEST(gl_entity, kind_and_is_layer_come_from_the_class)
{
    lh_gl_entity_class_t entity_class;

    lh_gl_entity_class_init(lh_addr_of(entity_class), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_t plain;

    lh_gl_entity_init(lh_addr_of(plain), rect_of(0, 0, 1, 1), lh_addr_of(entity_class));
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 1, 1));
    EXPECT_EQ(lh_gl_entity_get_kind(lh_addr_of(plain)), lh_gl_entity_class_kind_entity);
    EXPECT_FALSE(lh_gl_entity_is_layer(lh_addr_of(plain)));
    EXPECT_EQ(lh_gl_entity_get_kind(lh_gl_layer_get_entity(lh_addr_of(layer))),
              lh_gl_entity_class_kind_layer);
    EXPECT_TRUE(lh_gl_entity_is_layer(lh_gl_layer_get_entity(lh_addr_of(layer))));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_entity, draw_calls_the_class_with_entity_and_canvas)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_canvas canvas = {1};

    draw_calls = 0;
    lh_gl_entity_draw(lh_addr_of(entity), lh_addr_of(canvas));
    EXPECT_EQ(draw_calls, 1);
    EXPECT_EQ(drawn_entity, lh_addr_of(entity));
    EXPECT_EQ(drawn_canvas, lh_addr_of(canvas));
}

TEST(gl_entity, draw_without_a_call_does_nothing)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_entity_t entity;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_canvas canvas = {1};

    draw_calls = 0;
    lh_gl_entity_draw(lh_addr_of(entity), lh_addr_of(canvas));
    EXPECT_EQ(draw_calls, 0);
}

TEST(gl_entity, initializer_fills_fields_in_order)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    const lh_gl_entity_t entity = lh_gl_entity_initializer(
        lh_gl_rect_initializer(lh_gl_point_initializer(1, 2), lh_gl_size_initializer(3, 4)),
        nullptr, lh_addr_of(cls), lh_list_node_t{});

    EXPECT_TRUE(has_rect(lh_addr_of(entity), 1, 2, 3, 4));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(entity)));
    EXPECT_EQ(lh_gl_entity_get_class(lh_addr_of(entity)), lh_addr_of(cls));
}

} // namespace