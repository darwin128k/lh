/**
 * @file entity.cpp
 * @brief The five entities XML predefines.
 */

#include <gtest/gtest.h>

#include <lh/core/web/xml/entity.h>

TEST(web_xml_entity, knows_the_five_predefined)
{
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("lt")), '<');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("gt")), '>');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("amp")), '&');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("quot")), '"');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("apos")), '\'');
}

TEST(web_xml_entity, anything_else_is_nul)
{
    lh_str_view_t empty;

    lh_str_view_init_empty(&empty);
    EXPECT_EQ(lh_web_xml_entity_get_char(empty), '\0');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("LT")), '\0');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("nbsp")), '\0');
    EXPECT_EQ(lh_web_xml_entity_get_char(lh_str_view_lit("ampx")), '\0');
}
