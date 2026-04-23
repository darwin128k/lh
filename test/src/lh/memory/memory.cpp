#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory.h>
#include <lh/null.h>

namespace {

TEST(memory_copy, destination_smaller_truncates) {
    lh_uchar_t dst[2] = {};
    const lh_uchar_t src[] = {1, 2, 3, 4};
    lh_ptr end = lh_memory_copy(dst, 2, src, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 2));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
}

TEST(memory_copy, source_smaller_copies_only_source_length) {
    lh_uchar_t dst[8] = {};
    const lh_uchar_t src[] = {9, 8};
    lh_ptr end = lh_memory_copy(dst, 8, src, 2);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 2));
    EXPECT_EQ(dst[0], 9);
    EXPECT_EQ(dst[1], 8);
    EXPECT_EQ(dst[2], 0);
}

TEST(memory_copy_rev, min_length_and_reversed_in_destination) {
    lh_uchar_t dst[8] = {};
    const lh_uchar_t src[] = {1, 2, 3, 4};
    lh_ptr end = lh_memory_copy_rev(dst, 3, src, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 3));
    EXPECT_EQ(dst[0], 3);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 1);
    EXPECT_EQ(dst[3], 0);
}

TEST(memory_rcopy, overlapping_backward_copy_respects_min_size) {
    lh_uchar_t v[] = {1, 2, 3, 4, 5};
    lh_ptr r = lh_memory_rcopy(v + 1, 4, v, 5);
    EXPECT_EQ(r, static_cast<lh_ptr>(v + 1));
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 1);
    EXPECT_EQ(v[2], 2);
    EXPECT_EQ(v[3], 3);
    EXPECT_EQ(v[4], 4);
}

TEST(memory_move, forward_overlap_with_bounded_length) {
    lh_uchar_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8};
    lh_ptr end = lh_memory_move(buf + 2, 6, buf, 8);
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

TEST(memory_move, non_overlapping) {
    lh_uchar_t dst[4] = {};
    const lh_uchar_t src[] = {7, 6, 5, 4};
    lh_ptr end = lh_memory_move(dst, 4, src, 4);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 4));
    EXPECT_EQ(dst[0], 7);
    EXPECT_EQ(dst[3], 4);
}

TEST(memory_set, fills_range) {
    lh_uchar_t block[8];
    lh_ptr end = lh_memory_set(block, 8, 0xCD);
    EXPECT_EQ(end, static_cast<lh_ptr>(block + 8));
    for (lh_usize_t i = 0; i < 8; ++i) {
        EXPECT_EQ(block[i], 0xCD);
    }
}

TEST(memory_set_pattern, repeats_pattern) {
    lh_uchar_t dst[7] = {};
    const lh_uchar_t pat[] = {0xA, 0xB};
    lh_ptr end = lh_memory_set_pattern(dst, 7, pat, 2);
    EXPECT_EQ(end, static_cast<lh_ptr>(dst + 7));
    const lh_uchar_t expect[] = {0xA, 0xB, 0xA, 0xB, 0xA, 0xB, 0xA};
    for (lh_usize_t i = 0; i < 7; ++i) {
        EXPECT_EQ(dst[i], expect[i]);
    }
}

TEST(memory_set_pattern, returns_null_when_dst_size_zero) {
    lh_uchar_t dst[1] = {1};
    const lh_uchar_t pat[] = {2};
    const lh_ptr r = lh_memory_set_pattern(dst, 0, pat, 1);
    EXPECT_TRUE(lh_null_eq(r));
    EXPECT_EQ(dst[0], 1);
}

TEST(memory_set_pattern, returns_null_when_src_size_zero) {
    lh_uchar_t dst[4] = {};
    const lh_uchar_t pat[] = {1};
    const lh_ptr r = lh_memory_set_pattern(dst, 4, pat, 0);
    EXPECT_TRUE(lh_null_eq(r));
    EXPECT_EQ(dst[0], 0);
}

TEST(memory_compare, compares_only_min_length_when_equal) {
    const lh_uchar_t lhs[] = {1, 2, 3};
    const lh_uchar_t rhs[] = {1, 2};
    const lh_ptr d = lh_memory_compare(lhs, 3, rhs, 2);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_compare, mismatch_within_min_length) {
    const lh_uchar_t lhs[] = {1, 9, 3};
    const lh_uchar_t rhs[] = {1, 2, 3};
    const lh_ptr d = lh_memory_compare(lhs, 3, rhs, 3);
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&lhs[1]));
}

TEST(memory_rcompare, equal_suffix_returns_null) {
    const lh_uchar_t lhs[] = {0, 0, 1, 2, 3, 4};
    const lh_uchar_t rhs[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_rcompare(lhs, 6, rhs, 4);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_rcompare, suffix_mismatch_returns_pointer_in_lhs_window) {
    const lh_uchar_t lhs[] = {1, 2, 3, 9};
    const lh_uchar_t rhs[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_rcompare(lhs, 4, rhs, 4);
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, static_cast<const lh_ptr>(&lhs[3]));
}

TEST(memory_find, finds_first_occurrence) {
    const lh_uchar_t hay[] = {'a', 'b', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_find(hay, 5, needle, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[0]));
}

TEST(memory_find, finds_second_when_first_differs) {
    const lh_uchar_t hay[] = {'x', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_find(hay, 4, needle, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[1]));
}

TEST(memory_find, not_found_returns_null) {
    const lh_uchar_t hay[] = {1, 2, 3};
    const lh_uchar_t needle[] = {9, 9};
    const lh_ptr p = lh_memory_find(hay, 3, needle, 2);
    EXPECT_TRUE(lh_null_eq(p));
}

TEST(memory_find_step, step_one_matches_memory_find) {
    const lh_uchar_t hay[] = {'x', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr a = lh_memory_find(hay, 4, needle, 2);
    const lh_ptr b = lh_memory_find_step(hay, 4, needle, 2, 1);
    EXPECT_EQ(a, b);
}

TEST(memory_find_step, aligned_step_skips_misaligned_match) {
    const lh_uchar_t hay[] = {'x', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p_byte = lh_memory_find(hay, 4, needle, 2);
    ASSERT_TRUE(lh_null_ne(p_byte));
    EXPECT_EQ(p_byte, static_cast<const lh_ptr>(&hay[1]));

    const lh_ptr p_step = lh_memory_find_step(hay, 4, needle, 2, 2);
    EXPECT_TRUE(lh_null_eq(p_step));
}

TEST(memory_find_step, aligned_step_finds_on_grid) {
    const lh_uchar_t hay[] = {'a', 'b', 'c', 'd'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_find_step(hay, 4, needle, 2, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[0]));
}

TEST(memory_rfind_step, step_one_matches_memory_rfind) {
    const lh_uchar_t hay[] = {'a', 'b', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr a = lh_memory_rfind(hay, 5, needle, 2);
    const lh_ptr b = lh_memory_rfind_step(hay, 5, needle, 2, 1);
    EXPECT_EQ(a, b);
}

TEST(memory_rfind_step, aligned_step_prefers_last_grid_match) {
    const lh_uchar_t hay[] = {'a', 'b', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_rfind_step(hay, 5, needle, 2, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[2]));
}

TEST(memory_rfind, finds_last_occurrence) {
    const lh_uchar_t hay[] = {'a', 'b', 'a', 'b', 'c'};
    const lh_uchar_t needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_rfind(hay, 5, needle, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, static_cast<const lh_ptr>(&hay[2]));
}

TEST(memory_rfind, not_found_returns_null) {
    const lh_uchar_t hay[] = {1, 2, 3};
    const lh_uchar_t needle[] = {9};
    const lh_ptr p = lh_memory_rfind(hay, 3, needle, 1);
    EXPECT_TRUE(lh_null_eq(p));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_set_pattern_death, null_dst) {
    const lh_uchar_t pat[] = {1};
    LH_EXPECT_DEATH(lh_memory_set_pattern(lh_null, 1, pat, 1));
}

TEST(memory_set_pattern_death, null_src) {
    lh_uchar_t dst[1] = {};
    LH_EXPECT_DEATH(lh_memory_set_pattern(dst, 1, lh_null, 1));
}

TEST(memory_find_death, null_lhs) {
    const lh_uchar_t needle[] = {1};
    LH_EXPECT_DEATH(lh_memory_find(lh_null, 0, needle, 1));
}

TEST(memory_find_death, null_rhs) {
    const lh_uchar_t hay[] = {1};
    LH_EXPECT_DEATH(lh_memory_find(hay, 1, lh_null, 1));
}

TEST(memory_rfind_death, null_lhs) {
    const lh_uchar_t needle[] = {1};
    LH_EXPECT_DEATH(lh_memory_rfind(lh_null, 0, needle, 1));
}

TEST(memory_rfind_death, null_rhs) {
    const lh_uchar_t hay[] = {1};
    LH_EXPECT_DEATH(lh_memory_rfind(hay, 1, lh_null, 1));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
