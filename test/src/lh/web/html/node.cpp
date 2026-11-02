/**
 * @file node.cpp
 * @brief ::lh_web_html_node_t: kinds, owned children and attributes.
 */

#include <gtest/gtest.h>

#include <lh/web/html/node.h>

#include <string>

namespace
{

std::string
text_of(const lh_str_t *text)
{
    return std::string(lh_str_get_data(text) ? lh_str_get_data(text) : "", lh_str_get_size(text));
}

} // namespace

TEST(web_html_node, init_has_no_kind_and_nothing_inside)
{
    lh_web_html_node_t node;

    lh_web_html_node_init(&node);
    EXPECT_TRUE(lh_web_html_node_has_kind(&node, lh_web_html_kind_none));
    EXPECT_EQ(lh_web_html_node_get_first_child(&node), nullptr);
    EXPECT_EQ(lh_web_html_node_get_first_attr(&node), nullptr);
    lh_web_html_node_deinit(&node);
}

TEST(web_html_node, composites_set_kind_name_and_text)
{
    lh_web_html_node_t *doctype = lh_web_html_node_create_doctype();
    lh_web_html_node_t *element = lh_web_html_node_create_element(lh_str_view_lit("p"));
    lh_web_html_node_t *empty = lh_web_html_node_create_void(lh_str_view_lit("br"));
    lh_web_html_node_t *text = lh_web_html_node_create_text(lh_str_view_lit("a<b"));
    lh_web_html_node_t *raw = lh_web_html_node_create_raw(lh_str_view_lit("<b>"));

    EXPECT_TRUE(lh_web_html_node_has_kind(doctype, lh_web_html_kind_doctype));
    EXPECT_TRUE(lh_web_html_node_has_kind(element, lh_web_html_kind_element));
    EXPECT_EQ(text_of(lh_web_html_node_get_name(element)), "p");
    EXPECT_TRUE(lh_web_html_node_has_kind(empty, lh_web_html_kind_void));
    EXPECT_EQ(text_of(lh_web_html_node_get_name(empty)), "br");
    EXPECT_TRUE(lh_web_html_node_has_kind(text, lh_web_html_kind_text));
    EXPECT_EQ(text_of(lh_web_html_node_get_text(text)), "a<b");
    EXPECT_TRUE(lh_web_html_node_has_kind(raw, lh_web_html_kind_raw));
    EXPECT_EQ(text_of(lh_web_html_node_get_text(raw)), "<b>");
    lh_web_html_node_destroy(raw);
    lh_web_html_node_destroy(text);
    lh_web_html_node_destroy(empty);
    lh_web_html_node_destroy(element);
    lh_web_html_node_destroy(doctype);
}

TEST(web_html_node, pushes_chain_and_keep_order)
{
    lh_web_html_node_t *body = lh_web_html_node_create_element(lh_str_view_lit("body"));
    lh_web_html_node_t *first = lh_web_html_node_create_text(lh_str_view_lit("1"));
    lh_web_html_node_t *second = lh_web_html_node_create_void(lh_str_view_lit("br"));

    EXPECT_EQ(lh_web_html_node_push_child(lh_web_html_node_push_child(body, first), second), body);
    EXPECT_EQ(lh_web_html_node_add_attr(body, lh_str_view_lit("id"), lh_str_view_lit("b")), body);
    EXPECT_EQ(lh_web_html_node_get_first_child(body), first);
    EXPECT_EQ(lh_web_html_node_get_next_child(body, first), second);
    EXPECT_EQ(lh_web_html_node_get_next_child(body, second), nullptr);
    const lh_web_html_attr_t *attr = lh_web_html_node_get_first_attr(body);
    ASSERT_NE(attr, nullptr);
    EXPECT_EQ(text_of(lh_web_html_attr_get_name(attr)), "id");
    EXPECT_EQ(text_of(lh_web_html_attr_get_value(attr)), "b");
    EXPECT_EQ(lh_web_html_node_get_next_attr(body, attr), nullptr);
    /* Destroying the parent frees the children and the attributes. */
    lh_web_html_node_destroy(body);
}

TEST(web_html_node, clear_drops_children_and_kind)
{
    lh_web_html_node_t node;

    lh_web_html_node_init(&node);
    lh_web_html_node_set_element(&node, lh_str_view_lit("div"));
    lh_web_html_node_push_child(&node, lh_web_html_node_create_text(lh_str_view_lit("x")));
    lh_web_html_node_add_attr(&node, lh_str_view_lit("id"), lh_str_view_lit("y"));
    lh_web_html_node_clear(&node);
    EXPECT_TRUE(lh_web_html_node_has_kind(&node, lh_web_html_kind_none));
    EXPECT_EQ(lh_web_html_node_get_first_child(&node), nullptr);
    EXPECT_EQ(lh_web_html_node_get_first_attr(&node), nullptr);
    EXPECT_TRUE(lh_str_is_empty(lh_web_html_node_get_name(&node)));
    lh_web_html_node_deinit(&node);
}
