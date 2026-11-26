/**
 * @file input.c
 * @brief Implementation of `lh/gl/web/player/page.h`: the script's input: the pointer, the mouse,
 * the wheel, the keys, the focus.
 */

#include <lh/gl/web/player/page.h>

lh_web_js_node_t *
lh_gl_web_player_page_create_pointer_state(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_var(lh_str_view_lit("held"), lh_web_js_node_create_number(0U));

    lh_web_js_node_add_declarator(self, lh_str_view_lit("mv"),
                                  lh_web_js_node_create_with_kind(lh_web_js_kind_null));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("mvRaf"), lh_web_js_node_create_number(0U));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_flush_move(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("flushMove"));

    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_ident_lit("mvRaf"),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_push_statement(
                          lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                          lh_web_js_node_create_expression(lh_web_js_node_push_back(
                              lh_web_js_node_create_call(
                                  lh_web_js_node_create_ident_lit("cancelAnimationFrame")),
                              lh_web_js_node_create_ident_lit("mvRaf")))),
                      lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                          lh_web_js_op_assign, lh_web_js_node_create_ident_lit("mvRaf"),
                          lh_web_js_node_create_number(0U)))),
                  lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_ident_lit("mv"),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_push_statement(
                          lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                          lh_web_js_node_create_expression(lh_web_js_node_push_back(
                              lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
                              lh_web_js_node_create_binary(
                                  lh_web_js_op_add,
                                  lh_web_js_node_create_binary(
                                      lh_web_js_op_add,
                                      lh_web_js_node_create_binary(
                                          lh_web_js_op_add, lh_web_js_node_create_string_lit("m "),
                                          lh_web_js_node_create_member_lit(
                                              lh_web_js_node_create_ident_lit("mv"), "x")),
                                      lh_web_js_node_create_string_lit(" ")),
                                  lh_web_js_node_create_member_lit(
                                      lh_web_js_node_create_ident_lit("mv"), "y"))))),
                      lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                          lh_web_js_op_assign, lh_web_js_node_create_ident_lit("mv"),
                          lh_web_js_node_create_with_kind(lh_web_js_kind_null)))),
                  lh_null));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_mouse_down(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "onmousedown"),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                              lh_str_view_lit("e")),
                    lh_web_js_node_create_var(
                        lh_str_view_lit("p"),
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("pos")),
                            lh_web_js_node_create_ident_lit("e")))),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("held"),
                    lh_web_js_node_create_number(1U)))),
            lh_web_js_node_create_expression(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
                lh_web_js_node_create_binary(
                    lh_web_js_op_add,
                    lh_web_js_node_create_binary(
                        lh_web_js_op_add,
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add, lh_web_js_node_create_string_lit("p "),
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("p"),
                                                             "x")),
                        lh_web_js_node_create_string_lit(" ")),
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("p"),
                                                     "y")))))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_mouse_up(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("window"), "onmouseup"),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                                      lh_str_view_lit("e")),
                            lh_web_js_node_create_if(
                                lh_web_js_node_create_unary(
                                    lh_web_js_op_not, lh_web_js_node_create_ident_lit("held")),
                                lh_web_js_node_create_return(lh_null), lh_null)),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("held"),
                            lh_web_js_node_create_number(0U)))),
                    lh_web_js_node_create_expression(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("flushMove")))),
                lh_web_js_node_create_var(
                    lh_str_view_lit("p"),
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("pos")),
                        lh_web_js_node_create_ident_lit("e")))),
            lh_web_js_node_create_expression(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
                lh_web_js_node_create_binary(
                    lh_web_js_op_add,
                    lh_web_js_node_create_binary(
                        lh_web_js_op_add,
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add, lh_web_js_node_create_string_lit("r "),
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("p"),
                                                             "x")),
                        lh_web_js_node_create_string_lit(" ")),
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("p"),
                                                     "y")))))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_mouse_move(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("window"), "onmousemove"),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                                  lh_str_view_lit("e")),
                        lh_web_js_node_create_if(
                            lh_web_js_node_create_unary(lh_web_js_op_not,
                                                        lh_web_js_node_create_ident_lit("held")),
                            lh_web_js_node_create_return(lh_null), lh_null)),
                    lh_web_js_node_create_if(
                        lh_web_js_node_create_binary(
                            lh_web_js_op_strict_eq,
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("e"),
                                                             "buttons"),
                            lh_web_js_node_create_number(0U)),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("window"), "onmouseup")),
                                    lh_web_js_node_create_ident_lit("e")))),
                            lh_web_js_node_create_return(lh_null)),
                        lh_null)),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("mv"),
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("pos")),
                        lh_web_js_node_create_ident_lit("e"))))),
            lh_web_js_node_create_if(
                lh_web_js_node_create_unary(lh_web_js_op_not,
                                            lh_web_js_node_create_ident_lit("mvRaf")),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("mvRaf"),
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("requestAnimationFrame")),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_create_function_expression(),
                                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("mvRaf"),
                                    lh_web_js_node_create_number(0U)))),
                            lh_web_js_node_create_expression(lh_web_js_node_create_call(
                                lh_web_js_node_create_ident_lit("flushMove"))))))),
                lh_null))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_wheel(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "onwheel"),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_push_param(
                                    lh_web_js_node_create_function_expression(),
                                    lh_str_view_lit("e")),
                                lh_web_js_node_create_expression(
                                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("e"), "preventDefault")))),
                            lh_web_js_node_create_var(
                                lh_str_view_lit("p"),
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(
                                        lh_web_js_node_create_ident_lit("pos")),
                                    lh_web_js_node_create_ident_lit("e")))),
                        lh_web_js_node_create_var(
                            lh_str_view_lit("dy"),
                            lh_web_js_node_create_conditional(
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_strict_eq,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("e"), "deltaY"),
                                    lh_web_js_node_create_number(0U)),
                                lh_web_js_node_create_number(0U),
                                lh_web_js_node_create_conditional(
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_gt,
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("e"), "deltaY"),
                                        lh_web_js_node_create_number(0U)),
                                    lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                                lh_web_js_node_create_number(1U)),
                                    lh_web_js_node_create_number(1U))))),
                    lh_web_js_node_create_var(
                        lh_str_view_lit("dx"),
                        lh_web_js_node_create_conditional(
                            lh_web_js_node_create_binary(
                                lh_web_js_op_strict_eq,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("e"), "deltaX"),
                                lh_web_js_node_create_number(0U)),
                            lh_web_js_node_create_number(0U),
                            lh_web_js_node_create_conditional(
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_gt,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("e"), "deltaX"),
                                    lh_web_js_node_create_number(0U)),
                                lh_web_js_node_create_unary(lh_web_js_op_minus,
                                                            lh_web_js_node_create_number(1U)),
                                lh_web_js_node_create_number(1U))))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_binary(
                        lh_web_js_op_and,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("e"),
                                                         "shiftKey"),
                        lh_web_js_node_create_ident_lit("dy")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("dx"),
                                lh_web_js_node_create_ident_lit("dy")))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("dy"),
                            lh_web_js_node_create_number(0U)))),
                    lh_null)),
            lh_web_js_node_create_expression(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
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
                                            lh_web_js_node_create_string_lit("w "),
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("p"), "x")),
                                        lh_web_js_node_create_string_lit(" ")),
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("p"), "y")),
                                lh_web_js_node_create_string_lit(" ")),
                            lh_web_js_node_create_ident_lit("dx")),
                        lh_web_js_node_create_string_lit(" ")),
                    lh_web_js_node_create_ident_lit("dy")))))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_key_down(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("document"), "onkeydown"),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                                      lh_str_view_lit("e")),
                            lh_web_js_node_add_declarator(
                                lh_web_js_node_create_var(
                                    lh_str_view_lit("k"),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("keys"),
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("e"), "key"))),
                                lh_str_view_lit("m"),
                                lh_web_js_node_create_index(
                                    lh_web_js_node_create_ident_lit("mods"),
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("e"), "key")))),
                        lh_web_js_node_create_var(
                            lh_str_view_lit("t"),
                            lh_web_js_node_create_binary(
                                lh_web_js_op_and,
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_and,
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_and,
                                        lh_web_js_node_create_binary(
                                            lh_web_js_op_and,
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("e"), "key"),
                                            lh_web_js_node_create_binary(
                                                lh_web_js_op_strict_eq,
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("e"),
                                                        "key"),
                                                    "length"),
                                                lh_web_js_node_create_number(1U))),
                                        lh_web_js_node_create_unary(
                                            lh_web_js_op_not,
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("e"), "ctrlKey"))),
                                    lh_web_js_node_create_unary(
                                        lh_web_js_op_not,
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("e"), "altKey"))),
                                lh_web_js_node_create_unary(
                                    lh_web_js_op_not,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("e"), "metaKey"))))),
                    lh_web_js_node_create_if(
                        lh_web_js_node_create_binary(
                            lh_web_js_op_or,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_and, lh_web_js_node_create_ident_lit("k"),
                                lh_web_js_node_create_unary(lh_web_js_op_not,
                                                            lh_web_js_node_create_ident_lit("m"))),
                            lh_web_js_node_create_ident_lit("t")),
                        lh_web_js_node_create_expression(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("e"), "preventDefault"))),
                        lh_null)),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_ident_lit("k"),
                    lh_web_js_node_create_expression(lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_add,
                            lh_web_js_node_create_binary(lh_web_js_op_add,
                                                         lh_web_js_node_create_string_lit("k "),
                                                         lh_web_js_node_create_ident_lit("k")),
                            lh_web_js_node_create_string_lit(" 1")))),
                    lh_null)),
            lh_web_js_node_create_if(
                lh_web_js_node_create_ident_lit("t"),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_add, lh_web_js_node_create_string_lit("t "),
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("e"), "key"),
                                "codePointAt")),
                            lh_web_js_node_create_number(0U))))),
                lh_null))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_key_up(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("document"), "onkeyup"),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_param(lh_web_js_node_create_function_expression(),
                                                  lh_str_view_lit("e")),
                        lh_web_js_node_create_var(
                            lh_str_view_lit("k"),
                            lh_web_js_node_create_index(
                                lh_web_js_node_create_ident_lit("keys"),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("e"), "key")))),
                    lh_web_js_node_create_if(
                        lh_web_js_node_create_unary(lh_web_js_op_not,
                                                    lh_web_js_node_create_ident_lit("k")),
                        lh_web_js_node_create_return(lh_null), lh_null)),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_unary(
                        lh_web_js_op_not, lh_web_js_node_create_index(
                                              lh_web_js_node_create_ident_lit("mods"),
                                              lh_web_js_node_create_member_lit(
                                                  lh_web_js_node_create_ident_lit("e"), "key"))),
                    lh_web_js_node_create_expression(
                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                            lh_web_js_node_create_ident_lit("e"), "preventDefault"))),
                    lh_null)),
            lh_web_js_node_create_expression(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
                lh_web_js_node_create_binary(
                    lh_web_js_op_add,
                    lh_web_js_node_create_binary(lh_web_js_op_add,
                                                 lh_web_js_node_create_string_lit("k "),
                                                 lh_web_js_node_create_ident_lit("k")),
                    lh_web_js_node_create_string_lit(" 0")))))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_focus(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_call(
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "focus")));
}
