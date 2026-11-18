/**
 * @file node.cpp
 * @brief Tests for `lh/gx/node.h`.
 *
 * The node is the tree of gx, and the tree is intrusive: it links nodes that
 * live wherever the test put them, and it owns their order, never their bytes.
 * What matters: the order of the children is the order they were attached in,
 * a detached node takes its subtree with it, and the checked attach refuses
 * anything that would stop the tree being a tree.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/gx/node.h>

namespace
{

TEST(gx_node, a_new_node_has_no_owner_and_no_children)
{
    lh_gx_node_t node;

    lh_gx_node_init(&node);
    EXPECT_EQ(lh_gx_node_is_attached(&node), lh_bool_false);
    EXPECT_EQ(lh_gx_node_has_children(&node), lh_bool_false);
    EXPECT_EQ(lh_gx_node_get_owner(&node), nullptr);
    EXPECT_EQ(lh_gx_node_get_first_child(&node), nullptr);
    EXPECT_EQ(lh_gx_node_get_last_child(&node), nullptr);
    EXPECT_EQ(lh_gx_node_get_next_sibling(&node), nullptr);
    EXPECT_EQ(lh_gx_node_get_root(&node), &node);
}

/* The order of the children is the order of drawing: the first at the back. */
TEST(gx_node, children_keep_the_order_they_were_attached_in)
{
    lh_gx_node_t owner;
    lh_gx_node_t back;
    lh_gx_node_t middle;
    lh_gx_node_t front;

    lh_gx_node_init(&owner);
    lh_gx_node_init(&back);
    lh_gx_node_init(&middle);
    lh_gx_node_init(&front);
    lh_gx_node_attach(&owner, &back);
    lh_gx_node_attach(&owner, &middle);
    lh_gx_node_attach(&owner, &front);

    EXPECT_EQ(lh_gx_node_get_first_child(&owner), &back);
    EXPECT_EQ(lh_gx_node_get_last_child(&owner), &front);
    EXPECT_EQ(lh_gx_node_get_next_sibling(&back), &middle);
    EXPECT_EQ(lh_gx_node_get_next_sibling(&middle), &front);
    EXPECT_EQ(lh_gx_node_get_next_sibling(&front), nullptr);
    EXPECT_EQ(lh_gx_node_get_prev_sibling(&front), &middle);
    EXPECT_EQ(lh_gx_node_get_prev_sibling(&back), nullptr);
    EXPECT_EQ(lh_gx_node_get_owner(&middle), &owner);
}

/* Taking a node out takes its own subtree with it; the rest closes the gap. */
TEST(gx_node, a_detached_node_keeps_its_own_children)
{
    lh_gx_node_t root;
    lh_gx_node_t card;
    lh_gx_node_t caption;
    lh_gx_node_t other;

    lh_gx_node_init(&root);
    lh_gx_node_init(&card);
    lh_gx_node_init(&caption);
    lh_gx_node_init(&other);
    lh_gx_node_attach(&root, &card);
    lh_gx_node_attach(&root, &other);
    lh_gx_node_attach(&card, &caption);

    lh_gx_node_detach(&card);

    EXPECT_EQ(lh_gx_node_is_attached(&card), lh_bool_false);
    EXPECT_EQ(lh_gx_node_get_first_child(&root), &other);
    EXPECT_EQ(lh_gx_node_get_first_child(&card), &caption);
    EXPECT_EQ(lh_gx_node_get_owner(&caption), &card);
    EXPECT_EQ(lh_gx_node_get_root(&caption), &card);

    lh_gx_node_detach(&card);
    EXPECT_EQ(lh_gx_node_is_attached(&card), lh_bool_false) << "detaching twice is nothing";
}

TEST(gx_node, the_root_and_the_ancestors_are_found_by_the_owners)
{
    lh_gx_node_t root;
    lh_gx_node_t layer;
    lh_gx_node_t leaf;

    lh_gx_node_init(&root);
    lh_gx_node_init(&layer);
    lh_gx_node_init(&leaf);
    lh_gx_node_attach(&root, &layer);
    lh_gx_node_attach(&layer, &leaf);

    EXPECT_EQ(lh_gx_node_get_root(&leaf), &root);
    EXPECT_EQ(lh_gx_node_is_ancestor_of(&root, &leaf), lh_bool_true);
    EXPECT_EQ(lh_gx_node_is_ancestor_of(&layer, &leaf), lh_bool_true);
    EXPECT_EQ(lh_gx_node_is_ancestor_of(&leaf, &root), lh_bool_false);
    EXPECT_EQ(lh_gx_node_is_ancestor_of(&leaf, &leaf), lh_bool_false) << "not its own ancestor";
}

/* A node lives wherever its application put it: here, in a plain array. */
TEST(gx_node, nodes_in_an_array_are_linked_not_moved)
{
    lh_gx_node_t rows[4];

    for (auto &row : rows)
    {
        lh_gx_node_init(&row);
    }
    for (int i = 1; i < 4; ++i)
    {
        lh_gx_node_attach(&rows[0], &rows[i]);
    }
    int count = 0;

    for (lh_gx_node_t *row = lh_gx_node_get_first_child(&rows[0]); row != nullptr;
         row = lh_gx_node_get_next_sibling(row))
    {
        EXPECT_EQ(row, &rows[count + 1]);
        ++count;
    }
    EXPECT_EQ(count, 3);
}

TEST(gx_node, the_checked_attach_keeps_the_tree_a_tree)
{
    lh_gx_node_t owner;
    lh_gx_node_t child;
    lh_gx_node_t grandchild;

    lh_gx_node_init(&owner);
    lh_gx_node_init(&child);
    lh_gx_node_init(&grandchild);
    lh_gx_node_attach_v(&owner, &child);
    lh_gx_node_attach_v(&child, &grandchild);
    EXPECT_EQ(lh_gx_node_get_first_child(&child), &grandchild);

    LH_EXPECT_CHECK_DEATH(lh_gx_node_attach_v(&owner, &owner));
    LH_EXPECT_CHECK_DEATH(lh_gx_node_attach_v(&owner, &child));
    LH_EXPECT_CHECK_DEATH(lh_gx_node_attach_v(&grandchild, &owner));
}

/* Drawing order without a stack: owner first, children in order, back up by the owners. */
TEST(gx_node, the_walk_goes_in_drawing_order_and_stays_under_its_top)
{
    lh_gx_node_t root;
    lh_gx_node_t a;
    lh_gx_node_t a1;
    lh_gx_node_t a2;
    lh_gx_node_t b;

    for (lh_gx_node_t *node : {&root, &a, &a1, &a2, &b})
    {
        lh_gx_node_init(node);
    }
    lh_gx_node_attach(&root, &a);
    lh_gx_node_attach(&a, &a1);
    lh_gx_node_attach(&a, &a2);
    lh_gx_node_attach(&root, &b);

    EXPECT_EQ(lh_gx_node_get_next(&root, &root), &a);
    EXPECT_EQ(lh_gx_node_get_next(&a, &root), &a1);
    EXPECT_EQ(lh_gx_node_get_next(&a1, &root), &a2);
    EXPECT_EQ(lh_gx_node_get_next(&a2, &root), &b) << "back up to the owner's next sibling";
    EXPECT_EQ(lh_gx_node_get_next(&b, &root), nullptr);
    EXPECT_EQ(lh_gx_node_get_next_skip(&a, &root), &b) << "a's subtree skipped";
    EXPECT_EQ(lh_gx_node_get_next(&a2, &a), nullptr) << "the walk of a stays under a";
}

} // namespace
