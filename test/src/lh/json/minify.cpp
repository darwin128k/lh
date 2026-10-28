#include <gtest/gtest.h>

#include <lh/json.h>

#include <string>

namespace
{

std::string
minify(std::string text)
{
    const lh_usize_t size = lh_json_minify(&text[0], text.size());
    text.resize(size);
    return text;
}

TEST(json_minify, drops_whitespace_outside_strings)
{
    EXPECT_EQ(minify(" {\n\t\"a b\" : [ 1 ,\r\n 2 ] } "), "{\"a b\":[1,2]}");
}

TEST(json_minify, drops_comments)
{
    EXPECT_EQ(minify("{ // the port\n \"port\": 27015, /* default */ \"on\": true }"),
              "{\"port\":27015,\"on\":true}");
}

TEST(json_minify, keeps_strings_exactly)
{
    EXPECT_EQ(minify("[\"a // not a comment\", \" /* nor this */ \", \"q\\\" \\\\\" , 1]"),
              "[\"a // not a comment\",\" /* nor this */ \",\"q\\\" \\\\\",1]");
}

TEST(json_minify, an_unterminated_string_or_comment_runs_to_the_end)
{
    EXPECT_EQ(minify("[1, \"a b"), "[1,\"a b");
    EXPECT_EQ(minify("[1 /* open"), "[1");
    EXPECT_EQ(minify("[1 // tail"), "[1");
    EXPECT_EQ(minify("\"ends in a backslash\\"), "\"ends in a backslash\\");
}

TEST(json_minify, an_empty_text_stays_empty)
{
    EXPECT_EQ(lh_json_minify(nullptr, 0U), 0U);
    EXPECT_EQ(minify(" \n\t "), "");
}

TEST(json_minify, its_output_reads_like_the_input)
{
    std::string text = "/* config */ {\n  \"a\": [1, 2], // two\n  \"b\": \"x y\"\n}\n";
    text.resize(lh_json_minify(&text[0], text.size()));

    lh_str_view_t view;
    lh_str_view_init_by_size(&view, text.data(), text.size());
    lh_json_reader_t reader;
    lh_json_node_t root;
    lh_json_node_init(&root);
    lh_json_reader_init(&reader, view);
    EXPECT_TRUE(lh_json_reader_read(&reader, &root)) << text;
    EXPECT_EQ(lh_json_node_get_size(&root), 2U);
    lh_json_node_deinit(&root);
}

} // namespace
