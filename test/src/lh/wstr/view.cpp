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
