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
    EXPECT_EQ(written([](lh_ui_web_buf_t *b) {
                  lh_ui_web_html_put_escaped(b, "a<b> & \"c\"", lh_bool_false);
              }),
              "a&lt;b&gt; &amp; \"c\"");
    EXPECT_EQ(written([](lh_ui_web_buf_t *b) {
                  lh_ui_web_html_put_escaped(b, "a<b> & \"c\"", lh_bool_true);
              }),
              "a&lt;b> &amp; &quot;c&quot;");
    EXPECT_EQ(written([](lh_ui_web_buf_t *b) {
                  lh_ui_web_html_put_escaped(b, nullptr, lh_bool_false);
              }),
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
