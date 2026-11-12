/**
 * @file page.c
 * @brief Implementation of `lh/ui/web/player/page.h`.
 */

#include <lh/ui/web/player/page.h>
#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/memory/std.h>
#include <lh/core/web/xml/writer.h>
#include <lh/core/web/js/writer.h>

/** The browser's `KeyboardEvent.key` for every ::lh_os_key_t from ::lh_os_key_tab on, in the
   enum's order; a key added to the enum without a name here stops the build below. */
static const lh_str_cptr m_ui_web_player_page_key_names[] = {
    "Tab",       "Enter",      "Escape",  "Backspace", "Delete", " ",
    "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Home",   "End",
    "PageUp",    "PageDown",   "Shift",   "Control",   "Alt"};

lh_assert_static(lh_array_ptr_get_size(m_ui_web_player_page_key_names) ==
                     lh_cast_static(lh_usize_t, lh_os_key_alt),
                 "m_ui_web_player_page_key_names must name every lh_os_key_t but lh_os_key_other");

/* ── keys ────────────────────────────────────────────────────────────────── */

lh_str_view_t
lh_ui_web_player_page_get_key_name(lh_os_key_t key)
{
    lh_str_view_t name;

    if (lh_math_eq(key, lh_os_key_other) || lh_math_gt(key, lh_os_key_alt))
    {
        lh_str_view_init_empty(lh_addr_of(name));
        return name;
    }
    lh_str_view_init(lh_addr_of(name), m_ui_web_player_page_key_names[lh_math_sub_one(key)]);
    return name;
}

lh_bool_t
lh_ui_web_player_page_is_modifier(lh_os_key_t key)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ge(key, lh_os_key_shift) && !lh_math_gt(key, lh_os_key_alt));
}

/* ── script parts ────────────────────────────────────────────────────────── */

lh_web_js_node_t *
lh_ui_web_player_page_create_rgba(lh_void)
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
lh_ui_web_player_page_create_state(lh_void)
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
lh_ui_web_player_page_create_scale_mode(lh_void)
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
lh_ui_web_player_page_create_hexv(lh_void)
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
lh_ui_web_player_page_create_decode_cover(lh_void)
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
lh_ui_web_player_page_create_ingest(lh_void)
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
lh_ui_web_player_page_create_tint(lh_void)
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
lh_ui_web_player_page_create_glyph(lh_void)
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
lh_ui_web_player_page_create_round(lh_void)
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
lh_ui_web_player_page_create_pane3d(lh_void)
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
lh_ui_web_player_page_create_fit(lh_void)
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
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                                                                lh_web_js_node_create_ident_lit(
                                                                    "fh"),
                                                                lh_web_js_node_create_ident_lit(
                                                                    "dpr"))))))),
                                        lh_web_js_node_create_if(
                                            lh_web_js_node_create_binary(
                                                lh_web_js_op_strict_ne,
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("c"), "width"),
                                                lh_web_js_node_create_ident_lit("bw")),
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_with_kind(
                                                    lh_web_js_kind_block),
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
                                                        lh_web_js_node_create_ident_lit("c"),
                                                        "height"),
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
                                lh_web_js_node_create_binary(
                                    lh_web_js_op_add, lh_web_js_node_create_ident_lit("fh"),
                                    lh_web_js_node_create_string_lit("px"))))),
                        lh_web_js_node_create_expression(lh_web_js_node_create_assign(
                            lh_web_js_op_assign,
                            lh_web_js_node_create_member_lit(
                                lh_web_js_node_create_member_lit(
                                    lh_web_js_node_create_ident_lit("c"), "style"),
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
            lh_null));
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
lh_ui_web_player_page_create_play(lh_void)
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
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_create_case(
                                            lh_web_js_node_create_string_lit("fill_mask")),
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_push_statement(
                                                    lh_web_js_node_create_with_kind(
                                                        lh_web_js_kind_block),
                                                    lh_web_js_node_create_var(
                                                        lh_str_view_lit("g"),
                                                        lh_web_js_node_push_back(
                                                            lh_web_js_node_create_call(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "glyph")),
                                                            lh_web_js_node_create_ident_lit(
                                                                "op")))),
                                                lh_web_js_node_create_if(
                                                    lh_web_js_node_create_ident_lit("g"),
                                                    lh_web_js_node_create_expression(lh_web_js_node_push_back(
                                                        lh_web_js_node_push_back(
                                                            lh_web_js_node_push_back(
                                                                lh_web_js_node_create_call(
                                                                    lh_web_js_node_create_member_lit(
                                                                        lh_web_js_node_create_ident_lit(
                                                                            "x"),
                                                                        "drawImage")),
                                                                lh_web_js_node_create_ident_lit(
                                                                    "g")),
                                                            lh_web_js_node_create_index(
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "op"),
                                                                    "origin"),
                                                                lh_web_js_node_create_number(0U))),
                                                        lh_web_js_node_create_index(
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "op"),
                                                                "origin"),
                                                            lh_web_js_node_create_number(1U)))),
                                                    lh_null)),
                                            lh_web_js_node_create_with_kind(
                                                lh_web_js_kind_break)))),
                                lh_web_js_node_push_statement(
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_case(
                                                    lh_web_js_node_create_string_lit("fill_rect")),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_assign(
                                                        lh_web_js_op_assign,
                                                        lh_web_js_node_create_ident_lit("s"),
                                                        lh_web_js_node_push_back(
                                                            lh_web_js_node_create_call(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "rgba")),
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "op"),
                                                                "color"))))),
                                            lh_web_js_node_create_if(
                                                lh_web_js_node_create_binary(
                                                    lh_web_js_op_strict_ne,
                                                    lh_web_js_node_create_ident_lit("s"),
                                                    lh_web_js_node_create_ident_lit("fs")),
                                                lh_web_js_node_push_statement(
                                                    lh_web_js_node_push_statement(
                                                        lh_web_js_node_create_with_kind(
                                                            lh_web_js_kind_block),
                                                        lh_web_js_node_create_expression(
                                                            lh_web_js_node_create_assign(
                                                                lh_web_js_op_assign,
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "x"),
                                                                    "fillStyle"),
                                                                lh_web_js_node_create_ident_lit(
                                                                    "s")))),
                                                    lh_web_js_node_create_expression(
                                                        lh_web_js_node_create_assign(
                                                            lh_web_js_op_assign,
                                                            lh_web_js_node_create_ident_lit("fs"),
                                                            lh_web_js_node_create_ident_lit("s")))),
                                                lh_null)),
                                        lh_web_js_node_create_expression(lh_web_js_node_push_back(
                                            lh_web_js_node_push_back(
                                                lh_web_js_node_push_back(
                                                    lh_web_js_node_push_back(
                                                        lh_web_js_node_create_call(
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "x"),
                                                                "fillRect")),
                                                        lh_web_js_node_create_index(
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "op"),
                                                                "rect"),
                                                            lh_web_js_node_create_number(0U))),
                                                    lh_web_js_node_create_index(
                                                        lh_web_js_node_create_member_lit(
                                                            lh_web_js_node_create_ident_lit("op"),
                                                            "rect"),
                                                        lh_web_js_node_create_number(1U))),
                                                lh_web_js_node_create_index(
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("op"),
                                                        "rect"),
                                                    lh_web_js_node_create_number(2U))),
                                            lh_web_js_node_create_index(
                                                lh_web_js_node_create_member_lit(
                                                    lh_web_js_node_create_ident_lit("op"), "rect"),
                                                lh_web_js_node_create_number(3U))))),
                                    lh_web_js_node_create_with_kind(lh_web_js_kind_break))),
                            lh_web_js_node_push_statement(
                                lh_web_js_node_push_statement(
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_case(
                                                    lh_web_js_node_create_string_lit(
                                                        "fill_round_rect")),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_assign(
                                                        lh_web_js_op_assign,
                                                        lh_web_js_node_create_ident_lit("s"),
                                                        lh_web_js_node_push_back(
                                                            lh_web_js_node_create_call(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "rgba")),
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "op"),
                                                                "color"))))),
                                            lh_web_js_node_create_if(
                                                lh_web_js_node_create_binary(
                                                    lh_web_js_op_strict_ne,
                                                    lh_web_js_node_create_ident_lit("s"),
                                                    lh_web_js_node_create_ident_lit("fs")),
                                                lh_web_js_node_push_statement(
                                                    lh_web_js_node_push_statement(
                                                        lh_web_js_node_create_with_kind(
                                                            lh_web_js_kind_block),
                                                        lh_web_js_node_create_expression(
                                                            lh_web_js_node_create_assign(
                                                                lh_web_js_op_assign,
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "x"),
                                                                    "fillStyle"),
                                                                lh_web_js_node_create_ident_lit(
                                                                    "s")))),
                                                    lh_web_js_node_create_expression(
                                                        lh_web_js_node_create_assign(
                                                            lh_web_js_op_assign,
                                                            lh_web_js_node_create_ident_lit("fs"),
                                                            lh_web_js_node_create_ident_lit("s")))),
                                                lh_null)),
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
                                                    lh_web_js_node_create_ident_lit("op"),
                                                    "radius"),
                                                lh_web_js_node_create_number(0U))))),
                                    lh_web_js_node_create_expression(
                                        lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                            lh_web_js_node_create_ident_lit("x"), "fill")))),
                                lh_web_js_node_create_with_kind(lh_web_js_kind_break))),
                        lh_web_js_node_push_statement(
                            lh_web_js_node_push_statement(
                                lh_web_js_node_push_statement(
                                    lh_web_js_node_push_statement(
                                        lh_web_js_node_create_case(
                                            lh_web_js_node_create_string_lit("clear")),
                                        lh_web_js_node_create_expression(
                                            lh_web_js_node_create_assign(
                                                lh_web_js_op_assign,
                                                lh_web_js_node_create_ident_lit("s"),
                                                lh_web_js_node_push_back(
                                                    lh_web_js_node_create_call(
                                                        lh_web_js_node_create_ident_lit("rgba")),
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("op"),
                                                        "color"))))),
                                    lh_web_js_node_create_if(
                                        lh_web_js_node_create_binary(
                                            lh_web_js_op_strict_ne,
                                            lh_web_js_node_create_ident_lit("s"),
                                            lh_web_js_node_create_ident_lit("fs")),
                                        lh_web_js_node_push_statement(
                                            lh_web_js_node_push_statement(
                                                lh_web_js_node_create_with_kind(
                                                    lh_web_js_kind_block),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_assign(
                                                        lh_web_js_op_assign,
                                                        lh_web_js_node_create_member_lit(
                                                            lh_web_js_node_create_ident_lit("x"),
                                                            "fillStyle"),
                                                        lh_web_js_node_create_ident_lit("s")))),
                                            lh_web_js_node_create_expression(
                                                lh_web_js_node_create_assign(
                                                    lh_web_js_op_assign,
                                                    lh_web_js_node_create_ident_lit("fs"),
                                                    lh_web_js_node_create_ident_lit("s")))),
                                        lh_null)),
                                lh_web_js_node_create_expression(lh_web_js_node_push_back(
                                    lh_web_js_node_push_back(
                                        lh_web_js_node_push_back(
                                            lh_web_js_node_push_back(
                                                lh_web_js_node_create_call(
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("x"),
                                                        "fillRect")),
                                                lh_web_js_node_create_number(0U)),
                                            lh_web_js_node_create_number(0U)),
                                        lh_web_js_node_create_ident_lit("fw")),
                                    lh_web_js_node_create_ident_lit("fh")))),
                            lh_web_js_node_create_with_kind(lh_web_js_kind_break))),
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
                                                            lh_web_js_node_push_statement(
                                                                lh_web_js_node_create_case(
                                                                    lh_web_js_node_create_string_lit(
                                                                        "shadow")),
                                                                lh_web_js_node_create_expression(
                                                                    lh_web_js_node_create_call(
                                                                        lh_web_js_node_create_member_lit(
                                                                            lh_web_js_node_create_ident_lit(
                                                                                "x"),
                                                                            "save")))),
                                                            lh_web_js_node_create_expression(
                                                                lh_web_js_node_create_assign(
                                                                    lh_web_js_op_assign,
                                                                    lh_web_js_node_create_member_lit(
                                                                        lh_web_js_node_create_ident_lit(
                                                                            "x"),
                                                                        "shadowColor"),
                                                                    lh_web_js_node_push_back(
                                                                        lh_web_js_node_create_call(
                                                                            lh_web_js_node_create_ident_lit(
                                                                                "rgba")),
                                                                        lh_web_js_node_create_member_lit(
                                                                            lh_web_js_node_create_ident_lit(
                                                                                "op"),
                                                                            "color"))))),
                                                        lh_web_js_node_create_expression(
                                                            lh_web_js_node_create_assign(
                                                                lh_web_js_op_assign,
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "x"),
                                                                    "shadowBlur"),
                                                                lh_web_js_node_create_binary(
                                                                    lh_web_js_op_or,
                                                                    lh_web_js_node_create_member_lit(
                                                                        lh_web_js_node_create_ident_lit(
                                                                            "op"),
                                                                        "spread"),
                                                                    lh_web_js_node_create_number(
                                                                        0U))))),
                                                    lh_web_js_node_create_expression(
                                                        lh_web_js_node_create_assign(
                                                            lh_web_js_op_assign,
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "x"),
                                                                "shadowOffsetX"),
                                                            lh_web_js_node_create_conditional(
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "op"),
                                                                    "offset"),
                                                                lh_web_js_node_create_index(
                                                                    lh_web_js_node_create_member_lit(
                                                                        lh_web_js_node_create_ident_lit(
                                                                            "op"),
                                                                        "offset"),
                                                                    lh_web_js_node_create_number(
                                                                        0U)),
                                                                lh_web_js_node_create_number(
                                                                    0U))))),
                                                lh_web_js_node_create_expression(
                                                    lh_web_js_node_create_assign(
                                                        lh_web_js_op_assign,
                                                        lh_web_js_node_create_member_lit(
                                                            lh_web_js_node_create_ident_lit("x"),
                                                            "shadowOffsetY"),
                                                        lh_web_js_node_create_conditional(
                                                            lh_web_js_node_create_member_lit(
                                                                lh_web_js_node_create_ident_lit(
                                                                    "op"),
                                                                "offset"),
                                                            lh_web_js_node_create_index(
                                                                lh_web_js_node_create_member_lit(
                                                                    lh_web_js_node_create_ident_lit(
                                                                        "op"),
                                                                    "offset"),
                                                                lh_web_js_node_create_number(1U)),
                                                            lh_web_js_node_create_number(0U))))),
                                            lh_web_js_node_create_expression(
                                                lh_web_js_node_create_assign(
                                                    lh_web_js_op_assign,
                                                    lh_web_js_node_create_member_lit(
                                                        lh_web_js_node_create_ident_lit("x"),
                                                        "fillStyle"),
                                                    lh_web_js_node_push_back(
                                                        lh_web_js_node_create_call(
                                                            lh_web_js_node_create_ident_lit(
                                                                "rgba")),
                                                        lh_web_js_node_create_member_lit(
                                                            lh_web_js_node_create_ident_lit("op"),
                                                            "color"))))),
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
                                                    lh_web_js_node_create_ident_lit("op"),
                                                    "radius"),
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
                        lh_web_js_node_create_with_kind(lh_web_js_kind_break))))));
    return self;
}

lh_web_js_node_t *
lh_ui_web_player_page_create_sized(lh_void)
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
lh_ui_web_player_page_create_show(lh_void)
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
lh_ui_web_player_page_create_pos(lh_void)
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
lh_ui_web_player_page_create_view_size(lh_void)
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
lh_ui_web_player_page_create_report_size(lh_void)
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
lh_ui_web_player_page_create_on_resize(lh_void)
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
lh_ui_web_player_page_create_fetch_state(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_var(lh_str_view_lit("busy"), lh_web_js_node_create_number(0U));

    lh_web_js_node_add_declarator(
        self, lh_str_view_lit("rev"),
        lh_web_js_node_create_unary(lh_web_js_op_minus, lh_web_js_node_create_number(1U)));
    return self;
}

lh_web_js_node_t *
lh_ui_web_player_page_create_load(lh_void)
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
lh_ui_web_player_page_create_poll(lh_void)
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
lh_ui_web_player_page_create_emit(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_function_declaration(lh_str_view_lit("emit"));

    lh_web_js_node_push_param(self, lh_str_view_lit("line"));
    lh_web_js_node_push_statement(
        self, lh_web_js_node_create_if(
                  lh_web_js_node_create_binary(
                      lh_web_js_op_and, lh_web_js_node_create_ident_lit("sock"),
                      lh_web_js_node_create_binary(
                          lh_web_js_op_strict_eq,
                          lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("sock"),
                                                           "readyState"),
                          lh_web_js_node_create_number(1U))),
                  lh_web_js_node_push_statement(
                      lh_web_js_node_push_statement(
                          lh_web_js_node_create_with_kind(lh_web_js_kind_block),
                          lh_web_js_node_create_expression(lh_web_js_node_push_back(
                              lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                  lh_web_js_node_create_ident_lit("sock"), "send")),
                              lh_web_js_node_create_ident_lit("line")))),
                      lh_web_js_node_create_return(lh_null)),
                  lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_var(
            lh_str_view_lit("p"),
            lh_web_js_node_push_back(lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                                         lh_web_js_node_create_ident_lit("line"), "split")),
                                     lh_web_js_node_create_string_lit(" "))));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                                    lh_web_js_op_add,
                                    lh_web_js_node_create_string_lit("/ui/key?k="),
                                    lh_web_js_node_create_index(
                                        lh_web_js_node_create_ident_lit("p"),
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    lh_web_js_node_push_statement(
        self,
        lh_web_js_node_create_if(
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
                        lh_web_js_node_create_return(lh_web_js_node_create_call(
                            lh_web_js_node_create_ident_lit("load"))))))),
            lh_null));
    return self;
}

lh_web_js_node_t *
lh_ui_web_player_page_create_canvas(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_var(
        lh_str_view_lit("c"),
        lh_web_js_node_push_back(
            lh_web_js_node_create_call(lh_web_js_node_create_member_lit(
                lh_web_js_node_create_ident_lit("document"), "getElementById")),
            lh_web_js_node_create_string_lit("c")));

    return self;
}

lh_web_js_node_t *
lh_ui_web_player_page_create_pointer_state(lh_void)
{
    lh_web_js_node_t *self =
        lh_web_js_node_create_var(lh_str_view_lit("held"), lh_web_js_node_create_number(0U));

    lh_web_js_node_add_declarator(self, lh_str_view_lit("mv"),
                                  lh_web_js_node_create_with_kind(lh_web_js_kind_null));
    lh_web_js_node_add_declarator(self, lh_str_view_lit("mvRaf"), lh_web_js_node_create_number(0U));
    return self;
}

lh_web_js_node_t *
lh_ui_web_player_page_create_flush_move(lh_void)
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
lh_ui_web_player_page_create_on_mouse_down(lh_void)
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
lh_ui_web_player_page_create_on_mouse_up(lh_void)
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
lh_ui_web_player_page_create_on_mouse_move(lh_void)
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
lh_ui_web_player_page_create_on_wheel(lh_void)
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
lh_ui_web_player_page_create_on_key_down(lh_void)
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
lh_ui_web_player_page_create_on_key_up(lh_void)
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
lh_ui_web_player_page_create_focus(lh_void)
{
    return lh_web_js_node_create_expression(lh_web_js_node_create_call(
        lh_web_js_node_create_member_lit(lh_web_js_node_create_ident_lit("c"), "focus")));
}

lh_web_js_node_t *
lh_ui_web_player_page_create_socket_state(lh_void)
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
lh_ui_web_player_page_create_stop_poll(lh_void)
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
lh_ui_web_player_page_create_start_poll(lh_void)
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
lh_ui_web_player_page_create_arm_retry(lh_void)
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
lh_ui_web_player_page_create_open_ws(lh_void)
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
lh_ui_web_player_page_create_start(lh_void)
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

/* ── document ────────────────────────────────────────────────────────────── */

lh_web_xml_node_t *
lh_ui_web_player_page_create_head(lh_void)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create_element(lh_str_view_lit("head"));

    lh_web_xml_node_push_child(
        self, lh_web_xml_node_add_attr(lh_web_xml_node_create_empty(lh_str_view_lit("meta")),
                                        lh_str_view_lit("charset"), lh_str_view_lit("utf-8")));
    lh_web_xml_node_push_child(
        self,
        lh_web_xml_node_add_attr(
            lh_web_xml_node_add_attr(lh_web_xml_node_create_empty(lh_str_view_lit("meta")),
                                      lh_str_view_lit("name"), lh_str_view_lit("viewport")),
            lh_str_view_lit("content"), lh_str_view_lit("width=device-width,initial-scale=1")));
    lh_web_xml_node_push_child(
        self, lh_web_xml_node_push_child(lh_web_xml_node_create_element(lh_str_view_lit("title")),
                                         lh_web_xml_node_create_text(lh_str_view_lit("lh_ui"))));
    lh_web_xml_node_push_child(
        self, lh_web_xml_node_push_child(
                  lh_web_xml_node_create_element(lh_str_view_lit("style")),
                  lh_web_xml_node_create_raw(lh_str_view_lit(
                      "html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
                      "#c{display:block;position:absolute;cursor:pointer;outline:none}"
                      "#v3d{position:absolute;display:none;pointer-events:none}"))));
    return self;
}

lh_web_xml_node_t *
lh_ui_web_player_page_create_body(lh_str_view_t script)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create_element(lh_str_view_lit("body"));
    lh_web_xml_node_t *canvas = lh_web_xml_node_create_element(lh_str_view_lit("canvas"));

    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("id"), lh_str_view_lit("c"));
    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("width"), lh_str_view_lit("980"));
    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("height"), lh_str_view_lit("620"));
    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("tabindex"), lh_str_view_lit("0"));
    lh_web_xml_node_push_child(self, canvas);
    lh_web_xml_node_push_child(
        self, lh_web_xml_node_add_attr(lh_web_xml_node_create_element(lh_str_view_lit("canvas")),
                                        lh_str_view_lit("id"), lh_str_view_lit("v3d")));
    /* The script is a raw child: ::lh_web_js_writer_try_write escapes every `<` a string holds
       and keeps `</` and `<!` out of code, so the text cannot close the element early. */
    lh_web_xml_node_push_child(
        self,
        lh_web_xml_node_push_child(
            lh_web_xml_node_push_child(lh_web_xml_node_create_element(lh_str_view_lit("script")),
                                       lh_web_xml_node_create_text(lh_str_view_lit("\n"))),
            lh_web_xml_node_create_raw(script)));
    return self;
}

lh_web_js_node_t *
lh_ui_web_player_page_create_key_table(lh_bool_t modifiers_only)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_object);

    for (lh_os_key_t key = lh_os_key_tab; !lh_math_gt(key, lh_os_key_alt);
         key = lh_cast_static(lh_os_key_t, lh_math_add_one(key)))
    {
        if (modifiers_only && !lh_ui_web_player_page_is_modifier(key))
        {
            continue;
        }
        lh_web_js_node_push_back(
            self, lh_web_js_node_create_property(lh_ui_web_player_page_get_key_name(key),
                                                 lh_web_js_node_create_number(key)));
    }
    return self;
}

lh_web_xml_node_t *
lh_ui_web_player_page_create_html(lh_str_view_t script)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create_element(lh_str_view_lit("html"));

    lh_web_xml_node_add_attr(self, lh_str_view_lit("lang"), lh_str_view_lit("ru"));
    lh_web_xml_node_push_child(self, lh_ui_web_player_page_create_head());
    return lh_web_xml_node_push_child(self, lh_ui_web_player_page_create_body(script));
}

lh_web_js_node_t *
lh_ui_web_player_page_create_keys(lh_void)
{
    return lh_web_js_node_create_var(lh_str_view_lit("keys"),
                                     lh_ui_web_player_page_create_key_table(lh_bool_false));
}

lh_web_js_node_t *
lh_ui_web_player_page_create_mods(lh_void)
{
    return lh_web_js_node_create_var(lh_str_view_lit("mods"),
                                     lh_ui_web_player_page_create_key_table(lh_bool_true));
}

lh_bool_t
lh_ui_web_player_page_try_write_document(lh_str_t *out, lh_str_view_t script)
{
    lh_web_xml_node_t *document = lh_web_xml_node_create_document();

    lh_web_xml_node_push_child(document, lh_web_xml_node_create_doctype(lh_str_view_lit("html")));
    lh_web_xml_node_push_child(document, lh_ui_web_player_page_create_html(script));
    lh_web_xml_writer_t writer;
    lh_web_xml_writer_init(lh_addr_of(writer), out);
    lh_web_xml_writer_set_dialect(lh_addr_of(writer), lh_web_xml_dialect_html);

    const lh_bool_t written = lh_web_xml_writer_try_write(lh_addr_of(writer), document);

    lh_web_xml_node_destroy(document);
    return written;
}

/* ── script ──────────────────────────────────────────────────────────────── */

lh_web_js_node_t *
lh_ui_web_player_page_create_script(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_program);

    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_rgba());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_state());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_scale_mode());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_hexv());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_decode_cover());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_ingest());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_tint());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_glyph());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_round());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_pane3d());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_fit());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_play());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_sized());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_show());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_pos());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_view_size());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_report_size());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_resize());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_fetch_state());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_load());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_poll());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_emit());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_canvas());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_pointer_state());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_flush_move());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_mouse_down());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_mouse_up());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_mouse_move());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_wheel());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_keys());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_mods());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_key_down());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_on_key_up());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_focus());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_socket_state());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_stop_poll());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_start_poll());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_arm_retry());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_open_ws());
    lh_web_js_node_push_statement(self, lh_ui_web_player_page_create_start());
    return self;
}

lh_bool_t
lh_ui_web_player_page_try_write_script(lh_str_t *out)
{
    lh_web_js_node_t *script = lh_ui_web_player_page_create_script();

    lh_web_js_writer_t writer;
    lh_web_js_writer_init(lh_addr_of(writer), out);
    lh_bool_t written = lh_web_js_writer_try_write(lh_addr_of(writer), script);
    lh_web_js_node_destroy(script);
    return written;
}

/* ── page ────────────────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_web_player_page_try_write(lh_str_t *out)
{
    lh_str_t script;

    lh_str_init(lh_addr_of(script));
    lh_bool_t written = lh_ui_web_player_page_try_write_script(lh_addr_of(script));
    if (written)
    {
        lh_str_view_t view;

        lh_str_as_view(lh_addr_of(script), lh_addr_of(view));
        written = lh_ui_web_player_page_try_write_document(out, view);
    }
    lh_str_deinit(lh_addr_of(script));
    return written;
}

lh_void
lh_ui_web_player_page_write(lh_str_t *out)
{
    lh_runtime_check_ifn(lh_ui_web_player_page_try_write(out),
                         lh_runtime_error_code_invalid_argument, "player page cannot be written");
}

lh_bool_t
lh_ui_web_player_page_build(lh_byte_ptr mem, lh_usize_t cap, lh_usize_t *out_len)
{
    if (lh_null_ne(out_len))
    {
        lh_ptr_deref(out_len) = 0U;
    }
    if (lh_null_eq(mem))
    {
        return lh_bool_false;
    }
    lh_str_t page;

    lh_str_init(lh_addr_of(page));
    /* The bytes and the terminator after them must fit. */
    const lh_bool_t built =
        lh_cast_static(lh_bool_t, lh_ui_web_player_page_try_write(lh_addr_of(page)) &&
                                      lh_math_lt(lh_str_get_size(lh_addr_of(page)), cap));
    if (built)
    {
        lh_str_ptr_copy_cstr(lh_ptr_rcast(lh_char_t, mem), cap, lh_str_get_data(lh_addr_of(page)),
                             lh_str_get_size(lh_addr_of(page)));
        if (lh_null_ne(out_len))
        {
            lh_ptr_deref(out_len) = lh_str_get_size(lh_addr_of(page));
        }
    }
    lh_str_deinit(lh_addr_of(page));
    return built;
}
