#include <gtest/gtest.h>

#include <lh/wstr/view.h>

TEST(wstr_view_lit, uses_sizeof_of_wide_string_literal)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"abc");

    EXPECT_EQ(lh_wstr_view_get_size(&v), 3u);
    EXPECT_EQ(lh_wstr_view_get_data(&v)[0], L'a');
    EXPECT_EQ(lh_wstr_view_get_end(&v), lh_wstr_view_get_begin(&v) + 3);
}

TEST(wstr_view_make, empty_string_is_empty)
{
    lh_wstr_view_t a;

    lh_wstr_view_init(lh_addr_of(a), L"");
    lh_wstr_view_t b;

    lh_wstr_view_init(lh_addr_of(b), L"");

    EXPECT_TRUE(lh_wstr_view_is_empty(&a));
    EXPECT_TRUE(lh_wstr_view_is_empty(&b));
}

/* The runtime spelling of L"" must agree with the literal spelling: the same
   endpoints, so "present and empty" never depends on which spelling was used. */
TEST(wstr_view_make, empty_string_matches_the_literal_spelling)
{
    lh_wstr_view_t v;
    const lh_wstr_view_t lit = lh_wstr_view_lit(L"");

    lh_wstr_view_init(lh_addr_of(v), L"");

    EXPECT_EQ(lh_wstr_view_get_begin(&v), lh_wstr_view_get_begin(&lit));
    EXPECT_EQ(lh_wstr_view_get_end(&v), lh_wstr_view_get_end(&lit));
}

TEST(wstr_view_make, empty_is_explicit)
{
    lh_wstr_view_t v;

    lh_wstr_view_init_empty(lh_addr_of(v));

    EXPECT_TRUE(lh_wstr_view_is_empty(&v));
}

/* -- death tests ----------------------------------------------------------- */

#if LH_TEST_EXPECT_DEATH_ENABLED

/* A null data pointer is a caller error, not an empty view: str_view_init_death
   .null_data pins the same contract on the narrow side. Empty is spelled out with
   init_empty. */
TEST(wstr_view_init_death, null_data)
{
    lh_wstr_view_t v;
    LH_EXPECT_DEATH(lh_wstr_view_init(lh_addr_of(v), reinterpret_cast<lh_wstr_cptr>(lh_null)));
}

#endif

TEST(wstr_view_make_from_offset, counts_wide_characters)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"key=value");
    const lh_wstr_view_t mid = lh_wstr_view_from_offset(&v, 3, 1);

    EXPECT_EQ(lh_wstr_view_get_size(&mid), 1u);
    EXPECT_EQ(lh_wstr_view_get_data(&mid)[0], L'=');
    EXPECT_EQ(lh_wstr_view_get_begin(&mid), lh_wstr_view_get_begin(&v) + 3);
}

TEST(wstr_view_make_tail, from_offset_to_end_and_empty_at_size)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"key=value");
    const lh_wstr_view_t value = lh_wstr_view_tail(&v, 4);
    const lh_wstr_view_t at_end = lh_wstr_view_tail(&v, 9);

    EXPECT_EQ(lh_wstr_view_get_size(&value), 5u);
    EXPECT_EQ(lh_wstr_view_get_data(&value)[0], L'v');
    EXPECT_TRUE(lh_wstr_view_is_empty(&at_end));
}
