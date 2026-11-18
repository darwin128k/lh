/**
 * @file walk.cpp
 * @brief Tests for `lh/gx/walk.h`.
 *
 * A small tree with places that add up — a screen, a card with two children,
 * a second card — walked whole, walked without entering a card, and walked from
 * a card in the middle: the order, the step taken each time and every place on
 * the screen.
 */

#include <gtest/gtest.h>

#include <lh/gx/walk.h>

namespace
{

/* screen (0,0) > a (2,2) > a1 (+1,+1), a2 (+5,+5); screen > b (12,0). */
struct Tree
{
    lh_gx_entity_t screen;
    lh_gx_entity_t a;
    lh_gx_entity_t a1;
    lh_gx_entity_t a2;
    lh_gx_entity_t b;

    Tree()
    {
        lh_gx_entity_init(&screen, nullptr, nullptr, 0, 0, 20, 20);
        lh_gx_entity_init(&a, nullptr, nullptr, 2, 2, 10, 10);
        lh_gx_entity_init(&a1, nullptr, nullptr, 1, 1, 3, 3);
        lh_gx_entity_init(&a2, nullptr, nullptr, 5, 5, 2, 2);
        lh_gx_entity_init(&b, nullptr, nullptr, 12, 0, 5, 5);
        lh_gx_entity_attach(&screen, &a);
        lh_gx_entity_attach(&a, &a1);
        lh_gx_entity_attach(&a, &a2);
        lh_gx_entity_attach(&screen, &b);
    }
};

lh_gx_scalar_t
x_of(const lh_gx_walk_t *walk)
{
    return lh_gx_point_get_x(lh_gx_rect_get_origin(lh_gx_walk_get_place(walk)));
}

lh_gx_scalar_t
y_of(const lh_gx_walk_t *walk)
{
    return lh_gx_point_get_y(lh_gx_rect_get_origin(lh_gx_walk_get_place(walk)));
}

TEST(gx_walk, the_whole_tree_in_drawing_order_with_its_places)
{
    Tree tree;
    lh_gx_walk_t walk;

    lh_gx_walk_init(&walk, &tree.screen);
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.screen);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_true), lh_gx_walk_step_down);
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.a);
    EXPECT_EQ(x_of(&walk), 2);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_true), lh_gx_walk_step_down);
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.a1);
    EXPECT_EQ(x_of(&walk), 3);
    EXPECT_EQ(y_of(&walk), 3);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_true), lh_gx_walk_step_across) << "a1 has no children";
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.a2);
    EXPECT_EQ(x_of(&walk), 7);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_true), lh_gx_walk_step_up);
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.b);
    EXPECT_EQ(x_of(&walk), 12);
    EXPECT_EQ(y_of(&walk), 0);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_true), lh_gx_walk_step_end);
    EXPECT_TRUE(lh_gx_walk_get_entity(&walk) == nullptr);
}

TEST(gx_walk, a_subtree_not_entered_is_skipped_whole)
{
    Tree tree;
    lh_gx_walk_t walk;

    lh_gx_walk_init(&walk, &tree.screen);
    lh_gx_walk_next(&walk, lh_bool_true);
    ASSERT_EQ(lh_gx_walk_get_entity(&walk), &tree.a);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_false), lh_gx_walk_step_across);
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.b);
}

TEST(gx_walk, a_walk_from_the_middle_stays_in_its_subtree)
{
    Tree tree;
    lh_gx_walk_t walk;

    lh_gx_walk_init(&walk, &tree.a);
    EXPECT_EQ(x_of(&walk), 2) << "counted from the owner's place";
    lh_gx_walk_next(&walk, lh_bool_true);
    lh_gx_walk_next(&walk, lh_bool_true);
    EXPECT_EQ(lh_gx_walk_get_entity(&walk), &tree.a2);
    EXPECT_EQ(lh_gx_walk_next(&walk, lh_bool_true), lh_gx_walk_step_end) << "b is not under a";
}

} // namespace
