#include <gtest/gtest.h>

#include <lh/list.h>

#include <algorithm>
#include <utility>
#include <vector>

namespace
{

struct Item
{
    int value;
    lh_list_node_t node;

    explicit Item(int v) : value(v)
    {
        lh_list_node_init(&node);
    }
};

std::vector<int>
values_forward(const lh_list_t *list)
{
    std::vector<int> out;
    for (lh_list_node_t *n = lh_list_get_first(list); n; n = lh_list_get_next(list, n))
    {
        out.push_back(lh_list_entry(Item, node, n)->value);
    }
    return out;
}

std::vector<int>
values_backward(const lh_list_t *list)
{
    std::vector<int> out;
    for (lh_list_node_t *n = lh_list_get_last(list); n; n = lh_list_get_prev(list, n))
    {
        out.push_back(lh_list_entry(Item, node, n)->value);
    }
    return out;
}

TEST(list_init, is_empty)
{
    lh_list_t list;
    lh_list_init(&list);

    EXPECT_TRUE(lh_list_is_empty(&list));
    EXPECT_EQ(lh_list_get_size(&list), 0u);
    EXPECT_EQ(lh_list_get_first(&list), nullptr);
    EXPECT_EQ(lh_list_get_last(&list), nullptr);
    EXPECT_EQ(lh_list_pop_front(&list), nullptr);
    EXPECT_EQ(lh_list_pop_back(&list), nullptr);
}

TEST(list_node_init, is_unlinked)
{
    Item a(1);

    EXPECT_FALSE(lh_list_node_is_linked(&a.node));
    lh_list_node_unlink(&a.node); /* no-op */
    EXPECT_FALSE(lh_list_node_is_linked(&a.node));
}

TEST(list_push, keeps_order_both_ways)
{
    lh_list_t list;
    Item a(1), b(2), c(3);

    lh_list_init(&list);
    lh_list_push_back(&list, &b.node);
    lh_list_push_back(&list, &c.node);
    lh_list_push_front(&list, &a.node);

    EXPECT_FALSE(lh_list_is_empty(&list));
    EXPECT_EQ(lh_list_get_size(&list), 3u);
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(values_backward(&list), (std::vector<int>{3, 2, 1}));
    EXPECT_TRUE(lh_list_node_is_linked(&b.node));
}

TEST(list_node_unlink, removes_from_the_middle)
{
    lh_list_t list;
    Item a(1), b(2), c(3);

    lh_list_init(&list);
    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &b.node);
    lh_list_push_back(&list, &c.node);

    lh_list_node_unlink(&b.node);
    EXPECT_FALSE(lh_list_node_is_linked(&b.node));
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 3}));
    EXPECT_EQ(values_backward(&list), (std::vector<int>{3, 1}));

    /* An unlinked node can go into a list again. */
    lh_list_push_front(&list, &b.node);
    EXPECT_EQ(values_forward(&list), (std::vector<int>{2, 1, 3}));
}

TEST(list_node_insert, after_and_before_an_element)
{
    lh_list_t list;
    Item a(1), b(2), c(3), d(4);

    lh_list_init(&list);
    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &d.node);
    lh_list_node_insert_after(&b.node, &a.node);
    lh_list_node_insert_before(&c.node, &d.node);

    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 2, 3, 4}));
}

TEST(list_pop, takes_from_either_end)
{
    lh_list_t list;
    Item a(1), b(2), c(3);

    lh_list_init(&list);
    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &b.node);
    lh_list_push_back(&list, &c.node);

    EXPECT_EQ(lh_list_entry(Item, node, lh_list_pop_back(&list))->value, 3);
    EXPECT_EQ(lh_list_entry(Item, node, lh_list_pop_front(&list))->value, 1);
    EXPECT_FALSE(lh_list_node_is_linked(&a.node));
    EXPECT_FALSE(lh_list_node_is_linked(&c.node));
    EXPECT_EQ(values_forward(&list), (std::vector<int>{2}));

    EXPECT_EQ(lh_list_pop_back(&list), &b.node);
    EXPECT_TRUE(lh_list_is_empty(&list));
}

TEST(list_get_at, counts_from_the_front)
{
    lh_list_t list;
    Item a(1), b(2), c(3);

    lh_list_init(&list);
    EXPECT_EQ(lh_list_get_at(&list, 0), nullptr);

    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &b.node);
    lh_list_push_back(&list, &c.node);

    EXPECT_EQ(lh_list_get_at(&list, 0), &a.node);
    EXPECT_EQ(lh_list_get_at(&list, 1), &b.node);
    EXPECT_EQ(lh_list_get_at(&list, 2), &c.node);
    EXPECT_EQ(lh_list_get_at(&list, 3), nullptr);
    EXPECT_EQ(lh_list_get_at(&list, 1000), nullptr);
}

TEST(list_index_of, finds_position_or_invalid)
{
    lh_list_t list;
    Item a(1), b(2), c(3), stranger(9);

    lh_list_init(&list);
    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &b.node);
    lh_list_push_back(&list, &c.node);

    EXPECT_EQ(lh_list_index_of(&list, &a.node), 0u);
    EXPECT_EQ(lh_list_index_of(&list, &c.node), 2u);
    EXPECT_EQ(lh_list_index_of(&list, &stranger.node), LH_LIST_INVALID);
    EXPECT_TRUE(lh_list_contains(&list, &b.node));
    EXPECT_FALSE(lh_list_contains(&list, &stranger.node));
}

TEST(list_node_move, within_and_across_lists)
{
    lh_list_t list, other;
    Item a(1), b(2), c(3), d(4);

    lh_list_init(&list);
    lh_list_init(&other);
    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &b.node);
    lh_list_push_back(&list, &c.node);
    lh_list_push_back(&other, &d.node);

    lh_list_node_move_after(&a.node, &c.node); /* 2 3 1 */
    EXPECT_EQ(values_forward(&list), (std::vector<int>{2, 3, 1}));
    lh_list_node_move_before(&a.node, &b.node); /* 1 2 3 */
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 2, 3}));
    lh_list_node_move_after(&b.node, &b.node); /* no-op */
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 2, 3}));

    lh_list_node_move_before(&d.node, &b.node); /* from the other list */
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 4, 2, 3}));
    EXPECT_TRUE(lh_list_is_empty(&other));
}

TEST(list_splice, joins_in_order_and_empties_the_other)
{
    lh_list_t list, back, front, empty;
    Item a(1), b(2), c(3), d(4), e(5);

    lh_list_init(&list);
    lh_list_init(&back);
    lh_list_init(&front);
    lh_list_init(&empty);
    lh_list_push_back(&list, &c.node);
    lh_list_push_back(&back, &d.node);
    lh_list_push_back(&back, &e.node);
    lh_list_push_back(&front, &a.node);
    lh_list_push_back(&front, &b.node);

    lh_list_splice_back(&list, &back);
    lh_list_splice_front(&list, &front);
    lh_list_splice_back(&list, &empty);

    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 2, 3, 4, 5}));
    EXPECT_EQ(values_backward(&list), (std::vector<int>{5, 4, 3, 2, 1}));
    EXPECT_TRUE(lh_list_is_empty(&back));
    EXPECT_TRUE(lh_list_is_empty(&front));
}

TEST(list_clear, unlinks_every_node)
{
    lh_list_t list;
    Item a(1), b(2);

    lh_list_init(&list);
    lh_list_push_back(&list, &a.node);
    lh_list_push_back(&list, &b.node);
    lh_list_clear(&list);

    EXPECT_TRUE(lh_list_is_empty(&list));
    EXPECT_FALSE(lh_list_node_is_linked(&a.node));
    EXPECT_FALSE(lh_list_node_is_linked(&b.node));
}

struct Keyed
{
    int key;
    int order; /* insertion order, to check stability */
    lh_list_node_t node;
};

lh_int_t
by_key(const lh_list_node_t *a, const lh_list_node_t *b, lh_ptr context)
{
    (void)context;
    const int ka = lh_list_entry(const Keyed, node, a)->key;
    const int kb = lh_list_entry(const Keyed, node, b)->key;
    return ka < kb ? -1 : ka > kb ? 1 : 0;
}

TEST(list_sort, empty_and_single)
{
    lh_list_t list;
    Item a(1);

    lh_list_init(&list);
    lh_list_sort(&list, by_key, nullptr);
    EXPECT_TRUE(lh_list_is_empty(&list));

    lh_list_push_back(&list, &a.node);
    lh_list_sort(
        &list, [](const lh_list_node_t *, const lh_list_node_t *, lh_ptr) -> lh_int_t { return 0; },
        nullptr);
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1}));
}

TEST(list_sort, matches_stable_sort_for_many_sizes)
{
    unsigned seed = 12345u;
    for (int size : {2, 3, 5, 8, 17, 100, 1000})
    {
        std::vector<Keyed> items(static_cast<size_t>(size));
        lh_list_t list;

        lh_list_init(&list);
        for (int i = 0; i < size; ++i)
        {
            seed = seed * 1103515245u + 12345u;
            items[static_cast<size_t>(i)].key =
                static_cast<int>((seed >> 16) % 10u); /* many ties */
            items[static_cast<size_t>(i)].order = i;
            lh_list_node_init(&items[static_cast<size_t>(i)].node);
            lh_list_push_back(&list, &items[static_cast<size_t>(i)].node);
        }
        lh_list_sort(&list, by_key, nullptr);

        std::vector<std::pair<int, int>> want;
        for (const Keyed &k : items)
        {
            want.emplace_back(k.key, k.order);
        }
        std::stable_sort(want.begin(), want.end(),
                         [](const std::pair<int, int> &x, const std::pair<int, int> &y)
                         { return x.first < y.first; });

        std::vector<std::pair<int, int>> got, got_back;
        for (lh_list_node_t *n = lh_list_get_first(&list); n; n = lh_list_get_next(&list, n))
        {
            got.emplace_back(lh_list_entry(Keyed, node, n)->key,
                             lh_list_entry(Keyed, node, n)->order);
        }
        for (lh_list_node_t *n = lh_list_get_last(&list); n; n = lh_list_get_prev(&list, n))
        {
            got_back.emplace_back(lh_list_entry(Keyed, node, n)->key,
                                  lh_list_entry(Keyed, node, n)->order);
        }
        std::reverse(got_back.begin(), got_back.end());

        EXPECT_EQ(got, want) << "size " << size;
        EXPECT_EQ(got_back, want) << "prev links, size " << size;
    }
}

TEST(list_entry, null_node_gives_null)
{
    lh_list_t list;
    lh_list_init(&list);

    EXPECT_EQ(lh_list_entry(Item, node, lh_list_get_first(&list)), nullptr);
}

TEST(list_walk, removing_while_walking_with_next_read_first)
{
    lh_list_t list;
    Item a(1), b(2), c(3), d(4);

    lh_list_init(&list);
    for (Item *item : {&a, &b, &c, &d})
    {
        lh_list_push_back(&list, &item->node);
    }
    /* Drop the even values. */
    for (lh_list_node_t *n = lh_list_get_first(&list), *next; n; n = next)
    {
        next = lh_list_get_next(&list, n);
        if (lh_list_entry(Item, node, n)->value % 2 == 0)
        {
            lh_list_node_unlink(n);
        }
    }
    EXPECT_EQ(values_forward(&list), (std::vector<int>{1, 3}));
}

} // namespace
