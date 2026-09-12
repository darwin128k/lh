#include <gtest/gtest.h>

#include <lh/util/algorithm.h>

namespace
{

// lh_algorithm_compare/rcompare/find_byte perform `return l;` on their match/mismatch,
// so each needs a tiny wrapper function whose return type is compatible with `const T *`.

const int *
compare_int(const int *lhs, const int *rhs, lh_usize_t n)
{
    lh_algorithm_compare(int, lhs, rhs, n);
    return nullptr;
}

const int *
rcompare_int(const int *lhs, const int *rhs, lh_usize_t n)
{
    lh_algorithm_rcompare(int, lhs, rhs, n);
    return nullptr;
}

const int *
find_byte_int(const int *lhs, int rhs, lh_usize_t n)
{
    lh_algorithm_find_byte(int, lhs, rhs, n);
    return nullptr;
}

TEST(util_algorithm, swap_exchanges_two_values)
{
    int a = 10;
    int b = 20;
    lh_algorithm_swap(int, a, b);
    EXPECT_EQ(a, 20);
    EXPECT_EQ(b, 10);
}

TEST(util_algorithm, swap_no_temp_exchanges_two_values)
{
    unsigned a = 1U;
    unsigned b = 2U;
    lh_algorithm_swap_no_temp(a, b);
    EXPECT_EQ(a, 2U);
    EXPECT_EQ(b, 1U);
}

// Every remaining test uses a multi-byte T (int) on purpose: every real call site
// elsewhere in this codebase only ever instantiates these macros with a 1-byte T
// (lh_uchar_t/lh_char_t), which is exactly how a byte-vs-element offset bug in
// lh_algorithm_rcopy/rcompare/copy_rev went unnoticed until measured directly here.

TEST(util_algorithm, copy_forward_copies_in_order)
{
    int src[3] = {1, 2, 3};
    int dst[3] = {0, 0, 0};
    lh_usize_t n = 3;
    lh_algorithm_copy(int, dst, src, n);
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 3);
}

TEST(util_algorithm, rcopy_handles_overlap_where_dst_starts_inside_src)
{
    // Matches the macro's own doc example.
    int v[5] = {1, 2, 3, 4, 5};
    lh_usize_t n = 4;
    lh_algorithm_rcopy(int, v + 1, v, n);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 1);
    EXPECT_EQ(v[2], 2);
    EXPECT_EQ(v[3], 3);
    EXPECT_EQ(v[4], 4);
}

TEST(util_algorithm, set_fills_every_element)
{
    int out[4] = {0, 0, 0, 0};
    lh_usize_t n = 4;
    lh_algorithm_set(int, out, 7, n);
    for (int v : out)
    {
        EXPECT_EQ(v, 7);
    }
}

TEST(util_algorithm, find_byte_returns_pointer_to_first_match)
{
    int v[5] = {1, 2, 3, 2, 1};
    lh_usize_t n = 5;
    const int *found = find_byte_int(v, 2, n);
    ASSERT_NE(found, lh_null);
    EXPECT_EQ(found - v, 1);
}

TEST(util_algorithm, find_byte_returns_null_when_absent)
{
    int v[3] = {1, 2, 3};
    lh_usize_t n = 3;
    EXPECT_EQ(find_byte_int(v, 99, n), lh_null);
}

TEST(util_algorithm, compare_returns_pointer_to_first_mismatch)
{
    int a[4] = {1, 2, 9, 4};
    int b[4] = {1, 2, 3, 4};
    lh_usize_t n = 4;
    const int *diff = compare_int(a, b, n);
    ASSERT_NE(diff, lh_null);
    EXPECT_EQ(diff - a, 2);
}

TEST(util_algorithm, compare_returns_null_when_equal)
{
    int a[4] = {1, 2, 3, 4};
    int b[4] = {1, 2, 3, 4};
    lh_usize_t n = 4;
    EXPECT_EQ(compare_int(a, b, n), lh_null);
}

TEST(util_algorithm, rcompare_returns_pointer_to_first_mismatch_at_the_start)
{
    // Mismatch at index 0 specifically exercises the initial "point at the last
    // element" offset computation, not just the walk-backwards loop.
    int a[4] = {9, 2, 3, 4};
    int b[4] = {1, 2, 3, 4};
    lh_usize_t n = 4;
    const int *diff = rcompare_int(a, b, n);
    ASSERT_NE(diff, lh_null);
    EXPECT_EQ(diff - a, 0);
}

TEST(util_algorithm, rcompare_returns_pointer_to_mismatch_near_the_end)
{
    int a[4] = {1, 2, 3, 9};
    int b[4] = {1, 2, 3, 4};
    lh_usize_t n = 4;
    const int *diff = rcompare_int(a, b, n);
    ASSERT_NE(diff, lh_null);
    EXPECT_EQ(diff - a, 3);
}

TEST(util_algorithm, rcompare_returns_null_when_equal)
{
    int a[4] = {1, 2, 3, 4};
    int b[4] = {1, 2, 3, 4};
    lh_usize_t n = 4;
    EXPECT_EQ(rcompare_int(a, b, n), lh_null);
}

TEST(util_algorithm, copy_rev_reverses_into_destination)
{
    // Matches the macro's own doc example.
    int src[4] = {1, 2, 3, 4};
    int dst[4] = {0, 0, 0, 0};
    lh_usize_t n = 4;
    lh_algorithm_copy_rev(int, dst, src, n);
    EXPECT_EQ(dst[0], 4);
    EXPECT_EQ(dst[1], 3);
    EXPECT_EQ(dst[2], 2);
    EXPECT_EQ(dst[3], 1);
}

TEST(util_algorithm, foreach_visits_every_element_in_order)
{
    int v[3] = {1, 2, 3};
    int seen[3] = {0, 0, 0};
    lh_usize_t i = 0;

    lh_algorithm_foreach(int, it, v, 3)
    {
        seen[i++] = *it;
    }

    EXPECT_EQ(i, 3U);
    EXPECT_EQ(seen[0], 1);
    EXPECT_EQ(seen[1], 2);
    EXPECT_EQ(seen[2], 3);
}

TEST(util_algorithm, foreach_can_mutate_elements_in_place)
{
    int v[3] = {1, 2, 3};
    lh_algorithm_foreach(int, it, v, 3)
    {
        *it *= 2;
    }
    EXPECT_EQ(v[0], 2);
    EXPECT_EQ(v[1], 4);
    EXPECT_EQ(v[2], 6);
}

TEST(util_algorithm, foreach_over_zero_elements_does_not_execute_body)
{
    int v[1] = {42};
    lh_usize_t n = 0;
    int calls = 0;

    lh_algorithm_foreach(int, it, v, n)
    {
        (void)it;
        ++calls;
    }

    EXPECT_EQ(calls, 0);
}

TEST(util_algorithm, foreach_can_be_nested_without_name_collision)
{
    int outer[2] = {1, 2};
    int inner[2] = {10, 20};
    int sum = 0;

    lh_algorithm_foreach(int, it, outer, 2)
    {
        lh_algorithm_foreach(int, jt, inner, 2)
        {
            sum += *it * *jt;
        }
    }

    // (1*10 + 1*20) + (2*10 + 2*20) = 30 + 60 = 90
    EXPECT_EQ(sum, 90);
}

} // namespace
