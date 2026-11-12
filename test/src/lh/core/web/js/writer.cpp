/**
 * @file writer.cpp
 * @brief ::lh_web_js_writer_t: JavaScript text from a node tree - names checked,
 *        strings escaped, parentheses only where precedence needs them.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/core/web/js/writer.h>

#include <string>

namespace
{

lh_web_js_node_t *
id(const char *name)
{
    lh_str_view_t view;

    lh_str_view_init(&view, name);
    return lh_web_js_node_create_ident(view);
}

lh_web_js_node_t *
num(lh_ullong_t value)
{
    return lh_web_js_node_create_number(value);
}

lh_web_js_node_t *
str(const std::string &value)
{
    lh_str_view_t view;

    lh_str_view_init_by_size(&view, value.data(), value.size());
    return lh_web_js_node_create_string(view);
}

lh_web_js_node_t *
bin(lh_web_js_op_t op, lh_web_js_node_t *left, lh_web_js_node_t *right)
{
    return lh_web_js_node_create_binary(op, left, right);
}

lh_web_js_node_t *
un(lh_web_js_op_t op, lh_web_js_node_t *operand)
{
    return lh_web_js_node_create_unary(op, operand);
}

/* Write @p node as a statement or program, report through @p ok, and free it. */
std::string
write(lh_web_js_node_t *node, bool *ok = nullptr)
{
    lh_str_t out;

    lh_str_init(&out);
    lh_web_js_writer_t writer;
    lh_web_js_writer_init(&writer, &out);
    const bool written = lh_web_js_writer_try_write(&writer, node);
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
    lh_web_js_node_destroy(node);
    return text;
}

/* Write the expression @p node alone (no statement around it) and free it. */
std::string
expr(lh_web_js_node_t *node, bool *ok = nullptr)
{
    lh_str_t out;

    lh_str_init(&out);
    lh_web_js_writer_t writer;
    lh_web_js_writer_init(&writer, &out);
    const bool written =
        lh_web_js_writer_try_write_expression(&writer, node, lh_web_js_precedence_lowest);
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
    lh_web_js_node_destroy(node);
    return text;
}

lh_web_js_node_t *
object(const char *key, lh_web_js_node_t *value)
{
    lh_str_view_t view;

    lh_str_view_init(&view, key);
    return lh_web_js_node_push_back(lh_web_js_node_create_with_kind(lh_web_js_kind_object),
                                    lh_web_js_node_create_property(view, value));
}

} // namespace

/* ── strings and keys (ported from the old string DSL's tests) ───────────── */

TEST(web_js_writer, strings_escape_quotes_backslashes_and_controls)
{
    EXPECT_EQ(expr(str("alpha'bravo\\charlie\n\r\tend")), "'alpha\\'bravo\\\\charlie\\n\\r\\tend'");
    EXPECT_EQ(expr(str("")), "''");
    EXPECT_EQ(expr(str("'\\\n\t\r\xc3\xa9")), "'\\'\\\\\\n\\t\\r\xc3\xa9'");
    EXPECT_EQ(expr(str(std::string("\x01\x1f\0", 3))), "'\\x01\\x1f\\x00'");
}

TEST(web_js_writer, strings_cannot_close_the_script_element)
{
    EXPECT_EQ(expr(str("</script><")), "'\\x3c/script>\\x3c'");
}

TEST(web_js_writer, object_keys_are_bare_names_or_computed_strings)
{
    lh_web_js_node_t *keys = lh_web_js_node_create_with_kind(lh_web_js_kind_object);
    const char *names[] = {
        "x", "null", "$a_9", "__proto__", "1x", "\xc3\xa9", "space and 'quote'", "</script>"};

    for (const char *name : names)
    {
        lh_str_view_t view;

        lh_str_view_init(&view, name);
        lh_web_js_node_push_back(keys, lh_web_js_node_create_property(view, num(7U)));
    }
    EXPECT_EQ(expr(keys), "{x:7,null:7,$a_9:7,['__proto__']:7,['1x']:7,['\xc3\xa9']:7,"
                          "['space and \\'quote\\'']:7,['\\x3c/script>']:7}");
    EXPECT_EQ(expr(lh_web_js_node_create_with_kind(lh_web_js_kind_object)), "{}");
}

/* ── checks ──────────────────────────────────────────────────────────────── */

TEST(web_js_writer, checks_names_numbers_and_regexes)
{
    EXPECT_TRUE(lh_web_js_writer_is_name(lh_str_view_lit("_a$1")));
    EXPECT_FALSE(lh_web_js_writer_is_name(lh_str_view_lit("1a")));
    EXPECT_FALSE(lh_web_js_writer_is_name(lh_str_view_lit("a-b")));
    EXPECT_TRUE(lh_web_js_writer_is_number(lh_str_view_lit("120")));
    EXPECT_FALSE(lh_web_js_writer_is_number(lh_str_view_lit("-1")));
    EXPECT_FALSE(lh_web_js_writer_is_number(lh_str_view_lit("")));
    EXPECT_TRUE(lh_web_js_writer_is_regex(lh_str_view_lit("scale=fit"), lh_str_view_lit("")));
    EXPECT_TRUE(lh_web_js_writer_is_regex(lh_str_view_lit("a\\/b[x]"), lh_str_view_lit("gi")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit(""), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("*a"), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("a/b"), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("[/]"), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("a\\"), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("a<"), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("a\n"), lh_str_view_lit("")));
    EXPECT_FALSE(lh_web_js_writer_is_regex(lh_str_view_lit("a"), lh_str_view_lit("x")));
    EXPECT_TRUE(lh_web_js_writer_needs_escape('<'));
    EXPECT_TRUE(lh_web_js_writer_needs_escape('\x1f'));
    EXPECT_FALSE(lh_web_js_writer_needs_escape('"'));
}

TEST(web_js_writer, rejects_what_it_cannot_write)
{
    bool ok = true;

    expr(id("a b"), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    expr(lh_web_js_node_create_regex(lh_str_view_lit("a/b"), lh_str_view_lit("")), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    expr(lh_web_js_node_create_member_lit(id("a"), "-x"), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    write(lh_web_js_node_create_with_kind(lh_web_js_kind_none), &ok);
    EXPECT_FALSE(ok);
    ok = true;
    write(lh_web_js_node_create_with_kind(lh_web_js_kind_var), &ok);
    EXPECT_FALSE(ok);
}

/* ── expressions ─────────────────────────────────────────────────────────── */

TEST(web_js_writer, parenthesizes_only_by_precedence)
{
    EXPECT_EQ(expr(bin(lh_web_js_op_mul, bin(lh_web_js_op_add, id("a"), id("b")), id("c"))),
              "(a+b)*c");
    EXPECT_EQ(expr(bin(lh_web_js_op_add, id("a"), bin(lh_web_js_op_mul, id("b"), id("c")))),
              "a+b*c");
    EXPECT_EQ(expr(bin(lh_web_js_op_sub, id("a"), bin(lh_web_js_op_sub, id("b"), id("c")))),
              "a-(b-c)");
    EXPECT_EQ(expr(bin(lh_web_js_op_sub, bin(lh_web_js_op_sub, id("a"), id("b")), id("c"))),
              "a-b-c");
    EXPECT_EQ(expr(bin(lh_web_js_op_or, bin(lh_web_js_op_and, id("a"), id("b")), id("c"))),
              "a&&b||c");
    EXPECT_EQ(expr(bin(lh_web_js_op_and, bin(lh_web_js_op_or, id("a"), id("b")), id("c"))),
              "(a||b)&&c");
    EXPECT_EQ(expr(un(lh_web_js_op_not, bin(lh_web_js_op_add, id("a"), id("b")))), "!(a+b)");
    EXPECT_EQ(expr(un(lh_web_js_op_typeof, lh_web_js_node_create_member_lit(id("op"), "m"))),
              "typeof op.m");
    EXPECT_EQ(expr(lh_web_js_node_create_member_lit(bin(lh_web_js_op_add, id("a"), id("b")), "c")),
              "(a+b).c");
    EXPECT_EQ(expr(lh_web_js_node_create_conditional(
                  lh_web_js_node_create_assign(lh_web_js_op_assign, id("a"), id("b")), id("c"),
                  lh_web_js_node_create_conditional(id("d"), id("e"), id("f")))),
              "(a=b)?c:d?e:f");
    EXPECT_EQ(expr(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, id("a"),
                  lh_web_js_node_create_assign(lh_web_js_op_add_assign, id("b"), num(1U)))),
              "a=b+=1");
    EXPECT_EQ(expr(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(
                      lh_web_js_node_create_call(id("f")),
                      lh_web_js_node_push_back(
                          lh_web_js_node_push_back(
                              lh_web_js_node_create_with_kind(lh_web_js_kind_sequence), id("a")),
                          id("b"))),
                  id("c"))),
              "f((a,b),c)");
}

TEST(web_js_writer, keeps_tokens_apart)
{
    EXPECT_EQ(expr(bin(lh_web_js_op_sub, id("a"), un(lh_web_js_op_minus, id("b")))), "a-(-b)");
    EXPECT_EQ(expr(bin(lh_web_js_op_add, id("a"),
                       lh_web_js_node_create_prefix(lh_web_js_op_increment, id("b")))),
              "a+(++b)");
    EXPECT_EQ(expr(un(lh_web_js_op_minus, un(lh_web_js_op_minus, id("b")))), "-(-b)");
    EXPECT_EQ(expr(bin(lh_web_js_op_div, id("a"),
                       lh_web_js_node_create_regex(lh_str_view_lit("x"), lh_str_view_lit("")))),
              "a/(/x/)");
    EXPECT_EQ(expr(bin(lh_web_js_op_lt, id("a"), un(lh_web_js_op_not, id("b")))), "a<(!b)");
    EXPECT_EQ(expr(bin(lh_web_js_op_in, id("a"), id("b"))), "a in b");
    EXPECT_EQ(expr(un(lh_web_js_op_minus, num(1U))), "-1");
    EXPECT_EQ(expr(lh_web_js_node_create_member_lit(num(1U), "x")), "(1).x");
}

TEST(web_js_writer, writes_calls_new_and_function_expressions)
{
    lh_web_js_node_t *call = lh_web_js_node_create_call(lh_web_js_node_push_statement(
        lh_web_js_node_create_function_expression(), lh_web_js_node_create_return(num(1U))));
    EXPECT_EQ(write(lh_web_js_node_create_expression(call)), "(function(){\nreturn 1;\n})();\n");
    EXPECT_EQ(expr(lh_web_js_node_push_back(lh_web_js_node_create_new(id("WebSocket")), str("u"))),
              "new WebSocket('u')");
    EXPECT_EQ(expr(lh_web_js_node_create_new(
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_call(id("f")), "C"))),
              "new (f().C)()");
    EXPECT_EQ(write(lh_web_js_node_create_expression(object("a", num(1U)))), "({a:1});\n");
    EXPECT_EQ(expr(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(lh_web_js_node_create_with_kind(lh_web_js_kind_array),
                                           num(1U)),
                  lh_web_js_node_create_with_kind(lh_web_js_kind_null))),
              "[1,null]");
}

/* ── statements ──────────────────────────────────────────────────────────── */

TEST(web_js_writer, writes_one_statement_per_line)
{
    lh_web_js_node_t *program = lh_web_js_node_create_with_kind(lh_web_js_kind_program);
    lh_web_js_node_t *function = lh_web_js_node_create_function_declaration(lh_str_view_lit("f"));

    lh_web_js_node_push_param(function, lh_str_view_lit("a"));
    lh_web_js_node_push_statement(function, lh_web_js_node_create_return(id("a")));
    lh_web_js_node_push_statement(
        program,
        lh_web_js_node_add_declarator(lh_web_js_node_create_var(lh_str_view_lit("x"), num(0U)),
                                      lh_str_view_lit("y"), lh_null));
    lh_web_js_node_push_statement(program, function);
    lh_web_js_node_push_statement(program, lh_web_js_node_create_with_kind(lh_web_js_kind_empty));
    EXPECT_EQ(write(program), "var x=0,y;\nfunction f(a){\nreturn a;\n}\n;\n");
}

TEST(web_js_writer, writes_if_else_and_guards_a_dangling_else)
{
    EXPECT_EQ(
        write(lh_web_js_node_create_if(
            id("a"), lh_web_js_node_create_expression(lh_web_js_node_create_call(id("f"))),
            lh_web_js_node_create_if(id("b"), lh_web_js_node_create_return(lh_null), lh_null))),
        "if(a)f();\nelse if(b)return;\n");
    EXPECT_EQ(write(lh_web_js_node_create_if(
                  id("a"),
                  lh_web_js_node_create_if(id("b"), lh_web_js_node_create_return(lh_null), lh_null),
                  lh_web_js_node_create_with_kind(lh_web_js_kind_block))),
              "if(a){\nif(b)return;\n}else{\n}\n");
}

TEST(web_js_writer, writes_for_switch_and_try)
{
    lh_web_js_node_t *loop = lh_web_js_node_create_for(
        lh_web_js_node_create_assign(lh_web_js_op_assign, id("i"), num(0U)),
        bin(lh_web_js_op_lt, id("i"), id("n")),
        lh_web_js_node_create_postfix(lh_web_js_op_increment, id("i")),
        lh_web_js_node_create_with_kind(lh_web_js_kind_block));
    EXPECT_EQ(write(loop), "for(i=0;i<n;i++){\n}\n");
    EXPECT_EQ(
        write(lh_web_js_node_create_for(lh_null, lh_null, lh_null,
                                        lh_web_js_node_create_with_kind(lh_web_js_kind_break))),
        "for(;;)break;\n");

    lh_web_js_node_t *choice = lh_web_js_node_create_switch(id("k"));
    lh_web_js_node_push_back(choice, lh_web_js_node_push_statement(
                                         lh_web_js_node_create_case(str("a")),
                                         lh_web_js_node_create_with_kind(lh_web_js_kind_break)));
    lh_web_js_node_push_back(choice, lh_web_js_node_push_statement(
                                         lh_web_js_node_create_with_kind(lh_web_js_kind_default),
                                         lh_web_js_node_create_return(num(0U))));
    EXPECT_EQ(write(choice), "switch(k){\ncase 'a':\nbreak;\ndefault:\nreturn 0;\n}\n");

    EXPECT_EQ(write(lh_web_js_node_create_try(
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_expression(lh_web_js_node_create_call(id("f")))),
                  lh_str_view_lit("e"), lh_web_js_node_create_with_kind(lh_web_js_kind_block))),
              "try{\nf();\n}catch(e){\n}\n");
}

TEST(web_js_writer, needs_braces_only_for_if_and_for)
{
    lh_web_js_node_t *test = lh_web_js_node_create_with_kind(lh_web_js_kind_if);
    lh_web_js_node_t *loop = lh_web_js_node_create_with_kind(lh_web_js_kind_for);
    lh_web_js_node_t *statement = lh_web_js_node_create_return(lh_null);

    EXPECT_TRUE(lh_web_js_writer_needs_braces(test));
    EXPECT_TRUE(lh_web_js_writer_needs_braces(loop));
    EXPECT_FALSE(lh_web_js_writer_needs_braces(statement));
    lh_web_js_node_destroy(statement);
    lh_web_js_node_destroy(loop);
    lh_web_js_node_destroy(test);
}

TEST(web_js_writer, write_checks_the_result)
{
    lh_web_js_node_t *bad = lh_web_js_node_create_expression(id("1a"));
    lh_str_t out;

    lh_str_init(&out);
    lh_web_js_writer_t writer;
    lh_web_js_writer_init(&writer, &out);
    LH_EXPECT_CHECK_DEATH(lh_web_js_writer_write(&writer, bad));
    lh_str_deinit(&out);
    lh_web_js_node_destroy(bad);
}
