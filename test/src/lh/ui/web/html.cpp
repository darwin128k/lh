/**
 * @file html.cpp
 * @brief HTML serialization: entities per context, attributes, a whole element.
 */

#include <gtest/gtest.h>

#include <lh/ui/web/html.h>

#include <string>

namespace
{

/* Run @p fill on a fresh buffer and return what it wrote. */
template <typename F>
std::string
written(F fill)
{
    lh_byte_t mem[256];
    lh_ui_web_buf_t buf;

    lh_ui_web_buf_init(&buf, mem, sizeof(mem));
    fill(&buf);
    EXPECT_TRUE(lh_ui_web_buf_ok(&buf));
    return std::string(reinterpret_cast<const char *>(lh_ui_web_buf_bytes(&buf)),
                       lh_ui_web_buf_len(&buf));
}

TEST(ui_web_html_get_kind, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_kind(lh_addr_of(node), lh_ui_web_html_kind_text);
    EXPECT_EQ(lh_ui_web_html_get_kind(lh_addr_of(node)), lh_ui_web_html_kind_text);
}

TEST(ui_web_html_set_kind, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_kind(lh_addr_of(node), lh_ui_web_html_kind_text);
    EXPECT_EQ(lh_ui_web_html_get_kind(lh_addr_of(node)), lh_ui_web_html_kind_text);
}

TEST(ui_web_html_get_text, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_text(lh_addr_of(node), "text");
    EXPECT_STREQ(lh_ui_web_html_get_text(lh_addr_of(node)), "text");
}

TEST(ui_web_html_set_text, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_text(lh_addr_of(node), "text");
    EXPECT_STREQ(lh_ui_web_html_get_text(lh_addr_of(node)), "text");
}

TEST(ui_web_html_get_raw, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_raw(lh_addr_of(node), "raw");
    EXPECT_STREQ(lh_ui_web_html_get_raw(lh_addr_of(node)), "raw");
}

TEST(ui_web_html_set_raw, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_raw(lh_addr_of(node), "raw");
    EXPECT_STREQ(lh_ui_web_html_get_raw(lh_addr_of(node)), "raw");
}

TEST(ui_web_html_get_tag_name, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_name(lh_addr_of(node), "section");
    EXPECT_STREQ(lh_ui_web_html_get_tag_name(lh_addr_of(node)), "section");
}

TEST(ui_web_html_set_tag_name, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_name(lh_addr_of(node), "section");
    EXPECT_STREQ(lh_ui_web_html_get_tag_name(lh_addr_of(node)), "section");
}

TEST(ui_web_html_get_tag_attrs, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_attr_t attrs[1];
    lh_ui_web_html_set_tag_attrs(lh_addr_of(node), attrs);
    EXPECT_EQ(lh_ui_web_html_get_tag_attrs(lh_addr_of(node)), attrs);
}

TEST(ui_web_html_set_tag_attrs, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_attr_t attrs[1];
    lh_ui_web_html_set_tag_attrs(lh_addr_of(node), attrs);
    EXPECT_EQ(lh_ui_web_html_get_tag_attrs(lh_addr_of(node)), attrs);
}

TEST(ui_web_html_get_tag_nattrs, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_nattrs(lh_addr_of(node), 3u);
    EXPECT_EQ(lh_ui_web_html_get_tag_nattrs(lh_addr_of(node)), 3u);
}

TEST(ui_web_html_set_tag_nattrs, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_nattrs(lh_addr_of(node), 3u);
    EXPECT_EQ(lh_ui_web_html_get_tag_nattrs(lh_addr_of(node)), 3u);
}

TEST(ui_web_html_get_tag_kids, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    const lh_ui_web_html_t *kids[1] = {lh_addr_of(node)};
    lh_ui_web_html_set_tag_kids(lh_addr_of(node), kids);
    EXPECT_EQ(lh_ui_web_html_get_tag_kids(lh_addr_of(node)), kids);
}

TEST(ui_web_html_set_tag_kids, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    const lh_ui_web_html_t *kids[1] = {lh_addr_of(node)};
    lh_ui_web_html_set_tag_kids(lh_addr_of(node), kids);
    EXPECT_EQ(lh_ui_web_html_get_tag_kids(lh_addr_of(node)), kids);
}

TEST(ui_web_html_get_tag_nkids, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_nkids(lh_addr_of(node), 2u);
    EXPECT_EQ(lh_ui_web_html_get_tag_nkids(lh_addr_of(node)), 2u);
}

TEST(ui_web_html_set_tag_nkids, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_nkids(lh_addr_of(node), 2u);
    EXPECT_EQ(lh_ui_web_html_get_tag_nkids(lh_addr_of(node)), 2u);
}

TEST(ui_web_html_get_tag_void_el, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_void_el(lh_addr_of(node), lh_bool_true);
    EXPECT_EQ(lh_ui_web_html_get_tag_void_el(lh_addr_of(node)), lh_bool_true);
}

TEST(ui_web_html_set_tag_void_el, stores_the_supplied_value)
{
    lh_ui_web_html_t node;
    lh_ui_web_html_set_tag_void_el(lh_addr_of(node), lh_bool_true);
    EXPECT_EQ(lh_ui_web_html_get_tag_void_el(lh_addr_of(node)), lh_bool_true);
}

TEST(ui_web_html_entity_of, depends_on_context)
{
    EXPECT_STREQ(lh_ui_web_html_entity_of('&', lh_bool_false), "&amp;");
    EXPECT_STREQ(lh_ui_web_html_entity_of('<', lh_bool_true), "&lt;");
    EXPECT_STREQ(lh_ui_web_html_entity_of('>', lh_bool_false), "&gt;");
    EXPECT_EQ(lh_ui_web_html_entity_of('>', lh_bool_true), nullptr);
    EXPECT_STREQ(lh_ui_web_html_entity_of('"', lh_bool_true), "&quot;");
    EXPECT_EQ(lh_ui_web_html_entity_of('"', lh_bool_false), nullptr);
    EXPECT_EQ(lh_ui_web_html_entity_of('a', lh_bool_true), nullptr);
}

TEST(ui_web_html_put_escaped, text_and_attr)
{
    EXPECT_EQ(written([](lh_ui_web_buf_t *b)
                      { lh_ui_web_html_put_escaped(b, "a<b> & \"c\"", lh_bool_false); }),
              "a&lt;b&gt; &amp; \"c\"");
    EXPECT_EQ(written([](lh_ui_web_buf_t *b)
                      { lh_ui_web_html_put_escaped(b, "a<b> & \"c\"", lh_bool_true); }),
              "a&lt;b> &amp; &quot;c&quot;");
    EXPECT_EQ(
        written([](lh_ui_web_buf_t *b) { lh_ui_web_html_put_escaped(b, nullptr, lh_bool_false); }),
        "");
}

TEST(ui_web_html_emit, element_with_attributes_and_text)
{
    lh_ui_web_html_attr_t attrs[2];
    lh_ui_web_html_t text;
    lh_ui_web_html_t el;
    const lh_ui_web_html_t *kids[1] = {&text};

    lh_ui_web_html_attr("title", "x\"y", &attrs[0]);
    lh_ui_web_html_attr("hidden", nullptr, &attrs[1]);
    lh_ui_web_html_text("1 < 2", &text);
    lh_ui_web_html_tag("p", attrs, 2u, kids, 1u, lh_bool_false, &el);
    EXPECT_EQ(written([&](lh_ui_web_buf_t *b) { lh_ui_web_html_emit(b, &el); }),
              "<p title=\"x&quot;y\" hidden>1 &lt; 2</p>");
}

} // namespace
