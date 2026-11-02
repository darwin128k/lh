/**
 * @file op.c
 * @brief Implementation of `lh/web/js/op.h`.
 */

#include <lh/web/js/op.h>
#include <lh/char/letter.h>

/** The text of every operator, by its value. */
static const lh_str_cptr m_web_js_op_texts[LH_WEB_JS_OP_COUNT] = {
    "",   "!",  "~",   "+",   "-",   "typeof", "void", "delete", "++", "--", "+",  "-",
    "*",  "/",  "%",   "<<",  ">>",  ">>>",    "<",    ">",      "<=", ">=", "in", "instanceof",
    "==", "!=", "===", "!==", "&",   "^",      "|",    "&&",     "||", "=",  "+=", "-=",
    "*=", "/=", "%=",  "<<=", ">>=", ">>>=",   "&=",   "^=",     "|="};

/** The precedence of every operator, by its value. */
static const lh_web_js_precedence_t m_web_js_op_precedences[LH_WEB_JS_OP_COUNT] = {
    lh_web_js_precedence_lowest,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_unary,
    lh_web_js_precedence_additive,
    lh_web_js_precedence_additive,
    lh_web_js_precedence_multiplicative,
    lh_web_js_precedence_multiplicative,
    lh_web_js_precedence_multiplicative,
    lh_web_js_precedence_shift,
    lh_web_js_precedence_shift,
    lh_web_js_precedence_shift,
    lh_web_js_precedence_relational,
    lh_web_js_precedence_relational,
    lh_web_js_precedence_relational,
    lh_web_js_precedence_relational,
    lh_web_js_precedence_relational,
    lh_web_js_precedence_relational,
    lh_web_js_precedence_equality,
    lh_web_js_precedence_equality,
    lh_web_js_precedence_equality,
    lh_web_js_precedence_equality,
    lh_web_js_precedence_bit_and,
    lh_web_js_precedence_bit_xor,
    lh_web_js_precedence_bit_or,
    lh_web_js_precedence_and,
    lh_web_js_precedence_or,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign,
    lh_web_js_precedence_assign};

lh_str_view_t
lh_web_js_op_get_text(lh_web_js_op_t op)
{
    lh_str_view_t text;

    lh_str_view_init(lh_addr_of(text),
                     lh_math_lt(op, LH_WEB_JS_OP_COUNT) ? m_web_js_op_texts[op] : "");
    return text;
}

lh_web_js_precedence_t
lh_web_js_op_get_precedence(lh_web_js_op_t op)
{
    return lh_math_lt(op, LH_WEB_JS_OP_COUNT) ? m_web_js_op_precedences[op]
                                              : lh_web_js_precedence_lowest;
}

lh_bool_t
lh_web_js_op_is_word(lh_web_js_op_t op)
{
    const lh_str_view_t text = lh_web_js_op_get_text(op);

    return lh_cast_static(
        lh_bool_t, !lh_math_is_zero(lh_str_view_get_size(lh_addr_of(text))) &&
                       lh_char_is_letter(lh_str_view_get_char_from_begin(lh_addr_of(text), 0U)));
}
