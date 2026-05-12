#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/char/map.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/util/str/ptr.h>

namespace
{

TEST(str_raw_find_of_char, returns_first_match_or_null)
{
    const lh_char_t a[] = {1, 2, 3, 2};
    lh_str_cptr p = lh_str_ptr_find_of_char(a, 4, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &a[1]);

    lh_str_cptr miss = lh_str_ptr_find_of_char(a, 4, 9);
    EXPECT_TRUE(lh_null_eq(miss));
}

TEST(str_raw_find_of_char_death, null_str)
{
    LH_EXPECT_DEATH(lh_str_ptr_find_of_char(reinterpret_cast<lh_str_cptr>(lh_null), 1, 'x'));
}

TEST(str_raw_rfind_of_char, returns_last_match_or_null)
{
    const lh_char_t a[] = {1, 2, 3, 2};
    lh_str_cptr p = lh_str_ptr_rfind_of_char(a, 4, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &a[3]);

    lh_str_cptr miss = lh_str_ptr_rfind_of_char(a, 4, 9);
    EXPECT_TRUE(lh_null_eq(miss));
}

TEST(str_raw_find_of_null_terminator_by_size, finds_nul_inside_prefix_or_null)
{
    const lh_char_t with_nul[] = {'a', 'b', lh_char_map_nul, 'c'};
    lh_str_cptr p = lh_str_ptr_find_of_null_terminator_by_size(with_nul, 4);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &with_nul[2]);

    lh_str_cptr miss_before = lh_str_ptr_find_of_null_terminator_by_size(with_nul, 2);
    EXPECT_TRUE(lh_null_eq(miss_before));

    const lh_char_t no_nul[] = {1, 2, 3};
    lh_str_cptr miss_absent = lh_str_ptr_find_of_null_terminator_by_size(no_nul, 3);
    EXPECT_TRUE(lh_null_eq(miss_absent));
}

TEST(str_raw_find_of_null_terminator_by_size_death, null_str)
{
    LH_EXPECT_DEATH(
        lh_str_ptr_find_of_null_terminator_by_size(reinterpret_cast<lh_str_cptr>(lh_null), 1));
}

TEST(str_raw_find_of_null_terminator, finds_first_nul)
{
    const lh_char_t s[] = {'x', lh_char_map_nul};
    lh_str_cptr p = lh_str_ptr_find_of_null_terminator(s);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &s[1]);
}

TEST(str_raw_find_of_null_terminator_death, null_str)
{
    LH_EXPECT_DEATH(lh_str_ptr_find_of_null_terminator(reinterpret_cast<lh_str_cptr>(lh_null)));
}

TEST(str_raw_len, counts_chars_before_nul)
{
    const lh_char_t empty[] = {lh_char_map_nul};
    EXPECT_EQ(lh_str_ptr_len(empty), 0U);

    const lh_char_t one[] = {'z', lh_char_map_nul};
    EXPECT_EQ(lh_str_ptr_len(one), 1U);

    const lh_char_t abc[] = {'a', 'b', 'c', lh_char_map_nul};
    EXPECT_EQ(lh_str_ptr_len(abc), 3U);
}

TEST(str_raw_len_death, no_null_terminator_when_scan_size_is_zero)
{
    // ::lh_str_ptr_find_of_null_terminator scans `LH_USIZE_T_MAX - lh_ptr_to_uaddr(str)` bytes.
    // If that value is 0, no memory is read, no NUL is found, and ::lh_str_ptr_len terminates with
    // ::lh_runtime_error_code_no_null_terminator.
    const lh_uaddr_t addr = static_cast<lh_uaddr_t>(LH_USIZE_T_MAX);
    lh_str_ptr p = reinterpret_cast<lh_str_ptr>(addr);
    ASSERT_TRUE(lh_null_ne(p));
    LH_EXPECT_DEATH((void)lh_str_ptr_len(p));
}

TEST(str_raw_contains, returns_true_if_substring_exists)
{
    const lh_char_t *hay = "hello world";
    EXPECT_TRUE(lh_str_ptr_contains(hay, "hello", lh_bool_false));
    EXPECT_TRUE(lh_str_ptr_contains(hay, "world", lh_bool_false));
    EXPECT_TRUE(lh_str_ptr_contains(hay, "o w", lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_contains(hay, "bye", lh_bool_false));
}

TEST(str_raw_contains, ignore_case_ascii)
{
    const lh_char_t *hay = "Hello World";
    EXPECT_TRUE(lh_str_ptr_contains(hay, "hello", lh_bool_true));
    EXPECT_TRUE(lh_str_ptr_contains(hay, "WORLD", lh_bool_true));
    EXPECT_FALSE(lh_str_ptr_contains(hay, "bye", lh_bool_true));
}

TEST(str_raw_contains_by_size, returns_true_if_substring_exists)
{
    const lh_char_t hay[] = {'h', 'e', 'l', 'l', 'o'};
    const lh_char_t nd[]  = {'l', 'l'};
    const lh_char_t nd2[] = {'x', 'y'};
    EXPECT_TRUE(lh_str_ptr_contains_by_size(hay, 5, nd, 2, lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_contains_by_size(hay, 5, nd2, 2, lh_bool_false));
}

TEST(str_raw_contains_by_size, ignore_case_ascii)
{
    const lh_char_t hay[] = {'H', 'e', 'l', 'l', 'o'};
    const lh_char_t nd[]  = {'h', 'e'};
    EXPECT_TRUE(lh_str_ptr_contains_by_size(hay, 5, nd, 2, lh_bool_true));
    EXPECT_FALSE(lh_str_ptr_contains_by_size(hay, 5, nd, 2, lh_bool_false));
}

TEST(str_raw_index_of, returns_first_position_or_invalid)
{
    const lh_char_t *hay = "hello world";
    EXPECT_EQ(lh_str_ptr_index_of(hay, "hello", lh_bool_false), 0U);
    EXPECT_EQ(lh_str_ptr_index_of(hay, "world", lh_bool_false), 6U);
    EXPECT_EQ(lh_str_ptr_index_of(hay, "bye", lh_bool_false), LH_STR_PTR_INVALID);
}

TEST(str_raw_index_of, supports_ignore_case)
{
    const lh_char_t *hay = "Hello World";
    EXPECT_EQ(lh_str_ptr_index_of(hay, "world", lh_bool_true), 6U);
}

TEST(str_raw_index_of_by_size, supports_non_terminated_buffers)
{
    const lh_char_t hay[] = {'a', 'b', 'c', 'd', 'e'};
    const lh_char_t nd[] = {'c', 'd'};
    EXPECT_EQ(lh_str_ptr_index_of_by_size(hay, 5, nd, 2, lh_bool_false), 2U);
    EXPECT_EQ(lh_str_ptr_index_of_by_size(hay, 5, nd, 0, lh_bool_false), 0U);
}

TEST(str_raw_equals, returns_true_only_if_strings_are_identical)
{
    const lh_char_t *s = "test";
    EXPECT_TRUE(lh_str_ptr_equals(s, "test", lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_equals(s, "tes", lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_equals(s, "tests", lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_equals(s, "TEST", lh_bool_false));
}

TEST(str_raw_equals, ignore_case_ascii)
{
    const lh_char_t *s = "test";
    EXPECT_TRUE(lh_str_ptr_equals(s, "TEST", lh_bool_true));
}

TEST(str_raw_equals_by_size, returns_true_only_if_size_and_content_match)
{
    const lh_char_t *s = "test";
    EXPECT_TRUE(lh_str_ptr_equals_by_size(s, 4, "test", 4, lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_equals_by_size(s, 4, "tes", 3, lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_equals_by_size(s, 4, "tests", 5, lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_equals_by_size(s, 4, "TEST", 4, lh_bool_false));
}

TEST(str_raw_equals_by_size, ignore_case_ascii)
{
    const lh_char_t *s = "test";
    EXPECT_TRUE(lh_str_ptr_equals_by_size(s, 4, "TEST", 4, lh_bool_true));
    EXPECT_FALSE(lh_str_ptr_equals_by_size(s, 4, "TES", 3, lh_bool_true));
}

TEST(str_raw_starts_with, returns_true_if_str_starts_with_src)
{
    const lh_char_t *s = "hello world";
    EXPECT_TRUE(lh_str_ptr_starts_with(s, "hello", lh_bool_false));
    EXPECT_TRUE(lh_str_ptr_starts_with(s, "h", lh_bool_false));
    EXPECT_TRUE(lh_str_ptr_starts_with(s, "", lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_starts_with(s, "world", lh_bool_false));
}

TEST(str_raw_starts_with, ignore_case_ascii)
{
    const lh_char_t *s = "Hello world";
    EXPECT_TRUE(lh_str_ptr_starts_with(s, "hello", lh_bool_true));
}

TEST(str_raw_starts_with_by_size, matches_prefix)
{
    const lh_char_t hay[]    = {'h', 'e', 'l', 'l', 'o'};
    const lh_char_t prefix[] = {'h', 'e'};
    const lh_char_t other[]  = {'l', 'o'};
    EXPECT_TRUE(lh_str_ptr_starts_with_by_size(hay, 5, prefix, 2, lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_starts_with_by_size(hay, 5, other, 2, lh_bool_false));
}

TEST(str_raw_starts_with_by_size, returns_false_when_prefix_longer)
{
    const lh_char_t hay[]    = {'h', 'i'};
    const lh_char_t prefix[] = {'h', 'i', 'x'};
    EXPECT_FALSE(lh_str_ptr_starts_with_by_size(hay, 2, prefix, 3, lh_bool_false));
}

TEST(str_raw_starts_with_by_size, ignore_case_ascii)
{
    const lh_char_t hay[]    = {'H', 'E', 'l', 'l', 'o'};
    const lh_char_t prefix[] = {'h', 'e'};
    EXPECT_TRUE(lh_str_ptr_starts_with_by_size(hay, 5, prefix, 2, lh_bool_true));
    EXPECT_FALSE(lh_str_ptr_starts_with_by_size(hay, 5, prefix, 2, lh_bool_false));
}

TEST(str_raw_ends_with, returns_true_if_str_ends_with_src)
{
    const lh_char_t *s = "hello world";
    EXPECT_TRUE(lh_str_ptr_ends_with(s, "world", lh_bool_false));
    EXPECT_TRUE(lh_str_ptr_ends_with(s, "d", lh_bool_false));
    EXPECT_TRUE(lh_str_ptr_ends_with(s, "", lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_ends_with(s, "hello", lh_bool_false));
}

TEST(str_raw_ends_with, ignore_case_ascii)
{
    const lh_char_t *s = "hello WORLD";
    EXPECT_TRUE(lh_str_ptr_ends_with(s, "world", lh_bool_true));
}

TEST(str_raw_ends_with_by_size, matches_suffix)
{
    const lh_char_t hay[]    = {'h', 'e', 'l', 'l', 'o'};
    const lh_char_t suffix[] = {'l', 'o'};
    const lh_char_t other[]  = {'h', 'e'};
    EXPECT_TRUE(lh_str_ptr_ends_with_by_size(hay, 5, suffix, 2, lh_bool_false));
    EXPECT_FALSE(lh_str_ptr_ends_with_by_size(hay, 5, other, 2, lh_bool_false));
}

TEST(str_raw_ends_with_by_size, returns_false_when_suffix_longer)
{
    const lh_char_t hay[]    = {'h', 'i'};
    const lh_char_t suffix[] = {'x', 'h', 'i'};
    EXPECT_FALSE(lh_str_ptr_ends_with_by_size(hay, 2, suffix, 3, lh_bool_false));
}

TEST(str_raw_ends_with_by_size, ignore_case_ascii)
{
    const lh_char_t hay[]    = {'h', 'e', 'l', 'l', 'O'};
    const lh_char_t suffix[] = {'l', 'o'};
    EXPECT_TRUE(lh_str_ptr_ends_with_by_size(hay, 5, suffix, 2, lh_bool_true));
    EXPECT_FALSE(lh_str_ptr_ends_with_by_size(hay, 5, suffix, 2, lh_bool_false));
}

TEST(str_raw_find_by_ignore_case, finds_needle)
{
    const lh_char_t hay[] = {'x', 'x', 'H', 'e', 'l', 'l', 'o'};
    const lh_char_t nd[] = {'h', 'e', 'l', 'l', 'o'};
    lh_str_cptr p = lh_str_ptr_find_by_ignore_case(hay, 7, nd, 5);
    EXPECT_EQ(p, lh_str_ptr_find(hay, 7, nd, 5, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, hay + 2);
}

TEST(str_raw_compare_by_ignore_case, first_mismatch_after_fold)
{
    const lh_char_t lhs[] = {'A', 'x', 'c'};
    const lh_char_t rhs[] = {'a', 'Y', 'c'};
    lh_str_cptr p = lh_str_ptr_compare_by_ignore_case(lhs, 3, rhs, 3);
    EXPECT_EQ(p, lh_str_ptr_compare(lhs, 3, rhs, 3, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lhs + 1);
}

TEST(str_raw_trim_custom, trims_left_right_and_all_whitespace)
{
    lh_char_t s1[] = " \t\nabc \t";
    lh_char_t s2[] = " \t\nabc \t";
    lh_char_t s3[] = " \t\n ";
    const lh_char_t ws[] = " \t\n";
    const lh_usize_t ws_size = lh_str_ptr_len(ws);

    EXPECT_STREQ(lh_str_ptr_ltrim_custom(s1, lh_str_ptr_len(s1), ws, ws_size), "abc \t");
    EXPECT_STREQ(lh_str_ptr_rtrim_custom(s2, lh_str_ptr_len(s2), ws, ws_size), " \t\nabc");
    EXPECT_STREQ(lh_str_ptr_trim_custom(s3, lh_str_ptr_len(s3), ws, ws_size), "");
}

TEST(str_raw_trim_set_and_auto, infer_sizes_correctly)
{
    lh_char_t s1[] = "\n\t abc\t ";
    lh_char_t s2[] = "\n\t abc\t ";
    lh_char_t s3[] = "\n\t abc\t ";
    const lh_char_t ws[] = " \t\n";
    const lh_usize_t ws_size = lh_str_ptr_len(ws);

    EXPECT_STREQ(lh_str_ptr_ltrim_set(s1, ws, ws_size), "abc\t ");
    EXPECT_STREQ(lh_str_ptr_rtrim_set(s2, ws, ws_size), "\n\t abc");
    EXPECT_STREQ(lh_str_ptr_trim_auto(s3, ws), "abc");
}

TEST(str_raw_trim, entrypoints_produce_same_result)
{
    lh_char_t a[] = "  abc  ";
    lh_char_t b[] = "  abc  ";
    lh_char_t c[] = "  abc  ";
    lh_char_t d[] = "  abc  ";
    lh_char_t e[] = "  abc  ";
    lh_char_t f[] = "  abc  ";
    const lh_char_t ws[] = " ";
    const lh_usize_t ws_size = lh_str_ptr_len(ws);

    EXPECT_STREQ(lh_str_ptr_ltrim_custom(a, lh_str_ptr_len(a), ws, ws_size), "abc  ");
    EXPECT_STREQ(lh_str_ptr_rtrim_custom(b, lh_str_ptr_len(b), ws, ws_size), "  abc");
    EXPECT_STREQ(lh_str_ptr_trim_custom(c, lh_str_ptr_len(c), ws, ws_size), "abc");
    EXPECT_STREQ(lh_str_ptr_trim_set(d, ws, ws_size), "abc");
    EXPECT_STREQ(lh_str_ptr_trim_auto(e, ws), "abc");
    EXPECT_STREQ(lh_str_ptr_trim_set(f, ws, ws_size), "abc");
}

TEST(str_raw_trim_default, uses_builtin_whitespace_set)
{
    lh_char_t s1[] = " \r\n\tabc \v";
    lh_char_t s2[] = " \r\n\tabc \v";
    lh_char_t s3[] = " \r\n\tabc \v";

    EXPECT_STREQ(lh_str_ptr_ltrim(s1), "abc \v");
    EXPECT_STREQ(lh_str_ptr_rtrim(s2), " \r\n\tabc");
    EXPECT_STREQ(lh_str_ptr_trim(s3), "abc");
}

TEST(str_raw_trim_death, null_arguments)
{
    lh_char_t s[] = "abc";
    const lh_char_t ws[] = " ";

    LH_EXPECT_DEATH(
        lh_str_ptr_ltrim_custom(reinterpret_cast<lh_str_ptr>(lh_null), lh_str_ptr_len(s), ws, 1));
    LH_EXPECT_DEATH(
        lh_str_ptr_rtrim_custom(reinterpret_cast<lh_str_ptr>(lh_null), lh_str_ptr_len(s), ws, 1));
    LH_EXPECT_DEATH(
        lh_str_ptr_trim_custom(reinterpret_cast<lh_str_ptr>(lh_null), lh_str_ptr_len(s), ws, 1));

    LH_EXPECT_DEATH(
        lh_str_ptr_ltrim_custom(s, lh_str_ptr_len(s), reinterpret_cast<lh_str_cptr>(lh_null), 1));
}

TEST(str_raw_trim_auto, ltrim_and_rtrim_variants)
{
    lh_char_t s1[] = "  abc  ";
    lh_char_t s2[] = "  abc  ";
    const lh_char_t ws[] = " ";

    EXPECT_STREQ(lh_str_ptr_ltrim_auto(s1, ws), "abc  ");
    EXPECT_STREQ(lh_str_ptr_rtrim_auto(s2, ws), "  abc");
}

/* -- find_of_chars / rfind_of_chars ---------------------------------------- */

TEST(str_raw_find_of_chars, returns_first_char_from_set_or_null)
{
    const lh_char_t str[] = {'x', 'b', 'a', 'y'};
    const lh_char_t set[] = {'a', 'b'};

    lh_str_cptr p = lh_str_ptr_find_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, str + 1);

    const lh_char_t no_match[] = {'x', 'y', 'z'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_find_of_chars(no_match, 3, set, 2)));
}

TEST(str_raw_rfind_of_chars, returns_last_char_from_set_or_null)
{
    const lh_char_t str[] = {'a', 'x', 'b', 'y'};
    const lh_char_t set[] = {'a', 'b'};

    lh_str_cptr p = lh_str_ptr_rfind_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, str + 2);

    const lh_char_t no_match[] = {'x', 'y', 'z'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_rfind_of_chars(no_match, 3, set, 2)));
}

/* -- find_not_of_chars / rfind_not_of_chars -------------------------------- */

TEST(str_raw_find_not_of_chars, returns_first_char_outside_set_or_null)
{
    const lh_char_t str[] = {'a', 'a', 'b', 'x'};
    const lh_char_t set[] = {'a', 'b'};

    lh_str_cptr p = lh_str_ptr_find_not_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, str + 3);

    const lh_char_t all_in[] = {'a', 'b', 'a'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_find_not_of_chars(all_in, 3, set, 2)));
}

TEST(str_raw_rfind_not_of_chars, returns_last_char_outside_set_or_null)
{
    const lh_char_t str[] = {'x', 'a', 'b', 'b'};
    const lh_char_t set[] = {'a', 'b'};

    lh_str_cptr p = lh_str_ptr_rfind_not_of_chars(str, 4, set, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, str + 0);

    const lh_char_t all_in[] = {'a', 'b', 'a'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_rfind_not_of_chars(all_in, 3, set, 2)));
}

/* -- contains_char --------------------------------------------------------- */

TEST(str_raw_contains_char, finds_char_in_set_or_not)
{
    const lh_char_t set[] = {'a', 'b', 'c'};
    EXPECT_TRUE(lh_str_ptr_contains_char(set, 3, 'b'));
    EXPECT_FALSE(lh_str_ptr_contains_char(set, 3, 'z'));
    EXPECT_FALSE(lh_str_ptr_contains_char(set, 0, 'a'));
}

/* -- find / rfind ---------------------------------------------------------- */

TEST(str_raw_find, dispatches_exact_and_ignore_case)
{
    const lh_char_t hay[] = {'a', 'B', 'c', 'd'};
    const lh_char_t nd[]  = {'b', 'c'};

    EXPECT_TRUE(lh_null_eq(lh_str_ptr_find(hay, 4, nd, 2, lh_bool_false)));
    lh_str_cptr p = lh_str_ptr_find(hay, 4, nd, 2, lh_bool_true);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, hay + 1);
}

TEST(str_raw_rfind, returns_last_occurrence)
{
    const lh_char_t hay[] = {'a', 'b', 'x', 'a', 'b'};
    const lh_char_t nd[]  = {'a', 'b'};

    lh_str_cptr p = lh_str_ptr_rfind(hay, 5, nd, 2, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, hay + 3);
}

TEST(str_raw_rfind, dispatches_ignore_case)
{
    const lh_char_t hay[] = {'A', 'B', 'x', 'A', 'B'};
    const lh_char_t nd[]  = {'a', 'b'};

    EXPECT_TRUE(lh_null_eq(lh_str_ptr_rfind(hay, 5, nd, 2, lh_bool_false)));

    lh_str_cptr icase = lh_str_ptr_rfind(hay, 5, nd, 2, lh_bool_true);
    ASSERT_TRUE(lh_null_ne(icase));
    EXPECT_EQ(icase, hay + 3);
}

/* -- compare / rcompare ---------------------------------------------------- */

TEST(str_raw_compare, returns_null_when_prefix_matches)
{
    const lh_char_t a[] = {'a', 'b', 'c'};
    const lh_char_t b[] = {'a', 'b', 'c'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_compare(a, 3, b, 3, lh_bool_false)));
}

TEST(str_raw_compare, returns_pointer_to_first_mismatch)
{
    const lh_char_t a[] = {'a', 'x', 'c'};
    const lh_char_t b[] = {'a', 'y', 'c'};
    lh_str_cptr p = lh_str_ptr_compare(a, 3, b, 3, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, a + 1);
}

TEST(str_raw_compare, ignore_case_reports_equal)
{
    const lh_char_t a[] = {'A', 'B', 'C'};
    const lh_char_t b[] = {'a', 'b', 'c'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_compare(a, 3, b, 3, lh_bool_true)));
}

TEST(str_raw_rcompare, returns_null_when_suffix_matches)
{
    const lh_char_t a[] = {'a', 'b', 'c'};
    const lh_char_t b[] = {'a', 'b', 'c'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_rcompare(a, 3, b, 3, lh_bool_false)));
}

TEST(str_raw_rcompare, returns_pointer_to_suffix_mismatch)
{
    const lh_char_t a[] = {'a', 'x', 'c'};
    const lh_char_t b[] = {'a', 'y', 'c'};
    lh_str_cptr p = lh_str_ptr_rcompare(a, 3, b, 3, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, a + 1);
}

TEST(str_raw_rcompare, ignore_case_reports_equal)
{
    const lh_char_t a[] = {'A', 'B', 'C'};
    const lh_char_t b[] = {'a', 'b', 'c'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_rcompare(a, 3, b, 3, lh_bool_true)));
}

/* -- rcompare_by_ignore_case ----------------------------------------------- */

TEST(str_raw_rcompare_by_ignore_case, first_suffix_mismatch_after_fold)
{
    const lh_char_t lhs[] = {'A', 'x', 'C'};
    const lh_char_t rhs[] = {'a', 'Y', 'c'};
    lh_str_cptr p = lh_str_ptr_rcompare_by_ignore_case(lhs, 3, rhs, 3);
    EXPECT_EQ(p, lh_str_ptr_rcompare(lhs, 3, rhs, 3, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lhs + 1);
}

TEST(str_raw_rcompare_by_ignore_case, returns_null_when_suffix_folds_equal)
{
    const lh_char_t lhs[] = {'A', 'B', 'C'};
    const lh_char_t rhs[] = {'a', 'b', 'c'};
    EXPECT_TRUE(lh_null_eq(lh_str_ptr_rcompare_by_ignore_case(lhs, 3, rhs, 3)));
}

/* -- rfind_by_ignore_case -------------------------------------------------- */

TEST(str_raw_rfind_by_ignore_case, finds_last_needle)
{
    const lh_char_t hay[] = {'H', 'i', 'x', 'h', 'I'};
    const lh_char_t nd[]  = {'h', 'i'};
    lh_str_cptr p = lh_str_ptr_rfind_by_ignore_case(hay, 5, nd, 2);
    EXPECT_EQ(p, lh_str_ptr_rfind(hay, 5, nd, 2, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, hay + 3);
}

/* -- copy / move ----------------------------------------------------------- */

TEST(str_raw_copy, copies_min_of_both_sizes)
{
    const lh_char_t src[] = {'a', 'b', 'c', 'd'};
    lh_char_t dst[4] = {};

    lh_str_ptr end = lh_str_ptr_copy(dst, 4, src, 4);
    EXPECT_EQ(end, dst + 4);
    EXPECT_EQ(dst[0], 'a');
    EXPECT_EQ(dst[3], 'd');
}

TEST(str_raw_copy, respects_dst_size_limit)
{
    const lh_char_t src[] = {'a', 'b', 'c', 'd'};
    lh_char_t dst[2] = {};

    lh_str_ptr end = lh_str_ptr_copy(dst, 2, src, 4);
    EXPECT_EQ(end, dst + 2);
    EXPECT_EQ(dst[0], 'a');
    EXPECT_EQ(dst[1], 'b');
}

TEST(str_raw_move, handles_overlapping_buffers)
{
    lh_char_t buf[] = {'a', 'b', 'c', 'd', 'e'};
    lh_str_ptr end = lh_str_ptr_move(buf + 1, 3, buf, 3);
    EXPECT_EQ(end, buf + 4);
    EXPECT_EQ(buf[1], 'a');
    EXPECT_EQ(buf[2], 'b');
    EXPECT_EQ(buf[3], 'c');
}

/* -- set / set_pattern ----------------------------------------------------- */

TEST(str_raw_set, fills_buffer_with_char)
{
    lh_char_t buf[4] = {};
    lh_str_ptr end = lh_str_ptr_set(buf, 4, 'z');
    EXPECT_EQ(end, buf + 4);
    EXPECT_EQ(buf[0], 'z');
    EXPECT_EQ(buf[3], 'z');
}

TEST(str_raw_set_pattern, repeats_pattern)
{
    lh_char_t buf[6] = {};
    const lh_char_t pat[] = {'a', 'b'};
    lh_str_ptr end = lh_str_ptr_set_pattern(buf, 6, pat, 2);
    EXPECT_EQ(end, buf + 6);
    EXPECT_EQ(buf[0], 'a');
    EXPECT_EQ(buf[1], 'b');
    EXPECT_EQ(buf[2], 'a');
    EXPECT_EQ(buf[3], 'b');
    EXPECT_EQ(buf[4], 'a');
    EXPECT_EQ(buf[5], 'b');
}

/* -- to_lower / to_upper --------------------------------------------------- */

TEST(str_raw_to_lower, lowercases_ascii_letters)
{
    lh_char_t s[] = {'A', 'B', 'C', '1'};
    lh_str_ptr end = lh_str_ptr_to_lower(s, 4);
    EXPECT_EQ(end, s + 4);
    EXPECT_EQ(s[0], 'a');
    EXPECT_EQ(s[1], 'b');
    EXPECT_EQ(s[2], 'c');
    EXPECT_EQ(s[3], '1');
}

TEST(str_raw_to_upper, uppercases_ascii_letters)
{
    lh_char_t s[] = {'a', 'b', 'c', '1'};
    lh_str_ptr end = lh_str_ptr_to_upper(s, 4);
    EXPECT_EQ(end, s + 4);
    EXPECT_EQ(s[0], 'A');
    EXPECT_EQ(s[1], 'B');
    EXPECT_EQ(s[2], 'C');
    EXPECT_EQ(s[3], '1');
}

} // namespace
