#include <gtest/gtest.h>

#include <lh/json.h>

#include <string>

namespace
{

std::string
text(const lh_str_t *s)
{
    return std::string(lh_str_get_data(s) ? lh_str_get_data(s) : "", lh_str_get_size(s));
}

/* The tree in a canonical form: no whitespace, strings as their raw bytes in
   quotes (escapes already decoded), numbers as written. */
std::string
dump(const lh_json_node_t *node)
{
    std::string out;

    switch (lh_json_node_get_kind(node))
    {
    case lh_json_kind_null:
        return "null";
    case lh_json_kind_bool:
        return lh_json_node_get_bool(node) ? "true" : "false";
    case lh_json_kind_number:
        return text(lh_json_node_get_text(node));
    case lh_json_kind_string:
        return "\"" + text(lh_json_node_get_text(node)) + "\"";
    case lh_json_kind_array:
        out = "[";
        for (const lh_json_node_t *c = lh_json_node_get_first(node); c;
             c = lh_json_node_get_next(node, c))
        {
            out += (out.size() > 1 ? "," : "") + dump(c);
        }
        return out + "]";
    case lh_json_kind_object:
        out = "{";
        for (const lh_json_node_t *c = lh_json_node_get_first(node); c;
             c = lh_json_node_get_next(node, c))
        {
            out += (out.size() > 1 ? "," : "") + ("\"" + text(lh_json_node_get_key(c)) + "\":") +
                   dump(c);
        }
        return out + "}";
    default:
        return "<none>";
    }
}

struct parsed
{
    bool ok;
    std::string tree;
    lh_json_error_t error;
    lh_usize_t pos;
};

parsed
parse(const std::string &json)
{
    lh_str_view_t view;
    if (json.empty())
    {
        lh_str_view_init_empty(&view);
    }
    else
    {
        lh_str_view_init_by_size(&view, json.data(), json.size());
    }

    lh_json_reader_t reader;
    lh_json_node_t root;
    lh_json_node_init(&root);
    lh_json_reader_init(&reader, view);

    parsed out;
    out.ok = lh_json_reader_read(&reader, &root);
    out.tree = out.ok ? dump(&root) : "";
    out.error = lh_json_reader_get_error(&reader);
    out.pos = lh_json_reader_get_pos(&reader);
    lh_json_node_deinit(&root);
    return out;
}

std::string
tree(const std::string &json)
{
    const parsed p = parse(json);
    EXPECT_TRUE(p.ok) << json << " -> " << lh_json_error_get_message(p.error) << " at " << p.pos;
    return p.tree;
}

lh_json_error_t
error(const std::string &json)
{
    const parsed p = parse(json);
    EXPECT_FALSE(p.ok) << json << " was read as " << p.tree;
    return p.error;
}

TEST(json_reader, reads_every_scalar)
{
    EXPECT_EQ(tree("null"), "null");
    EXPECT_EQ(tree("true"), "true");
    EXPECT_EQ(tree("false"), "false");
    EXPECT_EQ(tree("42"), "42");
    EXPECT_EQ(tree("\"hi\""), "\"hi\"");
}

TEST(json_reader, keeps_numbers_as_written)
{
    EXPECT_EQ(tree("0"), "0");
    EXPECT_EQ(tree("-0"), "-0");
    EXPECT_EQ(tree("-12.50"), "-12.50");
    EXPECT_EQ(tree("3.5e2"), "3.5e2");
    EXPECT_EQ(tree("1E+400"), "1E+400");
    EXPECT_EQ(tree("2e-3"), "2e-3");
    EXPECT_EQ(tree("123456789012345678901234567890"), "123456789012345678901234567890");
}

TEST(json_reader, rejects_numbers_off_the_grammar)
{
    EXPECT_EQ(error("01"), lh_json_error_bad_number);
    EXPECT_EQ(error("-01"), lh_json_error_bad_number);
    EXPECT_EQ(error("1."), lh_json_error_bad_number);
    EXPECT_EQ(error("1.e3"), lh_json_error_bad_number);
    EXPECT_EQ(error("1e"), lh_json_error_bad_number);
    EXPECT_EQ(error("1e+"), lh_json_error_bad_number);
    EXPECT_EQ(error("-"), lh_json_error_bad_number);
    EXPECT_EQ(error("-a"), lh_json_error_bad_number);
    EXPECT_EQ(error(".5"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("+1"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("0x1F"), lh_json_error_trailing);
}

TEST(json_reader, reads_nested_arrays_and_objects)
{
    EXPECT_EQ(tree("[]"), "[]");
    EXPECT_EQ(tree("{}"), "{}");
    EXPECT_EQ(tree("[1,[2,[3]],{}]"), "[1,[2,[3]],{}]");
    EXPECT_EQ(tree(" { \"a\" : [ true , null ] ,\n\t\"b\" : { \"c\" : \"d\" } } "),
              "{\"a\":[true,null],\"b\":{\"c\":\"d\"}}");
}

TEST(json_reader, keeps_duplicate_keys_in_order)
{
    EXPECT_EQ(tree("{\"k\":1,\"k\":2}"), "{\"k\":1,\"k\":2}");
}

TEST(json_reader, decodes_escapes)
{
    EXPECT_EQ(tree("\"a\\\"b\\\\c\\/d\""), "\"a\"b\\c/d\"");
    EXPECT_EQ(tree("\"\\b\\f\\n\\r\\t\""), "\"\b\f\n\r\t\"");
    EXPECT_EQ(tree("\"\\u0041\\u00e9\\u20AC\""), "\"A\xC3\xA9\xE2\x82\xAC\"");
    /* A surrogate pair is one code point above U+FFFF: U+1F600. */
    EXPECT_EQ(tree("\"\\uD83D\\uDE00\""), "\"\xF0\x9F\x98\x80\"");
    /* UTF-8 written as is goes through untouched. */
    EXPECT_EQ(tree("\"\xD0\x9A\""), "\"\xD0\x9A\"");
}

TEST(json_reader, rejects_bad_strings)
{
    EXPECT_EQ(error("\"abc"), lh_json_error_unexpected_end);
    EXPECT_EQ(error("\"a\nb\""), lh_json_error_bad_string);
    EXPECT_EQ(error(std::string("\"a\0b\"", 5)), lh_json_error_bad_string);
    EXPECT_EQ(error("\"\\x\""), lh_json_error_bad_escape);
    EXPECT_EQ(error("\"\\"), lh_json_error_unexpected_end);
    EXPECT_EQ(error("\"\\u12\""), lh_json_error_bad_unicode);
    EXPECT_EQ(error("\"\\u0000\""), lh_json_error_bad_unicode);
    EXPECT_EQ(error("\"\\uDE00\""), lh_json_error_bad_unicode);
    EXPECT_EQ(error("\"\\uD83D\""), lh_json_error_bad_unicode);
    EXPECT_EQ(error("\"\\uD83D\\u0041\""), lh_json_error_bad_unicode);
}

TEST(json_reader, rejects_broken_structure)
{
    EXPECT_EQ(error(""), lh_json_error_unexpected_end);
    EXPECT_EQ(error("   "), lh_json_error_unexpected_end);
    EXPECT_EQ(error("["), lh_json_error_unexpected_end);
    EXPECT_EQ(error("[1"), lh_json_error_unexpected_end);
    EXPECT_EQ(error("[1,]"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("[1 2]"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("{\"a\" 1}"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("{a:1}"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("{\"a\":1,}"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("nul"), lh_json_error_bad_literal);
    EXPECT_EQ(error("True"), lh_json_error_unexpected_char);
    EXPECT_EQ(error("[1] x"), lh_json_error_trailing);
    EXPECT_EQ(error("1 2"), lh_json_error_trailing);
}

TEST(json_reader, reports_where_it_stopped)
{
    const parsed p = parse("{\"a\": [1, 2,, 3]}");
    EXPECT_EQ(p.error, lh_json_error_unexpected_char);
    EXPECT_EQ(p.pos, 12U); /* the second comma */
}

TEST(json_reader, skips_a_byte_order_mark)
{
    EXPECT_EQ(tree("\xEF\xBB\xBF[1]"), "[1]");
}

TEST(json_reader, only_four_characters_are_whitespace)
{
    EXPECT_EQ(tree(" \t\r\n1 \t\r\n"), "1");
    EXPECT_EQ(error("\f1"), lh_json_error_unexpected_char);
}

TEST(json_reader, limits_the_nesting_depth)
{
    const std::string deep_ok(LH_JSON_READER_DEPTH_MAX, '[');
    const std::string close_ok(LH_JSON_READER_DEPTH_MAX, ']');
    EXPECT_TRUE(parse(deep_ok + close_ok).ok);

    const std::string too_deep(LH_JSON_READER_DEPTH_MAX + 1U, '[');
    EXPECT_EQ(error(too_deep), lh_json_error_too_deep);
}

TEST(json_reader, a_failed_read_stays_failed)
{
    lh_json_reader_t reader;
    lh_json_node_t root;
    lh_str_view_t view = lh_str_view_lit("[1,]");

    lh_json_node_init(&root);
    lh_json_reader_init(&reader, view);
    EXPECT_FALSE(lh_json_reader_read(&reader, &root));
    EXPECT_TRUE(lh_json_reader_has_error(&reader));
    EXPECT_FALSE(lh_json_reader_read(&reader, &root));
    /* What was read before the error is in the tree, and deinit frees it. */
    EXPECT_EQ(lh_json_node_get_size(&root), 2U);
    lh_json_node_deinit(&root);
}

TEST(json_reader, reads_an_api_reply)
{
    lh_json_reader_t reader;
    lh_json_node_t root;
    lh_str_view_t view =
        lh_str_view_lit("{\"ip\":\"8.8.8.8\",\"countryCode\":\"US\",\"asn\":15169,\"block\":0}");

    lh_json_node_init(&root);
    lh_json_reader_init(&reader, view);
    ASSERT_TRUE(lh_json_reader_read(&reader, &root));

    lh_str_view_t code = lh_json_node_get_str_or(
        lh_json_node_get_member(&root, lh_str_view_lit("countryCode")), lh_str_view_lit("??"));
    EXPECT_EQ(std::string(lh_str_view_get_data(&code), lh_str_view_get_size(&code)), "US");
    EXPECT_EQ(lh_json_node_get_sint_or(lh_json_node_get_member(&root, lh_str_view_lit("asn")), 0),
              15169);
    EXPECT_EQ(
        lh_json_node_get_sint_or(lh_json_node_get_member(&root, lh_str_view_lit("missing")), -1),
        -1);
    lh_json_node_deinit(&root);
}

} // namespace
