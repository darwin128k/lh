/**
 * @file writer.cpp
 * @brief ::lh_web_html_writer_t: markup from a node tree, escaped and validated.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/web/html/writer.h>

#include <string>

namespace
{

/* Write @p node, report through @p ok, and free it. */
std::string
write(lh_web_html_node_t *node, bool *ok = nullptr)
{
    lh_web_html_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_web_html_writer_init(&writer, &out);
    const bool written = lh_web_html_writer_try_write(&writer, node);
    if (ok != nullptr)
    {
        *ok = written;
    }
    else
    {
        EXPECT_TRUE(written);
    }
    std::string text(lh_str_get_data(&out) ? lh_str_get_data(&out) : "", lh_str_get_size(&out));
    lh_str_deinit(&out);
    lh_web_html_node_destroy(node);
    return text;
}

} // namespace

TEST(web_html_writer, writes_the_doctype)
{
    EXPECT_EQ(write(lh_web_html_node_create_doctype()), "<!doctype html>");
}

TEST(web_html_writer, writes_elements_attributes_and_children_without_spacing)
{
    lh_web_html_node_t *html = lh_web_html_node_create_element(lh_str_view_lit("html"));
    lh_web_html_node_t *meta = lh_web_html_node_create_void(lh_str_view_lit("meta"));
    lh_web_html_attr_t *bare = lh_web_html_attr_create();

    lh_web_html_attr_set_name(bare, lh_str_view_lit("hidden"));
    lh_web_html_attr_set_bare(bare, lh_bool_true);
    lh_web_html_node_add_attr(html, lh_str_view_lit("lang"), lh_str_view_lit("ru"));
    lh_web_html_node_push_attr(html, bare);
    lh_web_html_node_add_attr(meta, lh_str_view_lit("charset"), lh_str_view_lit("utf-8"));
    lh_web_html_node_push_child(html, meta);
    lh_web_html_node_push_child(
        html, lh_web_html_node_push_child(lh_web_html_node_create_element(lh_str_view_lit("p")),
                                          lh_web_html_node_create_text(lh_str_view_lit("hi"))));
    EXPECT_EQ(write(html), "<html lang=\"ru\" hidden><meta charset=\"utf-8\"><p>hi</p></html>");
}

TEST(web_html_writer, escapes_text_and_values_but_not_raw)
{
    lh_web_html_node_t *p = lh_web_html_node_create_element(lh_str_view_lit("p"));

    lh_web_html_node_add_attr(p, lh_str_view_lit("title"), lh_str_view_lit("\"a\"<b>&"));
    lh_web_html_node_push_child(p, lh_web_html_node_create_text(lh_str_view_lit("1<2 & 3>2")));
    lh_web_html_node_push_child(p, lh_web_html_node_create_raw(lh_str_view_lit("<i>x</i>")));
    EXPECT_EQ(write(p), "<p title=\"&quot;a&quot;&lt;b>&amp;\">1&lt;2 &amp; 3&gt;2<i>x</i></p>");
}

TEST(web_html_writer, an_empty_element_still_closes)
{
    EXPECT_EQ(write(lh_web_html_node_create_element(lh_str_view_lit("canvas"))),
              "<canvas></canvas>");
}

TEST(web_html_writer, rejects_bad_names_void_children_and_no_kind)
{
    bool ok = true;

    write(lh_web_html_node_create_element(lh_str_view_lit("a b")), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    lh_web_html_node_t *p = lh_web_html_node_create_element(lh_str_view_lit("p"));
    lh_web_html_node_add_attr(p, lh_str_view_lit("x=y"), lh_str_view_lit("1"));
    write(p, &ok);
    EXPECT_FALSE(ok);
    ok = true;
    write(lh_web_html_node_push_child(lh_web_html_node_create_void(lh_str_view_lit("br")),
                                      lh_web_html_node_create_text(lh_str_view_lit("x"))),
          &ok);
    EXPECT_FALSE(ok);
    ok = true;
    write(lh_web_html_node_create(), &ok);
    EXPECT_FALSE(ok);
}

TEST(web_html_writer, write_checks_the_result)
{
    lh_web_html_node_t *bad = lh_web_html_node_create_element(lh_str_view_lit("1"));
    lh_str_t out;

    lh_str_init(&out);
    lh_web_html_writer_t writer;
    lh_web_html_writer_init(&writer, &out);
    LH_EXPECT_CHECK_DEATH(lh_web_html_writer_write(&writer, bad));
    lh_str_deinit(&out);
    lh_web_html_node_destroy(bad);
}
