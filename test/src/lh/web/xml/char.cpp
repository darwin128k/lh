/**
 * @file char.cpp
 * @brief Which code points XML allows (the `Char` production).
 */

#include <gtest/gtest.h>

#include <lh/web/xml/char.h>

TEST(web_xml_char, allows_tab_line_ends_and_text)
{
    EXPECT_TRUE(lh_web_xml_char_is_valid(0x9U));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0xAU));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0xDU));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0x20U));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0xD7FFU));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0xE000U));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0xFFFDU));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0x10000U));
    EXPECT_TRUE(lh_web_xml_char_is_valid(0x10FFFFU));
}

TEST(web_xml_char, refuses_controls_surrogates_and_non_characters)
{
    EXPECT_FALSE(lh_web_xml_char_is_valid(0x0U));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0x8U));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0x1FU));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0xD800U));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0xDFFFU));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0xFFFEU));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0xFFFFU));
    EXPECT_FALSE(lh_web_xml_char_is_valid(0x110000U));
}
