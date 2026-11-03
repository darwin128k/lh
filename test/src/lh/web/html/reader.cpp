/**
 * @file reader.cpp
 * @brief ::lh_web_html_reader_t: HTML read into an XML tree with HTML's rules,
 *        and written back byte for byte in the HTML dialect.
 */

#include <gtest/gtest.h>

#include <lh/web/html.h>

#include <string>

namespace
{

std::string
text_of(const lh_str_t *s)
{
    return std::string(lh_str_get_data(s) ? lh_str_get_data(s) : "", lh_str_get_size(s));
}

struct parsed
{
    bool ok;
    std::string written;
    lh_web_xml_error_t error;
};

/* Read @p text as HTML and write the tree back in the HTML dialect. */
parsed
read(const std::string &text)
{
    lh_web_html_reader_t reader;
    lh_web_xml_node_t doc;
    lh_str_view_t view;
    parsed result{};

    lh_str_view_init_by_size(&view, text.data(), text.size());
    lh_web_xml_node_init(&doc);
    lh_web_html_reader_init(&reader, view);
    result.ok = lh_web_html_reader_read(&reader, &doc);
    result.error = lh_web_html_reader_get_error(&reader);
    if (result.ok)
    {
        lh_str_t out;
        lh_web_xml_writer_t writer;

        lh_str_init(&out);
        lh_web_xml_writer_init(&writer, &out);
        lh_web_xml_writer_set_dialect(&writer, lh_web_xml_dialect_html);
        EXPECT_TRUE(lh_web_xml_writer_try_write(&writer, &doc));
        result.written = text_of(&out);
        lh_str_deinit(&out);
    }
    lh_web_xml_node_deinit(&doc);
    return result;
}

lh_web_xml_error_t
error_of(const std::string &text)
{
    const parsed result = read(text);

    EXPECT_FALSE(result.ok) << text;
    return result.error;
}

} // namespace

TEST(web_html_reader, a_page_reads_into_an_xml_tree)
{
    lh_web_html_reader_t reader;
    lh_web_xml_node_t doc;

    lh_web_xml_node_init(&doc);
    lh_web_html_reader_init(&reader,
                            lh_str_view_lit("<!DOCTYPE html><p class=x hidden>a<br>b</P>"));
    ASSERT_TRUE(lh_web_html_reader_read(&reader, &doc));
    const lh_web_xml_node_t *doctype = lh_web_xml_node_get_first_child(&doc);
    ASSERT_NE(doctype, nullptr);
    EXPECT_TRUE(lh_web_xml_node_has_kind(doctype, lh_web_xml_kind_doctype));
    EXPECT_EQ(text_of(lh_web_xml_node_get_text(doctype)), "html");
    const lh_web_xml_node_t *p = lh_web_xml_node_get_next_child(&doc, doctype);
    ASSERT_NE(p, nullptr);
    const lh_web_xml_attr_t *cls = lh_web_xml_node_get_first_attr(p);
    ASSERT_NE(cls, nullptr);
    EXPECT_EQ(text_of(lh_web_xml_attr_get_value(cls)), "x");
    const lh_web_xml_attr_t *hidden = lh_web_xml_node_get_next_attr(p, cls);
    ASSERT_NE(hidden, nullptr);
    EXPECT_TRUE(lh_web_xml_attr_get_bare(hidden));
    const lh_web_xml_node_t *a = lh_web_xml_node_get_first_child(p);
    const lh_web_xml_node_t *br = lh_web_xml_node_get_next_child(p, a);
    ASSERT_NE(br, nullptr);
    EXPECT_TRUE(lh_web_xml_node_has_kind(br, lh_web_xml_kind_empty));
    lh_web_xml_node_deinit(&doc);
}

TEST(web_html_reader, what_the_writer_writes_reads_back_the_same)
{
    const std::string texts[] = {
        "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\"><title>t</title>"
        "<style>a>b{c:d}</style></head><body><canvas id=\"c\" tabindex=\"0\"></canvas>"
        "<script>\nif(a<b&&c)f(\"</p>\");\n</script></body></html>",
        "<p hidden title=\"&quot;a&quot;&lt;b>&amp;\">1&lt;2 &amp; 3&gt;2<!-- c --></p>",
        "<p>a</p><p>b</p>",
        "<script></script>",
        "",
    };

    for (const std::string &text : texts)
    {
        const parsed result = read(text);

        EXPECT_TRUE(result.ok) << text;
        EXPECT_EQ(result.written, text);
    }
}

TEST(web_html_reader, html_spellings_are_normalized_on_the_way_out)
{
    EXPECT_EQ(read("<!DOCTYPE html>\n<BR/>").written, "<!doctype html><BR>");
    EXPECT_EQ(read("<a href=x&amp;y w='1'>z</A >").written, "<a href=\"x&amp;y\" w=\"1\">z</a>");
    EXPECT_EQ(read("<input disabled  value = 2 >").written, "<input disabled value=\"2\">");
}

TEST(web_html_reader, errors_are_xml_errors)
{
    EXPECT_EQ(error_of("text"), lh_web_xml_error_trailing);
    EXPECT_EQ(error_of("<p>a"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<p>a<p>b</p>"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<p></div>"), lh_web_xml_error_mismatched_tag);
    EXPECT_EQ(error_of("<p>&nbsp;</p>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<p a=1 a=2></p>"), lh_web_xml_error_duplicate_attr);
    EXPECT_EQ(error_of("<p a=></p>"), lh_web_xml_error_unexpected_char);
    EXPECT_EQ(error_of("<script>x"), lh_web_xml_error_unexpected_end);
}
