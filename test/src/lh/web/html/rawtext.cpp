/**
 * @file rawtext.cpp
 * @brief Which HTML elements hold raw text.
 */

#include <gtest/gtest.h>

#include <lh/web/html/rawtext.h>

TEST(web_html_rawtext, script_and_style_in_any_case)
{
    EXPECT_TRUE(lh_web_html_rawtext_is_name(lh_str_view_lit("script")));
    EXPECT_TRUE(lh_web_html_rawtext_is_name(lh_str_view_lit("STYLE")));
    EXPECT_FALSE(lh_web_html_rawtext_is_name(lh_str_view_lit("title")));
    EXPECT_FALSE(lh_web_html_rawtext_is_name(lh_str_view_lit("scripts")));
}
