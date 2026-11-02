/**
 * @file node.cpp
 * @brief ::lh_web_js_node_t: shapes, owned children, functions and statements.
 */

#include <gtest/gtest.h>

#include <lh/web/js/node.h>

#include <limits>
#include <string>

namespace
{

std::string
view_text(lh_str_view_t view)
{
    return std::string(lh_str_view_get_data(&view) ? lh_str_view_get_data(&view) : "",
                       lh_str_view_get_size(&view));
}

} // namespace

TEST(web_js_node, init_has_no_kind_and_no_children)
{
    lh_web_js_node_t node;

    lh_web_js_node_init(&node);
    EXPECT_TRUE(lh_web_js_node_has_kind(&node, lh_web_js_kind_none));
    EXPECT_EQ(lh_web_js_node_get_op(&node), lh_web_js_op_none);
    EXPECT_FALSE(lh_web_js_node_get_flag(&node));
    EXPECT_EQ(lh_web_js_node_get_first(&node), nullptr);
    EXPECT_EQ(lh_web_js_node_get_size(&node), 0U);
    lh_web_js_node_deinit(&node);
}

TEST(web_js_node, literals_keep_their_value)
{
    lh_web_js_node_t *ident = lh_web_js_node_create_ident_lit("window");
    lh_web_js_node_t *number =
        lh_web_js_node_create_number(std::numeric_limits<lh_ullong_t>::max());
    lh_web_js_node_t *string = lh_web_js_node_create_string_lit("it's");
    lh_web_js_node_t *regex =
        lh_web_js_node_create_regex(lh_str_view_lit("a+"), lh_str_view_lit("g"));
    lh_web_js_node_t *yes = lh_web_js_node_create_bool(lh_bool_true);

    EXPECT_TRUE(lh_web_js_node_has_kind(ident, lh_web_js_kind_ident));
    EXPECT_EQ(view_text(lh_web_js_node_get_name_view(ident)), "window");
    EXPECT_TRUE(lh_web_js_node_has_kind(number, lh_web_js_kind_number));
    EXPECT_EQ(view_text(lh_web_js_node_get_text_view(number)), "18446744073709551615");
    EXPECT_TRUE(lh_web_js_node_has_kind(string, lh_web_js_kind_string));
    EXPECT_EQ(view_text(lh_web_js_node_get_text_view(string)), "it's");
    EXPECT_TRUE(lh_web_js_node_has_kind(regex, lh_web_js_kind_regex));
    EXPECT_EQ(view_text(lh_web_js_node_get_text_view(regex)), "a+");
    EXPECT_EQ(view_text(lh_web_js_node_get_name_view(regex)), "g");
    EXPECT_TRUE(lh_web_js_node_has_kind(yes, lh_web_js_kind_bool));
    EXPECT_TRUE(lh_web_js_node_get_flag(yes));
    lh_web_js_node_destroy(yes);
    lh_web_js_node_destroy(regex);
    lh_web_js_node_destroy(string);
    lh_web_js_node_destroy(number);
    lh_web_js_node_destroy(ident);
}

TEST(web_js_node, operators_own_their_operands)
{
    lh_web_js_node_t *left = lh_web_js_node_create_ident_lit("a");
    lh_web_js_node_t *right = lh_web_js_node_create_number(1U);
    lh_web_js_node_t *sum = lh_web_js_node_create_binary(lh_web_js_op_add, left, right);
    lh_web_js_node_t *postfix =
        lh_web_js_node_create_postfix(lh_web_js_op_increment, lh_web_js_node_create_ident_lit("i"));
    lh_web_js_node_t *prefix =
        lh_web_js_node_create_prefix(lh_web_js_op_decrement, lh_web_js_node_create_ident_lit("i"));

    EXPECT_TRUE(lh_web_js_node_has_kind(sum, lh_web_js_kind_binary));
    EXPECT_EQ(lh_web_js_node_get_op(sum), lh_web_js_op_add);
    EXPECT_EQ(lh_web_js_node_get_first(sum), left);
    EXPECT_EQ(lh_web_js_node_get_next(sum, left), right);
    EXPECT_EQ(lh_web_js_node_get_last(sum), right);
    EXPECT_EQ(lh_web_js_node_get_size(sum), 2U);
    EXPECT_FALSE(lh_web_js_node_get_flag(postfix));
    EXPECT_TRUE(lh_web_js_node_get_flag(prefix));
    lh_web_js_node_destroy(prefix);
    lh_web_js_node_destroy(postfix);
    lh_web_js_node_destroy(sum);
}

TEST(web_js_node, members_and_calls)
{
    lh_web_js_node_t *call = lh_web_js_node_create_call(
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("x"), "fill"));
    lh_web_js_node_t *argument = lh_web_js_node_create_number(0U);

    EXPECT_EQ(lh_web_js_node_push_back(call, argument), call);
    const lh_web_js_node_t *callee = lh_web_js_node_get_first(call);
    ASSERT_NE(callee, nullptr);
    EXPECT_TRUE(lh_web_js_node_has_kind(callee, lh_web_js_kind_member));
    EXPECT_EQ(view_text(lh_web_js_node_get_name_view(callee)), "fill");
    EXPECT_EQ(lh_web_js_node_get_next(call, callee), argument);
    lh_web_js_node_destroy(call);
}

TEST(web_js_node, functions_keep_parameters_before_the_body)
{
    lh_web_js_node_t *function = lh_web_js_node_create_function_declaration(lh_str_view_lit("f"));
    lh_web_js_node_t *statement = lh_web_js_node_create_return(lh_null);

    EXPECT_TRUE(lh_web_js_node_is_function(function));
    EXPECT_EQ(lh_web_js_node_push_param(function, lh_str_view_lit("a")), function);
    lh_web_js_node_push_param(function, lh_str_view_lit("b"));
    EXPECT_EQ(lh_web_js_node_push_statement(function, statement), function);
    ASSERT_EQ(lh_web_js_node_get_size(function), 3U);
    const lh_web_js_node_t *a = lh_web_js_node_get_first(function);
    const lh_web_js_node_t *b = lh_web_js_node_get_next(function, a);
    const lh_web_js_node_t *body = lh_web_js_node_get_last(function);
    EXPECT_EQ(view_text(lh_web_js_node_get_name_view(a)), "a");
    EXPECT_EQ(view_text(lh_web_js_node_get_name_view(b)), "b");
    EXPECT_TRUE(lh_web_js_node_has_kind(body, lh_web_js_kind_block));
    EXPECT_EQ(lh_web_js_node_get_first(body), statement);
    lh_web_js_node_destroy(function);

    lh_web_js_node_t *expression = lh_web_js_node_create_function_expression();
    EXPECT_TRUE(lh_web_js_node_is_function(expression));
    EXPECT_TRUE(lh_web_js_node_has_kind(lh_web_js_node_get_last(expression), lh_web_js_kind_block));
    lh_web_js_node_destroy(expression);
}

TEST(web_js_node, statements_have_their_shape)
{
    lh_web_js_node_t *var =
        lh_web_js_node_create_var(lh_str_view_lit("a"), lh_web_js_node_create_number(1U));
    lh_web_js_node_t *loop = lh_web_js_node_create_for(
        lh_null, lh_null, lh_null, lh_web_js_node_create_with_kind(lh_web_js_kind_block));
    lh_web_js_node_t *attempt = lh_web_js_node_create_try(
        lh_web_js_node_create_with_kind(lh_web_js_kind_block), lh_str_view_lit("e"),
        lh_web_js_node_create_with_kind(lh_web_js_kind_block));

    EXPECT_EQ(lh_web_js_node_add_declarator(var, lh_str_view_lit("b"), lh_null), var);
    EXPECT_EQ(lh_web_js_node_get_size(var), 2U);
    EXPECT_EQ(lh_web_js_node_get_first(lh_web_js_node_get_last(var)), nullptr);
    /* Missing clauses become empty nodes, so a loop always has four children. */
    ASSERT_EQ(lh_web_js_node_get_size(loop), 4U);
    EXPECT_TRUE(lh_web_js_node_has_kind(lh_web_js_node_get_first(loop), lh_web_js_kind_empty));
    const lh_web_js_node_t *handler = lh_web_js_node_get_last(attempt);
    EXPECT_TRUE(lh_web_js_node_has_kind(handler, lh_web_js_kind_catch));
    EXPECT_EQ(view_text(lh_web_js_node_get_name_view(handler)), "e");
    lh_web_js_node_destroy(attempt);
    lh_web_js_node_destroy(loop);
    lh_web_js_node_destroy(var);
}

TEST(web_js_node, clear_drops_children_and_text)
{
    lh_web_js_node_t node;

    lh_web_js_node_init(&node);
    lh_web_js_node_set_string(&node, lh_str_view_lit("x"));
    lh_web_js_node_push_back(&node, lh_web_js_node_create_ident_lit("a"));
    lh_web_js_node_clear(&node);
    EXPECT_TRUE(lh_web_js_node_has_kind(&node, lh_web_js_kind_none));
    EXPECT_EQ(lh_web_js_node_get_size(&node), 0U);
    EXPECT_TRUE(lh_str_is_empty(lh_web_js_node_get_text(&node)));
    lh_web_js_node_deinit(&node);
}
