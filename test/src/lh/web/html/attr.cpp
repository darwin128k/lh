/**
 * @file attr.cpp
 * @brief ::lh_web_html_attr_t: an owned name and value, linked into an element.
 */

#include <gtest/gtest.h>

#include <lh/web/html/attr.h>

#include <string>

namespace
{

std::string
text_of(const lh_str_t *text)
{
    return std::string(lh_str_get_data(text) ? lh_str_get_data(text) : "", lh_str_get_size(text));
}

} // namespace

TEST(web_html_attr, init_is_empty_and_not_bare)
{
    lh_web_html_attr_t attr;

    lh_web_html_attr_init(&attr);
    EXPECT_TRUE(lh_str_is_empty(lh_web_html_attr_get_name(&attr)));
    EXPECT_TRUE(lh_str_is_empty(lh_web_html_attr_get_value(&attr)));
    EXPECT_FALSE(lh_web_html_attr_get_bare(&attr));
    lh_web_html_attr_deinit(&attr);
}

TEST(web_html_attr, copies_name_and_value)
{
    lh_web_html_attr_t *attr = lh_web_html_attr_create();
    std::string name = "lang";
    std::string value = "ru";

    lh_web_html_attr_set_bare(attr, lh_bool_true);
    lh_web_html_attr_set_name(attr, lh_str_view_lit("lang"));
    lh_web_html_attr_set_value(attr, lh_str_view_lit("ru"));
    EXPECT_EQ(text_of(lh_web_html_attr_get_name(attr)), name);
    EXPECT_EQ(text_of(lh_web_html_attr_get_value(attr)), value);
    /* A value makes the attribute a `name="value"` pair again. */
    EXPECT_FALSE(lh_web_html_attr_get_bare(attr));
    lh_web_html_attr_destroy(attr);
}

TEST(web_html_attr, is_found_again_from_its_link)
{
    lh_web_html_attr_t *attr = lh_web_html_attr_create();

    EXPECT_EQ(lh_web_html_attr_get_by_link_mut(lh_web_html_attr_get_link_mut(attr)), attr);
    EXPECT_EQ(lh_web_html_attr_get_by_link(lh_web_html_attr_get_link(attr)), attr);
    lh_web_html_attr_destroy(attr);
}
