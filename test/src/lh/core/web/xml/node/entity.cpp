/**
 * @file entity.cpp
 * @brief XML tree entity data access and composition with node operations.
 */

#include <gtest/gtest.h>

#include <lh/core/web/xml/node.h>

TEST(web_xml_entity, node_operations_use_the_same_entity_data)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    lh_web_xml_entity_set_kind(&entity, lh_web_xml_kind_element);
    EXPECT_EQ(lh_web_xml_node_get_kind(&entity), lh_web_xml_kind_element);
    EXPECT_EQ(lh_web_xml_entity_get_link(&entity), lh_web_xml_node_get_link(&entity));
    EXPECT_EQ(lh_web_xml_entity_get_name(&entity), lh_web_xml_node_get_name(&entity));
    EXPECT_EQ(lh_web_xml_entity_get_text(&entity), lh_web_xml_node_get_text(&entity));
    EXPECT_EQ(lh_web_xml_entity_get_attrs(&entity), lh_web_xml_node_get_attrs(&entity));
    EXPECT_EQ(lh_web_xml_entity_get_children(&entity), lh_web_xml_node_get_children(&entity));
    lh_web_xml_node_set_element(&entity, lh_str_view_lit("div"));
    EXPECT_EQ(lh_web_xml_entity_get_kind(&entity), lh_web_xml_kind_element);
    EXPECT_EQ(lh_str_get_size(lh_web_xml_entity_get_name(&entity)), 3U);
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_link, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_link(&entity),
              lh_web_xml_entity_get_link_mut(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_link_mut, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_link_mut(&entity),
              lh_web_xml_entity_get_link(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_name, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_name(&entity),
              lh_web_xml_entity_get_name_mut(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_name_mut, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_name_mut(&entity),
              lh_web_xml_entity_get_name(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_text, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_text(&entity),
              lh_web_xml_entity_get_text_mut(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_text_mut, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_text_mut(&entity),
              lh_web_xml_entity_get_text(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_attrs, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_attrs(&entity),
              lh_web_xml_entity_get_attrs_mut(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_attrs_mut, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_attrs_mut(&entity),
              lh_web_xml_entity_get_attrs(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_children, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_children(&entity),
              lh_web_xml_entity_get_children_mut(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_children_mut, returns_the_entity_field)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_children_mut(&entity),
              lh_web_xml_entity_get_children(&entity));
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_get_kind, reads_the_initialized_kind)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    EXPECT_EQ(lh_web_xml_entity_get_kind(&entity), lh_web_xml_kind_none);
    lh_web_xml_node_deinit(&entity);
}

TEST(web_xml_node_entity_set_kind, stores_the_kind)
{
    lh_web_xml_entity_t entity;
    lh_web_xml_node_init(&entity);
    lh_web_xml_entity_set_kind(&entity, lh_web_xml_kind_text);
    EXPECT_EQ(lh_web_xml_entity_get_kind(&entity), lh_web_xml_kind_text);
    lh_web_xml_node_deinit(&entity);
}
