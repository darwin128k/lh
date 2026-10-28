#include <lh/json/reader.h>
#include <lh/assert.h>
#include <lh/char/xdigit.h>
#include <lh/str/utf8.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

const lh_str_view_t *
lh_json_reader_get_text(const lh_json_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_void
lh_json_reader_set_text(lh_json_reader_t *self, lh_str_view_t text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_usize_t
lh_json_reader_get_pos(const lh_json_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pos;
}

lh_void
lh_json_reader_set_pos(lh_json_reader_t *self, lh_usize_t pos)
{
    lh_assert_runtime_ref(self);
    self->pos = pos;
}

lh_usize_t
lh_json_reader_get_depth(const lh_json_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->depth;
}

lh_void
lh_json_reader_set_depth(lh_json_reader_t *self, lh_usize_t depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

lh_json_error_t
lh_json_reader_get_error(const lh_json_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error;
}

lh_void
lh_json_reader_set_error(lh_json_reader_t *self, lh_json_error_t error)
{
    lh_assert_runtime_ref(self);
    self->error = error;
}

lh_bool_t
lh_json_reader_fail(lh_json_reader_t *self, lh_json_error_t error)
{
    lh_json_reader_set_error(self, error);
    return lh_bool_false;
}

lh_bool_t
lh_json_reader_has_error(const lh_json_reader_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ne(lh_json_reader_get_error(self), lh_json_error_none));
}

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_json_reader_init(lh_json_reader_t *self, lh_str_view_t text)
{
    lh_str_view_skip_bom(lh_addr_of(text));
    lh_json_reader_set_text(self, text);
    lh_json_reader_set_pos(self, 0U);
    lh_json_reader_set_depth(self, 0U);
    lh_json_reader_set_error(self, lh_json_error_none);
}

/* ── cursor ──────────────────────────────────────────────────────────────── */

lh_bool_t
lh_json_reader_is_end(const lh_json_reader_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ge(lh_json_reader_get_pos(self),
                                     lh_str_view_get_size(lh_json_reader_get_text(self))));
}

lh_char_t
lh_json_reader_peek(const lh_json_reader_t *self, lh_usize_t ahead)
{
    const lh_str_view_t *text = lh_json_reader_get_text(self);
    const lh_usize_t at = lh_math_add(lh_json_reader_get_pos(self), ahead);

    if (lh_math_ge(at, lh_str_view_get_size(text)))
    {
        return lh_char_map_nul;
    }
    return lh_str_view_get_data(text)[at];
}

lh_char_t
lh_json_reader_get_char(const lh_json_reader_t *self)
{
    return lh_json_reader_peek(self, 0U);
}

lh_void
lh_json_reader_advance(lh_json_reader_t *self, lh_usize_t count)
{
    lh_json_reader_set_pos(self, lh_math_add(lh_json_reader_get_pos(self), count));
}

lh_bool_t
lh_json_reader_is_whitespace(lh_char_t ch)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(ch, lh_char_map_space) || lh_math_eq(ch, lh_char_map_ht) ||
                              lh_math_eq(ch, lh_char_map_lf) || lh_math_eq(ch, lh_char_map_cr));
}

lh_void
lh_json_reader_skip_whitespace(lh_json_reader_t *self)
{
    while (lh_json_reader_is_whitespace(lh_json_reader_get_char(self)))
    {
        lh_json_reader_advance(self, 1U);
    }
}

lh_bool_t
lh_json_reader_fail_unexpected(lh_json_reader_t *self)
{
    return lh_json_reader_fail(self, lh_json_reader_is_end(self) ? lh_json_error_unexpected_end
                                                                 : lh_json_error_unexpected_char);
}

lh_bool_t
lh_json_reader_skip_char(lh_json_reader_t *self, lh_char_t ch)
{
    if (lh_json_reader_is_end(self) || lh_math_ne(lh_json_reader_get_char(self), ch))
    {
        return lh_bool_false;
    }
    lh_json_reader_advance(self, 1U);
    return lh_bool_true;
}

lh_bool_t
lh_json_reader_skip_word(lh_json_reader_t *self, lh_str_view_t word)
{
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(word));
    const lh_str_cptr chars = lh_str_view_get_data(lh_addr_of(word));
    lh_usize_t i;

    for (i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (lh_math_ne(lh_json_reader_peek(self, i), chars[i]))
        {
            return lh_bool_false;
        }
    }
    lh_json_reader_advance(self, size);
    return lh_bool_true;
}

lh_bool_t
lh_json_reader_enter(lh_json_reader_t *self)
{
    const lh_usize_t depth = lh_math_add_one(lh_json_reader_get_depth(self));

    if (lh_math_gt(depth, LH_JSON_READER_DEPTH_MAX))
    {
        return lh_json_reader_fail(self, lh_json_error_too_deep);
    }
    lh_json_reader_set_depth(self, depth);
    return lh_bool_true;
}

lh_void
lh_json_reader_leave(lh_json_reader_t *self)
{
    lh_json_reader_set_depth(self, lh_math_sub_one(lh_json_reader_get_depth(self)));
}

/* ── values ──────────────────────────────────────────────────────────────── */

lh_bool_t
lh_json_reader_read_literal(lh_json_reader_t *self, lh_json_node_t *node)
{
    if (lh_json_reader_skip_word(self, lh_str_view_lit("null")))
    {
        lh_json_node_set_kind(node, lh_json_kind_null);
        return lh_bool_true;
    }
    if (lh_json_reader_skip_word(self, lh_str_view_lit("true")))
    {
        lh_json_node_set_kind(node, lh_json_kind_bool);
        lh_json_node_set_flag(node, lh_bool_true);
        return lh_bool_true;
    }
    if (lh_json_reader_skip_word(self, lh_str_view_lit("false")))
    {
        lh_json_node_set_kind(node, lh_json_kind_bool);
        lh_json_node_set_flag(node, lh_bool_false);
        return lh_bool_true;
    }
    return lh_json_reader_fail(self, lh_json_error_bad_literal);
}

lh_usize_t
lh_json_reader_skip_digits(lh_json_reader_t *self)
{
    lh_usize_t count = 0U;

    while (lh_char_is_digit(lh_json_reader_get_char(self)))
    {
        lh_json_reader_advance(self, 1U);
        count = lh_math_add_one(count);
    }
    return count;
}

lh_bool_t
lh_json_reader_read_number(lh_json_reader_t *self, lh_json_node_t *node)
{
    const lh_usize_t start = lh_json_reader_get_pos(self);

    lh_json_reader_skip_char(self, lh_char_map_minus);
    if (lh_json_reader_skip_char(self, lh_char_map_digit_0))
    {
        /* A leading zero is the whole integer part: `01` is not a number. */
        if (lh_char_is_digit(lh_json_reader_get_char(self)))
        {
            return lh_json_reader_fail(self, lh_json_error_bad_number);
        }
    }
    else if (lh_math_is_zero(lh_json_reader_skip_digits(self)))
    {
        return lh_json_reader_fail(self, lh_json_error_bad_number);
    }

    if (lh_json_reader_skip_char(self, lh_char_map_period) &&
        lh_math_is_zero(lh_json_reader_skip_digits(self)))
    {
        return lh_json_reader_fail(self, lh_json_error_bad_number);
    }

    if (lh_json_reader_skip_char(self, lh_char_map_small_e) ||
        lh_json_reader_skip_char(self, lh_char_map_capital_e))
    {
        if (!lh_json_reader_skip_char(self, lh_char_map_plus))
        {
            lh_json_reader_skip_char(self, lh_char_map_minus);
        }
        if (lh_math_is_zero(lh_json_reader_skip_digits(self)))
        {
            return lh_json_reader_fail(self, lh_json_error_bad_number);
        }
    }

    lh_json_node_set_kind(node, lh_json_kind_number);
    lh_str_append(lh_json_node_get_text_mut(node),
                  lh_str_view_get_data(lh_json_reader_get_text(self)) + start,
                  lh_math_sub(lh_json_reader_get_pos(self), start));
    return lh_bool_true;
}

lh_bool_t
lh_json_reader_read_hex4(lh_json_reader_t *self, lh_u32_t *code)
{
    lh_uint_t value = 0U;
    lh_usize_t i;

    lh_assert_runtime_ref(code);
    for (i = 0U; lh_math_lt(i, 4U); i = lh_math_add_one(i))
    {
        const lh_char_t ch = lh_json_reader_get_char(self);

        if (!lh_char_is_xdigit(ch))
        {
            return lh_json_reader_fail(self, lh_json_error_bad_unicode);
        }
        lh_char_xdigit_accumulate(lh_addr_of(value), lh_char_to_xdigit(ch));
        lh_json_reader_advance(self, 1U);
    }
    lh_ptr_deref(code) = value;
    return lh_bool_true;
}

lh_bool_t
lh_json_reader_read_unicode(lh_json_reader_t *self, lh_str_t *out)
{
    lh_char_t bytes[4];
    lh_u32_t code;
    lh_u32_t low;
    lh_u32_t size;

    if (!lh_json_reader_read_hex4(self, lh_addr_of(code)))
    {
        return lh_bool_false;
    }
    if (lh_math_ge(code, 0xDC00U) && lh_math_le(code, 0xDFFFU))
    {
        return lh_json_reader_fail(self, lh_json_error_bad_unicode);
    }
    if (lh_math_ge(code, 0xD800U) && lh_math_le(code, 0xDBFFU))
    {
        /* A high surrogate means nothing alone: the low half must follow as
           the next escape, and the two make one code point above U+FFFF. */
        if (!lh_json_reader_skip_word(self, lh_str_view_lit("\\u")) ||
            !lh_json_reader_read_hex4(self, lh_addr_of(low)))
        {
            return lh_json_reader_fail(self, lh_json_error_bad_unicode);
        }
        if (lh_math_lt(low, 0xDC00U) || lh_math_gt(low, 0xDFFFU))
        {
            return lh_json_reader_fail(self, lh_json_error_bad_unicode);
        }
        code = lh_math_add(0x10000U, lh_math_add(lh_math_bit_shl(lh_math_sub(code, 0xD800U), 10U),
                                                 lh_math_sub(low, 0xDC00U)));
    }

    size = lh_str_utf8_encode(bytes, 4U, code);
    if (lh_math_is_zero(size))
    {
        return lh_json_reader_fail(self, lh_json_error_bad_unicode);
    }
    lh_str_append(out, bytes, size);
    return lh_bool_true;
}

lh_bool_t
lh_json_reader_read_escape(lh_json_reader_t *self, lh_str_t *out)
{
    const lh_char_t ch = lh_json_reader_get_char(self);
    lh_char_t plain;

    switch (ch)
    {
    case lh_char_map_dquot:
    case lh_char_map_backslash:
    case lh_char_map_slash:
        plain = ch;
        break;
    case lh_char_map_small_b:
        plain = lh_char_map_bs;
        break;
    case lh_char_map_small_f:
        plain = lh_char_map_ff;
        break;
    case lh_char_map_small_n:
        plain = lh_char_map_lf;
        break;
    case lh_char_map_small_r:
        plain = lh_char_map_cr;
        break;
    case lh_char_map_small_t:
        plain = lh_char_map_ht;
        break;
    case lh_char_map_small_u:
        lh_json_reader_advance(self, 1U);
        return lh_json_reader_read_unicode(self, out);
    default:
        return lh_json_reader_is_end(self) ? lh_json_reader_fail_unexpected(self)
                                           : lh_json_reader_fail(self, lh_json_error_bad_escape);
    }
    lh_json_reader_advance(self, 1U);
    lh_str_push_back(out, plain);
    return lh_bool_true;
}

lh_bool_t
lh_json_reader_read_string_into(lh_json_reader_t *self, lh_str_t *out)
{
    const lh_str_cptr chars = lh_str_view_get_data(lh_json_reader_get_text(self));

    if (!lh_json_reader_skip_char(self, lh_char_map_dquot))
    {
        return lh_json_reader_fail_unexpected(self);
    }
    for (;;)
    {
        const lh_usize_t start = lh_json_reader_get_pos(self);
        lh_char_t ch = lh_json_reader_get_char(self);

        /* Copy a run of plain characters at once: most strings have no escape. */
        while (
            !lh_json_reader_is_end(self) && lh_math_ne(ch, lh_char_map_dquot) &&
            lh_math_ne(ch, lh_char_map_backslash) &&
            lh_math_ge(lh_cast_static(lh_byte_t, ch), lh_cast_static(lh_byte_t, lh_char_map_space)))
        {
            lh_json_reader_advance(self, 1U);
            ch = lh_json_reader_get_char(self);
        }
        if (lh_math_gt(lh_json_reader_get_pos(self), start))
        {
            lh_str_append(out, chars + start, lh_math_sub(lh_json_reader_get_pos(self), start));
        }

        if (lh_json_reader_is_end(self))
        {
            return lh_json_reader_fail_unexpected(self);
        }
        if (lh_json_reader_skip_char(self, lh_char_map_dquot))
        {
            return lh_bool_true;
        }
        if (!lh_json_reader_skip_char(self, lh_char_map_backslash))
        {
            return lh_json_reader_fail(self, lh_json_error_bad_string);
        }
        if (!lh_json_reader_read_escape(self, out))
        {
            return lh_bool_false;
        }
    }
}

lh_bool_t
lh_json_reader_read_string(lh_json_reader_t *self, lh_json_node_t *node)
{
    lh_json_node_set_kind(node, lh_json_kind_string);
    return lh_json_reader_read_string_into(self, lh_json_node_get_text_mut(node));
}

lh_bool_t
lh_json_reader_read_array(lh_json_reader_t *self, lh_json_node_t *node)
{
    if (!lh_json_reader_skip_char(self, lh_char_map_lbracket))
    {
        return lh_json_reader_fail_unexpected(self);
    }
    if (!lh_json_reader_enter(self))
    {
        return lh_bool_false;
    }
    lh_json_node_set_kind(node, lh_json_kind_array);

    lh_json_reader_skip_whitespace(self);
    if (lh_json_reader_skip_char(self, lh_char_map_rbracket))
    {
        lh_json_reader_leave(self);
        return lh_bool_true;
    }
    for (;;)
    {
        lh_json_node_t *child = lh_json_node_create();

        /* In the tree before it is read, so a failure halfway still frees it. */
        lh_json_node_push_back(node, child);
        if (!lh_json_reader_read_value(self, child))
        {
            return lh_bool_false;
        }
        lh_json_reader_skip_whitespace(self);
        if (lh_json_reader_skip_char(self, lh_char_map_rbracket))
        {
            lh_json_reader_leave(self);
            return lh_bool_true;
        }
        if (!lh_json_reader_skip_char(self, lh_char_map_comma))
        {
            return lh_json_reader_fail_unexpected(self);
        }
        lh_json_reader_skip_whitespace(self);
    }
}

lh_bool_t
lh_json_reader_read_object(lh_json_reader_t *self, lh_json_node_t *node)
{
    if (!lh_json_reader_skip_char(self, lh_char_map_lbrace))
    {
        return lh_json_reader_fail_unexpected(self);
    }
    if (!lh_json_reader_enter(self))
    {
        return lh_bool_false;
    }
    lh_json_node_set_kind(node, lh_json_kind_object);

    lh_json_reader_skip_whitespace(self);
    if (lh_json_reader_skip_char(self, lh_char_map_rbrace))
    {
        lh_json_reader_leave(self);
        return lh_bool_true;
    }
    for (;;)
    {
        lh_json_node_t *child = lh_json_node_create();

        lh_json_node_push_back(node, child);
        if (!lh_json_reader_read_string_into(self, lh_json_node_get_key_mut(child)))
        {
            return lh_bool_false;
        }
        lh_json_reader_skip_whitespace(self);
        if (!lh_json_reader_skip_char(self, lh_char_map_colon))
        {
            return lh_json_reader_fail_unexpected(self);
        }
        lh_json_reader_skip_whitespace(self);
        if (!lh_json_reader_read_value(self, child))
        {
            return lh_bool_false;
        }
        lh_json_reader_skip_whitespace(self);
        if (lh_json_reader_skip_char(self, lh_char_map_rbrace))
        {
            lh_json_reader_leave(self);
            return lh_bool_true;
        }
        if (!lh_json_reader_skip_char(self, lh_char_map_comma))
        {
            return lh_json_reader_fail_unexpected(self);
        }
        lh_json_reader_skip_whitespace(self);
    }
}

lh_bool_t
lh_json_reader_read_value(lh_json_reader_t *self, lh_json_node_t *node)
{
    const lh_char_t ch = lh_json_reader_get_char(self);

    if (lh_json_reader_is_end(self))
    {
        return lh_json_reader_fail_unexpected(self);
    }
    switch (ch)
    {
    case lh_char_map_lbrace:
        return lh_json_reader_read_object(self, node);
    case lh_char_map_lbracket:
        return lh_json_reader_read_array(self, node);
    case lh_char_map_dquot:
        return lh_json_reader_read_string(self, node);
    case lh_char_map_small_n:
    case lh_char_map_small_t:
    case lh_char_map_small_f:
        return lh_json_reader_read_literal(self, node);
    default:
        if (lh_math_eq(ch, lh_char_map_minus) || lh_char_is_digit(ch))
        {
            return lh_json_reader_read_number(self, node);
        }
        return lh_json_reader_fail_unexpected(self);
    }
}

lh_bool_t
lh_json_reader_read(lh_json_reader_t *self, lh_json_node_t *node)
{
    if (lh_json_reader_has_error(self))
    {
        return lh_bool_false;
    }
    lh_json_reader_skip_whitespace(self);
    if (!lh_json_reader_read_value(self, node))
    {
        return lh_bool_false;
    }
    lh_json_reader_skip_whitespace(self);
    if (!lh_json_reader_is_end(self))
    {
        return lh_json_reader_fail(self, lh_json_error_trailing);
    }
    return lh_bool_true;
}
