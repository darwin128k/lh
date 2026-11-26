/**
 * @file frame.c
 * @brief Implementation of `lh/gl/web/player/page.h`: the script's frame: its state, the scale,
 * fitting and playing a frame, the size.
 */

#include <lh/gl/web/player/page.h>

lh_web_js_node_t *
lh_gl_web_player_page_create_state(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_var(
        lh_str_view_lit("atlas"), lh_web_js_node_create_with_kind(lh_web_js_kind_array));

    lh_web_js_node_add_declarator(self, lh_str_view_lit("tints"),
                                  lh_web_js_node_create_with_kind(lh_web_js_kind_object));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("ntint"), lh_web_js_node_create_number(0U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("fw"), lh_web_js_node_create_number(980U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("fh"), lh_web_js_node_create_number(620U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("cssS"), lh_web_js_node_create_number(1U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("lastD"),
                                  lh_web_js_node_create_with_kind(lh_web_js_kind_null));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("pend"),
                                  lh_web_js_node_create_with_kind(lh_web_js_kind_null));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("raf"), lh_web_js_node_create_number(0U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("v3on"), lh_web_js_node_create_number(0U));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_scale_mode(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_var(
        lh_str_view_lit("scaleMode"),
        lh_web_js_node_create_conditional(
            lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_create_regex(lh_str_view_lit("scale=fit"), lh_str_view_lit("")),
                    "test")),
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("location"),
                                                 "search")),
            lh_web_js_node_create_string_lit("fit"), lh_web_js_node_create_string_lit("layout")));

    lh_web_js_node_add_declarator(self, lh_str_view_lit("lastZW"),
                                  lh_web_js_node_create_number(0U));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("lastZH"),
                                  lh_web_js_node_create_number(0U));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_fit_layout(lh_void)
{
    return lh_web_js_node_create_if(
        lh_web_js_node_create_binary(lh_web_js_op_strict_eq,
                                     lh_web_js_node_create_ident_lit("scaleMode"),
                                     lh_web_js_node_create_string_lit("layout")),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_push_statement(
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_with_kind(
                                                    lh_web_js_kind_block),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_assign(
                                                        lh_web_js_op_assign,
                                                        lh_web_js_node_create_ident_lit("cssS"),
                                                        lh_web_js_node_create_number(1U)))),
                                            lh_web_js_node_create_expression(
                                                lh_web_js_node_create_assign(
                                                    lh_web_js_op_assign,
                                                    lh_web_js_node_create_ident_lit("bw"),
                                                    lh_web_js_node_push_back(
                                                        lh_web_js_node_push_back(
                                                            lh_web_js_node_create_call(
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "Math"),
                                                                    "max")),
                                                            lh_web_js_node_create_number(1U)),
                                                        lh_web_js_node_push_back(
                                                            lh_web_js_node_create_call(
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "Math"),
                                                                    "round")),
                                                            lh_web_js_node_create_binary(
                                                                lh_web_js_op_mul,
                                                                lh_web_js_node_create_ident_lit(
                                                                    "fw"),
                                                                lh_web_js_node_create_ident_lit(
                                                                    "dpr"))))))),
                                        lh_web_js_node_create_expression(
                                            lh_web_js_node_create_assign(
                                                lh_web_js_op_assign,
                                                lh_web_js_node_create_ident_lit("bh"),
                                                lh_web_js_node_push_back(
                                                    lh_web_js_node_push_back(
                                                        lh_web_js_node_create_call(
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "Math"),
                                                                "max")),
                                                        lh_web_js_node_create_number(1U)),
                                                    lh_web_js_node_push_back(
                                                        lh_web_js_node_create_call(
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "Math"),
                                                                "round")),
                                                        lh_web_js_node_create_binary(
                                                            lh_web_js_op_mul,
                                                            lh_web_js_node_create_ident_lit("fh"),
                                                            lh_web_js_node_create_ident_lit(
                                                                "dpr"))))))),
                                    lh_web_js_node_create_if(
                                        lh_web_js_node_create_binary(
                                            lh_web_js_op_strict_ne,
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("c"), "width"),
                                            lh_web_js_node_create_ident_lit("bw")),
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                            lh_web_js_node_create_expression(
                                                lh_web_js_node_create_assign(
                                                    lh_web_js_op_assign,
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("c"),
                                                        "width"),
                                                    lh_web_js_node_create_ident_lit("bw")))),
                                        lh_null)),
                                lh_web_js_node_create_if(
                                    lh_web_js_node_create_binary(
                                        lh_web_js_op_strict_ne,
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("c"), "height"),
                                        lh_web_js_node_create_ident_lit("bh")),
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                        lh_web_js_node_create_expression(
                                            lh_web_js_node_create_assign(
                                                lh_web_js_op_assign,
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("c"), "height"),
                                                lh_web_js_node_create_ident_lit("bh")))),
                                    lh_null)),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("c"), "style"),
                                    "width"),
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add, lh_web_js_node_create_ident_lit("fw"),
                                    lh_web_js_node_create_string_lit("px"))))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign,
                            lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("c"), "style"),
                                "height"),
                            lh_web_js_node_create_binary(lh_web_js_op_add,
                                                         lh_web_js_node_create_ident_lit("fh"),
                                                         lh_web_js_node_create_string_lit("px"))))),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign,
                        lh_web_js_node_create_member_lit(
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"),
                                                             "style"),
                            "left"),
                        lh_web_js_node_create_string_lit("0")))),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign,
                    lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"),
                                                         "style"),
                        "top"),
                    lh_web_js_node_create_string_lit("0")))),
            lh_web_js_node_create_return(lh_web_js_node_push_back(
                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                    lh_web_js_node_create_ident_lit("c"), "getContext")),
                lh_web_js_node_create_string_lit("2d")))),
        lh_null);
}

lh_web_js_node_t *
lh_gl_web_player_page_create_play_clear(lh_void)
{
    return lh_web_js_node_push_statement(
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_case(lh_web_js_node_create_string_lit("clear")),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign, lh_web_js_node_create_ident_lit("s"),
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("rgba")),
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                             "color"))))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                 lh_web_js_node_create_ident_lit("s"),
                                                 lh_web_js_node_create_ident_lit("fs")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("x"), "fillStyle"),
                                lh_web_js_node_create_ident_lit("s")))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("fs"),
                            lh_web_js_node_create_ident_lit("s")))),
                    lh_null)),
            lh_web_js_node_create_expression(lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("x"), "fillRect")),
                            lh_web_js_node_create_number(0U)),
                        lh_web_js_node_create_number(0U)),
                    lh_web_js_node_create_ident_lit("fw")),
                lh_web_js_node_create_ident_lit("fh")))),
        lh_web_js_node_create_with_kind(lh_web_js_kind_break));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_play_fill_mask(lh_void)
{
    return lh_web_js_node_push_statement(
        lh_web_js_node_create_case(lh_web_js_node_create_string_lit("fill_mask")),
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                    lh_web_js_node_create_var(
                        lh_str_view_lit("g"),
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("glyph")),
                            lh_web_js_node_create_ident_lit("op")))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_ident_lit("g"),
                    lh_web_js_node_create_expression(lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_push_back(
                                lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("x"), "drawImage")),
                                lh_web_js_node_create_ident_lit("g")),
                            lh_web_js_node_create_index(
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "origin"),
                                lh_web_js_node_create_number(0U))),
                        lh_web_js_node_create_index(
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                             "origin"),
                            lh_web_js_node_create_number(1U)))),
                    lh_null)),
            lh_web_js_node_create_with_kind(lh_web_js_kind_break)));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_play_fill_rect(lh_void)
{
    return lh_web_js_node_push_statement(
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_create_case(lh_web_js_node_create_string_lit("fill_rect")),
                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                        lh_web_js_op_assign, lh_web_js_node_create_ident_lit("s"),
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("rgba")),
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                             "color"))))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                 lh_web_js_node_create_ident_lit("s"),
                                                 lh_web_js_node_create_ident_lit("fs")),
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("x"), "fillStyle"),
                                lh_web_js_node_create_ident_lit("s")))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("fs"),
                            lh_web_js_node_create_ident_lit("s")))),
                    lh_null)),
            lh_web_js_node_create_expression(lh_web_js_node_push_back(
                lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("x"), "fillRect")),
                            lh_web_js_node_create_index(
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "rect"),
                                lh_web_js_node_create_number(0U))),
                        lh_web_js_node_create_index(
                            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                             "rect"),
                            lh_web_js_node_create_number(1U))),
                    lh_web_js_node_create_index(lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("op"), "rect"),
                                                lh_web_js_node_create_number(2U))),
                lh_web_js_node_create_index(
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"), "rect"),
                    lh_web_js_node_create_number(3U))))),
        lh_web_js_node_create_with_kind(lh_web_js_kind_break));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_play_fill_round_rect(lh_void)
{
    return lh_web_js_node_push_statement(
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_create_case(
                            lh_web_js_node_create_string_lit("fill_round_rect")),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("s"),
                            lh_web_js_node_push_back(
                                lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("rgba")),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "color"))))),
                    lh_web_js_node_create_if(
                        lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                     lh_web_js_node_create_ident_lit("s"),
                                                     lh_web_js_node_create_ident_lit("fs")),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                    lh_web_js_op_assign,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("x"), "fillStyle"),
                                    lh_web_js_node_create_ident_lit("s")))),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("fs"),
                                lh_web_js_node_create_ident_lit("s")))),
                        lh_null)),
                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("round")),
                            lh_web_js_node_create_ident_lit("x")),
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                         "rect")),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_or,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("op"),
                                                         "radius"),
                        lh_web_js_node_create_number(0U))))),
            lh_web_js_node_create_expression(lh_web_js_node_create_call(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("x"), "fill")))),
        lh_web_js_node_create_with_kind(lh_web_js_kind_break));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_play_shadow(lh_void)
{
    return lh_web_js_node_push_statement(
        lh_web_js_node_push_statement(
            lh_web_js_node_push_statement(
                lh_web_js_node_push_statement(
                    lh_web_js_node_push_statement(
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_push_statement(
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_case(
                                                    lh_web_js_node_create_string_lit("shadow")),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_call(
                                                        lh_web_js_node_create_member_lit(
                                                            lh_web_js_node_create_ident_lit("x"),
                                                            "save")))),
                                            lh_web_js_node_create_expression(
                                                lh_web_js_node_create_assign(
                                                    lh_web_js_op_assign,
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("x"),
                                                        "shadowColor"),
                                                    lh_web_js_node_push_back(
                                                        lh_web_js_node_create_call(
                                                            lh_web_js_node_create_ident_lit(
                                                                "rgba")),
                                                        lh_web_js_node_create_member_lit(
                                                            lh_web_js_node_create_ident_lit("op"),
                                                            "color"))))),
                                        lh_web_js_node_create_expression(
                                            lh_web_js_node_create_assign(
                                                lh_web_js_op_assign,
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("x"),
                                                    "shadowBlur"),
                                                lh_web_js_node_create_binary(
                                                    lh_web_js_op_or,
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("op"),
                                                        "spread"),
                                                    lh_web_js_node_create_number(0U))))),
                                    lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                        lh_web_js_op_assign,
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("x"), "shadowOffsetX"),
                                        lh_web_js_node_create_conditional(
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("op"), "offset"),
                                            lh_web_js_node_create_index(
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("op"),
                                                    "offset"),
                                                lh_web_js_node_create_number(0U)),
                                            lh_web_js_node_create_number(0U))))),
                                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                    lh_web_js_op_assign,
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("x"), "shadowOffsetY"),
                                    lh_web_js_node_create_conditional(
                                        lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("op"), "offset"),
                                        lh_web_js_node_create_index(
                                            lh_web_js_node_create_member_lit(
                                                lh_web_js_node_create_ident_lit("op"), "offset"),
                                            lh_web_js_node_create_number(1U)),
                                        lh_web_js_node_create_number(0U))))),
                            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                lh_web_js_op_assign,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("x"), "fillStyle"),
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(
                                        lh_web_js_node_create_ident_lit("rgba")),
                                    lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("op"), "color"))))),
                        lh_web_js_node_create_expression(lh_web_js_node_push_back(
                            lh_web_js_node_push_back(
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(
                                        lh_web_js_node_create_ident_lit("round")),
                                    lh_web_js_node_create_ident_lit("x")),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "rect")),
                            lh_web_js_node_create_binary(
                                lh_web_js_op_or,
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("op"), "radius"),
                                lh_web_js_node_create_number(0U))))),
                    lh_web_js_node_create_expression(
                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                            lh_web_js_node_create_ident_lit("x"), "fill")))),
                lh_web_js_node_create_expression(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_ident_lit("x"), "restore")))),
            lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                lh_web_js_op_assign, lh_web_js_node_create_ident_lit("fs"),
                lh_web_js_node_create_with_kind(lh_web_js_kind_null)))),
        lh_web_js_node_create_with_kind(lh_web_js_kind_break));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_sized(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("sized"));

    lh_web_js_node_push_param(self, lh_str_view_lit("d"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_binary(
                      lh_web_js_op_or,
                      lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                   lh_web_js_node_create_ident_lit("scaleMode"),
                                                   lh_web_js_node_create_string_lit("layout")),
                      lh_web_js_node_create_unary(lh_web_js_op_not,
                                                  lh_web_js_node_create_ident_lit("lastZW"))),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_return(lh_web_js_node_create_number(1U))),
                  lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(
                lh_web_js_op_and,
                lh_web_js_node_create_binary(
                    lh_web_js_op_and,
                    lh_web_js_node_create_binary(lh_web_js_op_strict_eq,
                                                 lh_web_js_node_create_member_lit(
                                                     lh_web_js_node_create_ident_lit("d"), "width"),
                                                 lh_web_js_node_create_number(980U)),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_strict_eq,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("d"),
                                                         "height"),
                        lh_web_js_node_create_number(620U))),
                lh_web_js_node_create_binary(
                    lh_web_js_op_or,
                    lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                 lh_web_js_node_create_ident_lit("lastZW"),
                                                 lh_web_js_node_create_number(980U)),
                    lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                                 lh_web_js_node_create_ident_lit("lastZH"),
                                                 lh_web_js_node_create_number(620U)))),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_return(lh_web_js_node_create_number(0U))),
            lh_null));
    lh_web_js_node_push_statement(self,
                                  lh_web_js_node_create_return(lh_web_js_node_create_number(1U)));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_show(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("show"));

    lh_web_js_node_push_param(self, lh_str_view_lit("d"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_push_back(
                  lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("ingest")),
                  lh_web_js_node_create_ident_lit("d"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_unary(
                lh_web_js_op_not,
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("sized")),
                    lh_web_js_node_create_ident_lit("d"))),
            lh_web_js_node_push_statement(lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                          lh_web_js_node_create_return(lh_null)),
            lh_null));
    lh_web_js_node_push_statement(self,
                                  lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                      lh_web_js_op_assign, lh_web_js_node_create_ident_lit("pend"),
                                      lh_web_js_node_create_ident_lit("d"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_unary(lh_web_js_op_not, lh_web_js_node_create_ident_lit("raf")),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("raf"),
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("requestAnimationFrame")),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_push_statement(
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_create_function_expression(),
                                        lh_web_js_node_create_expression(
                                            lh_web_js_node_create_assign(
                                                lh_web_js_op_assign,
                                                lh_web_js_node_create_ident_lit("raf"),
                                                lh_web_js_node_create_number(0U)))),
                                    lh_web_js_node_create_var(
                                        lh_str_view_lit("p"),
                                        lh_web_js_node_create_ident_lit("pend"))),
                                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("pend"),
                                    lh_web_js_node_create_with_kind(lh_web_js_kind_null)))),
                            lh_web_js_node_create_if(
                                lh_web_js_node_create_ident_lit("p"),
                                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                                    lh_web_js_node_create_call(
                                        lh_web_js_node_create_ident_lit("play")),
                                    lh_web_js_node_create_ident_lit("p"))),
                                lh_null)))))),
            lh_null));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_pos(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("pos"));

    lh_web_js_node_push_param(self, lh_str_view_lit("e"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_add_declarator(
                  lh_web_js_node_create_var(
                      lh_str_view_lit("c"),
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("document"), "getElementById")),
                          lh_web_js_node_create_string_lit("c"))),
                  lh_str_view_lit("r"),
                  lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                      lh_web_js_node_create_ident_lit("c"), "getBoundingClientRect"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_return(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_object),
                      lh_web_js_node_create_property(
                          lh_str_view_lit("x"),
                          lh_web_js_node_push_back(
                              lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                  lh_web_js_node_create_ident_lit("Math"), "round")),
                              lh_web_js_node_create_binary(
                                  lh_web_js_op_div,
                                  lh_web_js_node_create_binary(
                                      lh_web_js_op_mul,
                                      lh_web_js_node_create_binary(
                                          lh_web_js_op_sub,
                                          lh_web_js_node_create_member_lit(
                                              lh_web_js_node_create_ident_lit("e"), "clientX"),
                                          lh_web_js_node_create_member_lit(
                                              lh_web_js_node_create_ident_lit("r"), "left")),
                                      lh_web_js_node_create_ident_lit("fw")),
                                  lh_web_js_node_create_member_lit(
                                      lh_web_js_node_create_ident_lit("r"), "width"))))),
                  lh_web_js_node_create_property(
                      lh_str_view_lit("y"),
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "round")),
                          lh_web_js_node_create_binary(
                              lh_web_js_op_div,
                              lh_web_js_node_create_binary(
                                  lh_web_js_op_mul,
                                  lh_web_js_node_create_binary(
                                      lh_web_js_op_sub,
                                      lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("e"), "clientY"),
                                      lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("r"), "top")),
                                  lh_web_js_node_create_ident_lit("fh")),
                              lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("r"),
                                                               "height")))))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_view_size(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("viewSize"));

    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_var(lh_str_view_lit("w"),
                                  lh_web_js_node_create_binary(
                                      lh_web_js_op_bit_or,
                                      lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("window"), "innerWidth"),
                                      lh_web_js_node_create_number(0U))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_var(lh_str_view_lit("h"),
                                  lh_web_js_node_create_binary(
                                      lh_web_js_op_bit_or,
                                      lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("window"), "innerHeight"),
                                      lh_web_js_node_create_number(0U))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(lh_web_js_node_create_binary(lh_web_js_op_lt,
                                                              lh_web_js_node_create_ident_lit("w"),
                                                              lh_web_js_node_create_number(1U)),
                                 lh_web_js_node_push_statement(
                                     lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                     lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                         lh_web_js_op_assign, lh_web_js_node_create_ident_lit("w"),
                                         lh_web_js_node_create_number(1U)))),
                                 lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(lh_web_js_node_create_binary(lh_web_js_op_lt,
                                                              lh_web_js_node_create_ident_lit("h"),
                                                              lh_web_js_node_create_number(1U)),
                                 lh_web_js_node_push_statement(
                                     lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                     lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                         lh_web_js_op_assign, lh_web_js_node_create_ident_lit("h"),
                                         lh_web_js_node_create_number(1U)))),
                                 lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_return(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_object),
                      lh_web_js_node_create_property(lh_str_view_lit("w"),
                                                     lh_web_js_node_create_ident_lit("w"))),
                  lh_web_js_node_create_property(lh_str_view_lit("h"),
                                                 lh_web_js_node_create_ident_lit("h")))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_report_size(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_function_declaration(lh_str_view_lit("reportSize"));

    lh_web_js_node_push_param(self, lh_str_view_lit("force"));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                         lh_web_js_node_create_ident_lit("scaleMode"),
                                         lh_web_js_node_create_string_lit("layout")),
            lh_web_js_node_push_statement(lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                          lh_web_js_node_create_return(lh_null)),
            lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_var(
                  lh_str_view_lit("s"),
                  lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("viewSize"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(
                lh_web_js_op_and,
                lh_web_js_node_create_unary(lh_web_js_op_not,
                                            lh_web_js_node_create_ident_lit("force")),
                lh_web_js_node_create_binary(
                    lh_web_js_op_and,
                    lh_web_js_node_create_binary(
                        lh_web_js_op_strict_eq,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("s"), "w"),
                        lh_web_js_node_create_ident_lit("lastZW")),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_strict_eq,
                        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("s"), "h"),
                        lh_web_js_node_create_ident_lit("lastZH")))),
            lh_web_js_node_push_statement(lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                                          lh_web_js_node_create_return(lh_null)),
            lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("lastZW"),
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("s"), "w"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("lastZH"),
                  lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("s"), "h"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_push_back(
            lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("emit")),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_create_binary(
                    lh_web_js_op_add,
                    lh_web_js_node_create_binary(lh_web_js_op_add,
                                                 lh_web_js_node_create_string_lit("z "),
                                                 lh_web_js_node_create_member_lit(
                                                     lh_web_js_node_create_ident_lit("s"), "w")),
                    lh_web_js_node_create_string_lit(" ")),
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("s"), "h")))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_on_resize(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_assign(
        lh_web_js_op_assign,
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("window"), "onresize"),
        lh_web_js_node_push_statement(
            lh_web_js_node_create_function_expression(),
            lh_web_js_node_create_if(
                lh_web_js_node_create_binary(lh_web_js_op_strict_eq,
                                             lh_web_js_node_create_ident_lit("scaleMode"),
                                             lh_web_js_node_create_string_lit("layout")),
                lh_web_js_node_create_expression(
                    lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("reportSize"))),
                lh_web_js_node_create_if(
                    lh_web_js_node_create_ident_lit("lastD"),
                    lh_web_js_node_create_expression(lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("play")),
                        lh_web_js_node_create_ident_lit("lastD"))),
                    lh_null)))));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_fit(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("fit"));

    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_add_declarator(
            lh_web_js_node_add_declarator(
                lh_web_js_node_add_declarator(
                    lh_web_js_node_add_declarator(
                        lh_web_js_node_add_declarator(
                            lh_web_js_node_add_declarator(
                                lh_web_js_node_create_var(
                                    lh_str_view_lit("c"),
                                    lh_web_js_node_push_back(
                                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("document"),
                                            "getElementById")),
                                        lh_web_js_node_create_string_lit("c"))),
                                lh_str_view_lit("vw"),
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("window"), "innerWidth")),
                            lh_str_view_lit("vh"),
                            lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("window"), "innerHeight")),
                        lh_str_view_lit("dpr"),
                        lh_web_js_node_create_binary(
                            lh_web_js_op_or,
                            lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_ident_lit("window"), "devicePixelRatio"),
                            lh_web_js_node_create_number(1U))),
                    lh_str_view_lit("s"), lh_null),
                lh_str_view_lit("bw"), lh_null),
            lh_str_view_lit("bh"), lh_null));
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_fit_layout());
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("s"),
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                              lh_web_js_node_create_ident_lit("Math"), "min")),
                          lh_web_js_node_create_binary(lh_web_js_op_div,
                                                       lh_web_js_node_create_ident_lit("vw"),
                                                       lh_web_js_node_create_ident_lit("fw"))),
                      lh_web_js_node_create_binary(lh_web_js_op_div,
                                                   lh_web_js_node_create_ident_lit("vh"),
                                                   lh_web_js_node_create_ident_lit("fh"))))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(
                lh_web_js_op_or,
                lh_web_js_node_create_unary(
                    lh_web_js_op_not,
                    lh_web_js_node_push_back(
                        lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("isFinite")),
                        lh_web_js_node_create_ident_lit("s"))),
                lh_web_js_node_create_binary(lh_web_js_op_le, lh_web_js_node_create_ident_lit("s"),
                                             lh_web_js_node_create_number(0U))),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign, lh_web_js_node_create_ident_lit("s"),
                    lh_web_js_node_create_number(1U)))),
            lh_null));
    lh_web_js_node_push_statement(self,
                                  lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                      lh_web_js_op_assign, lh_web_js_node_create_ident_lit("cssS"),
                                      lh_web_js_node_create_ident_lit("s"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("bw"),
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
                              lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                           lh_web_js_node_create_ident_lit("fw"),
                                                           lh_web_js_node_create_ident_lit("s")),
                              lh_web_js_node_create_ident_lit("dpr")))))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                  lh_web_js_op_assign, lh_web_js_node_create_ident_lit("bh"),
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
                              lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                           lh_web_js_node_create_ident_lit("fh"),
                                                           lh_web_js_node_create_ident_lit("s")),
                              lh_web_js_node_create_ident_lit("dpr")))))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
            lh_web_js_node_create_binary(
                lh_web_js_op_strict_ne,
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "width"),
                lh_web_js_node_create_ident_lit("bw")),
            lh_web_js_node_push_statement(
                lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                    lh_web_js_op_assign,
                    lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "width"),
                    lh_web_js_node_create_ident_lit("bw")))),
            lh_null));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_binary(lh_web_js_op_strict_ne,
                                               lh_web_js_node_create_member_lit(
                                                   lh_web_js_node_create_ident_lit("c"), "height"),
                                               lh_web_js_node_create_ident_lit("bh")),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                      lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                          lh_web_js_op_assign,
                          lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"),
                                                           "height"),
                          lh_web_js_node_create_ident_lit("bh")))),
                  lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "style"),
                "width"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_ident_lit("Math"), "round")),
                    lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                 lh_web_js_node_create_ident_lit("fw"),
                                                 lh_web_js_node_create_ident_lit("s"))),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "style"),
                "height"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_ident_lit("Math"), "round")),
                    lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                 lh_web_js_node_create_ident_lit("fh"),
                                                 lh_web_js_node_create_ident_lit("s"))),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "style"),
                "left"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_ident_lit("Math"), "round")),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_div,
                        lh_web_js_node_create_binary(
                            lh_web_js_op_sub, lh_web_js_node_create_ident_lit("vw"),
                            lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                         lh_web_js_node_create_ident_lit("fw"),
                                                         lh_web_js_node_create_ident_lit("s"))),
                        lh_web_js_node_create_number(2U))),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign,
            lh_web_js_node_create_member_lit(
                lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "style"),
                "top"),
            lh_web_js_node_create_binary(
                lh_web_js_op_add,
                lh_web_js_node_push_back(
                    lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                        lh_web_js_node_create_ident_lit("Math"), "round")),
                    lh_web_js_node_create_binary(
                        lh_web_js_op_div,
                        lh_web_js_node_create_binary(
                            lh_web_js_op_sub, lh_web_js_node_create_ident_lit("vh"),
                            lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                         lh_web_js_node_create_ident_lit("fh"),
                                                         lh_web_js_node_create_ident_lit("s"))),
                        lh_web_js_node_create_number(2U))),
                lh_web_js_node_create_string_lit("px")))));
    lh_web_js_node_push_statement(self,
                                  lh_web_js_node_create_return(lh_web_js_node_push_back(
                                      lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("c"), "getContext")),
                                      lh_web_js_node_create_string_lit("2d"))));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_play(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("play"));

    lh_web_js_node_push_param(self, lh_str_view_lit("d"));
    lh_web_js_node_push_statement(self,
                                  lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                                      lh_web_js_op_assign, lh_web_js_node_create_ident_lit("lastD"),
                                      lh_web_js_node_create_ident_lit("d"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("fw"),
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("d"), "width"))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
            lh_web_js_op_assign, lh_web_js_node_create_ident_lit("fh"),
            lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("d"), "height"))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_add_declarator(
                  lh_web_js_node_add_declarator(
                      lh_web_js_node_add_declarator(
                          lh_web_js_node_add_declarator(
                              lh_web_js_node_add_declarator(
                                  lh_web_js_node_add_declarator(
                                      lh_web_js_node_create_var(
                                          lh_str_view_lit("x"),
                                          lh_web_js_node_create_call(
                                              lh_web_js_node_create_ident_lit("fit"))),
                                      lh_str_view_lit("ops"),
                                      lh_web_js_node_create_binary(
                                          lh_web_js_op_or,
                                          lh_web_js_node_create_member_lit(
                                              lh_web_js_node_create_ident_lit("d"), "ops"),
                                          lh_web_js_node_create_with_kind(lh_web_js_kind_array))),
                                  lh_str_view_lit("i"), lh_null),
                              lh_str_view_lit("op"), lh_null),
                          lh_str_view_lit("s"), lh_null),
                      lh_str_view_lit("fs"), lh_web_js_node_create_with_kind(lh_web_js_kind_null)),
                  lh_str_view_lit("dpr"),
                  lh_web_js_node_create_binary(
                      lh_web_js_op_or,
                      lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("window"),
                                                       "devicePixelRatio"),
                      lh_web_js_node_create_number(1U))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_push_back(
                  lh_web_js_node_push_back(
                      lh_web_js_node_push_back(
                          lh_web_js_node_push_back(
                              lh_web_js_node_push_back(
                                  lh_web_js_node_push_back(
                                      lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                          lh_web_js_node_create_ident_lit("x"), "setTransform")),
                                      lh_web_js_node_create_binary(
                                          lh_web_js_op_mul, lh_web_js_node_create_ident_lit("cssS"),
                                          lh_web_js_node_create_ident_lit("dpr"))),
                                  lh_web_js_node_create_number(0U)),
                              lh_web_js_node_create_number(0U)),
                          lh_web_js_node_create_binary(lh_web_js_op_mul,
                                                       lh_web_js_node_create_ident_lit("cssS"),
                                                       lh_web_js_node_create_ident_lit("dpr"))),
                      lh_web_js_node_create_number(0U)),
                  lh_web_js_node_create_number(0U))));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_expression(lh_web_js_node_push_back(
                  lh_web_js_node_create_call(lh_web_js_node_create_ident_lit("pane3d")),
                  lh_web_js_node_create_ident_lit("d"))));
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
                lh_web_js_node_push_back(
                    lh_web_js_node_push_back(
                        lh_web_js_node_push_back(
                            lh_web_js_node_push_back(
                                lh_web_js_node_push_back(
                                    lh_web_js_node_create_switch(lh_web_js_node_create_member_lit(
                                        lh_web_js_node_create_ident_lit("op"), "op")),
                                    lh_gl_web_player_page_create_play_fill_mask()),
                                lh_gl_web_player_page_create_play_fill_rect()),
                            lh_gl_web_player_page_create_play_fill_round_rect()),
                        lh_gl_web_player_page_create_play_clear()),
                    lh_gl_web_player_page_create_play_shadow()))));
    return self;
}
