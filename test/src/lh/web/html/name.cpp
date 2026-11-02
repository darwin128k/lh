/**
 * @file name.cpp
 * @brief Which tag and attribute names the HTML writer accepts.
 */

#include <gtest/gtest.h>

#include <lh/web/html/name.h>

TEST(web_html_name, accepts_letters_digits_and_dashes_after_a_letter)
{
    EXPECT_TRUE(lh_web_html_name_is_valid(lh_str_view_lit("a")));
    EXPECT_TRUE(lh_web_html_name_is_valid(lh_str_view_lit("canvas")));
    EXPECT_TRUE(lh_web_html_name_is_valid(lh_str_view_lit("h1")));
    EXPECT_TRUE(lh_web_html_name_is_valid(lh_str_view_lit("data-id")));
    EXPECT_TRUE(lh_web_html_name_is_valid(lh_str_view_lit("tabIndex")));
}

TEST(web_html_name, rejects_everything_else)
{
    lh_str_view_t empty;

    lh_str_view_init_empty(&empty);
    EXPECT_FALSE(lh_web_html_name_is_valid(empty));
    EXPECT_FALSE(lh_web_html_name_is_valid(lh_str_view_lit("1a")));
    EXPECT_FALSE(lh_web_html_name_is_valid(lh_str_view_lit("-a")));
    EXPECT_FALSE(lh_web_html_name_is_valid(lh_str_view_lit("a b")));
    EXPECT_FALSE(lh_web_html_name_is_valid(lh_str_view_lit("a>")));
    EXPECT_FALSE(lh_web_html_name_is_valid(lh_str_view_lit("a\"")));
    EXPECT_FALSE(lh_web_html_name_is_valid(lh_str_view_lit("\xc3\xa9")));
}
