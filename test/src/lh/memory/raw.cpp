#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/raw.h>
#include <lh/null.h>
#include <lh/size.h>

namespace
{

TEST(memory_raw_copy, destination_smaller_truncates)
{
    lh_uchar_t dst[2] = {};
    const lh_uchar_t src[] = {1, 2, 3, 4};
    lh_ptr end = lh_memory_raw_copy(dst, dst + 2, src, src + 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 2));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
}

TEST(memory_raw_copy, source_smaller_copies_only_source_length)
{
    lh_uchar_t dst[8] = {};
    const lh_uchar_t src[] = {9, 8};
    lh_ptr end = lh_memory_raw_copy(dst, dst + 8, src, src + 2);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 2));
    EXPECT_EQ(dst[0], 9);
    EXPECT_EQ(dst[1], 8);
    EXPECT_EQ(dst[2], 0);
}

TEST(memory_raw_copy_rev, min_length_and_reversed_in_destination)
{
    lh_uchar_t dst[8] = {};
    const lh_uchar_t src[] = {1, 2, 3, 4};
    lh_ptr end = lh_memory_raw_copy_rev(dst, dst + 3, src, src + 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 3));
    EXPECT_EQ(dst[0], 3);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 1);
    EXPECT_EQ(dst[3], 0);
}

TEST(memory_raw_rcopy, overlapping_backward_copy_respects_min_size)
{
    lh_uchar_t v[] = {1, 2, 3, 4, 5};
    lh_ptr r = lh_memory_raw_rcopy(v + 1, v + 5, v, v + 5);
    EXPECT_EQ(r, static_cast<lh_ptr>(v + 1));
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 1);
    EXPECT_EQ(v[2], 2);
    EXPECT_EQ(v[3], 3);
    EXPECT_EQ(v[4], 4);
}

TEST(memory_raw_move, forward_overlap_with_bounded_length)
{
    lh_uchar_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8};
    lh_ptr end = lh_memory_raw_move(buf + 2, buf + 8, buf, buf + 8);
    EXPECT_EQ(end, static_cast<lh_ptr>(buf + 8));
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 1);
    EXPECT_EQ(buf[3], 2);
    EXPECT_EQ(buf[4], 3);
    EXPECT_EQ(buf[5], 4);
    EXPECT_EQ(buf[6], 5);
    EXPECT_EQ(buf[7], 6);
}

TEST(memory_raw_move, non_overlapping)
{
    lh_uchar_t dst[4] = {};
    const lh_uchar_t src[] = {7, 6, 5, 4};
    lh_ptr end = lh_memory_raw_move(dst, dst + 4, src, src + 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 4));
    EXPECT_EQ(dst[0], 7);
    EXPECT_EQ(dst[3], 4);
}

TEST(memory_raw_set, fills_range)
{
    lh_uchar_t block[8];
    lh_ptr end = lh_memory_raw_set(block, block + 8, 0xCD);
    EXPECT_EQ(end, static_cast<lh_ptr>(block + 8));
    for (lh_usize_t i = 0; i < 8; ++i)
    {
        EXPECT_EQ(block[i], 0xCD);
    }
}

TEST(memory_raw_set_pattern, repeats_pattern)
{
    lh_uchar_t dst[7] = {};
    const lh_uchar_t pat[] = {0xA, 0xB};
    lh_ptr end = lh_memory_raw_set_pattern(dst, dst + 7, pat, pat + 2);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 7));
    const lh_uchar_t expect[] = {0xA, 0xB, 0xA, 0xB, 0xA, 0xB, 0xA};
    for (lh_usize_t i = 0; i < 7; ++i)
    {
        EXPECT_EQ(dst[i], expect[i]);
    }
}

TEST(memory_raw_set_pattern, returns_null_when_dst_span_empty)
{
    lh_uchar_t dst[1] = {1};
    const lh_uchar_t pat[] = {2};
    const lh_ptr r = lh_memory_raw_set_pattern(dst, dst, pat, pat + 1);
    EXPECT_TRUE(lh_null_eq(r));
    EXPECT_EQ(dst[0], 1);
}

TEST(memory_raw_set_pattern, returns_null_when_pattern_span_empty)
{
    lh_uchar_t dst[4] = {};
    const lh_uchar_t pat[] = {1};
    const lh_ptr r = lh_memory_raw_set_pattern(dst, dst + 4, pat, pat);
    EXPECT_TRUE(lh_null_eq(r));
    EXPECT_EQ(dst[0], 0);
}

TEST(memory_raw_compare, compares_only_min_length_when_equal)
{
    const lh_uchar_t lhs[] = {1, 2, 3};
    const lh_uchar_t rhs[] = {1, 2};
    const lh_ptr d = lh_memory_raw_compare(lhs, lhs + 3, rhs, rhs + 2);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_raw_compare, mismatch_within_min_length)
{
    const lh_uchar_t lhs[] = {1, 9, 3};
    const lh_uchar_t rhs[] = {1, 2, 3};
    const lh_ptr d = lh_memory_raw_compare(lhs, lhs + 3, rhs, rhs + 3);
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&lhs[1]));
}

TEST(memory_raw_rcompare, equal_suffix_returns_null)
{
    const lh_uchar_t lhs[] = {0, 0, 1, 2, 3, 4};
    const lh_uchar_t rhs[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_raw_rcompare(lhs, lhs + 6, rhs, rhs + 4);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_raw_rcompare, suffix_mismatch_returns_pointer_in_lhs_window)
{
    const lh_uchar_t lhs[] = {1, 2, 3, 9};
    const lh_uchar_t rhs[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_raw_rcompare(lhs, lhs + 4, rhs, rhs + 4);
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&lhs[3]));
}

TEST(memory_raw_find, finds_first_occurrence)
{
    const lh_uchar_t hay[] = {'a', 'b', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_raw_find(hay, hay + 5, needle, needle + 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[0]));
}

TEST(memory_raw_find, finds_second_when_first_differs)
{
    const lh_uchar_t hay[] = {'x', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_raw_find(hay, hay + 4, needle, needle + 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[1]));
}

TEST(memory_raw_find, not_found_returns_null)
{
    const lh_uchar_t hay[] = {1, 2, 3};
    const lh_uchar_t needle[] = {9, 9};
    const lh_ptr p = lh_memory_raw_find(hay, hay + 3, needle, needle + 2);
    EXPECT_TRUE(lh_null_eq(p));
}

TEST(memory_raw_rfind, finds_last_occurrence)
{
    const lh_uchar_t hay[] = {'a', 'b', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_raw_rfind(hay, hay + 5, needle, needle + 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[2]));
}

TEST(memory_raw_rfind, not_found_returns_null)
{
    const lh_uchar_t hay[] = {1, 2, 3};
    const lh_uchar_t needle[] = {9};
    const lh_ptr p = lh_memory_raw_rfind(hay, hay + 3, needle, needle + 1);
    EXPECT_TRUE(lh_null_eq(p));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_raw_set_pattern_death, null_dst_end)
{
    const lh_uchar_t pat[] = {1};
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_raw_set_pattern(dst, lh_null, pat, pat + 1));
}

TEST(memory_raw_set_pattern_death, null_src_end)
{
    lh_uchar_t dst[1] = {};
    const lh_uchar_t pat[] = {1};
    LH_EXPECT_DEATH(lh_memory_raw_set_pattern(dst, dst + 1, pat, lh_null));
}

TEST(memory_raw_find_death, null_lhs_end)
{
    const lh_uchar_t hay[] = {1};
    const lh_uchar_t needle[] = {1};
    LH_EXPECT_DEATH(lh_memory_raw_find(hay, lh_null, needle, needle + 1));
}

TEST(memory_raw_find_death, null_rhs_end)
{
    const lh_uchar_t hay[] = {1};
    const lh_uchar_t needle[] = {1};
    LH_EXPECT_DEATH(lh_memory_raw_find(hay, hay + 1, needle, lh_null));
}

TEST(memory_raw_rfind_death, null_lhs_end)
{
    const lh_uchar_t hay[] = {1};
    const lh_uchar_t needle[] = {1};
    LH_EXPECT_DEATH(lh_memory_raw_rfind(hay, lh_null, needle, needle + 1));
}

TEST(memory_raw_rfind_death, null_rhs_end)
{
    const lh_uchar_t hay[] = {1};
    const lh_uchar_t needle[] = {1};
    LH_EXPECT_DEATH(lh_memory_raw_rfind(hay, hay + 1, needle, lh_null));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
