#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/layout.h>
#include <lh/gl/layer.h>
#include <lh/gl/surface.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

lh_gl_scalar_t
origin_x(const lh_gl_entity_t *entity)
{
    return lh_gl_point_get_x(lh_gl_rect_get_origin(lh_gl_entity_get_rect(entity)));
}

lh_gl_scalar_t
origin_y(const lh_gl_entity_t *entity)
{
    return lh_gl_point_get_y(lh_gl_rect_get_origin(lh_gl_entity_get_rect(entity)));
}

lh_gl_scalar_t
size_width(const lh_gl_entity_t *entity)
{
    return lh_gl_size_get_width(lh_gl_rect_get_size(lh_gl_entity_get_rect(entity)));
}

TEST(gl_layout, pack_writes_one_axis_and_leaves_size)
{
    lh_gl_entity_class_t plain;
    lh_gl_entity_class_init(lh_addr_of(plain), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 100, 100));
    lh_gl_entity_t first;
    lh_gl_entity_init(lh_addr_of(first), rect_of(3, 4, 10, 8), lh_addr_of(plain));
    lh_gl_entity_t second;
    lh_gl_entity_init(lh_addr_of(second), rect_of(3, 5, 20, 8), lh_addr_of(plain));
    lh_gl_entity_t third;
    lh_gl_entity_init(lh_addr_of(third), rect_of(3, 6, 5, 9), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(first));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(second));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(third));
    lh_gl_layout_t layout;
    lh_gl_layout_init(lh_addr_of(layout), 2);
    lh_gl_layout_pack(lh_addr_of(layout), lh_addr_of(layer), lh_bool_true);
    EXPECT_EQ(origin_x(lh_addr_of(first)), 0);
    EXPECT_EQ(origin_x(lh_addr_of(second)), 12);
    EXPECT_EQ(origin_x(lh_addr_of(third)), 34);
    EXPECT_EQ(origin_y(lh_addr_of(first)), 4);
    EXPECT_EQ(origin_y(lh_addr_of(second)), 5);
    EXPECT_EQ(origin_y(lh_addr_of(third)), 6);
    EXPECT_EQ(size_width(lh_addr_of(first)), 10);
    EXPECT_EQ(size_width(lh_addr_of(second)), 20);
    lh_gl_layout_pack(lh_addr_of(layout), lh_addr_of(layer), lh_bool_false);
    EXPECT_EQ(origin_y(lh_addr_of(first)), 0);
    EXPECT_EQ(origin_y(lh_addr_of(second)), 10);
    EXPECT_EQ(origin_y(lh_addr_of(third)), 20);
    EXPECT_EQ(origin_x(lh_addr_of(first)), 0);
    EXPECT_EQ(origin_x(lh_addr_of(third)), 34);
    lh_gl_layout_set_gap(lh_addr_of(layout), -2);
    lh_gl_layout_pack(lh_addr_of(layout), lh_addr_of(layer), lh_bool_true);
    EXPECT_EQ(origin_x(lh_addr_of(second)), 8);
    lh_gl_layout_pack(lh_addr_of(layout), lh_null, lh_bool_true);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layout, pack_invalidates_through_set_origin)
{
    lh_gl_entity_class_t plain;
    lh_gl_entity_class_init(lh_addr_of(plain), lh_gl_entity_class_kind_entity, lh_null);
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 40, 40));
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 40, 40);
    lh_gl_surface_t surface;
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(layer));
    lh_gl_entity_t child;
    lh_gl_entity_init(lh_addr_of(child), rect_of(1, 1, 10, 10), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(child));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_layout_t layout;
    lh_gl_layout_init(lh_addr_of(layout), 0);
    lh_gl_layout_pack(lh_addr_of(layout), lh_addr_of(layer), lh_bool_false);
    EXPECT_EQ(origin_y(lh_addr_of(child)), 0);
    EXPECT_GT(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
