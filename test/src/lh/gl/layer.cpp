#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/expect/death.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/initializer.h>
#include <lh/gl/layer.h>
#include <lh/gl/layer/initializer.h>
#include <lh/gl/point/initializer.h>
#include <lh/gl/rect/initializer.h>
#include <lh/gl/size/initializer.h>
#include <lh/gl/surface.h>

namespace
{

const lh_gl_entity_t *draw_entities[8];
lh_gl_rect_t draw_rects[8];
lh_gl_rect_t draw_clips[8];
lh_gl_point_t draw_origins[8];
lh_usize_t draw_count;

lh_void
record_layer_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_usize_t index = draw_count++;
    draw_entities[index] = entity;
    lh_gl_entity_get_absolute_rect(entity, lh_addr_of(draw_rects[index]));
    draw_clips[index] = *lh_gl_canvas_get_clip(canvas);
    draw_origins[index] = *lh_gl_canvas_get_origin(canvas);
}

lh_gl_rect_t
rect_of(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

lh_gl_entity_class_t
plain_class()
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    return cls;
}

TEST(gl_layer, init_makes_an_empty_root_layer)
{
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 100, 50));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 0U);
    EXPECT_TRUE(lh_gl_layer_is_empty(lh_addr_of(layer)));
    EXPECT_TRUE(lh_gl_entity_is_layer(lh_gl_layer_get_entity(lh_addr_of(layer))));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_gl_layer_get_entity(lh_addr_of(layer))));
    EXPECT_EQ(lh_gl_entity_get_class(lh_gl_layer_get_entity(lh_addr_of(layer))),
              lh_gl_layer_get_class());
    EXPECT_FALSE(
        lh_list_node_is_linked(lh_gl_entity_get_node(lh_gl_layer_get_entity(lh_addr_of(layer)))));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, entity_is_the_first_field)
{
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 1, 1));
    EXPECT_EQ(static_cast<const void *>(lh_gl_layer_get_entity(lh_addr_of(layer))),
              static_cast<const void *>(lh_addr_of(layer)));
    EXPECT_EQ(lh_gl_layer_get_entity_mut(lh_addr_of(layer)),
              lh_gl_layer_get_entity(lh_addr_of(layer)));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, try_add_appends_to_the_front_and_sets_the_parent)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t back;
    lh_gl_entity_t front;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(back), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(front), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    EXPECT_TRUE(lh_gl_layer_try_add(lh_addr_of(layer), lh_addr_of(back)));
    EXPECT_TRUE(lh_gl_layer_try_add(lh_addr_of(layer), lh_addr_of(front)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 2U);
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 0), lh_addr_of(back));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 1), lh_addr_of(front));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(back)), lh_addr_of(layer));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(front)), lh_addr_of(layer));
    EXPECT_EQ(lh_gl_entity_from_node(lh_list_get_first(lh_gl_layer_get_items(lh_addr_of(layer)))),
              lh_addr_of(back));
    EXPECT_EQ(lh_gl_entity_from_node(lh_list_get_last(lh_gl_layer_get_items(lh_addr_of(layer)))),
              lh_addr_of(front));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, try_add_refuses_an_entity_that_already_has_a_parent)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t entity;
    lh_gl_layer_t a;
    lh_gl_layer_t b;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(a), rect_of(0, 0, 10, 10));
    lh_gl_layer_init(lh_addr_of(b), rect_of(0, 0, 10, 10));
    EXPECT_TRUE(lh_gl_layer_try_add(lh_addr_of(a), lh_addr_of(entity)));
    EXPECT_FALSE(lh_gl_layer_try_add(lh_addr_of(b), lh_addr_of(entity)));
    EXPECT_FALSE(lh_gl_layer_try_add(lh_addr_of(a), lh_addr_of(entity)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(a)), 1U);
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(b)), 0U);
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(entity)), lh_addr_of(a));
    lh_gl_layer_deinit(lh_addr_of(a));
    lh_gl_layer_deinit(lh_addr_of(b));
}

TEST(gl_layer, a_layer_can_be_put_into_a_layer)
{
    lh_gl_layer_t outer;
    lh_gl_layer_t inner;

    lh_gl_layer_init(lh_addr_of(outer), rect_of(0, 0, 100, 100));
    lh_gl_layer_init(lh_addr_of(inner), rect_of(10, 10, 50, 50));
    EXPECT_TRUE(
        lh_gl_layer_try_add(lh_addr_of(outer), lh_gl_layer_get_entity_mut(lh_addr_of(inner))));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_gl_layer_get_entity(lh_addr_of(inner))),
              lh_addr_of(outer));
    EXPECT_TRUE(lh_gl_entity_is_layer(lh_gl_layer_get_item(lh_addr_of(outer), 0)));
    lh_gl_layer_deinit(lh_addr_of(outer));
    lh_gl_layer_deinit(lh_addr_of(inner));
}

TEST(gl_layer, try_add_refuses_the_layer_itself_and_its_ancestors)
{
    lh_gl_layer_t outer;
    lh_gl_layer_t inner;

    lh_gl_layer_init(lh_addr_of(outer), rect_of(0, 0, 100, 100));
    lh_gl_layer_init(lh_addr_of(inner), rect_of(0, 0, 50, 50));
    EXPECT_TRUE(
        lh_gl_layer_try_add(lh_addr_of(outer), lh_gl_layer_get_entity_mut(lh_addr_of(inner))));
    EXPECT_FALSE(
        lh_gl_layer_try_add(lh_addr_of(outer), lh_gl_layer_get_entity_mut(lh_addr_of(outer))));
    EXPECT_FALSE(
        lh_gl_layer_try_add(lh_addr_of(inner), lh_gl_layer_get_entity_mut(lh_addr_of(outer))));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(inner)), 0U);
    lh_gl_layer_deinit(lh_addr_of(outer));
    lh_gl_layer_deinit(lh_addr_of(inner));
}

TEST(gl_layer, is_within_walks_up_the_parents)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t stranger;
    lh_gl_layer_t outer;
    lh_gl_layer_t inner;

    lh_gl_entity_init(lh_addr_of(stranger), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(outer), rect_of(0, 0, 100, 100));
    lh_gl_layer_init(lh_addr_of(inner), rect_of(0, 0, 50, 50));
    lh_gl_layer_add(lh_addr_of(outer), lh_gl_layer_get_entity_mut(lh_addr_of(inner)));
    EXPECT_TRUE(
        lh_gl_layer_is_within(lh_addr_of(inner), lh_gl_layer_get_entity(lh_addr_of(inner))));
    EXPECT_TRUE(
        lh_gl_layer_is_within(lh_addr_of(inner), lh_gl_layer_get_entity(lh_addr_of(outer))));
    EXPECT_FALSE(
        lh_gl_layer_is_within(lh_addr_of(outer), lh_gl_layer_get_entity(lh_addr_of(inner))));
    EXPECT_FALSE(lh_gl_layer_is_within(lh_addr_of(inner), lh_addr_of(stranger)));
    lh_gl_layer_deinit(lh_addr_of(outer));
    lh_gl_layer_deinit(lh_addr_of(inner));
}

TEST(gl_layer, try_find_reports_the_index_of_a_child)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t first;
    lh_gl_entity_t second;
    lh_gl_entity_t stranger;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(first), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(second), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(stranger), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(first));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(second));
    lh_uindex_t index = 99;

    EXPECT_TRUE(lh_gl_layer_try_find(lh_addr_of(layer), lh_addr_of(second), lh_addr_of(index)));
    EXPECT_EQ(index, 1U);
    index = 99;
    EXPECT_FALSE(lh_gl_layer_try_find(lh_addr_of(layer), lh_addr_of(stranger), lh_addr_of(index)));
    EXPECT_EQ(index, 99U);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, is_valid_index_is_below_the_count)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t entity;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(entity), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    EXPECT_FALSE(lh_gl_layer_is_valid_index(lh_addr_of(layer), 0));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(entity));
    EXPECT_TRUE(lh_gl_layer_is_valid_index(lh_addr_of(layer), 0));
    EXPECT_FALSE(lh_gl_layer_is_valid_index(lh_addr_of(layer), 1));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, try_remove_keeps_the_order_and_clears_the_parent)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t a;
    lh_gl_entity_t b;
    lh_gl_entity_t c;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(a), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(b), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(c), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(a));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(b));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(c));
    EXPECT_TRUE(lh_gl_layer_try_remove(lh_addr_of(layer), lh_addr_of(b)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 2U);
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 0), lh_addr_of(a));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 1), lh_addr_of(c));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(b)));
    EXPECT_FALSE(lh_list_node_is_linked(lh_gl_entity_get_node(lh_addr_of(b))));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(a)), lh_addr_of(layer));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, try_remove_of_a_stranger_changes_nothing)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t child;
    lh_gl_entity_t stranger;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(stranger), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(child));
    EXPECT_FALSE(lh_gl_layer_try_remove(lh_addr_of(layer), lh_addr_of(stranger)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 1U);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, remove_at_the_last_child_empties_the_layer)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t child;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(child));
    lh_gl_layer_remove_at(lh_addr_of(layer), 0);
    EXPECT_TRUE(lh_gl_layer_is_empty(lh_addr_of(layer)));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(child)));
    EXPECT_FALSE(lh_list_node_is_linked(lh_gl_entity_get_node(lh_addr_of(child))));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, a_removed_entity_can_be_added_again)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t child;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(child));
    ASSERT_TRUE(lh_gl_layer_try_remove(lh_addr_of(layer), lh_addr_of(child)));
    EXPECT_TRUE(lh_gl_layer_try_add(lh_addr_of(layer), lh_addr_of(child)));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, items_are_level_one_accessors)
{
    lh_gl_layer_t layer;

    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    EXPECT_TRUE(lh_list_is_empty(lh_gl_layer_get_items(lh_addr_of(layer))));
    EXPECT_EQ(lh_gl_layer_get_items_mut(lh_addr_of(layer)),
              lh_gl_layer_get_items(lh_addr_of(layer)));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, initializer_fills_fields_in_order)
{
    lh_gl_layer_t layer = lh_gl_layer_initializer(
        lh_gl_entity_initializer(
            lh_gl_rect_initializer(lh_gl_point_initializer(1, 2), lh_gl_size_initializer(3, 4)),
            nullptr, lh_gl_layer_get_class(), lh_list_node_t{}),
        lh_list_t{});

    lh_list_init(lh_gl_layer_get_items_mut(lh_addr_of(layer)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 0U);
    EXPECT_TRUE(lh_gl_entity_is_layer(lh_gl_layer_get_entity(lh_addr_of(layer))));
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(
                  lh_gl_entity_get_rect(lh_gl_layer_get_entity(lh_addr_of(layer))))),
              1);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, deinit_clears_remaining_parents)
{
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t a;
    lh_gl_entity_t b;
    lh_gl_layer_t layer;

    lh_gl_entity_init(lh_addr_of(a), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(b), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(a));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(b));
    lh_gl_layer_deinit(lh_addr_of(layer));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(a)));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(b)));
    EXPECT_FALSE(lh_list_node_is_linked(lh_gl_entity_get_node(lh_addr_of(a))));
    EXPECT_FALSE(lh_list_node_is_linked(lh_gl_entity_get_node(lh_addr_of(b))));
}

TEST(gl_layer, dirty_propagates_through_nested_layers_and_set_rect)
{
    lh_gl_layer_t root;
    lh_gl_layer_t nested;
    lh_gl_entity_t child;
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_surface_t surface;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 100, 100);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_layer_init(lh_addr_of(nested), rect_of(10, 20, 20, 20));
    lh_gl_entity_init(lh_addr_of(child), rect_of(15, 15, 10, 10), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(root), lh_gl_layer_get_entity_mut(lh_addr_of(nested)));
    lh_gl_layer_add(lh_addr_of(nested), lh_addr_of(child));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_entity_invalidate(lh_addr_of(child));
    lh_gl_rect_t expected = rect_of(25, 35, 5, 5);
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_entity_set_rect(lh_addr_of(child), rect_of(0, 0, 2, 2));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 2U);
    lh_gl_layer_deinit(lh_addr_of(root));
    lh_gl_layer_deinit(lh_addr_of(nested));
}

TEST(gl_layer, mutations_invalidate_and_remove_checks_owner)
{
    lh_gl_layer_t root;
    lh_gl_layer_t other;
    lh_gl_entity_t child;
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_surface_t surface;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 100, 100);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 1, 1));
    lh_gl_layer_init(lh_addr_of(other), rect_of(0, 0, 100, 100));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_entity_init(lh_addr_of(child), rect_of(10, 20, 5, 6), lh_addr_of(cls));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_rect_t expected = rect_of(10, 20, 5, 6);
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    EXPECT_FALSE(lh_gl_layer_try_remove(lh_addr_of(other), lh_addr_of(child)));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    EXPECT_TRUE(lh_gl_layer_try_remove(lh_addr_of(root), lh_addr_of(child)));
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_layer_deinit(lh_addr_of(root));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_EQ(lh_gl_surface_get_root(lh_addr_of(surface)), nullptr);
    lh_gl_layer_deinit(lh_addr_of(other));
}

TEST(gl_layer, raise_and_lower_change_order_and_invalidate)
{
    lh_gl_layer_t layer;
    lh_gl_surface_t surface;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 100, 100);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 100, 100));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(layer));
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t a;
    lh_gl_entity_t b;
    lh_gl_entity_t c;
    lh_gl_entity_init(lh_addr_of(a), rect_of(1, 2, 3, 4), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(b), rect_of(10, 20, 3, 4), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(c), rect_of(30, 40, 3, 4), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(a));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(b));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(c));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_layer_raise(lh_addr_of(layer), lh_addr_of(a));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 0), lh_addr_of(b));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 2), lh_addr_of(a));
    lh_gl_rect_t expected = rect_of(1, 2, 3, 4);
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_layer_raise(lh_addr_of(layer), lh_addr_of(a));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_layer_lower(lh_addr_of(layer), lh_addr_of(c));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 0), lh_addr_of(c));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 1), lh_addr_of(b));
    expected = rect_of(30, 40, 3, 4);
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_layer_lower(lh_addr_of(layer), lh_addr_of(c));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_entity_t stranger;
    lh_gl_entity_init(lh_addr_of(stranger), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    LH_EXPECT_CHECK_DEATH(lh_gl_layer_raise(lh_addr_of(layer), lh_addr_of(stranger)));
    LH_EXPECT_CHECK_DEATH(lh_gl_layer_lower(lh_addr_of(layer), lh_addr_of(stranger)));
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_layer, insert_before_and_after_and_refuse_invalid_members)
{
    lh_gl_layer_t layer;
    lh_gl_layer_t other;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 100, 100));
    lh_gl_layer_init(lh_addr_of(other), rect_of(0, 0, 100, 100));
    lh_gl_surface_t surface;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 100, 100);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(layer));
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t a;
    lh_gl_entity_t b;
    lh_gl_entity_t c;
    lh_gl_entity_t stranger;
    lh_gl_entity_init(lh_addr_of(a), rect_of(1, 1, 2, 2), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(b), rect_of(10, 10, 2, 2), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(c), rect_of(20, 20, 2, 2), lh_addr_of(cls));
    lh_gl_entity_init(lh_addr_of(stranger), rect_of(30, 30, 2, 2), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(b));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    EXPECT_TRUE(lh_gl_layer_try_insert_before(lh_addr_of(layer), lh_addr_of(a), lh_addr_of(b)));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 0), lh_addr_of(a));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_addr_of(a)), lh_addr_of(layer));
    lh_gl_rect_t expected = rect_of(1, 1, 2, 2);
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    EXPECT_TRUE(lh_gl_layer_try_insert_after(lh_addr_of(layer), lh_addr_of(c), lh_addr_of(a)));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 1), lh_addr_of(c));
    EXPECT_EQ(lh_gl_layer_get_item(lh_addr_of(layer), 2), lh_addr_of(b));
    expected = rect_of(20, 20, 2, 2);
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    EXPECT_FALSE(lh_gl_layer_try_insert_before(lh_addr_of(layer), lh_addr_of(stranger),
                                               lh_addr_of(stranger)));
    EXPECT_FALSE(
        lh_gl_layer_try_insert_after(lh_addr_of(other), lh_addr_of(stranger), lh_addr_of(b)));
    EXPECT_FALSE(lh_gl_layer_try_insert_before(lh_addr_of(layer), lh_addr_of(c), lh_addr_of(b)));
    EXPECT_FALSE(lh_gl_layer_try_insert_after(lh_addr_of(layer), lh_addr_of(a), lh_addr_of(a)));
    EXPECT_TRUE(lh_gl_entity_is_root(lh_addr_of(stranger)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 3U);
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_layer_deinit(lh_addr_of(layer));
    lh_gl_layer_deinit(lh_addr_of(other));
}

TEST(gl_layer, insert_refuses_ancestor)
{
    lh_gl_layer_t root;
    lh_gl_layer_t nested;
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 10, 10));
    lh_gl_layer_init(lh_addr_of(nested), rect_of(0, 0, 10, 10));
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t sibling;
    lh_gl_entity_init(lh_addr_of(sibling), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(root), lh_gl_layer_get_entity_mut(lh_addr_of(nested)));
    lh_gl_layer_add(lh_addr_of(nested), lh_addr_of(sibling));
    EXPECT_FALSE(lh_gl_layer_try_insert_before(
        lh_addr_of(nested), lh_gl_layer_get_entity_mut(lh_addr_of(root)), lh_addr_of(sibling)));
    EXPECT_FALSE(lh_gl_layer_try_insert_after(
        lh_addr_of(nested), lh_gl_layer_get_entity_mut(lh_addr_of(root)), lh_addr_of(sibling)));
    lh_gl_layer_deinit(lh_addr_of(root));
    lh_gl_layer_deinit(lh_addr_of(nested));
}

TEST(gl_layer, insert_refuses_root_attached_to_surface)
{
    lh_gl_layer_t layer;
    lh_gl_layer_t attached;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 10, 10));
    lh_gl_layer_init(lh_addr_of(attached), rect_of(0, 0, 10, 10));
    lh_gl_surface_t surface;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 10, 10);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(attached));
    lh_gl_entity_class_t cls = plain_class();
    lh_gl_entity_t sibling;
    lh_gl_entity_init(lh_addr_of(sibling), rect_of(0, 0, 1, 1), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(sibling));
    EXPECT_FALSE(lh_gl_layer_try_insert_before(
        lh_addr_of(layer), lh_gl_layer_get_entity_mut(lh_addr_of(attached)), lh_addr_of(sibling)));
    EXPECT_FALSE(lh_gl_layer_try_insert_after(
        lh_addr_of(layer), lh_gl_layer_get_entity_mut(lh_addr_of(attached)), lh_addr_of(sibling)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(layer)), 1U);
    EXPECT_EQ(lh_gl_surface_get_root(lh_addr_of(surface)), lh_addr_of(attached));
    lh_gl_layer_deinit(lh_addr_of(layer));
    lh_gl_layer_deinit(lh_addr_of(attached));
}

TEST(gl_layer, draw_visits_own_callback_then_children_with_clipped_canvas)
{
    lh_gl_entity_class_t layer_class;
    lh_gl_entity_class_init(lh_addr_of(layer_class), lh_gl_entity_class_kind_layer,
                            record_layer_draw);
    lh_gl_entity_class_t entity_class;
    lh_gl_entity_class_init(lh_addr_of(entity_class), lh_gl_entity_class_kind_entity,
                            record_layer_draw);
    lh_gl_layer_t root;
    lh_gl_layer_t nested;
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 80, 80));
    lh_gl_layer_init(lh_addr_of(nested), rect_of(10, 10, 20, 20));
    lh_gl_entity_set_class(lh_gl_layer_get_entity_mut(lh_addr_of(root)), lh_addr_of(layer_class));
    lh_gl_entity_set_class(lh_gl_layer_get_entity_mut(lh_addr_of(nested)), lh_addr_of(layer_class));
    lh_gl_entity_t back;
    lh_gl_entity_t inner;
    lh_gl_entity_t outside;
    lh_gl_entity_t front;
    lh_gl_entity_init(lh_addr_of(back), rect_of(0, 0, 15, 15), lh_addr_of(entity_class));
    lh_gl_entity_init(lh_addr_of(inner), rect_of(15, 15, 10, 10), lh_addr_of(entity_class));
    lh_gl_entity_init(lh_addr_of(outside), rect_of(60, 60, 5, 5), lh_addr_of(entity_class));
    lh_gl_entity_init(lh_addr_of(front), rect_of(5, 5, 8, 8), lh_addr_of(entity_class));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(back));
    lh_gl_layer_add(lh_addr_of(root), lh_gl_layer_get_entity_mut(lh_addr_of(nested)));
    lh_gl_layer_add(lh_addr_of(nested), lh_addr_of(inner));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(outside));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(front));
    lh_gl_point_t original_origin;
    lh_gl_point_init(lh_addr_of(original_origin), 77, 88);
    lh_gl_rect_t original_clip = rect_of(40, 40, 2, 2);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), original_origin, original_clip);
    lh_gl_rect_t dirty = rect_of(5, 5, 50, 50);
    draw_count = 0;
    lh_gl_layer_draw(lh_addr_of(root), lh_addr_of(canvas), lh_addr_of(dirty));
    ASSERT_EQ(draw_count, 5U);
    EXPECT_EQ(draw_entities[0], lh_gl_layer_get_entity(lh_addr_of(root)));
    EXPECT_EQ(draw_entities[1], lh_addr_of(back));
    EXPECT_EQ(draw_entities[2], lh_gl_layer_get_entity(lh_addr_of(nested)));
    EXPECT_EQ(draw_entities[3], lh_addr_of(inner));
    EXPECT_EQ(draw_entities[4], lh_addr_of(front));
    lh_gl_rect_t expected = rect_of(25, 25, 10, 10);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(draw_rects[3]), lh_addr_of(expected)));
    expected = rect_of(25, 25, 5, 5);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(draw_clips[3]), lh_addr_of(expected)));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(draw_origins[3])), 25);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(draw_origins[3])), 25);
    EXPECT_TRUE(
        lh_gl_point_eq(lh_gl_canvas_get_origin(lh_addr_of(canvas)), lh_addr_of(original_origin)));
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_canvas_get_clip(lh_addr_of(canvas)), lh_addr_of(original_clip)));
    draw_count = 0;
    lh_gl_layer_draw(lh_addr_of(nested), lh_addr_of(canvas), lh_addr_of(dirty));
    ASSERT_EQ(draw_count, 2U);
    EXPECT_EQ(draw_entities[0], lh_gl_layer_get_entity(lh_addr_of(nested)));
    EXPECT_EQ(draw_entities[1], lh_addr_of(inner));
    expected = rect_of(25, 25, 5, 5);
    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(draw_clips[1]), lh_addr_of(expected)));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(draw_origins[1])), 25);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(draw_origins[1])), 25);
    EXPECT_TRUE(
        lh_gl_point_eq(lh_gl_canvas_get_origin(lh_addr_of(canvas)), lh_addr_of(original_origin)));
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_canvas_get_clip(lh_addr_of(canvas)), lh_addr_of(original_clip)));
    draw_count = 0;
    dirty = rect_of(90, 90, 5, 5);
    lh_gl_layer_draw(lh_addr_of(root), lh_addr_of(canvas), lh_addr_of(dirty));
    EXPECT_EQ(draw_count, 0U);
    EXPECT_TRUE(
        lh_gl_point_eq(lh_gl_canvas_get_origin(lh_addr_of(canvas)), lh_addr_of(original_origin)));
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_canvas_get_clip(lh_addr_of(canvas)), lh_addr_of(original_clip)));
    lh_gl_layer_deinit(lh_addr_of(root));
    lh_gl_layer_deinit(lh_addr_of(nested));
}

} // namespace
