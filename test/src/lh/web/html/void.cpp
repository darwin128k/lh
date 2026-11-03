/**
 * @file void.cpp
 * @brief Which HTML elements are void.
 */

#include <gtest/gtest.h>

#include <lh/web/html/void.h>

TEST(web_html_void, knows_the_void_elements_in_any_case)
{
    EXPECT_TRUE(lh_web_html_void_is_name(lh_str_view_lit("br")));
    EXPECT_TRUE(lh_web_html_void_is_name(lh_str_view_lit("meta")));
    EXPECT_TRUE(lh_web_html_void_is_name(lh_str_view_lit("IMG")));
    EXPECT_TRUE(lh_web_html_void_is_name(lh_str_view_lit("wbr")));
}

TEST(web_html_void, everything_else_has_content)
{
    EXPECT_FALSE(lh_web_html_void_is_name(lh_str_view_lit("p")));
    EXPECT_FALSE(lh_web_html_void_is_name(lh_str_view_lit("canvas")));
    EXPECT_FALSE(lh_web_html_void_is_name(lh_str_view_lit("brr")));
    EXPECT_FALSE(lh_web_html_void_is_name(lh_str_view_lit("b")));
}
