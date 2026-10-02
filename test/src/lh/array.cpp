#include <gtest/gtest.h>

#include <lh/array.h>

#include <algorithm>
#include <initializer_list>
#include <vector>

namespace
{

struct IntVector
{
    lh_array_t v{};

    IntVector()
    {
        lh_array_init(&v, sizeof(int));
    }

    ~IntVector()
    {
        lh_array_deinit(&v);
    }

    void
    push(int value)
    {
        lh_array_push_back(&v, &value);
    }

    int
    at(lh_uindex_t index) const
    {
        return *static_cast<const int *>(lh_array_get_ptr(&v, index));
    }
};

TEST(array_push_back, appends_from_empty_and_grows)
{
    IntVector iv;
    EXPECT_EQ(lh_array_get_size(&iv.v), 0u);

    for (int i = 0; i < 100; ++i)
    {
        iv.push(i);
    }

    ASSERT_EQ(lh_array_get_size(&iv.v), 100u);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(iv.at(static_cast<lh_uindex_t>(i)), i);
    }
}

TEST(array_push_back, fast_path_within_reserved_capacity_matches_growth_path)
{
    IntVector iv;
    lh_array_reserve(&iv.v, 8);
    const lh_usize_t capacity_before = lh_array_get_capacity(&iv.v);

    for (int i = 0; i < 8; ++i)
    {
        iv.push(i);
    }

    // No reallocation should have happened: capacity is unchanged, size matches pushes.
    EXPECT_EQ(lh_array_get_capacity(&iv.v), capacity_before);
    ASSERT_EQ(lh_array_get_size(&iv.v), 8u);
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(iv.at(static_cast<lh_uindex_t>(i)), i);
    }
}

TEST(array_push_back, crossing_reserved_capacity_falls_back_and_still_grows)
{
    IntVector iv;
    lh_array_reserve(&iv.v, 4);

    for (int i = 0; i < 4; ++i)
    {
        iv.push(i); // fast path: room already reserved
    }
    iv.push(4); // must fall back to the growing path: capacity was exactly full

    ASSERT_EQ(lh_array_get_size(&iv.v), 5u);
    EXPECT_GE(lh_array_get_capacity(&iv.v), 5u);
    for (int i = 0; i <= 4; ++i)
    {
        EXPECT_EQ(iv.at(static_cast<lh_uindex_t>(i)), i);
    }
}

TEST(array_assign, copies_elements)
{
    IntVector a;
    IntVector b;

    a.push(1);
    a.push(2);
    b.push(9);
    lh_array_assign(&b.v, &a.v);
    ASSERT_EQ(lh_array_get_size(&b.v), 2u);
    EXPECT_EQ(b.at(0), 1);
    EXPECT_EQ(b.at(1), 2);
    lh_array_assign(&a.v, &a.v);
    ASSERT_EQ(lh_array_get_size(&a.v), 2u);
}

std::vector<int>
contents(const IntVector &iv)
{
    std::vector<int> out;
    for (lh_uindex_t i = 0; i < lh_array_get_size(&iv.v); ++i)
    {
        out.push_back(iv.at(i));
    }
    return out;
}

void
fill_with(IntVector &iv, std::initializer_list<int> values)
{
    for (int value : values)
    {
        iv.push(value);
    }
}

lh_int_t
cmp_int(const lh_ptr a, const lh_ptr b, lh_ptr)
{
    const int x = *static_cast<const int *>(a);
    const int y = *static_cast<const int *>(b);
    return (x > y) - (x < y);
}

TEST(array_erase_of, removes_a_range_and_keeps_order)
{
    IntVector iv;
    fill_with(iv, {0, 1, 2, 3, 4, 5});
    lh_array_erase_of(&iv.v, 1, 3);
    EXPECT_EQ(contents(iv), (std::vector<int>{0, 4, 5}));
    lh_array_erase_of(&iv.v, 1, 2); // the tail
    EXPECT_EQ(contents(iv), (std::vector<int>{0}));
    lh_array_erase_of(&iv.v, 1, 0); // empty range at the end
    EXPECT_EQ(contents(iv), (std::vector<int>{0}));

    int removed = 0;
    lh_array_erase(&iv.v, 0, &removed);
    EXPECT_EQ(removed, 0);
    EXPECT_TRUE(lh_array_is_empty(&iv.v));
}

TEST(array_append, copies_other_and_leaves_it_untouched)
{
    IntVector a;
    IntVector b;
    fill_with(a, {1, 2});
    fill_with(b, {3, 4, 5});
    lh_array_append(&a.v, &b.v);
    EXPECT_EQ(contents(a), (std::vector<int>{1, 2, 3, 4, 5}));
    EXPECT_EQ(contents(b), (std::vector<int>{3, 4, 5}));
}

TEST(array_append, to_itself_doubles_even_when_growing)
{
    IntVector a;
    fill_with(a, {1, 2, 3});
    lh_array_reserve(&a.v, 3); // full: appending must reallocate first
    lh_array_append(&a.v, &a.v);
    EXPECT_EQ(contents(a), (std::vector<int>{1, 2, 3, 1, 2, 3}));
}

TEST(array_merge, into_empty_takes_the_block)
{
    IntVector a;
    IntVector b;
    fill_with(b, {7, 8, 9});
    const lh_ptr block = lh_array_get_data(&b.v);
    lh_array_merge(&a.v, &b.v);
    EXPECT_EQ(contents(a), (std::vector<int>{7, 8, 9}));
    EXPECT_EQ(lh_array_get_data(&a.v), block);
    EXPECT_TRUE(lh_array_is_empty(&b.v));
    EXPECT_EQ(lh_array_get_capacity(&b.v), 0u);
    b.push(1); // still usable
    EXPECT_EQ(contents(b), (std::vector<int>{1}));
}

TEST(array_merge, into_non_empty_appends_and_drains)
{
    IntVector a;
    IntVector b;
    fill_with(a, {1});
    fill_with(b, {2, 3});
    lh_array_merge(&a.v, &b.v);
    EXPECT_EQ(contents(a), (std::vector<int>{1, 2, 3}));
    EXPECT_TRUE(lh_array_is_empty(&b.v));
    lh_array_merge(&a.v, &a.v); // self: no-op
    EXPECT_EQ(contents(a), (std::vector<int>{1, 2, 3}));
}

TEST(array_index_of, finds_first_whole_element_only)
{
    IntVector iv;
    // 0x100 then 0: bytes "00 01 00 00 | 00 00 00 00" on little-endian
    // contain 0x00000001's bytes at a misaligned offset, which must not match.
    fill_with(iv, {0x100, 0, 5, 1, 5});
    int five = 5;
    int one = 1;
    int missing = 42;
    EXPECT_EQ(lh_array_index_of(&iv.v, &five), 2u);
    EXPECT_EQ(lh_array_index_of(&iv.v, &one), 3u);
    EXPECT_EQ(lh_array_index_of(&iv.v, &missing), LH_ARRAY_INVALID);
    EXPECT_TRUE(lh_array_contains(&iv.v, &one));
    EXPECT_FALSE(lh_array_contains(&iv.v, &missing));

    IntVector empty;
    EXPECT_EQ(lh_array_index_of(&empty.v, &one), LH_ARRAY_INVALID);
}

TEST(array_fill, sets_every_element)
{
    IntVector iv;
    lh_array_resize(&iv.v, 5);
    int value = -3;
    lh_array_fill(&iv.v, &value);
    EXPECT_EQ(contents(iv), (std::vector<int>(5, -3)));
}

TEST(array_reverse, odd_even_and_tiny)
{
    IntVector odd;
    fill_with(odd, {1, 2, 3, 4, 5});
    lh_array_reverse(&odd.v);
    EXPECT_EQ(contents(odd), (std::vector<int>{5, 4, 3, 2, 1}));

    IntVector even;
    fill_with(even, {1, 2, 3, 4});
    lh_array_reverse(&even.v);
    EXPECT_EQ(contents(even), (std::vector<int>{4, 3, 2, 1}));

    IntVector one;
    fill_with(one, {9});
    lh_array_reverse(&one.v);
    EXPECT_EQ(contents(one), (std::vector<int>{9}));
}

TEST(array_sort, sorts_many_values)
{
    IntVector iv;
    std::vector<int> expected;
    unsigned state = 12345u;
    for (int i = 0; i < 1000; ++i)
    {
        state = state * 1103515245u + 12345u;
        const int value = static_cast<int>((state >> 16) % 100u) - 50;
        iv.push(value);
        expected.push_back(value);
    }
    std::sort(expected.begin(), expected.end());
    lh_array_sort(&iv.v, cmp_int, nullptr);
    EXPECT_EQ(contents(iv), expected);
}

struct Pair
{
    int key;
    int order;
};

lh_int_t
cmp_pair_key(const lh_ptr a, const lh_ptr b, lh_ptr)
{
    return static_cast<const Pair *>(a)->key - static_cast<const Pair *>(b)->key;
}

TEST(array_sort, is_stable)
{
    lh_array_t v;
    lh_array_init(&v, sizeof(Pair));
    const Pair input[] = {{2, 0}, {1, 1}, {2, 2}, {1, 3}, {0, 4}, {2, 5}, {1, 6}};
    lh_array_push_back_of(&v, input, 7);
    lh_array_sort(&v, cmp_pair_key, nullptr);

    const Pair expected[] = {{0, 4}, {1, 1}, {1, 3}, {1, 6}, {2, 0}, {2, 2}, {2, 5}};
    for (lh_uindex_t i = 0; i < 7; ++i)
    {
        const Pair *p = static_cast<const Pair *>(lh_array_get_ptr(&v, i));
        EXPECT_EQ(p->key, expected[i].key);
        EXPECT_EQ(p->order, expected[i].order);
    }
    lh_array_deinit(&v);
}

TEST(array_unique, after_sort_leaves_distinct_values)
{
    IntVector iv;
    fill_with(iv, {3, 1, 3, 2, 1, 1, 3});
    lh_array_sort(&iv.v, cmp_int, nullptr);
    lh_array_unique(&iv.v, cmp_int, nullptr);
    EXPECT_EQ(contents(iv), (std::vector<int>{1, 2, 3}));
}

lh_bool_t
is_even(const lh_ptr value, lh_ptr context)
{
    ++*static_cast<int *>(context);
    return *static_cast<const int *>(value) % 2 == 0;
}

TEST(array_filter, keeps_matching_in_order)
{
    IntVector iv;
    fill_with(iv, {1, 2, 3, 4, 6, 7, 8});
    int calls = 0;
    lh_array_filter(&iv.v, is_even, &calls);
    EXPECT_EQ(contents(iv), (std::vector<int>{2, 4, 6, 8}));
    EXPECT_EQ(calls, 7);
}

} // namespace
