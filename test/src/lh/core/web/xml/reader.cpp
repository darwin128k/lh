/**
 * @file reader.cpp
 * @brief ::lh_web_xml_reader_t: documents read into trees, the steps on their
 *        own, every error with its position, and the writer's output read back.
 */

#include <gtest/gtest.h>

#include <lh/core/web/xml.h>

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
    lh_usize_t pos;
};

/* Read @p text and write the tree back. */
parsed
read(const std::string &text)
{
    parsed result{};

    lh_str_view_t view;
    lh_str_view_init_by_size(&view, text.data(), text.size());
    lh_web_xml_node_t doc;
    lh_web_xml_node_init(&doc);
    lh_web_xml_reader_t reader;
    lh_web_xml_reader_init(&reader, view);
    result.ok = lh_web_xml_reader_read(&reader, &doc);
    result.error = lh_web_xml_reader_get_error(&reader);
    result.pos = lh_str_cursor_get_pos(lh_web_xml_reader_get_cursor(&reader));
    if (result.ok)
    {
        lh_str_t out;
        lh_web_xml_writer_t writer;

        lh_str_init(&out);
        lh_web_xml_writer_init(&writer, &out);
        EXPECT_TRUE(lh_web_xml_writer_try_write(&writer, &doc));
        result.written = text_of(&out);
        lh_str_deinit(&out);
    }
    lh_web_xml_node_deinit(&doc);
    return result;
}

/* The error @p text stops on. */
lh_web_xml_error_t
error_of(const std::string &text)
{
    const parsed result = read(text);

    EXPECT_FALSE(result.ok) << text;
    return result.error;
}

} // namespace

TEST(web_xml_reader, a_document_reads_into_a_tree)
{
    lh_web_xml_node_t doc;

    lh_web_xml_node_init(&doc);
    lh_web_xml_reader_t reader;
    lh_web_xml_reader_init(&reader, lh_str_view_lit("<a x=\"1\" y='2'>b &amp; c<d/></a>"));
    ASSERT_TRUE(lh_web_xml_reader_read(&reader, &doc));
    EXPECT_TRUE(lh_web_xml_node_has_kind(&doc, lh_web_xml_kind_document));
    const lh_web_xml_node_t *a = lh_web_xml_node_get_first_child(&doc);
    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(lh_web_xml_node_has_kind(a, lh_web_xml_kind_element));
    EXPECT_EQ(text_of(lh_web_xml_node_get_name(a)), "a");
    const lh_web_xml_attr_t *y = lh_web_xml_node_find_attr(a, lh_str_view_lit("y"));
    ASSERT_NE(y, nullptr);
    EXPECT_EQ(text_of(lh_web_xml_attr_get_value(y)), "2");
    const lh_web_xml_node_t *text = lh_web_xml_node_get_first_child(a);
    ASSERT_NE(text, nullptr);
    EXPECT_TRUE(lh_web_xml_node_has_kind(text, lh_web_xml_kind_text));
    EXPECT_EQ(text_of(lh_web_xml_node_get_text(text)), "b & c");
    const lh_web_xml_node_t *d = lh_web_xml_node_get_next_child(a, text);
    ASSERT_NE(d, nullptr);
    EXPECT_TRUE(lh_web_xml_node_has_kind(d, lh_web_xml_kind_empty));
    EXPECT_EQ(lh_web_xml_node_get_next_child(&doc, a), nullptr);
    lh_web_xml_node_deinit(&doc);
}

TEST(web_xml_reader, what_the_writer_writes_reads_back_the_same)
{
    const std::string texts[] = {
        "<a/>",
        "<a></a>",
        "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE note SYSTEM \"n.dtd\"><note "
        "id=\"1\"><to>T&amp;J</to><!-- c --><![CDATA[<raw & stuff>]]><?pi data?></note><!--end-->",
        "<r a=\"&lt;&amp;&quot;&#9;&#10;&#13;\">x&#13;y&lt;z&gt;</r>",
        "<\xd0\xb8\xd0\xbc\xd1\x8f \xd0\xb0=\"\xd0\xb1\">\xd0\xb2</\xd0\xb8\xd0\xbc\xd1\x8f>",
        "<a>\n  <b>  </b>\n</a>",
        "<!DOCTYPE r [<!ENTITY e \"a>b\">]><r/>",
    };

    for (const std::string &text : texts)
    {
        const parsed result = read(text);

        EXPECT_TRUE(result.ok) << text;
        EXPECT_EQ(result.written, text);
    }
}

TEST(web_xml_reader, writing_normalizes_quotes_references_and_line_ends)
{
    EXPECT_EQ(read("<a x='\"'/>").written, "<a x=\"&quot;\"/>");
    EXPECT_EQ(read("<a>&#65;&#x42;&apos;&#x416;&#x1F600;</a>").written,
              "<a>AB'\xd0\x96\xf0\x9f\x98\x80</a>");
    EXPECT_EQ(read("<a x=\"1\r\n2\t3\">a\r\nb\rc</a>").written, "<a x=\"1 2 3\">a\nb\nc</a>");
    EXPECT_EQ(read("\xef\xbb\xbf  <a / >").ok, false);
    EXPECT_EQ(read("\xef\xbb\xbf  <a />\n").written, "<a/>");
    EXPECT_EQ(read("<a ></a >").written, "<a></a>");
}

TEST(web_xml_reader, errors_say_what_and_where)
{
    EXPECT_EQ(error_of(""), lh_web_xml_error_no_root);
    EXPECT_EQ(error_of("<!-- only -->"), lh_web_xml_error_no_root);
    EXPECT_EQ(error_of("text"), lh_web_xml_error_trailing);
    EXPECT_EQ(error_of("<a/><b/>"), lh_web_xml_error_trailing);
    EXPECT_EQ(error_of("<a/>x"), lh_web_xml_error_trailing);
    EXPECT_EQ(error_of("<a/><!DOCTYPE a>"), lh_web_xml_error_trailing);
    EXPECT_EQ(error_of("<a>"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<a"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<a x=\"1></a>"), lh_web_xml_error_unexpected_char);
    EXPECT_EQ(error_of("<a x=1/>"), lh_web_xml_error_unexpected_char);
    EXPECT_EQ(error_of("<a x/>"), lh_web_xml_error_unexpected_char);
    EXPECT_EQ(error_of("<a x=\"1\"y=\"2\"/>"), lh_web_xml_error_unexpected_char);
    EXPECT_EQ(error_of("<1/>"), lh_web_xml_error_bad_name);
    EXPECT_EQ(error_of("< a/>"), lh_web_xml_error_bad_name);
    EXPECT_EQ(error_of("<a>&nbsp;</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a>&amp</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a>& b</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a>&#;</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a>&#0;</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a>&#xD800;</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a>&#99999999999999999999;</a>"), lh_web_xml_error_bad_reference);
    EXPECT_EQ(error_of("<a><!-- a -- b --></a>"), lh_web_xml_error_bad_comment);
    EXPECT_EQ(error_of("<a><!-- open"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<a><![CDATA[x</a>"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<a></b>"), lh_web_xml_error_mismatched_tag);
    EXPECT_EQ(error_of("<a></ab>"), lh_web_xml_error_mismatched_tag);
    EXPECT_EQ(error_of("<a x=\"1\" x=\"2\"/>"), lh_web_xml_error_duplicate_attr);
    EXPECT_EQ(error_of("<?pi"), lh_web_xml_error_unexpected_end);
    EXPECT_EQ(error_of("<?pi\"x?><a/>"), lh_web_xml_error_unexpected_char);
    EXPECT_EQ(read("<a></b>").pos, 5U);
    EXPECT_EQ(read("<a>&nbsp;</a>").pos, 8U);
}

TEST(web_xml_reader, depth_is_limited)
{
    std::string deep;

    for (unsigned i = 0; i <= LH_WEB_XML_READER_DEPTH_MAX; ++i)
    {
        deep += "<a>";
    }
    EXPECT_EQ(error_of(deep), lh_web_xml_error_too_deep);
}

TEST(web_xml_reader, steps_work_on_their_own)
{
    lh_str_t out;

    lh_str_init(&out);
    lh_web_xml_reader_t reader;
    lh_web_xml_reader_init(&reader, lh_str_view_lit("ab:c-1 rest"));
    EXPECT_TRUE(lh_web_xml_reader_read_name_into(&reader, &out));
    EXPECT_EQ(text_of(&out), "ab:c-1");
    lh_str_clear(&out);
    lh_web_xml_reader_init(&reader, lh_str_view_lit("&#x41;&lt;"));
    EXPECT_TRUE(lh_web_xml_reader_read_reference(&reader, &out));
    EXPECT_TRUE(lh_web_xml_reader_read_reference(&reader, &out));
    EXPECT_EQ(text_of(&out), "A<");
    lh_str_clear(&out);
    lh_web_xml_reader_init(&reader, lh_str_view_lit("'a&amp;b'"));
    EXPECT_TRUE(lh_web_xml_reader_read_attr_value_into(&reader, &out));
    EXPECT_EQ(text_of(&out), "a&b");
    lh_web_xml_reader_init(&reader, lh_str_view_lit("\r\nx"));
    EXPECT_TRUE(lh_web_xml_reader_skip_newline(&reader));
    EXPECT_FALSE(lh_web_xml_reader_skip_newline(&reader));
    EXPECT_STREQ(lh_web_xml_error_get_message(lh_web_xml_error_mismatched_tag),
                 "end tag does not match the open element");
    EXPECT_STREQ(lh_web_xml_error_get_message(200U), "unknown error");
    lh_str_deinit(&out);
}
