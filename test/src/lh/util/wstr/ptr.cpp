#include <gtest/gtest.h>

#include <cstddef>

#include <lh/bool.h>
#include <lh/char/map.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/size.h>
#include <lh/util/wchar.h>
#include <lh/util/wstr/ptr.h>
#include <lh/wchar.h>

namespace
{

void
expect_wchar_grid_aligned(lh_wstr_cptr base, lh_wstr_cptr p)
{
    ASSERT_TRUE(lh_null_ne(base));
    ASSERT_TRUE(lh_null_ne(p));
    const auto byte_diff = reinterpret_cast<const char *>(p) - reinterpret_cast<const char *>(base);
    EXPECT_EQ(byte_diff % static_cast<std::ptrdiff_t>(LH_WCHAR_T_SIZE), 0)
        << "returned pointer must fall on a wchar_t boundary relative to base";
}

TEST(wstr_raw_find_char, returns_aligned_pointer_to_matching_code_unit)
{
    const lh_wchar_t units[] = {1, 2, 3, 2};
    lh_wstr_cptr p = lh_wstr_ptr_find_char(units, 4, static_cast<lh_wchar_t>(2));
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(units, p);
    EXPECT_EQ(p, units + 1);
    EXPECT_EQ(*p, static_cast<lh_wchar_t>(2));

    lh_wstr_cptr miss = lh_wstr_ptr_find_char(units, 4, static_cast<lh_wchar_t>(99));
    EXPECT_TRUE(lh_null_eq(miss));
}

TEST(wstr_raw_find_char_death, null_str)
{
    LH_EXPECT_DEATH(lh_wstr_ptr_find_char(reinterpret_cast<lh_wstr_cptr>(lh_null), 1, L'x'));
}

TEST(wstr_raw_rfind_char, returns_last_match_or_null)
{
    const lh_wchar_t units[] = {1, 2, 3, 2};
    lh_wstr_cptr p = lh_wstr_ptr_rfind_char(units, 4, static_cast<lh_wchar_t>(2));
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(units, p);
    EXPECT_EQ(p, units + 3);

    lh_wstr_cptr miss = lh_wstr_ptr_rfind_char(units, 4, static_cast<lh_wchar_t>(99));
    EXPECT_TRUE(lh_null_eq(miss));
}

TEST(wstr_raw_find_of_null_terminator_by_size, finds_wide_nul_or_null)
{
    const lh_wchar_t with_nul[] = {L'a', L'b', lh_wstr_ptr_cat_va(lh_char_map_nul), L'c'};
    lh_wstr_cptr p = lh_wstr_ptr_find_of_null_terminator_by_size(with_nul, 4);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(with_nul, p);
    EXPECT_EQ(p, with_nul + 2);
    EXPECT_EQ(*p, lh_wstr_ptr_cat_va(lh_char_map_nul));

    lh_wstr_cptr miss_before = lh_wstr_ptr_find_of_null_terminator_by_size(with_nul, 2);
    EXPECT_TRUE(lh_null_eq(miss_before));

    const lh_wchar_t no_nul[] = {1, 2, 3};
    lh_wstr_cptr miss_absent = lh_wstr_ptr_find_of_null_terminator_by_size(no_nul, 3);
    EXPECT_TRUE(lh_null_eq(miss_absent));
}

TEST(wstr_raw_find_of_null_terminator_by_size_death, null_str)
{
    LH_EXPECT_DEATH(
        lh_wstr_ptr_find_of_null_terminator_by_size(reinterpret_cast<lh_wstr_cptr>(lh_null), 1));
}

TEST(wstr_raw_find_of_null_terminator, finds_first_wide_nul)
{
    const lh_wchar_t s[] = {L'x', lh_wstr_ptr_cat_va(lh_char_map_nul)};
    lh_wstr_cptr p = lh_wstr_ptr_find_of_null_terminator(s);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(s, p);
    EXPECT_EQ(p, s + 1);
    EXPECT_EQ(*p, lh_wstr_ptr_cat_va(lh_char_map_nul));
}

TEST(wstr_raw_find_of_null_terminator_death, null_str)
{
    LH_EXPECT_DEATH(lh_wstr_ptr_find_of_null_terminator(reinterpret_cast<lh_wstr_cptr>(lh_null)));
}

TEST(wstr_raw_len, counts_wide_chars_before_nul)
{
    const lh_wchar_t empty[] = {lh_wstr_ptr_cat_va(lh_char_map_nul)};
    EXPECT_EQ(lh_wstr_ptr_len(empty), 0U);

    const lh_wchar_t one[] = {L'z', lh_wstr_ptr_cat_va(lh_char_map_nul)};
    EXPECT_EQ(lh_wstr_ptr_len(one), 1U);

    const lh_wchar_t abc[] = {L'a', L'b', L'c', lh_wstr_ptr_cat_va(lh_char_map_nul)};
    EXPECT_EQ(lh_wstr_ptr_len(abc), 3U);
}

TEST(wstr_raw_len_death, no_null_terminator_when_scan_size_is_zero)
{
    const lh_uaddr_t addr = static_cast<lh_uaddr_t>(LH_USIZE_T_MAX);
    lh_wstr_ptr p = reinterpret_cast<lh_wstr_ptr>(addr);
    ASSERT_TRUE(lh_null_ne(p));
    LH_EXPECT_DEATH((void)lh_wstr_ptr_len(p));
}

TEST(wstr_raw_copy, copies_code_units)
{
    lh_wchar_t dst[8] = {};
    const lh_wchar_t src[] = {L'h', L'i', lh_wstr_ptr_cat_va(lh_char_map_nul)};
    lh_wstr_ptr end = lh_wstr_ptr_copy(dst, 8, src, 3);
    ASSERT_TRUE(lh_null_ne(end));
    expect_wchar_grid_aligned(dst, end - 1);
    EXPECT_EQ(dst[0], L'h');
    EXPECT_EQ(dst[1], L'i');
    EXPECT_EQ(dst[2], lh_wstr_ptr_cat_va(lh_char_map_nul));
}

TEST(wstr_raw_set, fills_with_code_unit)
{
    lh_wchar_t buf[4] = {L'a', L'b', L'c', L'd'};
    lh_wstr_ptr end = lh_wstr_ptr_set(buf, 4, L'z');
    ASSERT_TRUE(lh_null_ne(end));
    expect_wchar_grid_aligned(buf, end - 1);
    for (lh_usize_t i = 0; i < 4; ++i)
    {
        EXPECT_EQ(buf[i], L'z') << "index " << i;
    }
}

TEST(wstr_raw_set_pattern, repeats_pattern_into_destination)
{
    lh_wchar_t buf[5] = {};
    const lh_wchar_t pat[] = {L'a', L'b'};
    lh_wstr_ptr end = lh_wstr_ptr_set_pattern(buf, 5, pat, 2);
    ASSERT_TRUE(lh_null_ne(end));
    expect_wchar_grid_aligned(buf, end - 1);
    EXPECT_EQ(buf[0], L'a');
    EXPECT_EQ(buf[1], L'b');
    EXPECT_EQ(buf[2], L'a');
    EXPECT_EQ(buf[3], L'b');
    EXPECT_EQ(buf[4], L'a');
}

TEST(wstr_raw_contains, returns_true_if_substring_exists)
{
    const lh_wchar_t *hay = L"hello world";
    EXPECT_TRUE(lh_wstr_ptr_contains(hay, L"hello", lh_bool_false));
    EXPECT_TRUE(lh_wstr_ptr_contains(hay, L"world", lh_bool_false));
    EXPECT_TRUE(lh_wstr_ptr_contains(hay, L"o w", lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_contains(hay, L"bye", lh_bool_false));
}

TEST(wstr_raw_contains_by_size, returns_true_if_substring_exists)
{
    const lh_wchar_t hay[] = {L'h', L'e', L'l', L'l', L'o'};
    const lh_wchar_t nd[]  = {L'l', L'l'};
    const lh_wchar_t nd2[] = {L'x', L'y'};
    EXPECT_TRUE(lh_wstr_ptr_contains_by_size(hay, 5, nd, 2, lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_contains_by_size(hay, 5, nd2, 2, lh_bool_false));
}

TEST(wstr_raw_contains_by_size, ignore_case)
{
    const lh_wchar_t hay[] = {L'H', L'e', L'l', L'l', L'o'};
    const lh_wchar_t nd[]  = {L'h', L'e'};
    EXPECT_TRUE(lh_wstr_ptr_contains_by_size(hay, 5, nd, 2, lh_bool_true));
    EXPECT_FALSE(lh_wstr_ptr_contains_by_size(hay, 5, nd, 2, lh_bool_false));
}

TEST(wstr_raw_equals, returns_true_only_if_strings_are_identical)
{
    const lh_wchar_t *s = L"test";
    EXPECT_TRUE(lh_wstr_ptr_equals(s, L"test", lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_equals(s, L"tes", lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_equals(s, L"tests", lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_equals(s, L"TEST", lh_bool_false));
}

TEST(wstr_raw_fold_case, cyrillic_capital_maps_to_lower)
{
    EXPECT_EQ(lh_wchar_fold_case(static_cast<lh_wchar_t>(0x0410)), static_cast<lh_wchar_t>(0x0430));
}

TEST(wstr_raw_equals, ascii_and_cyrillic_ignore_case)
{
    const lh_wchar_t *s = L"test";
    EXPECT_TRUE(lh_wstr_ptr_equals(s, L"TEST", lh_bool_true));
    const lh_wchar_t cap_a[] = {static_cast<lh_wchar_t>(0x0410), static_cast<lh_wchar_t>(0)};
    const lh_wchar_t low_a[] = {static_cast<lh_wchar_t>(0x0430), static_cast<lh_wchar_t>(0)};
    EXPECT_TRUE(lh_wstr_ptr_equals(cap_a, low_a, lh_bool_true));
}

TEST(wstr_raw_equals_by_size, returns_true_only_if_size_and_content_match)
{
    const lh_wchar_t *s = L"test";
    EXPECT_TRUE(lh_wstr_ptr_equals_by_size(s, 4, L"test", 4, lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_equals_by_size(s, 4, L"tes", 3, lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_equals_by_size(s, 4, L"tests", 5, lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_equals_by_size(s, 4, L"TEST", 4, lh_bool_false));
}

TEST(wstr_raw_equals_by_size, ignore_case)
{
    const lh_wchar_t *s = L"test";
    EXPECT_TRUE(lh_wstr_ptr_equals_by_size(s, 4, L"TEST", 4, lh_bool_true));
    EXPECT_FALSE(lh_wstr_ptr_equals_by_size(s, 4, L"TES", 3, lh_bool_true));
}

TEST(wstr_raw_contains, finds_substring_ignore_case)
{
    const lh_wchar_t *hay = L"Hello World";
    EXPECT_TRUE(lh_wstr_ptr_contains(hay, L"hello", lh_bool_true));
    EXPECT_TRUE(lh_wstr_ptr_contains(hay, L"WORLD", lh_bool_true));
    EXPECT_FALSE(lh_wstr_ptr_contains(hay, L"bye", lh_bool_true));
}

TEST(wstr_raw_index_of, returns_first_position_or_invalid)
{
    const lh_wchar_t *hay = L"hello world";
    EXPECT_EQ(lh_wstr_ptr_index_of(hay, L"hello", lh_bool_false), 0U);
    EXPECT_EQ(lh_wstr_ptr_index_of(hay, L"world", lh_bool_false), 6U);
    EXPECT_EQ(lh_wstr_ptr_index_of(hay, L"bye", lh_bool_false), LH_WSTR_PTR_INVALID);
}

TEST(wstr_raw_index_of, supports_ignore_case)
{
    const lh_wchar_t *hay = L"Hello World";
    EXPECT_EQ(lh_wstr_ptr_index_of(hay, L"world", lh_bool_true), 6U);
}

TEST(wstr_raw_index_of_by_size, supports_non_terminated_buffers)
{
    const lh_wchar_t hay[] = {L'a', L'b', L'c', L'd', L'e'};
    const lh_wchar_t nd[] = {L'c', L'd'};
    EXPECT_EQ(lh_wstr_ptr_index_of_by_size(hay, 5, nd, 2, lh_bool_false), 2U);
    EXPECT_EQ(lh_wstr_ptr_index_of_by_size(hay, 5, nd, 0, lh_bool_false), 0U);
}

TEST(wstr_raw_starts_with, returns_true_if_str_starts_with_src)
{
    const lh_wchar_t *s = L"hello world";
    EXPECT_TRUE(lh_wstr_ptr_starts_with(s, L"hello", lh_bool_false));
    EXPECT_TRUE(lh_wstr_ptr_starts_with(s, L"h", lh_bool_false));
    EXPECT_TRUE(lh_wstr_ptr_starts_with(s, L"", lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_starts_with(s, L"world", lh_bool_false));
}

TEST(wstr_raw_starts_with, ignore_case)
{
    const lh_wchar_t *s = L"Hello world";
    EXPECT_TRUE(lh_wstr_ptr_starts_with(s, L"hello", lh_bool_true));
}

TEST(wstr_raw_starts_with_by_size, matches_prefix)
{
    const lh_wchar_t hay[]    = {L'h', L'e', L'l', L'l', L'o'};
    const lh_wchar_t prefix[] = {L'h', L'e'};
    const lh_wchar_t other[]  = {L'l', L'o'};
    EXPECT_TRUE(lh_wstr_ptr_starts_with_by_size(hay, 5, prefix, 2, lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_starts_with_by_size(hay, 5, other, 2, lh_bool_false));
}

TEST(wstr_raw_starts_with_by_size, returns_false_when_prefix_longer)
{
    const lh_wchar_t hay[]    = {L'h', L'i'};
    const lh_wchar_t prefix[] = {L'h', L'i', L'x'};
    EXPECT_FALSE(lh_wstr_ptr_starts_with_by_size(hay, 2, prefix, 3, lh_bool_false));
}

TEST(wstr_raw_starts_with_by_size, ignore_case)
{
    const lh_wchar_t hay[]    = {L'H', L'E', L'l', L'l', L'o'};
    const lh_wchar_t prefix[] = {L'h', L'e'};
    EXPECT_TRUE(lh_wstr_ptr_starts_with_by_size(hay, 5, prefix, 2, lh_bool_true));
    EXPECT_FALSE(lh_wstr_ptr_starts_with_by_size(hay, 5, prefix, 2, lh_bool_false));
}

TEST(wstr_raw_ends_with, returns_true_if_str_ends_with_src)
{
    const lh_wchar_t *s = L"hello world";
    EXPECT_TRUE(lh_wstr_ptr_ends_with(s, L"world", lh_bool_false));
    EXPECT_TRUE(lh_wstr_ptr_ends_with(s, L"d", lh_bool_false));
    EXPECT_TRUE(lh_wstr_ptr_ends_with(s, L"", lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_ends_with(s, L"hello", lh_bool_false));
}

TEST(wstr_raw_ends_with, ignore_case)
{
    const lh_wchar_t *s = L"hello WORLD";
    EXPECT_TRUE(lh_wstr_ptr_ends_with(s, L"world", lh_bool_true));
}

TEST(wstr_raw_ends_with_by_size, matches_suffix)
{
    const lh_wchar_t hay[]    = {L'h', L'e', L'l', L'l', L'o'};
    const lh_wchar_t suffix[] = {L'l', L'o'};
    const lh_wchar_t other[]  = {L'h', L'e'};
    EXPECT_TRUE(lh_wstr_ptr_ends_with_by_size(hay, 5, suffix, 2, lh_bool_false));
    EXPECT_FALSE(lh_wstr_ptr_ends_with_by_size(hay, 5, other, 2, lh_bool_false));
}

TEST(wstr_raw_ends_with_by_size, returns_false_when_suffix_longer)
{
    const lh_wchar_t hay[]    = {L'h', L'i'};
    const lh_wchar_t suffix[] = {L'x', L'h', L'i'};
    EXPECT_FALSE(lh_wstr_ptr_ends_with_by_size(hay, 2, suffix, 3, lh_bool_false));
}

TEST(wstr_raw_ends_with_by_size, ignore_case)
{
    const lh_wchar_t hay[]    = {L'h', L'e', L'l', L'l', L'O'};
    const lh_wchar_t suffix[] = {L'l', L'o'};
    EXPECT_TRUE(lh_wstr_ptr_ends_with_by_size(hay, 5, suffix, 2, lh_bool_true));
    EXPECT_FALSE(lh_wstr_ptr_ends_with_by_size(hay, 5, suffix, 2, lh_bool_false));
}

TEST(wstr_raw_find, substring_match_points_at_needle_first_unit)
{
    const lh_wchar_t hay[] = {L'x', L'x', L'h', L'e', L'l', L'l', L'o'};
    const lh_wchar_t nd[] = {L'h', L'e', L'l', L'l', L'o'};
    lh_wstr_cptr p = lh_wstr_ptr_find(hay, 7, nd, 5, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(hay, p);
    EXPECT_EQ(p, hay + 2);
    EXPECT_EQ(*p, L'h');
    EXPECT_EQ(p[1], L'e');
}

TEST(wstr_raw_rfind, rightmost_substring_starts_at_expected_unit)
{
    const lh_wchar_t hay[] = {L'a', L'b', L'a', L'b', L'a'};
    const lh_wchar_t nd[] = {L'b', L'a'};
    lh_wstr_cptr p = lh_wstr_ptr_rfind(hay, 5, nd, 2, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(hay, p);
    EXPECT_EQ(p, hay + 3);
    EXPECT_EQ(*p, L'b');
    EXPECT_EQ(p[1], L'a');
}

TEST(wstr_raw_compare, first_mismatch_points_into_lhs)
{
    const lh_wchar_t lhs[] = {L'a', L'x', L'c'};
    const lh_wchar_t rhs[] = {L'a', L'y', L'c'};
    lh_wstr_cptr p = lh_wstr_ptr_compare(lhs, 3, rhs, 3, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(lhs, p);
    EXPECT_EQ(p, lhs + 1);
    EXPECT_EQ(*p, L'x');
}

TEST(wstr_raw_rcompare, suffix_mismatch_points_into_lhs)
{
    const lh_wchar_t lhs[] = {L'a', L'b', L'c'};
    const lh_wchar_t rhs[] = {L'x', L'b', L'c'};
    lh_wstr_cptr p = lh_wstr_ptr_rcompare(lhs, 3, rhs, 3, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(lhs, p);
    EXPECT_EQ(p, lhs);
    EXPECT_EQ(*p, L'a');
}

TEST(wstr_raw_compare_by_ignore_case, first_mismatch_after_case_norm)
{
    const lh_wchar_t lhs[] = {L'A', L'x', L'c'};
    const lh_wchar_t rhs[] = {L'a', L'Y', L'c'};
    lh_wstr_cptr p = lh_wstr_ptr_compare_by_ignore_case(lhs, 3, rhs, 3);
    EXPECT_EQ(p, lh_wstr_ptr_compare(lhs, 3, rhs, 3, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(lhs, p);
    EXPECT_EQ(p, lhs + 1);
}

TEST(wstr_raw_find_by_ignore_case, finds_needle)
{
    const lh_wchar_t hay[] = {L'x', L'x', L'H', L'e', L'l', L'l', L'o'};
    const lh_wchar_t nd[] = {L'h', L'e', L'l', L'l', L'o'};
    lh_wstr_cptr p = lh_wstr_ptr_find_by_ignore_case(hay, 7, nd, 5);
    EXPECT_EQ(p, lh_wstr_ptr_find(hay, 7, nd, 5, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(hay, p);
    EXPECT_EQ(p, hay + 2);
}

/* -- find_of_chars / rfind_of_chars ---------------------------------------- */

TEST(wstr_raw_find_of_chars, returns_first_code_unit_from_set_or_null)
{
    const lh_wchar_t str[] = {L'x', L'b', L'a', L'y'};
    const lh_wchar_t set[] = {L'a', L'b'};

    lh_wstr_cptr p = lh_wstr_ptr_find_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(str, p);
    EXPECT_EQ(p, str + 1);

    const lh_wchar_t no_match[] = {L'x', L'y', L'z'};
    EXPECT_TRUE(lh_null_eq(lh_wstr_ptr_find_of_chars(no_match, 3, set, 2)));
}

TEST(wstr_raw_rfind_of_chars, returns_last_code_unit_from_set_or_null)
{
    const lh_wchar_t str[] = {L'a', L'x', L'b', L'y'};
    const lh_wchar_t set[] = {L'a', L'b'};

    lh_wstr_cptr p = lh_wstr_ptr_rfind_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(str, p);
    EXPECT_EQ(p, str + 2);

    const lh_wchar_t no_match[] = {L'x', L'y', L'z'};
    EXPECT_TRUE(lh_null_eq(lh_wstr_ptr_rfind_of_chars(no_match, 3, set, 2)));
}

/* -- find_not_of_chars / rfind_not_of_chars -------------------------------- */

TEST(wstr_raw_find_not_of_chars, returns_first_code_unit_outside_set_or_null)
{
    const lh_wchar_t str[] = {L'a', L'a', L'b', L'x'};
    const lh_wchar_t set[] = {L'a', L'b'};

    lh_wstr_cptr p = lh_wstr_ptr_find_not_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(str, p);
    EXPECT_EQ(p, str + 3);

    const lh_wchar_t all_in[] = {L'a', L'b', L'a'};
    EXPECT_TRUE(lh_null_eq(lh_wstr_ptr_find_not_of_chars(all_in, 3, set, 2)));
}

TEST(wstr_raw_rfind_not_of_chars, returns_last_code_unit_outside_set_or_null)
{
    const lh_wchar_t str[] = {L'x', L'a', L'b', L'b'};
    const lh_wchar_t set[] = {L'a', L'b'};

    lh_wstr_cptr p = lh_wstr_ptr_rfind_not_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    expect_wchar_grid_aligned(str, p);
    EXPECT_EQ(p, str + 0);

    const lh_wchar_t all_in[] = {L'a', L'b', L'a'};
    EXPECT_TRUE(lh_null_eq(lh_wstr_ptr_rfind_not_of_chars(all_in, 3, set, 2)));
}

/* -- contains_char --------------------------------------------------------- */

TEST(wstr_raw_contains_char, finds_code_unit_in_set_or_not)
{
    const lh_wchar_t set[] = {L'a', L'b', L'c'};
    EXPECT_TRUE(lh_wstr_ptr_contains_char(set, 3, L'b'));
    EXPECT_FALSE(lh_wstr_ptr_contains_char(set, 3, L'z'));
    EXPECT_FALSE(lh_wstr_ptr_contains_char(set, 0, L'a'));
}

TEST(wstr_raw_trim_custom, trims_left_right_and_all_whitespace)
{
    lh_wchar_t s1[] = L" \t\nabc \t";
    lh_wchar_t s2[] = L" \t\nabc \t";
    lh_wchar_t s3[] = L" \t\n ";
    const lh_wchar_t ws[] = L" \t\n";
    const lh_usize_t ws_size = lh_wstr_ptr_len(ws);

    EXPECT_STREQ(lh_wstr_ptr_ltrim_custom(s1, lh_wstr_ptr_len(s1), ws, ws_size), L"abc \t");
    EXPECT_STREQ(lh_wstr_ptr_rtrim_custom(s2, lh_wstr_ptr_len(s2), ws, ws_size), L" \t\nabc");
    EXPECT_STREQ(lh_wstr_ptr_trim_custom(s3, lh_wstr_ptr_len(s3), ws, ws_size), L"");
}

TEST(wstr_raw_trim_set_and_auto, infer_sizes_correctly)
{
    lh_wchar_t s1[] = L"\n\t abc\t ";
    lh_wchar_t s2[] = L"\n\t abc\t ";
    lh_wchar_t s3[] = L"\n\t abc\t ";
    const lh_wchar_t ws[] = L" \t\n";
    const lh_usize_t ws_size = lh_wstr_ptr_len(ws);

    EXPECT_STREQ(lh_wstr_ptr_ltrim_set(s1, ws, ws_size), L"abc\t ");
    EXPECT_STREQ(lh_wstr_ptr_rtrim_set(s2, ws, ws_size), L"\n\t abc");
    EXPECT_STREQ(lh_wstr_ptr_trim_auto(s3, ws), L"abc");
}

TEST(wstr_raw_trim, entrypoints_produce_same_result)
{
    lh_wchar_t a[] = L"  abc  ";
    lh_wchar_t b[] = L"  abc  ";
    lh_wchar_t c[] = L"  abc  ";
    lh_wchar_t d[] = L"  abc  ";
    lh_wchar_t e[] = L"  abc  ";
    lh_wchar_t f[] = L"  abc  ";
    const lh_wchar_t ws[] = L" ";
    const lh_usize_t ws_size = lh_wstr_ptr_len(ws);

    EXPECT_STREQ(lh_wstr_ptr_ltrim_custom(a, lh_wstr_ptr_len(a), ws, ws_size), L"abc  ");
    EXPECT_STREQ(lh_wstr_ptr_rtrim_custom(b, lh_wstr_ptr_len(b), ws, ws_size), L"  abc");
    EXPECT_STREQ(lh_wstr_ptr_trim_custom(c, lh_wstr_ptr_len(c), ws, ws_size), L"abc");
    EXPECT_STREQ(lh_wstr_ptr_trim_set(d, ws, ws_size), L"abc");
    EXPECT_STREQ(lh_wstr_ptr_trim_auto(e, ws), L"abc");
    EXPECT_STREQ(lh_wstr_ptr_trim_set(f, ws, ws_size), L"abc");
}

TEST(wstr_raw_trim_default, uses_builtin_whitespace_set)
{
    lh_wchar_t s1[] = L" \r\n\tabc \v";
    lh_wchar_t s2[] = L" \r\n\tabc \v";
    lh_wchar_t s3[] = L" \r\n\tabc \v";

    EXPECT_STREQ(lh_wstr_ptr_ltrim(s1), L"abc \v");
    EXPECT_STREQ(lh_wstr_ptr_rtrim(s2), L" \r\n\tabc");
    EXPECT_STREQ(lh_wstr_ptr_trim(s3), L"abc");
}

TEST(wstr_raw_trim_auto, ltrim_and_rtrim_variants)
{
    lh_wchar_t s1[] = L"  abc  ";
    lh_wchar_t s2[] = L"  abc  ";
    const lh_wchar_t ws[] = L" ";

    EXPECT_STREQ(lh_wstr_ptr_ltrim_auto(s1, ws), L"abc  ");
    EXPECT_STREQ(lh_wstr_ptr_rtrim_auto(s2, ws), L"  abc");
}

TEST(wstr_raw_trim_death, null_arguments)
{
    lh_wchar_t s[] = L"abc";
    const lh_wchar_t ws[] = L" ";

    LH_EXPECT_DEATH(lh_wstr_ptr_ltrim_custom(reinterpret_cast<lh_wstr_ptr>(lh_null),
                                             lh_wstr_ptr_len(s), ws, 1));
    LH_EXPECT_DEATH(lh_wstr_ptr_rtrim_custom(reinterpret_cast<lh_wstr_ptr>(lh_null),
                                             lh_wstr_ptr_len(s), ws, 1));
    LH_EXPECT_DEATH(
        lh_wstr_ptr_trim_custom(reinterpret_cast<lh_wstr_ptr>(lh_null), lh_wstr_ptr_len(s), ws, 1));

    LH_EXPECT_DEATH(lh_wstr_ptr_ltrim_custom(s, lh_wstr_ptr_len(s),
                                             reinterpret_cast<lh_wstr_cptr>(lh_null), 1));
}

} // namespace
