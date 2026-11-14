#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/entity/initializer.h>
#include <lh/gl/layer.h>
#include <lh/gl/layer/initializer.h>
#include <lh/gl/point/initializer.h>
#include <lh/gl/rect/initializer.h>
#include <lh/gl/size/initializer.h>

namespace
{

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
    lh_list_t items;

    lh_list_init(lh_addr_of(items));
    lh_gl_layer_t layer = lh_gl_layer_initializer(
        lh_gl_entity_initializer(
            lh_gl_rect_initializer(lh_gl_point_initializer(1, 2), lh_gl_size_initializer(3, 4)),
            nullptr, lh_gl_layer_get_class(), lh_list_node_t{}),
        items);

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

} // namespace
