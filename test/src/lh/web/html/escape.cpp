/**
 * @file escape.cpp
 * @brief HTML escaping: which bytes become entities in text and in attribute values.
 */

#include <gtest/gtest.h>

#include <lh/web/html/escape.h>

#include <string>

namespace
{

std::string
view_text(lh_str_view_t view)
{
    return std::string(lh_str_view_get_data(&view) ? lh_str_view_get_data(&view) : "",
                       lh_str_view_get_size(&view));
}

std::string
escape(const std::string &text, lh_bool_t in_attr)
{
    lh_str_t out;
    lh_str_view_t view;

    lh_str_init(&out);
    lh_str_view_init_by_size(&view, text.data(), text.size());
    lh_web_html_escape_append(&out, view, in_attr);
    std::string result(lh_str_get_data(&out) ? lh_str_get_data(&out) : "", lh_str_get_size(&out));
    lh_str_deinit(&out);
    return result;
}

} // namespace

TEST(web_html_escape, entities_depend_on_the_context)
{
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('&', lh_bool_false)), "&amp;");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('<', lh_bool_false)), "&lt;");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('>', lh_bool_false)), "&gt;");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('"', lh_bool_false)), "");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('&', lh_bool_true)), "&amp;");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('<', lh_bool_true)), "&lt;");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('"', lh_bool_true)), "&quot;");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('>', lh_bool_true)), "");
    EXPECT_EQ(view_text(lh_web_html_escape_get_entity('a', lh_bool_true)), "");
}

TEST(web_html_escape, is_needed_follows_the_entities)
{
    EXPECT_TRUE(lh_web_html_escape_is_needed('<', lh_bool_false));
    EXPECT_FALSE(lh_web_html_escape_is_needed('"', lh_bool_false));
    EXPECT_TRUE(lh_web_html_escape_is_needed('"', lh_bool_true));
    EXPECT_FALSE(lh_web_html_escape_is_needed('\n', lh_bool_true));
}

TEST(web_html_escape, append_keeps_runs_and_replaces_specials)
{
    EXPECT_EQ(escape("", lh_bool_false), "");
    EXPECT_EQ(escape("plain text", lh_bool_false), "plain text");
    EXPECT_EQ(escape("a<b>&\"c\"", lh_bool_false), "a&lt;b&gt;&amp;\"c\"");
    EXPECT_EQ(escape("a<b>&\"c\"", lh_bool_true), "a&lt;b>&amp;&quot;c&quot;");
    EXPECT_EQ(escape("\xc3\xa9<", lh_bool_false), "\xc3\xa9&lt;");
}
