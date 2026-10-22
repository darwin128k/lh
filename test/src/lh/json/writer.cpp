#include <gtest/gtest.h>

#include <lh/json.h>

#include <string>

namespace
{

lh_str_view_t
view_of(const std::string &s)
{
    lh_str_view_t v;
    if (s.empty())
    {
        lh_str_view_init_empty(&v);
    }
    else
    {
        lh_str_init_by_size(&v, s.data(), s.size());
    }
    return v;
}

std::string
write(const lh_json_node_t *node, lh_usize_t indent)
{
    lh_json_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_json_writer_init(&writer, &out, indent);
    lh_json_writer_write(&writer, node);
    std::string text(lh_str_get_data(&out) ? lh_str_get_data(&out) : "", lh_str_get_size(&out));
    lh_str_deinit(&out);
    return text;
}

/* Read @p json and write it back with @p indent. */
std::string
rewrite(const std::string &json, lh_usize_t indent = 0U)
{
    lh_json_reader_t reader;
    lh_json_node_t root;

    lh_json_node_init(&root);
    lh_json_reader_init(&reader, view_of(json));
    EXPECT_TRUE(lh_json_reader_read(&reader, &root)) << json;
    std::string text = write(&root, indent);
    lh_json_node_deinit(&root);
    return text;
}

TEST(json_writer, writes_compact_text)
{
    EXPECT_EQ(
        rewrite(" { \"a\" : [ 1 , -2.5e3 , true , false , null ] , \"b\" : { } , \"c\" : [ ] } "),
        "{\"a\":[1,-2.5e3,true,false,null],\"b\":{},\"c\":[]}");
}

TEST(json_writer, numbers_come_back_with_the_same_digits)
{
    EXPECT_EQ(rewrite("[0,-0,1.50,1E+400,123456789012345678901234567890]"),
              "[0,-0,1.50,1E+400,123456789012345678901234567890]");
}

TEST(json_writer, writes_pretty_text)
{
    const std::string pretty = "{\n"
                               "  \"a\": [\n"
                               "    1,\n"
                               "    2\n"
                               "  ],\n"
                               "  \"b\": {},\n"
                               "  \"c\": {\n"
                               "    \"d\": null\n"
                               "  }\n"
                               "}";

    EXPECT_EQ(rewrite("{\"a\":[1,2],\"b\":{},\"c\":{\"d\":null}}", 2U), pretty);
    EXPECT_EQ(rewrite("[]", 4U), "[]");
    EXPECT_EQ(rewrite("7", 4U), "7");
}

TEST(json_writer, escapes_what_a_string_cannot_hold)
{
    EXPECT_EQ(rewrite("\"q\\\" b\\\\ s/ \\b\\f\\n\\r\\t \\u0001\\u001f\""),
              "\"q\\\" b\\\\ s/ \\b\\f\\n\\r\\t \\u0001\\u001F\"");
}

TEST(json_writer, writes_utf8_as_it_is)
{
    EXPECT_EQ(rewrite("\"\\u041a\\u20ac\\ud83d\\ude00\""),
              "\"\xD0\x9A\xE2\x82\xAC\xF0\x9F\x98\x80\"");
}

TEST(json_writer, escapes_keys_too)
{
    EXPECT_EQ(rewrite("{\"a\\nb\":1,\"\":2}"), "{\"a\\nb\":1,\"\":2}");
}

TEST(json_writer, a_node_holding_nothing_is_null)
{
    lh_json_node_t node;
    lh_json_node_init(&node);
    EXPECT_EQ(write(&node, 0U), "null");
    lh_json_node_deinit(&node);
}

TEST(json_writer, appends_and_never_clears)
{
    lh_json_node_t node;
    lh_json_writer_t writer;
    lh_str_t out;

    lh_json_node_init(&node);
    lh_json_node_set_bool(&node, lh_bool_true);
    lh_str_init(&out);
    lh_str_append_view(&out, lh_str_view_lit("x="));
    lh_json_writer_init(&writer, &out, 0U);
    lh_json_writer_write(&writer, &node);
    EXPECT_EQ(std::string(lh_str_get_data(&out), lh_str_get_size(&out)), "x=true");
    lh_str_deinit(&out);
    lh_json_node_deinit(&node);
}

TEST(json_writer, what_it_writes_reads_back_to_an_equal_tree)
{
    const std::string json =
        "{\"name\":\"a \\\"b\\\" \\u00e9\",\"list\":[1,[2,{\"x\":null}],-3.5e-7],\"on\":false}";

    for (const lh_usize_t indent : {0U, 1U, 4U})
    {
        lh_json_reader_t reader;
        lh_json_node_t first;
        lh_json_node_t second;

        lh_json_node_init(&first);
        lh_json_node_init(&second);
        lh_json_reader_init(&reader, view_of(json));
        ASSERT_TRUE(lh_json_reader_read(&reader, &first));

        const std::string text = write(&first, indent);
        lh_json_reader_init(&reader, view_of(text));
        ASSERT_TRUE(lh_json_reader_read(&reader, &second)) << text;
        EXPECT_TRUE(lh_json_node_equals(&first, &second)) << text;

        lh_json_node_deinit(&first);
        lh_json_node_deinit(&second);
    }
}

} // namespace
