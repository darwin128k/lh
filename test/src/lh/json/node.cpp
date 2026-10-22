#include <gtest/gtest.h>

#include <lh/json.h>

#include <cstring>
#include <limits>
#include <string>

namespace
{

std::string
text(lh_str_view_t v)
{
    return std::string(lh_str_view_get_data(&v), lh_str_view_get_size(&v));
}

lh_json_node_t *
make_number(const char *digits)
{
    lh_json_node_t *n = lh_json_node_create();
    lh_str_view_t view;

    lh_str_init_by_size(&view, digits, std::strlen(digits));
    lh_json_node_set_kind(n, lh_json_kind_number);
    lh_str_assign_view(lh_json_node_get_text_mut(n), view);
    return n;
}

/* Read @p json into @p root, which the caller deinits. */
void
read(lh_json_node_t *root, lh_str_view_t json)
{
    lh_json_reader_t reader;
    lh_json_node_init(root);
    lh_json_reader_init(&reader, json);
    ASSERT_TRUE(lh_json_reader_read(&reader, root));
}

TEST(json_node, init_holds_nothing)
{
    lh_json_node_t node;
    lh_json_node_init(&node);

    EXPECT_TRUE(lh_json_node_has_kind(&node, lh_json_kind_none));
    EXPECT_EQ(lh_json_node_get_size(&node), 0U);
    EXPECT_EQ(lh_json_node_get_first(&node), nullptr);
    EXPECT_TRUE(lh_str_is_empty(lh_json_node_get_key(&node)));
    EXPECT_TRUE(lh_str_is_empty(lh_json_node_get_text(&node)));
    lh_json_node_deinit(&node);
}

TEST(json_node, push_back_keeps_order_and_deinit_frees_the_tree)
{
    lh_json_node_t root;
    lh_json_node_init(&root);
    lh_json_node_set_kind(&root, lh_json_kind_array);

    lh_json_node_t *inner = lh_json_node_create();
    lh_json_node_set_kind(inner, lh_json_kind_array);
    lh_json_node_push_back(inner, make_number("3"));

    lh_json_node_push_back(&root, make_number("1"));
    lh_json_node_push_back(&root, make_number("2"));
    lh_json_node_push_back(&root, inner);

    ASSERT_EQ(lh_json_node_get_size(&root), 3U);
    EXPECT_EQ(lh_json_node_get_sint_or(lh_json_node_get_at(&root, 0), 0), 1);
    EXPECT_EQ(lh_json_node_get_sint_or(lh_json_node_get_at(&root, 1), 0), 2);
    EXPECT_EQ(lh_json_node_get_at(&root, 2), inner);
    EXPECT_EQ(lh_json_node_get_at(&root, 3), nullptr);
    EXPECT_EQ(lh_json_node_get_next(&root, inner), nullptr);
    EXPECT_EQ(lh_json_node_get_sint_or(lh_json_node_get_first(inner), 0), 3);
    lh_json_node_deinit(&root);
}

TEST(json_node, get_member_finds_the_first_matching_key)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("{\"a\":1,\"b\":2,\"a\":3,\"A\":4}"));

    EXPECT_EQ(lh_json_node_get_sint_or(lh_json_node_get_member(&root, lh_str_view_lit("a")), 0), 1);
    EXPECT_EQ(lh_json_node_get_sint_or(lh_json_node_get_member(&root, lh_str_view_lit("A")), 0), 4);
    EXPECT_EQ(lh_json_node_get_member(&root, lh_str_view_lit("c")), nullptr);

    lh_json_node_t *b = lh_json_node_get_member_mut(&root, lh_str_view_lit("b"));
    ASSERT_NE(b, nullptr);
    lh_json_node_set_kind(b, lh_json_kind_null);
    EXPECT_TRUE(lh_json_node_has_kind(lh_json_node_get_at(&root, 1), lh_json_kind_null));
    lh_json_node_deinit(&root);
}

TEST(json_node, get_member_is_for_objects_only)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("[\"a\"]"));
    EXPECT_EQ(lh_json_node_get_member(&root, lh_str_view_lit("")), nullptr);
    lh_json_node_deinit(&root);
}

TEST(json_node, integers_come_out_of_their_text)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("[9223372036854775807,-9223372036854775808,"
                                "18446744073709551615,-1,1.0,1e2,\"7\",true]"));
    lh_sllong_t s = 0;
    lh_ullong_t u = 0U;

    EXPECT_TRUE(lh_json_node_get_sint(lh_json_node_get_at(&root, 0), &s));
    EXPECT_EQ(s, std::numeric_limits<lh_sllong_t>::max());
    EXPECT_TRUE(lh_json_node_get_sint(lh_json_node_get_at(&root, 1), &s));
    EXPECT_EQ(s, std::numeric_limits<lh_sllong_t>::min());

    EXPECT_FALSE(lh_json_node_get_sint(lh_json_node_get_at(&root, 2), &s));
    EXPECT_TRUE(lh_json_node_get_uint(lh_json_node_get_at(&root, 2), &u));
    EXPECT_EQ(u, std::numeric_limits<lh_ullong_t>::max());

    EXPECT_FALSE(lh_json_node_get_uint(lh_json_node_get_at(&root, 3), &u)); /* negative */
    EXPECT_FALSE(lh_json_node_get_sint(lh_json_node_get_at(&root, 4), &s)); /* 1.0 */
    EXPECT_FALSE(lh_json_node_get_sint(lh_json_node_get_at(&root, 5), &s)); /* 1e2 */
    EXPECT_FALSE(lh_json_node_get_sint(lh_json_node_get_at(&root, 6), &s)); /* a string */
    EXPECT_FALSE(lh_json_node_get_sint(lh_json_node_get_at(&root, 7), &s)); /* a bool */
    lh_json_node_deinit(&root);
}

TEST(json_node, fallbacks_cover_a_missing_node_and_another_kind)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("{\"on\":true,\"name\":\"x\",\"n\":5}"));

    const lh_json_node_t *on = lh_json_node_get_member(&root, lh_str_view_lit("on"));
    const lh_json_node_t *name = lh_json_node_get_member(&root, lh_str_view_lit("name"));
    const lh_json_node_t *n = lh_json_node_get_member(&root, lh_str_view_lit("n"));

    EXPECT_TRUE(lh_json_node_get_bool(on));
    EXPECT_EQ(text(lh_json_node_get_str(name)), "x");

    EXPECT_EQ(lh_json_node_get_bool_or(nullptr, lh_bool_true), lh_bool_true);
    EXPECT_EQ(lh_json_node_get_bool_or(name, lh_bool_false), lh_bool_false);
    EXPECT_EQ(text(lh_json_node_get_str_or(n, lh_str_view_lit("none"))), "none");
    EXPECT_EQ(text(lh_json_node_get_str_or(nullptr, lh_str_view_lit("none"))), "none");
    EXPECT_EQ(lh_json_node_get_sint_or(nullptr, -1), -1);
    EXPECT_EQ(lh_json_node_get_uint_or(name, 9U), 9U);
    EXPECT_EQ(lh_json_node_get_uint_or(n, 9U), 5U);
    lh_json_node_deinit(&root);
}

TEST(json_node, an_empty_string_is_a_string)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("\"\""));
    EXPECT_TRUE(lh_json_node_has_kind(&root, lh_json_kind_string));
    EXPECT_EQ(text(lh_json_node_get_str(&root)), "");
    lh_json_node_deinit(&root);
}

std::string
compact(const lh_json_node_t *node)
{
    lh_json_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_json_writer_init(&writer, &out, 0U);
    lh_json_writer_write(&writer, node);
    std::string s(lh_str_get_data(&out) ? lh_str_get_data(&out) : "", lh_str_get_size(&out));
    lh_str_deinit(&out);
    return s;
}

TEST(json_node, setters_replace_whatever_was_there)
{
    lh_json_node_t node;
    read(&node, lh_str_view_lit("[1,[2],{\"a\":3}]"));

    lh_json_node_set_sint(&node, std::numeric_limits<lh_sllong_t>::min());
    EXPECT_EQ(lh_json_node_get_size(&node), 0U);
    EXPECT_EQ(compact(&node), "-9223372036854775808");

    lh_json_node_set_uint(&node, std::numeric_limits<lh_ullong_t>::max());
    EXPECT_EQ(compact(&node), "18446744073709551615");
    lh_json_node_set_str(&node, lh_str_view_lit("a\"b"));
    EXPECT_EQ(compact(&node), "\"a\\\"b\"");
    lh_json_node_set_bool(&node, lh_bool_false);
    EXPECT_EQ(compact(&node), "false");
    lh_json_node_set_null(&node);
    EXPECT_EQ(compact(&node), "null");
    lh_json_node_set_array(&node);
    EXPECT_EQ(compact(&node), "[]");
    lh_json_node_set_object(&node);
    EXPECT_EQ(compact(&node), "{}");
    lh_json_node_deinit(&node);
}

TEST(json_node, set_number_takes_only_a_json_number)
{
    lh_json_node_t node;
    lh_json_node_init(&node);

    EXPECT_TRUE(lh_json_node_set_number(&node, lh_str_view_lit("-3.25e-9")));
    EXPECT_EQ(compact(&node), "-3.25e-9");
    EXPECT_FALSE(lh_json_node_set_number(&node, lh_str_view_lit("01")));
    EXPECT_TRUE(lh_json_node_has_kind(&node, lh_json_kind_none));
    EXPECT_FALSE(lh_json_node_set_number(&node, lh_str_view_lit("1 ")));
    EXPECT_FALSE(lh_json_node_set_number(&node, lh_str_view_lit("NaN")));
    lh_json_node_deinit(&node);
}

TEST(json_node, builds_an_object_member_by_member)
{
    lh_json_node_t root;
    lh_json_node_init(&root);
    lh_json_node_set_object(&root);

    lh_json_node_t *ip = lh_json_node_create();
    lh_json_node_set_str(ip, lh_str_view_lit("8.8.8.8"));
    lh_json_node_add_member(&root, lh_str_view_lit("ip"), ip);

    lh_json_node_t *ports = lh_json_node_create();
    lh_json_node_set_array(ports);
    for (lh_sllong_t p : {27015, 27016})
    {
        lh_json_node_t *port = lh_json_node_create();
        lh_json_node_set_sint(port, p);
        lh_json_node_push_back(ports, port);
    }
    lh_json_node_add_member(&root, lh_str_view_lit("ports"), ports);

    EXPECT_EQ(compact(&root), "{\"ip\":\"8.8.8.8\",\"ports\":[27015,27016]}");
    lh_json_node_deinit(&root);
}

TEST(json_node, insert_detach_remove_and_replace)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("{\"a\":1,\"b\":2,\"c\":3}"));

    lh_json_node_t *b = lh_json_node_get_member_mut(&root, lh_str_view_lit("b"));
    lh_json_node_t *z = lh_json_node_create();
    lh_json_node_set_key(z, lh_str_view_lit("z"));
    lh_json_node_set_null(z);
    lh_json_node_insert_before(b, z);
    EXPECT_EQ(compact(&root), "{\"a\":1,\"z\":null,\"b\":2,\"c\":3}");

    lh_json_node_detach(b);
    EXPECT_EQ(compact(&root), "{\"a\":1,\"z\":null,\"c\":3}");
    lh_json_node_push_back(&root, b);
    EXPECT_EQ(compact(&root), "{\"a\":1,\"z\":null,\"c\":3,\"b\":2}");

    EXPECT_TRUE(lh_json_node_remove_member(&root, lh_str_view_lit("z")));
    EXPECT_FALSE(lh_json_node_remove_member(&root, lh_str_view_lit("z")));
    EXPECT_EQ(compact(&root), "{\"a\":1,\"c\":3,\"b\":2}");

    lh_json_node_t *with = lh_json_node_create();
    lh_json_node_set_str(with, lh_str_view_lit("three"));
    lh_json_node_replace(lh_json_node_get_member_mut(&root, lh_str_view_lit("c")), with);
    EXPECT_EQ(compact(&root), "{\"a\":1,\"c\":\"three\",\"b\":2}");

    /* Destroying a child takes it out of its parent first. */
    lh_json_node_destroy(lh_json_node_get_first_mut(&root));
    EXPECT_EQ(compact(&root), "{\"c\":\"three\",\"b\":2}");
    lh_json_node_deinit(&root);
}

TEST(json_node, assign_is_a_deep_copy_that_keeps_its_own_key)
{
    lh_json_node_t src;
    read(&src, lh_str_view_lit("{\"a\":[1,{\"b\":\"x\"}],\"c\":true}"));
    lh_json_node_t dst;
    lh_json_node_init(&dst);
    lh_json_node_set_key(&dst, lh_str_view_lit("mine"));

    lh_json_node_assign(&dst, &src);
    EXPECT_TRUE(lh_json_node_equals(&dst, &src));
    EXPECT_TRUE(lh_json_node_has_key(&dst, lh_str_view_lit("mine")));

    /* Changing the copy leaves the source alone. */
    lh_json_node_set_null(lh_json_node_get_member_mut(&dst, lh_str_view_lit("c")));
    EXPECT_FALSE(lh_json_node_equals(&dst, &src));
    EXPECT_EQ(compact(&src), "{\"a\":[1,{\"b\":\"x\"}],\"c\":true}");
    lh_json_node_deinit(&dst);
    lh_json_node_deinit(&src);
}

#if LH_REAL_ENABLED
TEST(json_node, reals_come_out_of_their_text_and_go_back_shortest)
{
    lh_json_node_t root;
    read(&root, lh_str_view_lit("[3.25,-1e-3,7,1e400,\"2.5\"]"));
    lh_real_t r = 0;

    EXPECT_TRUE(lh_json_node_get_real(lh_json_node_get_at(&root, 0), &r));
    EXPECT_EQ(r, static_cast<lh_real_t>(3.25));
    EXPECT_EQ(lh_json_node_get_real_or(lh_json_node_get_at(&root, 1), 0),
              static_cast<lh_real_t>(-1e-3));
    EXPECT_EQ(lh_json_node_get_real_or(lh_json_node_get_at(&root, 2), 0), 7);
    EXPECT_FALSE(lh_json_node_get_real(lh_json_node_get_at(&root, 3), &r)); /* too big */
    EXPECT_FALSE(lh_json_node_get_real(lh_json_node_get_at(&root, 4), &r)); /* a string */
    EXPECT_EQ(lh_json_node_get_real_or(nullptr, 9), 9);
    lh_json_node_deinit(&root);

    lh_json_node_t node;
    lh_json_node_init(&node);
    EXPECT_TRUE(lh_json_node_set_real(&node, static_cast<lh_real_t>(0.1)));
    EXPECT_EQ(compact(&node), "0.1");
    EXPECT_TRUE(lh_json_node_set_real(&node, static_cast<lh_real_t>(-2.5)));
    EXPECT_EQ(compact(&node), "-2.5");
    EXPECT_FALSE(lh_json_node_set_real(&node, std::numeric_limits<lh_real_t>::infinity()));
    EXPECT_TRUE(lh_json_node_has_kind(&node, lh_json_kind_none));
    EXPECT_FALSE(lh_json_node_set_real(&node, std::numeric_limits<lh_real_t>::quiet_NaN()));
    lh_json_node_deinit(&node);
}
#endif

TEST(json_node, equals_compares_values)
{
    const auto same = [](const char *a, const char *b)
    {
        lh_json_node_t x;
        lh_json_node_t y;
        lh_str_view_t va;
        lh_str_view_t vb;
        lh_str_init_by_size(&va, a, std::strlen(a));
        lh_str_init_by_size(&vb, b, std::strlen(b));
        read(&x, va);
        read(&y, vb);
        const bool eq = lh_json_node_equals(&x, &y);
        lh_json_node_deinit(&x);
        lh_json_node_deinit(&y);
        return eq;
    };

    EXPECT_TRUE(same("{\"a\":1,\"b\":[true,null]}", "{\"b\":[true,null],\"a\":1}"));
    EXPECT_FALSE(same("[1,2]", "[2,1]"));
    EXPECT_FALSE(same("[1,2]", "[1,2,3]"));
    EXPECT_FALSE(same("{\"a\":1}", "{\"a\":1,\"b\":2}"));
    EXPECT_FALSE(same("{\"a\":1}", "{\"b\":1}"));
    EXPECT_FALSE(same("1", "1.0"));
    EXPECT_FALSE(same("\"1\"", "1"));
    EXPECT_TRUE(same("\"\\u00e9\"", "\"\xC3\xA9\""));
}

} // namespace
