#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/char/map.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/util/str/raw.h>

namespace
{

TEST(str_raw_find_of_char, returns_first_match_or_null)
{
    const lh_char_t a[] = {1, 2, 3, 2};
    lh_str_cptr p = lh_str_raw_find_of_char(a, 4, 2);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &a[1]);

    lh_str_cptr miss = lh_str_raw_find_of_char(a, 4, 9);
    EXPECT_TRUE(lh_null_eq(miss));
}

TEST(str_raw_find_of_char_death, null_str)
{
    LH_EXPECT_DEATH(lh_str_raw_find_of_char(reinterpret_cast<lh_str_cptr>(lh_null), 1, 'x'));
}

TEST(str_raw_find_of_null_terminator_by_size, finds_nul_inside_prefix_or_null)
{
    const lh_char_t with_nul[] = {'a', 'b', lh_char_map_nul, 'c'};
    lh_str_cptr p = lh_str_raw_find_of_null_terminator_by_size(with_nul, 4);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &with_nul[2]);

    lh_str_cptr miss_before = lh_str_raw_find_of_null_terminator_by_size(with_nul, 2);
    EXPECT_TRUE(lh_null_eq(miss_before));

    const lh_char_t no_nul[] = {1, 2, 3};
    lh_str_cptr miss_absent = lh_str_raw_find_of_null_terminator_by_size(no_nul, 3);
    EXPECT_TRUE(lh_null_eq(miss_absent));
}

TEST(str_raw_find_of_null_terminator_by_size_death, null_str)
{
    LH_EXPECT_DEATH(
        lh_str_raw_find_of_null_terminator_by_size(reinterpret_cast<lh_str_cptr>(lh_null), 1));
}

TEST(str_raw_find_of_null_terminator, finds_first_nul)
{
    const lh_char_t s[] = {'x', lh_char_map_nul};
    lh_str_cptr p = lh_str_raw_find_of_null_terminator(s);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, &s[1]);
}

TEST(str_raw_find_of_null_terminator_death, null_str)
{
    LH_EXPECT_DEATH(lh_str_raw_find_of_null_terminator(reinterpret_cast<lh_str_cptr>(lh_null)));
}

TEST(str_raw_len, counts_chars_before_nul)
{
    const lh_char_t empty[] = {lh_char_map_nul};
    EXPECT_EQ(lh_str_raw_len(empty), 0U);

    const lh_char_t one[] = {'z', lh_char_map_nul};
    EXPECT_EQ(lh_str_raw_len(one), 1U);

    const lh_char_t abc[] = {'a', 'b', 'c', lh_char_map_nul};
    EXPECT_EQ(lh_str_raw_len(abc), 3U);
}

TEST(str_raw_len_death, no_null_terminator_when_scan_size_is_zero)
{
    // ::lh_str_raw_find_of_null_terminator scans `LH_USIZE_T_MAX - lh_ptr_to_uaddr(str)` bytes.
    // If that value is 0, no memory is read, no NUL is found, and ::lh_str_raw_len terminates with
    // ::lh_runtime_error_code_no_null_terminator.
    const lh_uaddr_t addr = static_cast<lh_uaddr_t>(LH_USIZE_T_MAX);
    lh_str_ptr p = reinterpret_cast<lh_str_ptr>(addr);
    ASSERT_TRUE(lh_null_ne(p));
    LH_EXPECT_DEATH((void)lh_str_raw_len(p));
}

TEST(str_raw_contains, returns_true_if_substring_exists)
{
    const lh_char_t *hay = "hello world";
    EXPECT_TRUE(lh_str_raw_contains(hay, "hello", lh_bool_false));
    EXPECT_TRUE(lh_str_raw_contains(hay, "world", lh_bool_false));
    EXPECT_TRUE(lh_str_raw_contains(hay, "o w", lh_bool_false));
    EXPECT_FALSE(lh_str_raw_contains(hay, "bye", lh_bool_false));
}

TEST(str_raw_contains, ignore_case_ascii)
{
    const lh_char_t *hay = "Hello World";
    EXPECT_TRUE(lh_str_raw_contains(hay, "hello", lh_bool_true));
    EXPECT_TRUE(lh_str_raw_contains(hay, "WORLD", lh_bool_true));
    EXPECT_FALSE(lh_str_raw_contains(hay, "bye", lh_bool_true));
}

TEST(str_raw_index_of, returns_first_position_or_invalid)
{
    const lh_char_t *hay = "hello world";
    EXPECT_EQ(lh_str_raw_index_of(hay, "hello", lh_bool_false), 0U);
    EXPECT_EQ(lh_str_raw_index_of(hay, "world", lh_bool_false), 6U);
    EXPECT_EQ(lh_str_raw_index_of(hay, "bye", lh_bool_false), LH_STR_RAW_INVALID);
}

TEST(str_raw_index_of, supports_ignore_case)
{
    const lh_char_t *hay = "Hello World";
    EXPECT_EQ(lh_str_raw_index_of(hay, "world", lh_bool_true), 6U);
}

TEST(str_raw_index_of_by_size, supports_non_terminated_buffers)
{
    const lh_char_t hay[] = {'a', 'b', 'c', 'd', 'e'};
    const lh_char_t nd[] = {'c', 'd'};
    EXPECT_EQ(lh_str_raw_index_of_by_size(hay, 5, nd, 2, lh_bool_false), 2U);
    EXPECT_EQ(lh_str_raw_index_of_by_size(hay, 5, nd, 0, lh_bool_false), 0U);
}

TEST(str_raw_equals, returns_true_only_if_strings_are_identical)
{
    const lh_char_t *s = "test";
    EXPECT_TRUE(lh_str_raw_equals(s, "test", lh_bool_false));
    EXPECT_FALSE(lh_str_raw_equals(s, "tes", lh_bool_false));
    EXPECT_FALSE(lh_str_raw_equals(s, "tests", lh_bool_false));
    EXPECT_FALSE(lh_str_raw_equals(s, "TEST", lh_bool_false));
}

TEST(str_raw_equals, ignore_case_ascii)
{
    const lh_char_t *s = "test";
    EXPECT_TRUE(lh_str_raw_equals(s, "TEST", lh_bool_true));
}

TEST(str_raw_starts_with, returns_true_if_str_starts_with_src)
{
    const lh_char_t *s = "hello world";
    EXPECT_TRUE(lh_str_raw_starts_with(s, "hello", lh_bool_false));
    EXPECT_TRUE(lh_str_raw_starts_with(s, "h", lh_bool_false));
    EXPECT_TRUE(lh_str_raw_starts_with(s, "", lh_bool_false));
    EXPECT_FALSE(lh_str_raw_starts_with(s, "world", lh_bool_false));
}

TEST(str_raw_starts_with, ignore_case_ascii)
{
    const lh_char_t *s = "Hello world";
    EXPECT_TRUE(lh_str_raw_starts_with(s, "hello", lh_bool_true));
}

TEST(str_raw_ends_with, returns_true_if_str_ends_with_src)
{
    const lh_char_t *s = "hello world";
    EXPECT_TRUE(lh_str_raw_ends_with(s, "world", lh_bool_false));
    EXPECT_TRUE(lh_str_raw_ends_with(s, "d", lh_bool_false));
    EXPECT_TRUE(lh_str_raw_ends_with(s, "", lh_bool_false));
    EXPECT_FALSE(lh_str_raw_ends_with(s, "hello", lh_bool_false));
}

TEST(str_raw_ends_with, ignore_case_ascii)
{
    const lh_char_t *s = "hello WORLD";
    EXPECT_TRUE(lh_str_raw_ends_with(s, "world", lh_bool_true));
}

TEST(str_raw_find_by_ignore_case, finds_needle)
{
    const lh_char_t hay[] = {'x', 'x', 'H', 'e', 'l', 'l', 'o'};
    const lh_char_t nd[] = {'h', 'e', 'l', 'l', 'o'};
    lh_str_cptr p = lh_str_raw_find_by_ignore_case(hay, 7, nd, 5);
    EXPECT_EQ(p, lh_str_raw_find(hay, 7, nd, 5, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, hay + 2);
}

TEST(str_raw_compare_by_ignore_case, first_mismatch_after_fold)
{
    const lh_char_t lhs[] = {'A', 'x', 'c'};
    const lh_char_t rhs[] = {'a', 'Y', 'c'};
    lh_str_cptr p = lh_str_raw_compare_by_ignore_case(lhs, 3, rhs, 3);
    EXPECT_EQ(p, lh_str_raw_compare(lhs, 3, rhs, 3, lh_bool_true));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lhs + 1);
}

TEST(str_raw_trim_custom, trims_left_right_and_all_whitespace)
{
    lh_char_t s1[] = " \t\nabc \t";
    lh_char_t s2[] = " \t\nabc \t";
    lh_char_t s3[] = " \t\n ";
    const lh_char_t ws[] = " \t\n";
    const lh_usize_t ws_size = lh_str_raw_len(ws);

    EXPECT_STREQ(lh_str_raw_ltrim_custom(s1, lh_str_raw_len(s1), ws, ws_size), "abc \t");
    EXPECT_STREQ(lh_str_raw_rtrim_custom(s2, lh_str_raw_len(s2), ws, ws_size), " \t\nabc");
    EXPECT_STREQ(lh_str_raw_trim_custom(s3, lh_str_raw_len(s3), ws, ws_size), "");
}

TEST(str_raw_trim_set_and_auto, infer_sizes_correctly)
{
    lh_char_t s1[] = "\n\t abc\t ";
    lh_char_t s2[] = "\n\t abc\t ";
    lh_char_t s3[] = "\n\t abc\t ";
    const lh_char_t ws[] = " \t\n";
    const lh_usize_t ws_size = lh_str_raw_len(ws);

    EXPECT_STREQ(lh_str_raw_ltrim_set(s1, ws, ws_size), "abc\t ");
    EXPECT_STREQ(lh_str_raw_rtrim_set(s2, ws, ws_size), "\n\t abc");
    EXPECT_STREQ(lh_str_raw_trim_auto(s3, ws), "abc");
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
    const lh_usize_t ws_size = lh_str_raw_len(ws);

    EXPECT_STREQ(lh_str_raw_ltrim_custom(a, lh_str_raw_len(a), ws, ws_size), "abc  ");
    EXPECT_STREQ(lh_str_raw_rtrim_custom(b, lh_str_raw_len(b), ws, ws_size), "  abc");
    EXPECT_STREQ(lh_str_raw_trim_custom(c, lh_str_raw_len(c), ws, ws_size), "abc");
    EXPECT_STREQ(lh_str_raw_trim_set(d, ws, ws_size), "abc");
    EXPECT_STREQ(lh_str_raw_trim_auto(e, ws), "abc");
    EXPECT_STREQ(lh_str_raw_trim_set(f, ws, ws_size), "abc");
}

TEST(str_raw_trim_default, uses_builtin_whitespace_set)
{
    lh_char_t s1[] = " \r\n\tabc \v";
    lh_char_t s2[] = " \r\n\tabc \v";
    lh_char_t s3[] = " \r\n\tabc \v";

    EXPECT_STREQ(lh_str_raw_ltrim(s1), "abc \v");
    EXPECT_STREQ(lh_str_raw_rtrim(s2), " \r\n\tabc");
    EXPECT_STREQ(lh_str_raw_trim(s3), "abc");
}

TEST(str_raw_trim_death, null_arguments)
{
    lh_char_t s[] = "abc";
    const lh_char_t ws[] = " ";

    LH_EXPECT_DEATH(
        lh_str_raw_ltrim_custom(reinterpret_cast<lh_str_ptr>(lh_null), lh_str_raw_len(s), ws, 1));
    LH_EXPECT_DEATH(
        lh_str_raw_rtrim_custom(reinterpret_cast<lh_str_ptr>(lh_null), lh_str_raw_len(s), ws, 1));
    LH_EXPECT_DEATH(
        lh_str_raw_trim_custom(reinterpret_cast<lh_str_ptr>(lh_null), lh_str_raw_len(s), ws, 1));

    LH_EXPECT_DEATH(
        lh_str_raw_ltrim_custom(s, lh_str_raw_len(s), reinterpret_cast<lh_str_cptr>(lh_null), 1));
}

} // namespace
