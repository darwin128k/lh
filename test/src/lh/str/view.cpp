#include <gtest/gtest.h>

#include <lh/bool.h>
#include <lh/char/map.h>
#include <lh/expect/death.h>
#include <lh/null.h>
#include <lh/str/view.h>

namespace
{

lh_str_view_t
make(const lh_char_t *s)
{
    lh_str_view_t v;
    lh_str_view_init(&v, s);
    return v;
}

lh_str_view_t
make_n(const lh_char_t *s, lh_usize_t n)
{
    lh_str_view_t v;
    lh_str_init_by_size(&v, s, n);
    return v;
}

/* -- init ------------------------------------------------------------------ */

TEST(str_view_init, sets_endpoints_from_cstr)
{
    const lh_char_t s[] = {'a', 'b', 'c', lh_char_map_nul};
    lh_str_view_t v;
    lh_str_view_init(&v, s);

    EXPECT_EQ(lh_str_view_get_data(&v), s);
    EXPECT_EQ(lh_str_view_get_size(&v), 3u);
    EXPECT_EQ(lh_str_view_get_begin(&v), s);
    EXPECT_EQ(lh_str_view_get_end(&v), s + 3);
}

TEST(str_view_init_by_size, uses_explicit_size)
{
    const lh_char_t s[] = {'x', 'y', 'z', 'w'};
    lh_str_view_t v;
    lh_str_init_by_size(&v, s, 2);

    EXPECT_EQ(lh_str_view_get_data(&v), s);
    EXPECT_EQ(lh_str_view_get_size(&v), 2u);
    EXPECT_EQ(lh_str_view_get_end(&v), s + 2);
}

TEST(str_view_init_empty, view_is_empty)
{
    lh_str_view_t v;
    lh_str_view_init_empty(&v);
    EXPECT_TRUE(lh_str_view_is_empty(&v));
}

TEST(str_view_init_by_other, copies_endpoints)
{
    const lh_char_t s[] = {'h', 'i', lh_char_map_nul};
    lh_str_view_t src;
    lh_str_view_init(&src, s);

    lh_str_view_t dst;
    lh_str_view_init_by_other(&dst, &src);

    EXPECT_EQ(lh_str_view_get_data(&dst), s);
    EXPECT_EQ(lh_str_view_get_size(&dst), 2u);
}

/* -- getters --------------------------------------------------------------- */

TEST(str_view_getters, begin_end_data_size)
{
    const lh_char_t s[] = {'a', 'b', 'c', lh_char_map_nul};
    lh_str_view_t v = make(s);

    EXPECT_EQ(lh_str_view_get_data(&v), s);
    EXPECT_EQ(lh_str_view_get_begin(&v), s);
    EXPECT_EQ(lh_str_view_get_end(&v), s + 3);
    EXPECT_EQ(lh_str_view_get_size(&v), 3u);
}

/* -- state ----------------------------------------------------------------- */

TEST(str_view_is_empty, empty_view)
{
    lh_str_view_t v;
    lh_str_view_init_empty(&v);
    EXPECT_TRUE(lh_str_view_is_empty(&v));
}

TEST(str_view_is_empty, non_empty_view)
{
    lh_str_view_t v = make("abc");
    EXPECT_FALSE(lh_str_view_is_empty(&v));
}

/* -- find_char ------------------------------------------------------------- */

TEST(str_view_find_char, returns_first_occurrence)
{
    lh_str_view_t v = make("abcabc");
    EXPECT_EQ(lh_str_view_find_char(&v, 'a'), 0u);
    EXPECT_EQ(lh_str_view_find_char(&v, 'b'), 1u);
    EXPECT_EQ(lh_str_view_find_char(&v, 'c'), 2u);
}

TEST(str_view_find_char, returns_invalid_when_absent)
{
    lh_str_view_t v = make("abc");
    EXPECT_EQ(lh_str_view_find_char(&v, 'z'), LH_STR_VIEW_INVALID);
}

/* -- find ------------------------------------------------------------------ */

TEST(str_view_find, finds_substring_at_start)
{
    lh_str_view_t hay = make("hello world hello");
    lh_str_view_t nd = make("hello");
    EXPECT_EQ(lh_str_view_find(&hay, &nd, lh_bool_false), 0u);
}

TEST(str_view_find, finds_internal_substring)
{
    lh_str_view_t hay = make("hello world");
    lh_str_view_t nd = make("world");
    EXPECT_EQ(lh_str_view_find(&hay, &nd, lh_bool_false), 6u);
}

TEST(str_view_find, returns_invalid_when_absent)
{
    lh_str_view_t hay = make("hello");
    lh_str_view_t nd = make("bye");
    EXPECT_EQ(lh_str_view_find(&hay, &nd, lh_bool_false), LH_STR_VIEW_INVALID);
}

TEST(str_view_find, ignore_case)
{
    lh_str_view_t hay = make("Hello World");
    lh_str_view_t nd = make("world");
    EXPECT_EQ(lh_str_view_find(&hay, &nd, lh_bool_true), 6u);
}

/* -- rfind ----------------------------------------------------------------- */

TEST(str_view_rfind, finds_last_occurrence)
{
    lh_str_view_t hay = make("hello world hello");
    lh_str_view_t nd = make("hello");
    EXPECT_EQ(lh_str_view_rfind(&hay, &nd, lh_bool_false), 12u);
}

TEST(str_view_rfind, returns_invalid_when_absent)
{
    lh_str_view_t hay = make("hello");
    lh_str_view_t nd = make("bye");
    EXPECT_EQ(lh_str_view_rfind(&hay, &nd, lh_bool_false), LH_STR_VIEW_INVALID);
}

TEST(str_view_rfind, ignore_case)
{
    lh_str_view_t hay = make("abcABC");
    lh_str_view_t nd = make("abc");
    EXPECT_EQ(lh_str_view_rfind(&hay, &nd, lh_bool_true), 3u);
}

/* -- find_of / rfind_of ---------------------------------------------------- */

TEST(str_view_find_of, finds_first_char_in_set)
{
    lh_str_view_t v = make("hello");
    lh_str_view_t chars = make("aeiou");
    EXPECT_EQ(lh_str_view_find_of(&v, &chars), 1u);
}

TEST(str_view_find_of, returns_invalid_when_no_match)
{
    lh_str_view_t v = make("xyz");
    lh_str_view_t chars = make("aeiou");
    EXPECT_EQ(lh_str_view_find_of(&v, &chars), LH_STR_VIEW_INVALID);
}

TEST(str_view_rfind_of, finds_last_char_in_set)
{
    lh_str_view_t v = make("hello");
    lh_str_view_t chars = make("aeiou");
    EXPECT_EQ(lh_str_view_rfind_of(&v, &chars), 4u);
}

TEST(str_view_rfind_of, returns_invalid_when_no_match)
{
    lh_str_view_t v = make("xyz");
    lh_str_view_t chars = make("aeiou");
    EXPECT_EQ(lh_str_view_rfind_of(&v, &chars), LH_STR_VIEW_INVALID);
}

/* -- find_not_of / rfind_not_of -------------------------------------------- */

TEST(str_view_find_not_of, finds_first_char_not_in_set)
{
    lh_str_view_t v = make("aabbc");
    lh_str_view_t chars = make("ab");
    EXPECT_EQ(lh_str_view_find_not_of(&v, &chars), 4u);
}

TEST(str_view_find_not_of, returns_invalid_when_all_match)
{
    lh_str_view_t v = make("aaaa");
    lh_str_view_t chars = make("a");
    EXPECT_EQ(lh_str_view_find_not_of(&v, &chars), LH_STR_VIEW_INVALID);
}

TEST(str_view_rfind_not_of, finds_last_char_not_in_set)
{
    lh_str_view_t v = make("cabba");
    lh_str_view_t chars = make("ab");
    EXPECT_EQ(lh_str_view_rfind_not_of(&v, &chars), 0u);
}

TEST(str_view_rfind_not_of, returns_invalid_when_all_match)
{
    lh_str_view_t v = make("bbbb");
    lh_str_view_t chars = make("b");
    EXPECT_EQ(lh_str_view_rfind_not_of(&v, &chars), LH_STR_VIEW_INVALID);
}

/* -- compare --------------------------------------------------------------- */

TEST(str_view_compare, returns_null_when_equal)
{
    lh_str_view_t a = make("abc");
    lh_str_view_t b = make("abc");
    EXPECT_TRUE(lh_null_eq(lh_str_view_compare(&a, &b, lh_bool_false)));
}

TEST(str_view_compare, returns_pointer_to_first_difference)
{
    lh_str_view_t a = make("axc");
    lh_str_view_t b = make("ayc");
    lh_str_cptr diff = lh_str_view_compare(&a, &b, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(diff));
    EXPECT_EQ(*diff, 'x');
}

TEST(str_view_compare, ignore_case_reports_equal)
{
    lh_str_view_t a = make("ABC");
    lh_str_view_t b = make("abc");
    EXPECT_TRUE(lh_null_eq(lh_str_view_compare(&a, &b, lh_bool_true)));
}

TEST(str_view_rcompare, returns_null_when_suffix_equal)
{
    lh_str_view_t a = make("abc");
    lh_str_view_t b = make("abc");
    EXPECT_TRUE(lh_null_eq(lh_str_view_rcompare(&a, &b, lh_bool_false)));
}

TEST(str_view_rcompare, returns_pointer_to_first_suffix_mismatch)
{
    lh_str_view_t a = make("axc");
    lh_str_view_t b = make("ayc");
    lh_str_cptr diff = lh_str_view_rcompare(&a, &b, lh_bool_false);
    ASSERT_TRUE(lh_null_ne(diff));
    EXPECT_EQ(*diff, 'x');
}

/* -- equals ---------------------------------------------------------------- */

TEST(str_view_equals, equal_content)
{
    lh_str_view_t a = make("hello");
    lh_str_view_t b = make("hello");
    EXPECT_TRUE(lh_str_view_equals(&a, &b, lh_bool_false));
}

TEST(str_view_equals, different_content)
{
    lh_str_view_t a = make("hello");
    lh_str_view_t b = make("world");
    EXPECT_FALSE(lh_str_view_equals(&a, &b, lh_bool_false));
}

TEST(str_view_equals, different_size)
{
    lh_str_view_t a = make("hello");
    lh_str_view_t b = make("hell");
    EXPECT_FALSE(lh_str_view_equals(&a, &b, lh_bool_false));
}

TEST(str_view_equals, ignore_case_equal)
{
    lh_str_view_t a = make("Hello");
    lh_str_view_t b = make("hello");
    EXPECT_TRUE(lh_str_view_equals(&a, &b, lh_bool_true));
}

TEST(str_view_equals, ignore_case_different_content)
{
    lh_str_view_t a = make("Hello");
    lh_str_view_t b = make("world");
    EXPECT_FALSE(lh_str_view_equals(&a, &b, lh_bool_true));
}

/* -- null terminator ------------------------------------------------------- */

TEST(str_view_find_null_terminator, finds_nul_inside_view)
{
    const lh_char_t s[] = {'a', 'b', lh_char_map_nul, 'c'};
    lh_str_view_t v = make_n(s, 4);
    EXPECT_EQ(lh_str_view_find_null_terminator(&v), 2u);
}

TEST(str_view_find_null_terminator, returns_invalid_when_absent)
{
    const lh_char_t s[] = {'a', 'b', 'c'};
    lh_str_view_t v = make_n(s, 3);
    EXPECT_EQ(lh_str_view_find_null_terminator(&v), LH_STR_VIEW_INVALID);
}

TEST(str_view_is_null_terminated, true_when_nul_present)
{
    const lh_char_t s[] = {'a', lh_char_map_nul, 'b'};
    lh_str_view_t v = make_n(s, 3);
    EXPECT_TRUE(lh_str_view_is_null_terminated(&v));
}

TEST(str_view_is_null_terminated, false_when_nul_absent)
{
    const lh_char_t s[] = {'a', 'b', 'c'};
    lh_str_view_t v = make_n(s, 3);
    EXPECT_FALSE(lh_str_view_is_null_terminated(&v));
}

/* -- character access ------------------------------------------------------ */

TEST(str_view_char_access, first_and_last)
{
    lh_str_view_t v = make("xyz");
    EXPECT_EQ(lh_str_view_get_first_char(&v), 'x');
    EXPECT_EQ(lh_str_view_get_last_char(&v), 'z');
}

TEST(str_view_char_access, from_begin)
{
    lh_str_view_t v = make("abcde");
    EXPECT_EQ(lh_str_view_get_char_from_begin(&v, 0), 'a');
    EXPECT_EQ(lh_str_view_get_char_from_begin(&v, 2), 'c');
    EXPECT_EQ(lh_str_view_get_char_from_begin(&v, 4), 'e');
}

TEST(str_view_char_access, from_end)
{
    lh_str_view_t v = make("abcde");
    EXPECT_EQ(lh_str_view_get_char_from_end(&v, 0), 'e');
    EXPECT_EQ(lh_str_view_get_char_from_end(&v, 2), 'c');
    EXPECT_EQ(lh_str_view_get_char_from_end(&v, 4), 'a');
}

TEST(str_view_char_access, by_signed_offset)
{
    lh_str_view_t v = make("abcde");
    EXPECT_EQ(lh_str_view_get_char_by_offset(&v, 0), 'a');
    EXPECT_EQ(lh_str_view_get_char_by_offset(&v, 3), 'd');
}

/* -- mutation -------------------------------------------------------------- */

TEST(str_view_clear, resets_to_empty)
{
    lh_str_view_t v = make("hello");
    lh_str_view_clear(&v);
    EXPECT_TRUE(lh_str_view_is_empty(&v));
}

TEST(str_view_assign, copies_source_into_self)
{
    lh_str_view_t src = make("world");
    lh_str_view_t dst = make("hello");
    lh_str_view_assign(&dst, &src);
    EXPECT_TRUE(lh_str_view_equals(&dst, &src, lh_bool_false));
}

TEST(str_view_swap, exchanges_contents)
{
    lh_str_view_t a = make("hello");
    lh_str_view_t b = make("world");
    lh_str_cptr a_data = lh_str_view_get_data(&a);
    lh_str_cptr b_data = lh_str_view_get_data(&b);

    lh_str_view_swap(&a, &b);

    EXPECT_EQ(lh_str_view_get_data(&a), b_data);
    EXPECT_EQ(lh_str_view_get_data(&b), a_data);
}

TEST(str_view_swap_and_clear, moves_other_into_self_clears_other)
{
    lh_str_view_t a = make("hello");
    lh_str_view_t b = make("world");
    lh_str_cptr b_data = lh_str_view_get_data(&b);

    lh_str_view_swap_and_clear(&a, &b);

    EXPECT_EQ(lh_str_view_get_data(&a), b_data);
    EXPECT_TRUE(lh_str_view_is_empty(&b));
}

/* -- death tests ----------------------------------------------------------- */

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(str_view_init_death, null_self)
{
    const lh_char_t s[] = {'a', lh_char_map_nul};
    LH_EXPECT_DEATH(lh_str_view_init(reinterpret_cast<lh_str_view_t *>(lh_null), s));
}

TEST(str_view_init_death, null_data)
{
    lh_str_view_t v;
    LH_EXPECT_DEATH(lh_str_view_init(&v, reinterpret_cast<lh_str_cptr>(lh_null)));
}

TEST(str_view_find_char_death, null_self)
{
    LH_EXPECT_DEATH(
        lh_str_view_find_char(reinterpret_cast<const lh_str_view_t *>(lh_null), 'a'));
}

TEST(str_view_find_death, null_self)
{
    lh_str_view_t nd = make("x");
    LH_EXPECT_DEATH(
        lh_str_view_find(reinterpret_cast<const lh_str_view_t *>(lh_null), &nd, lh_bool_false));
}

TEST(str_view_find_of_death, null_self)
{
    lh_str_view_t chars = make("ab");
    LH_EXPECT_DEATH(
        lh_str_view_find_of(reinterpret_cast<const lh_str_view_t *>(lh_null), &chars));
}

TEST(str_view_equals_death, null_self)
{
    lh_str_view_t b = make("x");
    LH_EXPECT_DEATH(
        lh_str_view_equals(reinterpret_cast<const lh_str_view_t *>(lh_null), &b, lh_bool_false));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
