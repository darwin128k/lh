/**
 * @file writer.cpp
 * @brief ::lh_web_xml_writer_t: markup from a node tree, escaped and validated.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/web/xml/writer.h>

#include <string>

namespace
{

/* Write @p node, report through @p ok, and free it. */
std::string
write(lh_web_xml_node_t *node, bool *ok = nullptr)
{
    lh_web_xml_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_web_xml_writer_init(&writer, &out);
    const bool written = lh_web_xml_writer_try_write(&writer, node);
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
    lh_web_xml_node_destroy(node);
    return text;
}

/* Whether @p node can be written; frees it. */
bool
writes(lh_web_xml_node_t *node)
{
    bool ok = false;

    write(node, &ok);
    return ok;
}

} // namespace

TEST(web_xml_writer, writes_a_document_as_its_children)
{
    lh_web_xml_node_t *doc = lh_web_xml_node_create_document();

    lh_web_xml_node_push_child(
        doc, lh_web_xml_node_create_pi(lh_str_view_lit("xml"), lh_str_view_lit("version=\"1.0\"")));
    lh_web_xml_node_push_child(doc, lh_web_xml_node_create_doctype(lh_str_view_lit("note")));
    lh_web_xml_node_push_child(doc, lh_web_xml_node_create_empty(lh_str_view_lit("note")));
    lh_web_xml_node_push_child(doc, lh_web_xml_node_create_comment(lh_str_view_lit(" end ")));
    EXPECT_EQ(write(doc), "<?xml version=\"1.0\"?><!DOCTYPE note><note/><!-- end -->");
}

TEST(web_xml_writer, writes_elements_attributes_and_children_without_spacing)
{
    lh_web_xml_node_t *root = lh_web_xml_node_create_element(lh_str_view_lit("root"));
    lh_web_xml_node_t *item = lh_web_xml_node_create_empty(lh_str_view_lit("item"));

    lh_web_xml_node_add_attr(root, lh_str_view_lit("xml:lang"), lh_str_view_lit("ru"));
    lh_web_xml_node_add_attr(item, lh_str_view_lit("id"), lh_str_view_lit("1"));
    lh_web_xml_node_push_child(root, item);
    lh_web_xml_node_push_child(
        root, lh_web_xml_node_push_child(lh_web_xml_node_create_element(lh_str_view_lit("p")),
                                         lh_web_xml_node_create_text(lh_str_view_lit("hi"))));
    EXPECT_EQ(write(root), "<root xml:lang=\"ru\"><item id=\"1\"/><p>hi</p></root>");
}

TEST(web_xml_writer, escapes_text_and_values_but_not_raw_or_cdata)
{
    lh_web_xml_node_t *p = lh_web_xml_node_create_element(lh_str_view_lit("p"));

    lh_web_xml_node_add_attr(p, lh_str_view_lit("title"), lh_str_view_lit("\"a\"<b>&\n"));
    lh_web_xml_node_push_child(p, lh_web_xml_node_create_text(lh_str_view_lit("1<2 & 3>2")));
    lh_web_xml_node_push_child(p, lh_web_xml_node_create_raw(lh_str_view_lit("<i>x</i>")));
    lh_web_xml_node_push_child(p, lh_web_xml_node_create_cdata(lh_str_view_lit("<&>")));
    EXPECT_EQ(write(p), "<p title=\"&quot;a&quot;&lt;b>&amp;&#10;\">1&lt;2 &amp; 3&gt;2<i>x</i>"
                        "<![CDATA[<&>]]></p>");
}

TEST(web_xml_writer, an_empty_element_kind_differs_from_an_element_without_children)
{
    EXPECT_EQ(write(lh_web_xml_node_create_element(lh_str_view_lit("a"))), "<a></a>");
    EXPECT_EQ(write(lh_web_xml_node_create_empty(lh_str_view_lit("a"))), "<a/>");
    EXPECT_EQ(write(lh_web_xml_node_create_pi(lh_str_view_lit("go"), lh_str_view_lit(""))),
              "<?go?>");
}

TEST(web_xml_writer, refuses_what_would_break_the_markup)
{
    lh_web_xml_node_t *p = lh_web_xml_node_create_element(lh_str_view_lit("p"));
    lh_web_xml_attr_t *bare = lh_web_xml_attr_create();

    EXPECT_FALSE(writes(lh_web_xml_node_create_element(lh_str_view_lit("a b"))));
    EXPECT_FALSE(
        writes(lh_web_xml_node_add_attr(lh_web_xml_node_create_element(lh_str_view_lit("p")),
                                        lh_str_view_lit("x=y"), lh_str_view_lit("1"))));
    lh_web_xml_attr_set_name(bare, lh_str_view_lit("hidden"));
    lh_web_xml_attr_set_bare(bare, lh_bool_true);
    EXPECT_FALSE(writes(lh_web_xml_node_push_attr(p, bare)));
    EXPECT_FALSE(
        writes(lh_web_xml_node_push_child(lh_web_xml_node_create_empty(lh_str_view_lit("br")),
                                          lh_web_xml_node_create_text(lh_str_view_lit("x")))));
    EXPECT_FALSE(writes(lh_web_xml_node_create_comment(lh_str_view_lit("a--b"))));
    EXPECT_FALSE(writes(lh_web_xml_node_create_comment(lh_str_view_lit("a-"))));
    EXPECT_FALSE(writes(lh_web_xml_node_create_cdata(lh_str_view_lit("a]]>b"))));
    EXPECT_FALSE(writes(lh_web_xml_node_create_pi(lh_str_view_lit("go"), lh_str_view_lit("a?>"))));
    EXPECT_FALSE(writes(lh_web_xml_node_create_pi(lh_str_view_lit("1"), lh_str_view_lit("a"))));
    EXPECT_FALSE(writes(lh_web_xml_node_create()));
}

TEST(web_xml_writer, write_checks_the_result)
{
    lh_web_xml_node_t *bad = lh_web_xml_node_create_element(lh_str_view_lit("1"));
    lh_str_t out;

    lh_str_init(&out);
    lh_web_xml_writer_t writer;
    lh_web_xml_writer_init(&writer, &out);
    LH_EXPECT_CHECK_DEATH(lh_web_xml_writer_write(&writer, bad));
    lh_str_deinit(&out);
    lh_web_xml_node_destroy(bad);
}

namespace
{

/* Write @p node in the HTML dialect, report through @p ok, and free it. */
std::string
write_html(lh_web_xml_node_t *node, bool *ok = nullptr)
{
    lh_web_xml_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_web_xml_writer_init(&writer, &out);
    lh_web_xml_writer_set_dialect(&writer, lh_web_xml_dialect_html);
    EXPECT_TRUE(lh_web_xml_writer_is_html(&writer));
    const bool written = lh_web_xml_writer_try_write(&writer, node);
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
    lh_web_xml_node_destroy(node);
    return text;
}

} // namespace

TEST(web_xml_writer, init_writes_xml)
{
    lh_web_xml_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_web_xml_writer_init(&writer, &out);
    EXPECT_EQ(lh_web_xml_writer_get_dialect(&writer), lh_web_xml_dialect_xml);
    EXPECT_FALSE(lh_web_xml_writer_is_html(&writer));
    lh_str_deinit(&out);
}

TEST(web_xml_writer, html_writes_the_doctype_in_lower_case)
{
    EXPECT_EQ(write_html(lh_web_xml_node_create_doctype(lh_str_view_lit("html"))),
              "<!doctype html>");
}

TEST(web_xml_writer, html_writes_void_elements_and_bare_attributes)
{
    lh_web_xml_node_t *html = lh_web_xml_node_create_element(lh_str_view_lit("html"));
    lh_web_xml_node_t *meta = lh_web_xml_node_create_empty(lh_str_view_lit("meta"));
    lh_web_xml_attr_t *bare = lh_web_xml_attr_create();

    lh_web_xml_attr_set_name(bare, lh_str_view_lit("hidden"));
    lh_web_xml_attr_set_bare(bare, lh_bool_true);
    lh_web_xml_node_add_attr(html, lh_str_view_lit("lang"), lh_str_view_lit("ru"));
    lh_web_xml_node_push_attr(html, bare);
    lh_web_xml_node_add_attr(meta, lh_str_view_lit("charset"), lh_str_view_lit("utf-8"));
    lh_web_xml_node_push_child(html, meta);
    lh_web_xml_node_push_child(
        html, lh_web_xml_node_push_child(lh_web_xml_node_create_element(lh_str_view_lit("p")),
                                         lh_web_xml_node_create_text(lh_str_view_lit("hi"))));
    EXPECT_EQ(write_html(html),
              "<html lang=\"ru\" hidden><meta charset=\"utf-8\"><p>hi</p></html>");
}

TEST(web_xml_writer, html_escapes_like_xml_and_keeps_raw_and_comments)
{
    lh_web_xml_node_t *p = lh_web_xml_node_create_element(lh_str_view_lit("p"));

    lh_web_xml_node_add_attr(p, lh_str_view_lit("title"), lh_str_view_lit("\"a\"<b>&"));
    lh_web_xml_node_push_child(p, lh_web_xml_node_create_text(lh_str_view_lit("1<2 & 3>2")));
    lh_web_xml_node_push_child(p, lh_web_xml_node_create_raw(lh_str_view_lit("<i>x</i>")));
    lh_web_xml_node_push_child(p, lh_web_xml_node_create_comment(lh_str_view_lit(" c ")));
    EXPECT_EQ(write_html(p),
              "<p title=\"&quot;a&quot;&lt;b>&amp;\">1&lt;2 &amp; 3&gt;2<i>x</i><!-- c --></p>");
}

TEST(web_xml_writer, html_refuses_cdata_and_processing_instructions)
{
    bool ok = true;

    write_html(lh_web_xml_node_create_cdata(lh_str_view_lit("x")), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    write_html(lh_web_xml_node_create_pi(lh_str_view_lit("x"), lh_str_view_lit("y")), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    write_html(lh_web_xml_node_push_child(lh_web_xml_node_create_empty(lh_str_view_lit("br")),
                                          lh_web_xml_node_create_text(lh_str_view_lit("x"))),
               &ok);
    EXPECT_FALSE(ok);
}
