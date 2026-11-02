/**
 * @file writer.c
 * @brief Implementation of `lh/web/js/writer.h`.
 */

#include <lh/web/js/writer.h>
#include <lh/assert.h>
#include <lh/char/xdigit.h>
#include <lh/web/js/lexer/name.h>

/** The printable bytes a string literal escapes; every control character is escaped too. */
static const lh_char_t m_web_js_writer_escapes[] = {lh_char_map_backslash, lh_char_map_squote,
                                                    lh_char_map_less};

/** The flags a regular expression may carry. */
static const lh_char_t m_web_js_writer_regex_flags[] = {
    lh_char_map_small_d, lh_char_map_small_g, lh_char_map_small_i, lh_char_map_small_m,
    lh_char_map_small_s, lh_char_map_small_u, lh_char_map_small_y, lh_char_map_small_v};

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_str_t *
lh_web_js_writer_get_out_mut(lh_web_js_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->out;
}

lh_void
lh_web_js_writer_set_out(lh_web_js_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(self);
    self->out = out;
}

/* ── checks ──────────────────────────────────────────────────────────────── */

lh_bool_t
lh_web_js_writer_needs_escape(lh_char_t ch)
{
    return lh_cast_static(
        lh_bool_t,
        lh_math_lt(lh_cast_static(lh_byte_t, ch), lh_cast_static(lh_byte_t, lh_char_map_space)) ||
            lh_str_ptr_contains_char(m_web_js_writer_escapes,
                                     lh_array_ptr_get_size(m_web_js_writer_escapes), ch));
}

lh_bool_t
lh_web_js_writer_is_name(lh_str_view_t name)
{
    return lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(name));
}

lh_bool_t
lh_web_js_writer_is_number(lh_str_view_t text)
{
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(text));

    if (lh_math_is_zero(size))
    {
        return lh_bool_false;
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (!lh_char_is_digit(lh_str_view_get_char_from_begin(lh_addr_of(text), i)))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_js_writer_is_regex(lh_str_view_t pattern, lh_str_view_t flags)
{
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(pattern));
    const lh_usize_t flag_count = lh_str_view_get_size(lh_addr_of(flags));
    lh_bool_t escaped = lh_bool_false;

    if (lh_math_is_zero(size) ||
        lh_math_eq(lh_str_view_get_char_from_begin(lh_addr_of(pattern), 0U), lh_char_map_asterisk))
    {
        return lh_bool_false;
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        const lh_char_t ch = lh_str_view_get_char_from_begin(lh_addr_of(pattern), i);

        if (lh_math_eq(ch, lh_char_map_lf) || lh_math_eq(ch, lh_char_map_cr) ||
            lh_math_eq(ch, lh_char_map_less))
        {
            return lh_bool_false;
        }
        if (escaped)
        {
            escaped = lh_bool_false;
            continue;
        }
        if (lh_math_eq(ch, lh_char_map_slash))
        {
            return lh_bool_false;
        }
        escaped = lh_cast_static(lh_bool_t, lh_math_eq(ch, lh_char_map_backslash));
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, flag_count); i = lh_math_add_one(i))
    {
        if (!lh_str_ptr_contains_char(m_web_js_writer_regex_flags,
                                      lh_array_ptr_get_size(m_web_js_writer_regex_flags),
                                      lh_str_view_get_char_from_begin(lh_addr_of(flags), i)))
        {
            return lh_bool_false;
        }
    }
    return lh_cast_static(lh_bool_t, !escaped);
}

lh_web_js_precedence_t
lh_web_js_writer_get_precedence(const lh_web_js_node_t *node)
{
    switch (lh_web_js_node_get_kind(node))
    {
    case lh_web_js_kind_member:
    case lh_web_js_kind_index:
    case lh_web_js_kind_call:
    case lh_web_js_kind_new:
        return lh_web_js_precedence_call;
    case lh_web_js_kind_unary:
        return lh_web_js_precedence_unary;
    case lh_web_js_kind_update:
        return lh_web_js_node_get_flag(node) ? lh_web_js_precedence_unary
                                             : lh_web_js_precedence_postfix;
    case lh_web_js_kind_binary:
    case lh_web_js_kind_assign:
        return lh_web_js_op_get_precedence(lh_web_js_node_get_op(node));
    case lh_web_js_kind_conditional:
        return lh_web_js_precedence_conditional;
    case lh_web_js_kind_sequence:
        return lh_web_js_precedence_sequence;
    default:
        return lh_web_js_precedence_primary;
    }
}

lh_bool_t
lh_web_js_writer_has_leading_sign(const lh_web_js_node_t *node)
{
    const lh_web_js_op_t op = lh_web_js_node_get_op(node);

    if (lh_web_js_node_has_kind(node, lh_web_js_kind_unary))
    {
        return lh_cast_static(lh_bool_t, lh_math_eq(op, lh_web_js_op_plus) ||
                                             lh_math_eq(op, lh_web_js_op_minus));
    }
    return lh_cast_static(lh_bool_t, lh_web_js_node_has_kind(node, lh_web_js_kind_update) &&
                                         lh_web_js_node_get_flag(node));
}

lh_bool_t
lh_web_js_writer_needs_new_parens(const lh_web_js_node_t *node)
{
    const lh_web_js_node_t *at = node;

    while (lh_null_ne(at) && (lh_web_js_node_has_kind(at, lh_web_js_kind_member) ||
                              lh_web_js_node_has_kind(at, lh_web_js_kind_index)))
    {
        at = lh_web_js_node_get_first(at);
    }
    return lh_cast_static(lh_bool_t,
                          lh_null_ne(at) && lh_web_js_node_has_kind(at, lh_web_js_kind_call));
}

lh_bool_t
lh_web_js_writer_needs_braces(const lh_web_js_node_t *node)
{
    return lh_cast_static(lh_bool_t, lh_web_js_node_has_kind(node, lh_web_js_kind_if) ||
                                         lh_web_js_node_has_kind(node, lh_web_js_kind_for));
}

/* ── init and parenthesis checks ─────────────────────────────────────────── */

lh_void
lh_web_js_writer_init(lh_web_js_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(out);
    lh_web_js_writer_set_out(self, out);
}

lh_bool_t
lh_web_js_writer_needs_parens_after(lh_web_js_op_t op, const lh_web_js_node_t *right)
{
    switch (op)
    {
    case lh_web_js_op_add:
    case lh_web_js_op_sub:
        return lh_web_js_writer_has_leading_sign(right);
    case lh_web_js_op_div:
        return lh_web_js_node_has_kind(right, lh_web_js_kind_regex);
    case lh_web_js_op_lt:
        return lh_cast_static(lh_bool_t,
                              lh_web_js_node_has_kind(right, lh_web_js_kind_regex) ||
                                  (lh_web_js_node_has_kind(right, lh_web_js_kind_unary) &&
                                   lh_math_eq(lh_web_js_node_get_op(right), lh_web_js_op_not)));
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_web_js_writer_needs_statement_parens(const lh_web_js_node_t *node)
{
    const lh_web_js_node_t *first;

    if (lh_null_eq(node))
    {
        return lh_bool_false;
    }
    first = lh_web_js_node_get_first(node);
    switch (lh_web_js_node_get_kind(node))
    {
    case lh_web_js_kind_function_expression:
    case lh_web_js_kind_object:
        return lh_bool_true;
    case lh_web_js_kind_call:
        /* A function callee is written in parentheses of its own. */
        return lh_cast_static(
            lh_bool_t, lh_null_ne(first) &&
                           !lh_web_js_node_has_kind(first, lh_web_js_kind_function_expression) &&
                           lh_web_js_writer_needs_statement_parens(first));
    case lh_web_js_kind_member:
    case lh_web_js_kind_index:
        return lh_cast_static(lh_bool_t,
                              lh_null_ne(first) &&
                                  !lh_web_js_node_has_kind(first, lh_web_js_kind_number) &&
                                  lh_web_js_writer_needs_statement_parens(first));
    case lh_web_js_kind_update:
        return lh_cast_static(lh_bool_t, !lh_web_js_node_get_flag(node) &&
                                             lh_web_js_writer_needs_statement_parens(first));
    case lh_web_js_kind_binary:
    case lh_web_js_kind_assign:
    case lh_web_js_kind_conditional:
    case lh_web_js_kind_sequence:
        /* The first operand starts the text unless its own parentheses come first. */
        return lh_cast_static(lh_bool_t, lh_null_ne(first) &&
                                             lh_math_ge(lh_web_js_writer_get_precedence(first),
                                                        lh_web_js_writer_get_precedence(node)) &&
                                             lh_web_js_writer_needs_statement_parens(first));
    default:
        return lh_bool_false;
    }
}

/* ── text ────────────────────────────────────────────────────────────────── */

lh_void
lh_web_js_writer_put_char(lh_web_js_writer_t *self, lh_char_t ch)
{
    lh_str_push_back(lh_web_js_writer_get_out_mut(self), ch);
}

lh_void
lh_web_js_writer_put_view(lh_web_js_writer_t *self, lh_str_view_t text)
{
    lh_str_append_view(lh_web_js_writer_get_out_mut(self), text);
}

lh_void
lh_web_js_writer_put_escape(lh_web_js_writer_t *self, lh_char_t ch)
{
    const lh_byte_t code = lh_cast_static(lh_byte_t, ch);

    lh_web_js_writer_put_char(self, lh_char_map_backslash);
    switch (ch)
    {
    case lh_char_map_backslash:
    case lh_char_map_squote:
        lh_web_js_writer_put_char(self, ch);
        return;
    case lh_char_map_lf:
        lh_web_js_writer_put_char(self, lh_char_map_small_n);
        return;
    case lh_char_map_cr:
        lh_web_js_writer_put_char(self, lh_char_map_small_r);
        return;
    case lh_char_map_ht:
        lh_web_js_writer_put_char(self, lh_char_map_small_t);
        return;
    default:
        /* No short name: the byte in two hex digits, `\x3c` for `<`. */
        lh_web_js_writer_put_char(self, lh_char_map_small_x);
        lh_web_js_writer_put_char(self,
                                  lh_char_from_xdigit(lh_math_bit_shr(code, 4U), lh_bool_false));
        lh_web_js_writer_put_char(self,
                                  lh_char_from_xdigit(lh_math_bit_and(code, 0x0FU), lh_bool_false));
        return;
    }
}

lh_bool_t
lh_web_js_writer_try_put_name(lh_web_js_writer_t *self, lh_str_view_t name)
{
    if (!lh_web_js_writer_is_name(name))
    {
        return lh_bool_false;
    }
    lh_web_js_writer_put_view(self, name);
    return lh_bool_true;
}

lh_void
lh_web_js_writer_put_string(lh_web_js_writer_t *self, lh_str_view_t text)
{
    const lh_str_cptr chars = lh_str_view_get_data(lh_addr_of(text));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(text));
    lh_str_t *out = lh_web_js_writer_get_out_mut(self);
    lh_usize_t start = 0U;

    lh_web_js_writer_put_char(self, lh_char_map_squote);
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (!lh_web_js_writer_needs_escape(chars[i]))
        {
            continue;
        }
        /* Everything since the last escape goes in as one run. */
        if (lh_math_gt(i, start))
        {
            lh_str_append(out, chars + start, lh_math_sub(i, start));
        }
        lh_web_js_writer_put_escape(self, chars[i]);
        start = lh_math_add_one(i);
    }
    if (lh_math_gt(size, start))
    {
        lh_str_append(out, chars + start, lh_math_sub(size, start));
    }
    lh_web_js_writer_put_char(self, lh_char_map_squote);
}

lh_void
lh_web_js_writer_put_key(lh_web_js_writer_t *self, lh_str_view_t key)
{
    const lh_str_view_t prototype = lh_str_view_lit("__proto__");

    if (lh_web_js_writer_is_name(key) &&
        !lh_str_view_equals(lh_addr_of(key), lh_addr_of(prototype), lh_bool_false))
    {
        lh_web_js_writer_put_view(self, key);
        return;
    }
    lh_web_js_writer_put_char(self, lh_char_map_lbracket);
    lh_web_js_writer_put_string(self, key);
    lh_web_js_writer_put_char(self, lh_char_map_rbracket);
}

/* ── tree ────────────────────────────────────────────────────────────────── */

lh_bool_t
lh_web_js_writer_try_write_statement(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    const lh_web_js_node_t *first;
    const lh_web_js_node_t *second;
    const lh_web_js_node_t *third;
    lh_bool_t written;

    if (lh_null_eq(node))
    {
        return lh_bool_false;
    }
    first = lh_web_js_node_get_first(node);
    second = lh_null_ne(first) ? lh_web_js_node_get_next(node, first) : lh_null;
    third = lh_null_ne(second) ? lh_web_js_node_get_next(node, second) : lh_null;
    switch (lh_web_js_node_get_kind(node))
    {
    case lh_web_js_kind_function_declaration:
        written = lh_web_js_writer_try_write_function(self, node);
        break;
    case lh_web_js_kind_var:
        written = lh_web_js_writer_try_write_declarations(self, node);
        lh_web_js_writer_put_char(self, lh_char_map_semicolon);
        break;
    case lh_web_js_kind_expression:
        if (lh_web_js_writer_needs_statement_parens(first))
        {
            lh_web_js_writer_put_char(self, lh_char_map_lparen);
            written = lh_web_js_writer_try_write_bare(self, first);
            lh_web_js_writer_put_char(self, lh_char_map_rparen);
        }
        else
        {
            written =
                lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_lowest);
        }
        lh_web_js_writer_put_char(self, lh_char_map_semicolon);
        break;
    case lh_web_js_kind_block:
        written = lh_web_js_writer_try_write_body(self, node);
        break;
    case lh_web_js_kind_if:
        lh_web_js_writer_put_view(self, lh_str_view_lit("if("));
        if (lh_null_eq(second) ||
            !lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_lowest))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_rparen);
        if (lh_null_eq(third))
        {
            return lh_web_js_writer_try_write_statement(self, second);
        }
        if (lh_web_js_node_has_kind(second, lh_web_js_kind_block))
        {
            written = lh_web_js_writer_try_write_body(self, second);
        }
        else if (!lh_web_js_writer_needs_braces(second))
        {
            written = lh_web_js_writer_try_write_statement(self, second);
        }
        else
        {
            /* Braces, so an inner `if` cannot take this `else`. */
            lh_web_js_writer_put_char(self, lh_char_map_lbrace);
            lh_web_js_writer_put_char(self, lh_char_map_lf);
            written = lh_web_js_writer_try_write_statement(self, second);
            lh_web_js_writer_put_char(self, lh_char_map_rbrace);
        }
        if (!written)
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_view(self, lh_str_view_lit("else"));
        if (lh_web_js_node_has_kind(third, lh_web_js_kind_block))
        {
            written = lh_web_js_writer_try_write_body(self, third);
            break;
        }
        lh_web_js_writer_put_char(self, lh_char_map_space);
        return lh_web_js_writer_try_write_statement(self, third);
    case lh_web_js_kind_for:
        lh_web_js_writer_put_view(self, lh_str_view_lit("for("));
        if (lh_null_eq(third) || !lh_web_js_writer_try_write_clause(self, first))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_semicolon);
        if (!lh_web_js_writer_try_write_clause(self, second))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_semicolon);
        if (!lh_web_js_writer_try_write_clause(self, third))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_rparen);
        return lh_web_js_writer_try_write_statement(self, lh_web_js_node_get_next(node, third));
    case lh_web_js_kind_return:
        lh_web_js_writer_put_view(self, lh_str_view_lit("return"));
        written = lh_bool_true;
        if (lh_null_ne(first))
        {
            lh_web_js_writer_put_char(self, lh_char_map_space);
            written =
                lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_lowest);
        }
        lh_web_js_writer_put_char(self, lh_char_map_semicolon);
        break;
    case lh_web_js_kind_break:
        lh_web_js_writer_put_view(self, lh_str_view_lit("break;"));
        written = lh_bool_true;
        break;
    case lh_web_js_kind_switch:
        lh_web_js_writer_put_view(self, lh_str_view_lit("switch("));
        if (!lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_lowest))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_view(self, lh_str_view_lit("){\n"));
        for (const lh_web_js_node_t *clause = second; lh_null_ne(clause);
             clause = lh_web_js_node_get_next(node, clause))
        {
            const lh_web_js_node_t *test = lh_web_js_node_get_first(clause);

            if (lh_web_js_node_has_kind(clause, lh_web_js_kind_default))
            {
                lh_web_js_writer_put_view(self, lh_str_view_lit("default:\n"));
                written = lh_web_js_writer_try_write_statements(self, clause, test);
            }
            else if (lh_web_js_node_has_kind(clause, lh_web_js_kind_case))
            {
                lh_web_js_writer_put_view(self, lh_str_view_lit("case "));
                written =
                    lh_web_js_writer_try_write_expression(self, test, lh_web_js_precedence_lowest);
                lh_web_js_writer_put_char(self, lh_char_map_colon);
                lh_web_js_writer_put_char(self, lh_char_map_lf);
                written = written && lh_null_ne(test) &&
                          lh_web_js_writer_try_write_statements(
                              self, clause, lh_web_js_node_get_next(clause, test));
            }
            else
            {
                written = lh_bool_false;
            }
            if (!written)
            {
                return lh_bool_false;
            }
        }
        lh_web_js_writer_put_char(self, lh_char_map_rbrace);
        written = lh_bool_true;
        break;
    case lh_web_js_kind_try:
        lh_web_js_writer_put_view(self, lh_str_view_lit("try"));
        if (lh_null_eq(second) || !lh_web_js_node_has_kind(second, lh_web_js_kind_catch) ||
            !lh_web_js_writer_try_write_body(self, first))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_view(self, lh_str_view_lit("catch("));
        if (!lh_web_js_writer_try_put_name(self, lh_web_js_node_get_name_view(second)))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_rparen);
        written = lh_web_js_writer_try_write_body(self, lh_web_js_node_get_first(second));
        break;
    case lh_web_js_kind_empty:
        lh_web_js_writer_put_char(self, lh_char_map_semicolon);
        written = lh_bool_true;
        break;
    default:
        return lh_bool_false;
    }
    lh_web_js_writer_put_char(self, lh_char_map_lf);
    return written;
}

lh_bool_t
lh_web_js_writer_try_write_statements(lh_web_js_writer_t *self, const lh_web_js_node_t *node,
                                      const lh_web_js_node_t *first)
{
    for (const lh_web_js_node_t *statement = first; lh_null_ne(statement);
         statement = lh_web_js_node_get_next(node, statement))
    {
        if (!lh_web_js_writer_try_write_statement(self, statement))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_js_writer_try_write_items(lh_web_js_writer_t *self, const lh_web_js_node_t *node,
                                 const lh_web_js_node_t *first)
{
    for (const lh_web_js_node_t *item = first; lh_null_ne(item);
         item = lh_web_js_node_get_next(node, item))
    {
        if (item != first)
        {
            lh_web_js_writer_put_char(self, lh_char_map_comma);
        }
        if (!lh_web_js_writer_try_write_expression(self, item, lh_web_js_precedence_assign))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_js_writer_try_write_body(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    if (!lh_web_js_node_has_kind(node, lh_web_js_kind_block))
    {
        return lh_bool_false;
    }
    lh_web_js_writer_put_char(self, lh_char_map_lbrace);
    lh_web_js_writer_put_char(self, lh_char_map_lf);
    if (!lh_web_js_writer_try_write_statements(self, node, lh_web_js_node_get_first(node)))
    {
        return lh_bool_false;
    }
    lh_web_js_writer_put_char(self, lh_char_map_rbrace);
    return lh_bool_true;
}

lh_bool_t
lh_web_js_writer_try_write_function(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    const lh_str_view_t name = lh_web_js_node_get_name_view(node);
    const lh_web_js_node_t *body = lh_web_js_node_get_last(node);

    lh_web_js_writer_put_view(self, lh_str_view_lit("function"));
    if (!lh_math_is_zero(lh_str_view_get_size(lh_addr_of(name))))
    {
        lh_web_js_writer_put_char(self, lh_char_map_space);
        if (!lh_web_js_writer_try_put_name(self, name))
        {
            return lh_bool_false;
        }
    }
    lh_web_js_writer_put_char(self, lh_char_map_lparen);
    for (const lh_web_js_node_t *param = lh_web_js_node_get_first(node); param != body;
         param = lh_web_js_node_get_next(node, param))
    {
        if (param != lh_web_js_node_get_first(node))
        {
            lh_web_js_writer_put_char(self, lh_char_map_comma);
        }
        if (!lh_web_js_node_has_kind(param, lh_web_js_kind_ident) ||
            !lh_web_js_writer_try_put_name(self, lh_web_js_node_get_name_view(param)))
        {
            return lh_bool_false;
        }
    }
    lh_web_js_writer_put_char(self, lh_char_map_rparen);
    return lh_cast_static(lh_bool_t,
                          lh_null_ne(body) && lh_web_js_writer_try_write_body(self, body));
}

lh_bool_t
lh_web_js_writer_try_write_bare(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    const lh_web_js_node_t *first = lh_web_js_node_get_first(node);
    const lh_web_js_node_t *second =
        lh_null_ne(first) ? lh_web_js_node_get_next(node, first) : lh_null;
    const lh_web_js_op_t op = lh_web_js_node_get_op(node);
    const lh_web_js_precedence_t level = lh_web_js_writer_get_precedence(node);

    switch (lh_web_js_node_get_kind(node))
    {
    case lh_web_js_kind_ident:
        return lh_web_js_writer_try_put_name(self, lh_web_js_node_get_name_view(node));
    case lh_web_js_kind_number:
        if (!lh_web_js_writer_is_number(lh_web_js_node_get_text_view(node)))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_view(self, lh_web_js_node_get_text_view(node));
        return lh_bool_true;
    case lh_web_js_kind_string:
        lh_web_js_writer_put_string(self, lh_web_js_node_get_text_view(node));
        return lh_bool_true;
    case lh_web_js_kind_regex:
        if (!lh_web_js_writer_is_regex(lh_web_js_node_get_text_view(node),
                                       lh_web_js_node_get_name_view(node)))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_slash);
        lh_web_js_writer_put_view(self, lh_web_js_node_get_text_view(node));
        lh_web_js_writer_put_char(self, lh_char_map_slash);
        lh_web_js_writer_put_view(self, lh_web_js_node_get_name_view(node));
        return lh_bool_true;
    case lh_web_js_kind_null:
        lh_web_js_writer_put_view(self, lh_str_view_lit("null"));
        return lh_bool_true;
    case lh_web_js_kind_bool:
        lh_web_js_writer_put_view(self, lh_web_js_node_get_flag(node) ? lh_str_view_lit("true")
                                                                      : lh_str_view_lit("false"));
        return lh_bool_true;
    case lh_web_js_kind_array:
        lh_web_js_writer_put_char(self, lh_char_map_lbracket);
        if (!lh_web_js_writer_try_write_items(self, node, first))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_rbracket);
        return lh_bool_true;
    case lh_web_js_kind_object:
        lh_web_js_writer_put_char(self, lh_char_map_lbrace);
        for (const lh_web_js_node_t *property = first; lh_null_ne(property);
             property = lh_web_js_node_get_next(node, property))
        {
            if (property != first)
            {
                lh_web_js_writer_put_char(self, lh_char_map_comma);
            }
            if (!lh_web_js_node_has_kind(property, lh_web_js_kind_property))
            {
                return lh_bool_false;
            }
            lh_web_js_writer_put_key(self, lh_web_js_node_get_name_view(property));
            lh_web_js_writer_put_char(self, lh_char_map_colon);
            if (!lh_web_js_writer_try_write_expression(self, lh_web_js_node_get_first(property),
                                                       lh_web_js_precedence_assign))
            {
                return lh_bool_false;
            }
        }
        lh_web_js_writer_put_char(self, lh_char_map_rbrace);
        return lh_bool_true;
    case lh_web_js_kind_function_expression:
        return lh_web_js_writer_try_write_function(self, node);
    case lh_web_js_kind_member:
        if (lh_null_ne(first) && lh_web_js_node_has_kind(first, lh_web_js_kind_number))
        {
            /* `1.x` would read as a number: `(1).x`. */
            lh_web_js_writer_put_char(self, lh_char_map_lparen);
            if (!lh_web_js_writer_try_write_bare(self, first))
            {
                return lh_bool_false;
            }
            lh_web_js_writer_put_char(self, lh_char_map_rparen);
        }
        else if (!lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_call))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_period);
        return lh_web_js_writer_try_put_name(self, lh_web_js_node_get_name_view(node));
    case lh_web_js_kind_index:
        if (!lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_call))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_lbracket);
        if (!lh_web_js_writer_try_write_expression(self, second, lh_web_js_precedence_lowest))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_rbracket);
        return lh_bool_true;
    case lh_web_js_kind_call:
    case lh_web_js_kind_new:
        if (lh_null_eq(first))
        {
            return lh_bool_false;
        }
        if (lh_web_js_node_has_kind(node, lh_web_js_kind_new))
        {
            lh_web_js_writer_put_view(self, lh_str_view_lit("new "));
        }
        if (lh_web_js_node_has_kind(first, lh_web_js_kind_function_expression) ||
            (lh_web_js_node_has_kind(node, lh_web_js_kind_new) &&
             lh_web_js_writer_needs_new_parens(first)))
        {
            lh_web_js_writer_put_char(self, lh_char_map_lparen);
            if (!lh_web_js_writer_try_write_bare(self, first))
            {
                return lh_bool_false;
            }
            lh_web_js_writer_put_char(self, lh_char_map_rparen);
        }
        else if (!lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_call))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_lparen);
        if (!lh_web_js_writer_try_write_items(self, node, second))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_rparen);
        return lh_bool_true;
    case lh_web_js_kind_unary:
        lh_web_js_writer_put_view(self, lh_web_js_op_get_text(op));
        if (lh_web_js_op_is_word(op))
        {
            lh_web_js_writer_put_char(self, lh_char_map_space);
        }
        /* `- -x` must not run together into `--x`. */
        if (lh_null_ne(first) &&
            (lh_math_eq(op, lh_web_js_op_plus) || lh_math_eq(op, lh_web_js_op_minus)) &&
            lh_web_js_writer_has_leading_sign(first))
        {
            lh_web_js_writer_put_char(self, lh_char_map_lparen);
            if (!lh_web_js_writer_try_write_bare(self, first))
            {
                return lh_bool_false;
            }
            lh_web_js_writer_put_char(self, lh_char_map_rparen);
            return lh_bool_true;
        }
        return lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_unary);
    case lh_web_js_kind_update:
        if (lh_web_js_node_get_flag(node))
        {
            lh_web_js_writer_put_view(self, lh_web_js_op_get_text(op));
            return lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_call);
        }
        if (!lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_call))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_view(self, lh_web_js_op_get_text(op));
        return lh_bool_true;
    case lh_web_js_kind_binary:
        if (!lh_web_js_writer_try_write_expression(self, first, level))
        {
            return lh_bool_false;
        }
        if (lh_web_js_op_is_word(op))
        {
            lh_web_js_writer_put_char(self, lh_char_map_space);
            lh_web_js_writer_put_view(self, lh_web_js_op_get_text(op));
            lh_web_js_writer_put_char(self, lh_char_map_space);
        }
        else
        {
            lh_web_js_writer_put_view(self, lh_web_js_op_get_text(op));
        }
        if (lh_null_ne(second) && lh_web_js_writer_needs_parens_after(op, second))
        {
            lh_web_js_writer_put_char(self, lh_char_map_lparen);
            if (!lh_web_js_writer_try_write_bare(self, second))
            {
                return lh_bool_false;
            }
            lh_web_js_writer_put_char(self, lh_char_map_rparen);
            return lh_bool_true;
        }
        return lh_web_js_writer_try_write_expression(self, second, lh_math_add_one(level));
    case lh_web_js_kind_assign:
        if (!lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_call))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_view(self, lh_web_js_op_get_text(op));
        return lh_web_js_writer_try_write_expression(self, second, lh_web_js_precedence_assign);
    case lh_web_js_kind_conditional:
        if (lh_null_eq(second) ||
            !lh_web_js_writer_try_write_expression(self, first, lh_web_js_precedence_or))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_question);
        if (!lh_web_js_writer_try_write_expression(self, second, lh_web_js_precedence_assign))
        {
            return lh_bool_false;
        }
        lh_web_js_writer_put_char(self, lh_char_map_colon);
        return lh_web_js_writer_try_write_expression(self, lh_web_js_node_get_next(node, second),
                                                     lh_web_js_precedence_assign);
    case lh_web_js_kind_sequence:
        return lh_cast_static(lh_bool_t, lh_null_ne(first) &&
                                             lh_web_js_writer_try_write_items(self, node, first));
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_web_js_writer_try_write_expression(lh_web_js_writer_t *self, const lh_web_js_node_t *node,
                                      lh_web_js_precedence_t min)
{
    if (lh_null_eq(node))
    {
        return lh_bool_false;
    }
    const lh_bool_t group =
        lh_cast_static(lh_bool_t, lh_math_lt(lh_web_js_writer_get_precedence(node), min));
    if (group)
    {
        lh_web_js_writer_put_char(self, lh_char_map_lparen);
    }
    const lh_bool_t written = lh_web_js_writer_try_write_bare(self, node);
    if (group)
    {
        lh_web_js_writer_put_char(self, lh_char_map_rparen);
    }
    return written;
}

lh_bool_t
lh_web_js_writer_try_write_declarations(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    const lh_web_js_node_t *first = lh_web_js_node_get_first(node);

    if (lh_null_eq(first))
    {
        return lh_bool_false;
    }
    lh_web_js_writer_put_view(self, lh_str_view_lit("var "));
    for (const lh_web_js_node_t *declarator = first; lh_null_ne(declarator);
         declarator = lh_web_js_node_get_next(node, declarator))
    {
        if (declarator != first)
        {
            lh_web_js_writer_put_char(self, lh_char_map_comma);
        }
        if (!lh_web_js_node_has_kind(declarator, lh_web_js_kind_declarator) ||
            !lh_web_js_writer_try_put_name(self, lh_web_js_node_get_name_view(declarator)))
        {
            return lh_bool_false;
        }
        if (lh_null_eq(lh_web_js_node_get_first(declarator)))
        {
            continue;
        }
        lh_web_js_writer_put_char(self, lh_char_map_equals);
        if (!lh_web_js_writer_try_write_expression(self, lh_web_js_node_get_first(declarator),
                                                   lh_web_js_precedence_assign))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_js_writer_try_write_clause(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    if (lh_null_eq(node))
    {
        return lh_bool_false;
    }
    switch (lh_web_js_node_get_kind(node))
    {
    case lh_web_js_kind_empty:
        return lh_bool_true;
    case lh_web_js_kind_var:
        return lh_web_js_writer_try_write_declarations(self, node);
    default:
        return lh_web_js_writer_try_write_expression(self, node, lh_web_js_precedence_lowest);
    }
}

lh_bool_t
lh_web_js_writer_try_write(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    if (lh_web_js_node_has_kind(node, lh_web_js_kind_program))
    {
        return lh_web_js_writer_try_write_statements(self, node, lh_web_js_node_get_first(node));
    }
    return lh_web_js_writer_try_write_statement(self, node);
}

lh_void
lh_web_js_writer_write(lh_web_js_writer_t *self, const lh_web_js_node_t *node)
{
    lh_runtime_check_ifn(lh_web_js_writer_try_write(self, node),
                         lh_runtime_error_code_invalid_argument, "js tree cannot be written");
}
