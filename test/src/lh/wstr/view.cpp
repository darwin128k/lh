#include <gtest/gtest.h>

#include <lh/wstr/view.h>

TEST(wstr_view_lit, uses_sizeof_of_wide_string_literal)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"abc");

    EXPECT_EQ(lh_wstr_view_get_size(&v), 3u);
    EXPECT_EQ(lh_wstr_view_get_data(&v)[0], L'a');
    EXPECT_EQ(lh_wstr_view_get_end(&v), lh_wstr_view_get_begin(&v) + 3);
}

TEST(wstr_view_make, empty_and_null_are_empty)
{
    const lh_wstr_view_t a = lh_wstr_view_make(nullptr);
    const lh_wstr_view_t b = lh_wstr_view_make(L"");

    EXPECT_TRUE(lh_wstr_view_is_empty(&a));
    EXPECT_TRUE(lh_wstr_view_is_empty(&b));
}

TEST(wstr_view_make_from_offset, counts_wide_characters)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"key=value");
    const lh_wstr_view_t mid = lh_wstr_view_make_from_offset(&v, 3, 1);

    EXPECT_EQ(lh_wstr_view_get_size(&mid), 1u);
    EXPECT_EQ(lh_wstr_view_get_data(&mid)[0], L'=');
    EXPECT_EQ(lh_wstr_view_get_begin(&mid), lh_wstr_view_get_begin(&v) + 3);
}

TEST(wstr_view_make_tail, from_offset_to_end_and_empty_at_size)
{
    const lh_wstr_view_t v = lh_wstr_view_lit(L"key=value");
    const lh_wstr_view_t value = lh_wstr_view_make_tail(&v, 4);
    const lh_wstr_view_t at_end = lh_wstr_view_make_tail(&v, 9);

    EXPECT_EQ(lh_wstr_view_get_size(&value), 5u);
    EXPECT_EQ(lh_wstr_view_get_data(&value)[0], L'v');
    EXPECT_TRUE(lh_wstr_view_is_empty(&at_end));
}
