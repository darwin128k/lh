/**
 * @file net.c
 * @brief Implementation of `lh/gl/web/player/page.h`: the script's connection: fetching, polling,
 * the socket, sending, starting.
 */

#include <lh/gl/web/player/page.h>

lh_web_js_node_t *
lh_gl_web_player_page_create_fetch_state(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_var(lh_str_view_lit("busy"), lh_web_js_node_create_number(0U));

    lh_web_js_node_add_declarator(
        self, lh_str_view_lit("rev"),
        lh_web_js_node_create_unary(lh_web_js_op_minus, lh_web_js_node_create_number(1U)));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_load(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("load"));

    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_ident_lit("busy"),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("busy"))),
                  lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("busy"),
            lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                        lh_web_js_node_push_back(
                                            lh_web_js_node_create_call(
                                                lh_web_js_node_create_ident_lit("fetch")),
                                            lh_web_js_node_create_string_lit("/ui.json")),
                                        "then")),
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_push_param(
                                            lh_web_js_node_create_function_expression(),
                                            lh_str_view_lit("r")),
                                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("r"), "json"))))),
                                "then")),
                            lh_web_js_node_create_ident_lit("show")),
                        "then")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("busy"),
                            lh_web_js_node_create_number(0U))))),
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_function_expression(),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign, lh_web_js_node_create_ident_lit("busy"),
                        lh_web_js_node_create_number(0U))))))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("busy")));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_poll(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("poll"));

    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_push_back(
            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                            lh_web_js_node_create_string_lit("/ui.rev")),
                        "then")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                                  lh_str_view_lit("r")),
                        lh_web_js_node_create_return(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("r"), "text"))))),
                "then")),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                              lh_str_view_lit("t")),
                    lh_web_js_node_create_var(
                        lh_str_view_lit("n"),
                        lh_web_js_node_create_unary(lh_web_js_op_plus,
                                                    lh_web_js_node_create_ident_lit("t")))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                 lh_web_js_node_create_ident_lit("n"),
                                                 lh_web_js_node_create_ident_lit("rev")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                                lh_web_js_node_create_ident_lit("n")))),
                        lh_web_js_node_create_expression(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load")))),
                    lh_null)))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_key(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("k")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add, lh_web_js_node_create_string_lit("/ui/key?k="),
                                lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                            lh_web_js_node_create_number(1U))),
                            lh_web_js_node_create_conditional(
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_gt,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("p"), "length"),
                                    lh_web_js_node_create_number(2U)),
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add, lh_web_js_node_create_string_lit("&p="),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(2U))),
                                lh_web_js_node_create_string_lit("")))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_move(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("m")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add,
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add,
                                    lh_web_js_node_create_string_lit("/ui/move?x="),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(1U))),
                                lh_web_js_node_create_string_lit("&y=")),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(2U)))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_press(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("p")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add,
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add,
                                    lh_web_js_node_create_string_lit("/ui/press?x="),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(1U))),
                                lh_web_js_node_create_string_lit("&y=")),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(2U)))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_release(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("r")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add,
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add,
                                    lh_web_js_node_create_string_lit("/ui/release?x="),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(1U))),
                                lh_web_js_node_create_string_lit("&y=")),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(2U)))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_size(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("z")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add,
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add,
                                    lh_web_js_node_create_string_lit("/ui/size?w="),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(1U))),
                                lh_web_js_node_create_string_lit("&h=")),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(2U)))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_socket(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_and, lh_web_js_node_create_ident_lit("sock"),
            lh_web_js_node_create_binary(lh_web_js_op_strict_eq,
                                         lh_web_js_node_create_member_lit(
                                             lh_web_js_node_create_ident_lit("sock"), "readyState"),
                                         lh_web_js_node_create_number(1U))),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_ident_lit("sock"), "send")),
                    lh_web_js_node_create_ident_lit("line")))),
            lh_web_js_node_create_return(lh_null)),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_text(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("t")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add, lh_web_js_node_create_string_lit("/ui/text?c="),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(1U)))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit_wheel(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(
            lh_web_js_op_strict_eq,
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_string_lit("w")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("fetch")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add,
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add,
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_add,
                                        lh_web_js_node_create_binary(
                                            lh_web_js_op_add,
                                            lh_web_js_node_create_binary(
                                                lh_web_js_op_add,
                                                lh_web_js_node_create_binary(
                                                    lh_web_js_op_add,
                                                    lh_web_js_node_create_string_lit(
                                                        "/ui/wheel?x="),
                                                    lh_web_js_node_create_index(
                                                        lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(1U))),
                                                lh_web_js_node_create_string_lit("&y=")),
                                            lh_web_js_node_create_index(
                                                lh_web_js_node_create_ident_lit("p"),
                                                lh_web_js_node_create_number(2U))),
                                        lh_web_js_node_create_string_lit("&dx=")),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
                                        lh_web_js_node_create_number(3U))),
                                lh_web_js_node_create_string_lit("&dy=")),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                        lh_web_js_node_create_number(4U)))),
                    "then")),
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("rev"),
                            lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                        lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_return(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load"))))))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_socket_state(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_var(
        lh_str_view_lit("sock"), lh_web_js_node_create_with_kind(lh_web_js_kind_null));

    lh_web_js_node_add_declarator(self, lh_str_view_lit("retry"),
                                  lh_web_js_node_create_number(200U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("pollId"),
                                  lh_web_js_node_create_number(0U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("retryId"),
                                  lh_web_js_node_create_number(0U));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_stop_poll(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("stopPoll"));

    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_ident_lit("pollId"),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_push_statement(
                          lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                          lh_web_js_node_create_expression(lh_web_js_node_push_back(
                              lh_web_js_node_create_call(
                                  lh_web_js_node_create_ident_lit("clearInterval")),
                              lh_web_js_node_create_ident_lit("pollId")))),
                      lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                          lh_web_js_op_assign, lh_web_js_node_create_ident_lit("pollId"),
                          lh_web_js_node_create_number(0U)))),
                  lh_null));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_start_poll(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("startPoll"));

    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_unary(lh_web_js_op_not,
                                              lh_web_js_node_create_ident_lit("pollId")),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                          lh_web_js_op_assign, lh_web_js_node_create_ident_lit("pollId"),
                          lh_web_js_node_push_back(
                              lh_web_js_node_push_back(
                                  lh_web_js_node_create_call(
                                      lh_web_js_node_create_ident_lit("setInterval")),
                                  lh_web_js_node_create_ident_lit("poll")),
                              lh_web_js_node_create_number(80U))))),
                  lh_null));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_arm_retry(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("armRetry"));

    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(lh_web_js_node_create_ident_lit("retryId"),
                                       lh_web_js_node_push_statement(
                                           lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                           lh_web_js_node_create_return(lh_null)),
                                       lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("retryId"),
            lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("setTimeout")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_function_expression(),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("retryId"),
                                lh_web_js_node_create_number(0U)))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("openWs"))))),
                lh_web_js_node_create_ident_lit("retry")))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("retry"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "min")),
                          lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                       lh_web_js_node_create_ident_lit("retry"),
                                                       lh_web_js_node_create_number(2U))),
                      lh_web_js_node_create_number(5000U)))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_open_ws(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("openWs"));

    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(
                lh_web_js_op_and, lh_web_js_node_create_ident_lit("sock"),
                lh_web_js_node_create_binary(
                    lh_web_js_op_or,
                    lh_web_js_node_create_binary(
                        lh_web_js_op_strict_eq,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"),
                                                         "readyState"),
                        lh_web_js_node_create_number(0U)),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_strict_eq,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"),
                                                         "readyState"),
                        lh_web_js_node_create_number(1U)))),
            lh_web_js_node_push_statement(lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                          lh_web_js_node_create_return(lh_null)),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_try(
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("sock"),
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_new(lh_web_js_node_create_ident_lit("WebSocket")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_add,
                                lh_web_js_node_create_conditional(
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_strict_eq,
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("location"),
                                            "protocol"),
                                        lh_web_js_node_create_string_lit("https:")),
                                    lh_web_js_node_create_string_lit("wss://"),
                                    lh_web_js_node_create_string_lit("ws://")),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("location"), "host")),
                            lh_web_js_node_create_string_lit("/ws")))))),
            lh_str_view_lit("e"),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("sock"),
                                lh_web_js_node_create_with_kind(lh_web_js_kind_null)))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("startPoll")))),
                    lh_web_js_node_create_expression(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("armRetry")))),
                lh_web_js_node_create_return(lh_null))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"), "onopen"),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("retry"),
                            lh_web_js_node_create_number(200U)))),
                    lh_web_js_node_create_expression(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("stopPoll")))),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("reportSize")),
                    lh_web_js_node_create_number(1U)))))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"), "onmessage"),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                          lh_str_view_lit("ev")),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("show")),
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                            lh_web_js_node_create_ident_lit("JSON"), "parse")),
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("ev"),
                                                         "data"))))))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"), "onclose"),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_function_expression(),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("sock"),
                            lh_web_js_node_create_with_kind(lh_web_js_kind_null)))),
                    lh_web_js_node_create_expression(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("startPoll")))),
                lh_web_js_node_create_expression(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("armRetry")))))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"), "onerror"),
            lh_web_js_node_create_function_expression())));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_start(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(lh_web_js_op_strict_eq,
                                     lh_web_js_node_create_ident_lit("scaleMode"),
                                     lh_web_js_node_create_string_lit("layout")),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
            lh_web_js_node_create_expression(
                lh_web_js_node_create_call(lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_create_function_expression(),
                                lh_web_js_node_create_var(
                                    lh_str_view_lit("s"),
                                    lh_web_js_node_create_call(
                                        lh_web_js_node_create_ident_lit("viewSize")))),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("lastZW"),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("s"), "w")))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("lastZH"),
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("s"),
                                                             "h")))),
                    lh_web_js_node_create_expression(lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_push_back(
                                    lh_web_js_node_push_back(
                                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                            lh_web_js_node_push_back(
                                                lh_web_js_node_create_call(
                                                    lh_web_js_node_create_ident_lit("fetch")),
                                                lh_web_js_node_create_binary(
                                                    lh_web_js_op_add,
                                                    lh_web_js_node_create_binary(
                                                        lh_web_js_op_add,
                                                        lh_web_js_node_create_binary(
                                                            lh_web_js_op_add,
                                                            lh_web_js_node_create_string_lit(
                                                                "/ui/size?w="),
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "s"),
                                                                "w")),
                                                        lh_web_js_node_create_string_lit("&h=")),
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("s"),
                                                        "h"))),
                                            "then")),
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_function_expression(),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_assign(
                                                        lh_web_js_op_assign,
                                                        lh_web_js_node_create_ident_lit("rev"),
                                                        lh_web_js_node_create_unary(
                                                            lh_web_js_op_minus,
                                                            lh_web_js_node_create_number(1U))))),
                                            lh_web_js_node_create_return(lh_web_js_node_create_call(
                                                lh_web_js_node_create_ident_lit("load"))))),
                                    lh_web_js_node_create_function_expression()),
                                "then")),
                            lh_web_js_node_push_statement(
                                lh_web_js_node_create_function_expression(),
                                lh_web_js_node_create_expression(lh_web_js_node_create_call(
                                    lh_web_js_node_create_ident_lit("openWs"))))),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_function_expression(),
                            lh_web_js_node_create_expression(lh_web_js_node_create_call(
                                lh_web_js_node_create_ident_lit("openWs")))))))))),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("load")))),
            lh_web_js_node_create_expression(
                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("openWs")))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_emit(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("emit"));

    lh_web_js_node_push_param(self, lh_str_view_lit("line"));
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_socket());
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_var(
            lh_str_view_lit("p"),
            lh_web_js_node_push_back(lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                         lh_web_js_node_create_ident_lit("line"), "split")),
                                     lh_web_js_node_create_string_lit(" "))));
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_press());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_release());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_wheel());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_move());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_key());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_text());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit_size());
    return self;
}
