/**
 * @file paint.c
 * @brief Implementation of `lh/gl/web/player/page.h`: the script's drawing: colours, glyphs,
 * covers, rounded boxes, the 3D pane, the canvas.
 */

#include <lh/gl/web/player/page.h>

lh_web_js_node_t *
lh_gl_web_player_page_create_rgba(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("rgba"));

    lh_web_js_node_push_param(self, lh_str_view_lit("c"));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_return(lh_web_js_node_create_binary(
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
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_add, lh_web_js_node_create_string_lit("rgba("),
                                        lh_web_js_node_create_index(
                                            lh_web_js_node_create_ident_lit("c"),
                                            lh_web_js_node_create_number(0U))),
                                    lh_web_js_node_create_string_lit(",")),
                                lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("c"),
                                                            lh_web_js_node_create_number(1U))),
                            lh_web_js_node_create_string_lit(",")),
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("c"),
                                                    lh_web_js_node_create_number(2U))),
                    lh_web_js_node_create_string_lit(",")),
                lh_web_js_node_create_binary(
                    lh_web_js_op_div,
                    lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("c"),
                                                lh_web_js_node_create_number(3U)),
                    lh_web_js_node_create_number(255U))),
            lh_web_js_node_create_string_lit(")"))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_hexv(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("hexv"));

    lh_web_js_node_push_param(self, lh_str_view_lit("c"));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_return(lh_web_js_node_create_conditional(
            lh_web_js_node_create_binary(lh_web_js_op_lt, lh_web_js_node_create_ident_lit("c"),
                                         lh_web_js_node_create_number(58U)),
            lh_web_js_node_create_binary(lh_web_js_op_sub, lh_web_js_node_create_ident_lit("c"),
                                         lh_web_js_node_create_number(48U)),
            lh_web_js_node_create_binary(
                lh_web_js_op_sub,
                lh_web_js_node_create_binary(lh_web_js_op_bit_or,
                                             lh_web_js_node_create_ident_lit("c"),
                                             lh_web_js_node_create_number(32U)),
                lh_web_js_node_create_number(87U)))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_decode_cover(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("decodeCover"));

    lh_web_js_node_push_param(self, lh_str_view_lit("w"));
    lh_web_js_node_push_param(self, lh_str_view_lit("h"));
    lh_web_js_node_push_param(self, lh_str_view_lit("cv"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(
                  lh_str_view_lit("im"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                          lh_web_js_node_create_ident_lit("document"), "createElement")),
                      lh_web_js_node_create_string_lit("canvas"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign,
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("im"), "width"),
                  lh_web_js_node_create_ident_lit("w"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign,
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("im"), "height"),
                  lh_web_js_node_create_ident_lit("h"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(
                lh_web_js_op_or,
                lh_web_js_node_create_binary(lh_web_js_op_le, lh_web_js_node_create_ident_lit("w"),
                                             lh_web_js_node_create_number(0U)),
                lh_web_js_node_create_binary(lh_web_js_op_le, lh_web_js_node_create_ident_lit("h"),
                                             lh_web_js_node_create_number(0U))),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("im"))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_var(
            lh_str_view_lit("g"),
            lh_web_js_node_push_back(lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                         lh_web_js_node_create_ident_lit("im"), "getContext")),
                                     lh_web_js_node_create_string_lit("2d"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(
                  lh_str_view_lit("id"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("g"), "createImageData")),
                          lh_web_js_node_create_ident_lit("w")),
                      lh_web_js_node_create_ident_lit("h"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(
                  lh_str_view_lit("p"),
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("id"), "data")));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(lh_str_view_lit("n"),
                                        lh_web_js_node_create_binary(
                                            lh_web_js_op_mul, lh_web_js_node_create_ident_lit("w"),
                                            lh_web_js_node_create_ident_lit("h"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_add_declarator(
            lh_web_js_node_add_declarator(lh_web_js_node_create_var(lh_str_view_lit("i"), lh_null),
                                          lh_str_view_lit("j"), lh_null),
            lh_str_view_lit("k"), lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_for(
            lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_create_with_kind(lh_web_js_kind_sequence),
                    lh_web_js_node_create_assign(lh_web_js_op_assign,
                                                 lh_web_js_node_create_ident_lit("i"),
                                                 lh_web_js_node_create_number(0U))),
                lh_web_js_node_create_assign(lh_web_js_op_assign,
                                             lh_web_js_node_create_ident_lit("j"),
                                             lh_web_js_node_create_number(0U))),
            lh_web_js_node_create_binary(lh_web_js_op_lt, lh_web_js_node_create_ident_lit("i"),
                                         lh_web_js_node_create_ident_lit("n")),
            lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_create_with_kind(lh_web_js_kind_sequence),
                    lh_web_js_node_create_postfix(lh_web_js_op_increment,
                                                  lh_web_js_node_create_ident_lit("i"))),
                lh_web_js_node_create_assign(lh_web_js_op_add_assign,
                                             lh_web_js_node_create_ident_lit("j"),
                                             lh_web_js_node_create_number(4U))),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("k"),
                            lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                         lh_web_js_node_create_ident_lit("i"),
                                                         lh_web_js_node_create_number(2U))))),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign,
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("p"),
                                                    lh_web_js_node_create_ident_lit("j")),
                        lh_web_js_node_create_assign(
                            lh_web_js_op_assign,
                            lh_web_js_node_create_index(
                                lh_web_js_node_create_ident_lit("p"),
                                lh_web_js_node_create_binary(lh_web_js_op_add,
                                                             lh_web_js_node_create_ident_lit("j"),
                                                             lh_web_js_node_create_number(1U))),
                            lh_web_js_node_create_assign(
                                lh_web_js_op_assign,
                                lh_web_js_node_create_index(
                                    lh_web_js_node_create_ident_lit("p"),
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_add, lh_web_js_node_create_ident_lit("j"),
                                        lh_web_js_node_create_number(2U))),
                                lh_web_js_node_create_number(255U)))))),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign,
                    lh_web_js_node_create_index(
                        lh_web_js_node_create_ident_lit("p"),
                        lh_web_js_node_create_binary(lh_web_js_op_add,
                                                     lh_web_js_node_create_ident_lit("j"),
                                                     lh_web_js_node_create_number(3U))),
                    lh_web_js_node_create_conditional(
                        lh_web_js_node_create_binary(
                            lh_web_js_op_lt,
                            lh_web_js_node_create_binary(lh_web_js_op_add,
                                                         lh_web_js_node_create_ident_lit("k"),
                                                         lh_web_js_node_create_number(1U)),
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("cv"),
                                                             "length")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_bit_or,
                            lh_web_js_node_create_binary(
                                lh_web_js_op_shl,
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(
                                        lh_web_js_node_create_ident_lit("hexv")),
                                    lh_web_js_node_push_back(
                                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("cv"), "charCodeAt")),
                                        lh_web_js_node_create_ident_lit("k"))),
                                lh_web_js_node_create_number(4U)),
                            lh_web_js_node_push_back(
                                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("hexv")),
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("cv"), "charCodeAt")),
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_add, lh_web_js_node_create_ident_lit("k"),
                                        lh_web_js_node_create_number(1U))))),
                        lh_web_js_node_create_number(0U)))))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("g"), "putImageData")),
                          lh_web_js_node_create_ident_lit("id")),
                      lh_web_js_node_create_number(0U)),
                  lh_web_js_node_create_number(0U))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("im")));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_ingest(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("ingest"));

    lh_web_js_node_push_param(self, lh_str_view_lit("d"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_add_declarator(
                  lh_web_js_node_add_declarator(
                      lh_web_js_node_create_var(
                          lh_str_view_lit("ops"),
                          lh_web_js_node_create_binary(
                              lh_web_js_op_or,
                              lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("d"),
                                                               "ops"),
                              lh_web_js_node_create_with_kind(lh_web_js_kind_array))),
                      lh_str_view_lit("i"), lh_null),
                  lh_str_view_lit("op"), lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_for(
            lh_web_js_node_create_assign(lh_web_js_op_assign, lh_web_js_node_create_ident_lit("i"),
                                         lh_web_js_node_create_number(0U)),
            lh_web_js_node_create_binary(
                lh_web_js_op_lt, lh_web_js_node_create_ident_lit("i"),
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("ops"), "length")),
            lh_web_js_node_create_postfix(lh_web_js_op_increment,
                                          lh_web_js_node_create_ident_lit("i")),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign, lh_web_js_node_create_ident_lit("op"),
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("ops"),
                                                    lh_web_js_node_create_ident_lit("i"))))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_binary(
                        lh_web_js_op_and,
                        lh_web_js_node_create_binary(
                            lh_web_js_op_strict_ne,
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                             "cover"),
                            lh_web_js_node_create_ident_lit("undefined")),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_strict_eq,
                            lh_web_js_node_create_unary(
                                lh_web_js_op_typeof,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "m")),
                            lh_web_js_node_create_string_lit("number"))),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign,
                            lh_web_js_node_create_index(
                                lh_web_js_node_create_ident_lit("atlas"),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "m")),
                            lh_web_js_node_push_back(
                                lh_web_js_node_push_back(
                                    lh_web_js_node_push_back(
                                        lh_web_js_node_create_call(
                                            lh_web_js_node_create_ident_lit("decodeCover")),
                                        lh_web_js_node_create_index(
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("op"), "size"),
                                            lh_web_js_node_create_number(0U))),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("op"), "size"),
                                        lh_web_js_node_create_number(1U))),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "cover"))))),
                    lh_null))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_tint(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("tint"));

    lh_web_js_node_push_param(self, lh_str_view_lit("a"));
    lh_web_js_node_push_param(self, lh_str_view_lit("c"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(
                  lh_str_view_lit("t"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                          lh_web_js_node_create_ident_lit("document"), "createElement")),
                      lh_web_js_node_create_string_lit("canvas"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("t"), "width"),
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("a"), "width"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("t"), "height"),
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("a"), "height"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_binary(
                      lh_web_js_op_or,
                      lh_web_js_node_create_unary(
                          lh_web_js_op_not, lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("a"), "width")),
                      lh_web_js_node_create_unary(
                          lh_web_js_op_not, lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("a"), "height"))),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("t"))),
                  lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_var(
            lh_str_view_lit("g"),
            lh_web_js_node_push_back(lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                         lh_web_js_node_create_ident_lit("t"), "getContext")),
                                     lh_web_js_node_create_string_lit("2d"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("g"), "fillStyle"),
            lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("rgba")),
                lh_web_js_node_create_ident_lit("c")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_push_back(
            lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                            lh_web_js_node_create_ident_lit("g"), "fillRect")),
                        lh_web_js_node_create_number(0U)),
                    lh_web_js_node_create_number(0U)),
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("t"), "width")),
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("t"), "height"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign,
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("g"),
                                                   "globalCompositeOperation"),
                  lh_web_js_node_create_string_lit("destination-in"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("g"), "drawImage")),
                          lh_web_js_node_create_ident_lit("a")),
                      lh_web_js_node_create_number(0U)),
                  lh_web_js_node_create_number(0U))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("t")));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_glyph(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("glyph"));

    lh_web_js_node_push_param(self, lh_str_view_lit("op"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(lh_str_view_lit("c"),
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("op"), "color")));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_binary(
                      lh_web_js_op_strict_ne,
                      lh_web_js_node_create_unary(lh_web_js_op_typeof,
                                                  lh_web_js_node_create_member_lit(
                                                      lh_web_js_node_create_ident_lit("op"), "m")),
                      lh_web_js_node_create_string_lit("number")),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_return(lh_web_js_node_push_back(
                          lh_web_js_node_push_back(
                              lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("tint")),
                              lh_web_js_node_push_back(
                                  lh_web_js_node_push_back(
                                      lh_web_js_node_push_back(
                                          lh_web_js_node_create_call(
                                              lh_web_js_node_create_ident_lit("decodeCover")),
                                          lh_web_js_node_create_index(
                                              lh_web_js_node_create_member_lit(
                                                  lh_web_js_node_create_ident_lit("op"), "size"),
                                              lh_web_js_node_create_number(0U))),
                                      lh_web_js_node_create_index(
                                          lh_web_js_node_create_member_lit(
                                              lh_web_js_node_create_ident_lit("op"), "size"),
                                          lh_web_js_node_create_number(1U))),
                                  lh_web_js_node_create_binary(
                                      lh_web_js_op_or,
                                      lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("op"), "cover"),
                                      lh_web_js_node_create_string_lit("")))),
                          lh_web_js_node_create_ident_lit("c")))),
                  lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_add_declarator(
            lh_web_js_node_create_var(
                lh_str_view_lit("k"),
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
                                            lh_web_js_node_create_binary(
                                                lh_web_js_op_add,
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("op"), "m"),
                                                lh_web_js_node_create_string_lit(",")),
                                            lh_web_js_node_create_index(
                                                lh_web_js_node_create_ident_lit("c"),
                                                lh_web_js_node_create_number(0U))),
                                        lh_web_js_node_create_string_lit(",")),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("c"),
                                        lh_web_js_node_create_number(1U))),
                                lh_web_js_node_create_string_lit(",")),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("c"),
                                                        lh_web_js_node_create_number(2U))),
                        lh_web_js_node_create_string_lit(",")),
                    lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("c"),
                                                lh_web_js_node_create_number(3U)))),
            lh_str_view_lit("t"),
            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("tints"),
                                        lh_web_js_node_create_ident_lit("k"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_ident_lit("t"),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("t"))),
                  lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(
                  lh_str_view_lit("a"),
                  lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("atlas"),
                                              lh_web_js_node_create_member_lit(
                                                  lh_web_js_node_create_ident_lit("op"), "m"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_unary(lh_web_js_op_not, lh_web_js_node_create_ident_lit("a")),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_return(lh_web_js_node_create_with_kind(lh_web_js_kind_null))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(lh_web_js_op_ge, lh_web_js_node_create_ident_lit("ntint"),
                                         lh_web_js_node_create_number(8192U)),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign, lh_web_js_node_create_ident_lit("tints"),
                        lh_web_js_node_create_with_kind(lh_web_js_kind_object)))),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("ntint"),
                    lh_web_js_node_create_number(0U)))),
            lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("t"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("tint")),
                          lh_web_js_node_create_ident_lit("a")),
                      lh_web_js_node_create_ident_lit("c")))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign,
                  lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("tints"),
                                              lh_web_js_node_create_ident_lit("k")),
                  lh_web_js_node_create_ident_lit("t"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_prefix(
                  lh_web_js_op_increment, lh_web_js_node_create_ident_lit("ntint"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_return(lh_web_js_node_create_ident_lit("t")));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_round(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("round"));

    lh_web_js_node_push_param(self, lh_str_view_lit("x"));
    lh_web_js_node_push_param(self, lh_str_view_lit("r"));
    lh_web_js_node_push_param(self, lh_str_view_lit("rad"));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_call(
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("x"), "beginPath"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("x"), "roundRect"),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_push_back(
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("x"), "roundRect")),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("r"),
                                        lh_web_js_node_create_number(0U))),
                                lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                            lh_web_js_node_create_number(1U))),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                        lh_web_js_node_create_number(2U))),
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                    lh_web_js_node_create_number(3U))),
                    lh_web_js_node_create_binary(lh_web_js_op_or,
                                                 lh_web_js_node_create_ident_lit("rad"),
                                                 lh_web_js_node_create_number(0U))))),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_push_back(
                                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("x"), "rect")),
                                lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                            lh_web_js_node_create_number(0U))),
                            lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                        lh_web_js_node_create_number(1U))),
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                    lh_web_js_node_create_number(2U))),
                    lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                lh_web_js_node_create_number(3U)))))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_pane3d(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("pane3d"));

    lh_web_js_node_push_param(self, lh_str_view_lit("d"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_add_declarator(
                  lh_web_js_node_add_declarator(
                      lh_web_js_node_create_var(
                          lh_str_view_lit("v"),
                          lh_web_js_node_push_back(
                              lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                  lh_web_js_node_create_ident_lit("document"), "getElementById")),
                              lh_web_js_node_create_string_lit("v3d"))),
                      lh_str_view_lit("r"),
                      lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("d"),
                                                       "viewport3d")),
                  lh_str_view_lit("c"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                          lh_web_js_node_create_ident_lit("document"), "getElementById")),
                      lh_web_js_node_create_string_lit("c"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_unary(lh_web_js_op_not, lh_web_js_node_create_ident_lit("r")),
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                    lh_web_js_node_create_if(
                        lh_web_js_node_create_ident_lit("v3on"),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                    lh_web_js_op_assign,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("v"), "style"),
                                        "display"),
                                    lh_web_js_node_create_string_lit("none")))),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("v3on"),
                                lh_web_js_node_create_number(0U)))),
                        lh_null)),
                lh_web_js_node_create_return(lh_null)),
            lh_null));
    lh_web_js_node_push_statement(self,
                                  lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                      lh_web_js_op_assign, lh_web_js_node_create_ident_lit("v3on"),
                                      lh_web_js_node_create_number(1U))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_add_declarator(
                  lh_web_js_node_create_var(
                      lh_str_view_lit("box"),
                      lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                          lh_web_js_node_create_ident_lit("c"), "getBoundingClientRect"))),
                  lh_str_view_lit("dpr"),
                  lh_web_js_node_create_binary(
                      lh_web_js_op_or,
                      lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("window"),
                                                       "devicePixelRatio"),
                      lh_web_js_node_create_number(1U))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "style"),
                "display"),
            lh_web_js_node_create_string_lit("block"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "style"),
                "left"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_create_binary(
                    lh_web_js_op_add,
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("box"),
                                                     "left"),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_mul,
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                    lh_web_js_node_create_number(0U)),
                        lh_web_js_node_create_ident_lit("cssS"))),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "style"),
                "top"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_create_binary(
                    lh_web_js_op_add,
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("box"), "top"),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_mul,
                        lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                    lh_web_js_node_create_number(1U)),
                        lh_web_js_node_create_ident_lit("cssS"))),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "style"),
                "width"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_create_binary(
                    lh_web_js_op_mul,
                    lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                lh_web_js_node_create_number(2U)),
                    lh_web_js_node_create_ident_lit("cssS")),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "style"),
                "height"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_create_binary(
                    lh_web_js_op_mul,
                    lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                lh_web_js_node_create_number(3U)),
                    lh_web_js_node_create_ident_lit("cssS")),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign,
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "width"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "max")),
                          lh_web_js_node_create_number(1U)),
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "round")),
                          lh_web_js_node_create_binary(
                              lh_web_js_op_mul,
                              lh_web_js_node_create_binary(
                                  lh_web_js_op_mul,
                                  lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                              lh_web_js_node_create_number(2U)),
                                  lh_web_js_node_create_ident_lit("cssS")),
                              lh_web_js_node_create_ident_lit("dpr")))))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign,
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "height"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "max")),
                          lh_web_js_node_create_number(1U)),
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "round")),
                          lh_web_js_node_create_binary(
                              lh_web_js_op_mul,
                              lh_web_js_node_create_binary(
                                  lh_web_js_op_mul,
                                  lh_web_js_node_create_index(lh_web_js_node_create_ident_lit("r"),
                                                              lh_web_js_node_create_number(3U)),
                                  lh_web_js_node_create_ident_lit("cssS")),
                              lh_web_js_node_create_ident_lit("dpr")))))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_unary(
                lh_web_js_op_not,
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "gl")),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign,
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("v"), "gl"),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_or,
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("v"), "getContext")),
                            lh_web_js_node_create_string_lit("webgl")),
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("v"), "getContext")),
                            lh_web_js_node_create_string_lit("experimental-webgl")))))),
            lh_null));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_canvas(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_var(
        lh_str_view_lit("c"),
        lh_web_js_node_push_back(
            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                lh_web_js_node_create_ident_lit("document"), "getElementById")),
            lh_web_js_node_create_string_lit("c")));

    return self;
}
