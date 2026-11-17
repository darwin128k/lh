/**
 * @file entity.cpp
 * @brief Tests for `lh/gx/entity.h`.
 *
 * One entity type: the screen, a card and its caption are the same struct, and
 * the tree links them wherever the application keeps them — here, the fields of
 * a screen struct, as an application would write it. gx stores nothing, so
 * what is checked is that the links come back to the same entities, and that a
 * new entity is what its init said.
 */

#include <gtest/gtest.h>

#include <lh/core/array.h>
#include <lh/gx/entity.h>

/* Defined by their own bricks later; an entity only points at them. */
struct lh_gx_class
{
    int tag;
};

struct lh_gx_style
{
    int tag;
};

namespace
{

const lh_gx_class box_class = {1};
const lh_gx_style card_style = {2};

/* One screen is one struct: the memory of the interface is its size. */
struct Screen
{
    lh_gx_entity_t root;
    lh_gx_entity_t card;
    lh_gx_entity_t caption;
};

TEST(gx_entity, a_new_entity_is_what_its_init_said)
{
    lh_gx_entity_t entity;

    lh_gx_entity_init(&entity, &box_class, &card_style, 24, 72, 120, 36);
    const lh_gx_rect_t *place = lh_gx_entity_get_place(&entity);

    EXPECT_EQ(lh_gx_entity_get_class(&entity), &box_class);
    EXPECT_EQ(lh_gx_entity_get_style(&entity), &card_style);
    EXPECT_EQ(lh_gx_entity_get_content(&entity), nullptr);
    EXPECT_EQ(lh_gx_entity_is_visible(&entity), lh_bool_true);
    EXPECT_EQ(lh_gx_point_get_x(lh_gx_rect_get_origin(place)), 24);
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_rect_get_origin(place)), 72);
    EXPECT_EQ(lh_gx_size_get_width(lh_gx_rect_get_size(place)), 120);
    EXPECT_EQ(lh_gx_size_get_height(lh_gx_rect_get_size(place)), 36);
    EXPECT_EQ(lh_gx_entity_get_owner(&entity), nullptr);
    EXPECT_EQ(lh_gx_entity_get_first_child(&entity), nullptr);
}

TEST(gx_entity, every_brick_reads_back_what_was_stored)
{
    lh_gx_entity_t entity;
    lh_gx_rect_t place;
    int text = 0;

    lh_gx_entity_init(&entity, nullptr, nullptr, 0, 0, 0, 0);
    lh_gx_rect_init(&place, 1, 2, 3, 4);
    lh_gx_entity_set_place(&entity, place);
    lh_gx_entity_set_visible(&entity, lh_bool_false);
    lh_gx_entity_set_class(&entity, &box_class);
    lh_gx_entity_set_style(&entity, &card_style);
    lh_gx_entity_set_content(&entity, &text);

    EXPECT_EQ(lh_gx_rect_eq(lh_gx_entity_get_place(&entity), &place), lh_bool_true);
    EXPECT_EQ(lh_gx_entity_is_visible(&entity), lh_bool_false);
    EXPECT_EQ(lh_gx_entity_get_class(&entity), &box_class);
    EXPECT_EQ(lh_gx_entity_get_style(&entity), &card_style);
    EXPECT_EQ(lh_gx_entity_get_content(&entity), &text);
}

TEST(gx_entity, the_node_leads_back_to_its_entity)
{
    lh_gx_entity_t entity;

    lh_gx_entity_init(&entity, nullptr, nullptr, 0, 0, 1, 1);
    EXPECT_EQ(lh_gx_entity_from_node(lh_gx_entity_get_node(&entity)), &entity);
    EXPECT_EQ(lh_gx_entity_from_node(nullptr), nullptr);
}

/* A control is a composition: a card owning its caption, under the screen. */
TEST(gx_entity, a_screen_struct_is_a_tree_of_its_own_fields)
{
    Screen screen;

    lh_gx_entity_init(&screen.root, nullptr, nullptr, 0, 0, 480, 320);
    lh_gx_entity_init(&screen.card, &box_class, &card_style, 24, 72, 120, 36);
    lh_gx_entity_init(&screen.caption, &box_class, &card_style, 0, 0, 120, 36);
    lh_gx_entity_attach_v(&screen.root, &screen.card);
    lh_gx_entity_attach_v(&screen.card, &screen.caption);

    EXPECT_EQ(lh_gx_entity_get_first_child(&screen.root), &screen.card);
    EXPECT_EQ(lh_gx_entity_get_first_child(&screen.card), &screen.caption);
    EXPECT_EQ(lh_gx_entity_get_owner(&screen.caption), &screen.card);
    EXPECT_EQ(lh_gx_entity_get_owner(&screen.card), &screen.root);
    EXPECT_EQ(lh_gx_entity_get_next_sibling(&screen.card), nullptr);

    lh_gx_entity_detach(&screen.card);
    EXPECT_EQ(lh_gx_entity_get_first_child(&screen.root), nullptr);
    EXPECT_EQ(lh_gx_entity_get_owner(&screen.caption), &screen.card) << "the subtree goes whole";
}

/* Rows known only at start-up: an lh_array_t reserved once, then linked. Nothing
   in gx allocated them, and running out is the array's answer, not gx's. */
TEST(gx_entity, rows_in_an_array_reserved_once_are_linked_in_place)
{
    lh_gx_entity_t list;
    lh_array_t rows;

    lh_array_init(&rows, sizeof(lh_gx_entity_t));
    ASSERT_EQ(lh_array_try_reserve(&rows, 8U), lh_bool_true);
    lh_array_resize(&rows, 8U);
    lh_gx_entity_init(&list, nullptr, nullptr, 0, 0, 480, 320);
    for (lh_uindex_t i = 0U; i < 8U; ++i)
    {
        auto *row = static_cast<lh_gx_entity_t *>(lh_array_get_ptr(&rows, i));

        lh_gx_entity_init(row, &box_class, &card_style, 0, static_cast<lh_gx_scalar_t>(i * 40U),
                          480, 40);
        lh_gx_entity_attach(&list, row);
    }
    lh_uindex_t seen = 0U;

    for (lh_gx_entity_t *row = lh_gx_entity_get_first_child(&list); row != nullptr;
         row = lh_gx_entity_get_next_sibling(row))
    {
        EXPECT_EQ(row, lh_array_get_ptr(&rows, seen));
        ++seen;
    }
    EXPECT_EQ(seen, 8U);
    for (lh_uindex_t i = 0U; i < 8U; ++i)
    {
        lh_gx_entity_detach(static_cast<lh_gx_entity_t *>(lh_array_get_ptr(&rows, i)));
    }
    lh_array_deinit(&rows);
}

} // namespace
