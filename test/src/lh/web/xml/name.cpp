/**
 * @file name.cpp
 * @brief Which names the XML writer accepts and the reader reads.
 */

#include <gtest/gtest.h>

#include <lh/web/xml/name.h>

TEST(web_xml_name, start_and_name_characters)
{
    EXPECT_TRUE(lh_web_xml_name_is_start_char('a'));
    EXPECT_TRUE(lh_web_xml_name_is_start_char('_'));
    EXPECT_TRUE(lh_web_xml_name_is_start_char(':'));
    EXPECT_TRUE(lh_web_xml_name_is_start_char('\xc3'));
    EXPECT_FALSE(lh_web_xml_name_is_start_char('1'));
    EXPECT_FALSE(lh_web_xml_name_is_start_char('-'));
    EXPECT_TRUE(lh_web_xml_name_is_char('1'));
    EXPECT_TRUE(lh_web_xml_name_is_char('-'));
    EXPECT_TRUE(lh_web_xml_name_is_char('.'));
    EXPECT_FALSE(lh_web_xml_name_is_char(' '));
    EXPECT_FALSE(lh_web_xml_name_is_char('='));
    EXPECT_FALSE(lh_web_xml_name_is_char('/'));
}

TEST(web_xml_name, accepts_the_name_production)
{
    EXPECT_TRUE(lh_web_xml_name_is_valid(lh_str_view_lit("a")));
    EXPECT_TRUE(lh_web_xml_name_is_valid(lh_str_view_lit("xml:lang")));
    EXPECT_TRUE(lh_web_xml_name_is_valid(lh_str_view_lit("_x.y-z1")));
    EXPECT_TRUE(lh_web_xml_name_is_valid(lh_str_view_lit("data-id")));
    EXPECT_TRUE(lh_web_xml_name_is_valid(lh_str_view_lit("\xd0\xb8\xd0\xbc\xd1\x8f")));
}

TEST(web_xml_name, rejects_everything_else)
{
    lh_str_view_t empty;

    lh_str_view_init_empty(&empty);
    EXPECT_FALSE(lh_web_xml_name_is_valid(empty));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit("1a")));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit("-a")));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit(".a")));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit("a b")));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit("a>")));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit("a\"")));
    EXPECT_FALSE(lh_web_xml_name_is_valid(lh_str_view_lit("a?")));
}
