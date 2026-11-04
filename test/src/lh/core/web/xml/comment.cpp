/**
 * @file comment.cpp
 * @brief Which texts an XML comment can hold.
 */

#include <gtest/gtest.h>

#include <lh/core/web/xml/comment.h>

TEST(web_xml_comment, accepts_text_without_double_dashes)
{
    lh_str_view_t empty;

    lh_str_view_init_empty(&empty);
    EXPECT_TRUE(lh_web_xml_comment_is_valid(empty));
    EXPECT_TRUE(lh_web_xml_comment_is_valid(lh_str_view_lit(" a - b ")));
    EXPECT_TRUE(lh_web_xml_comment_is_valid(lh_str_view_lit("-a")));
}

TEST(web_xml_comment, refuses_double_dashes_and_a_dash_at_the_end)
{
    EXPECT_FALSE(lh_web_xml_comment_is_valid(lh_str_view_lit("a--b")));
    EXPECT_FALSE(lh_web_xml_comment_is_valid(lh_str_view_lit("a-")));
    EXPECT_FALSE(lh_web_xml_comment_is_valid(lh_str_view_lit("-")));
}
