#include <lh/json/writer.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/char/map.h>
#include <lh/char/xdigit.h>
#include <lh/math/ops.h>
#include <lh/null.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_str_t *
lh_json_writer_get_out_mut(lh_json_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->out;
}

lh_void
lh_json_writer_set_out(lh_json_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(self);
    self->out = out;
}

lh_usize_t
lh_json_writer_get_indent(const lh_json_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->indent;
}

lh_void
lh_json_writer_set_indent(lh_json_writer_t *self, lh_usize_t indent)
{
    lh_assert_runtime_ref(self);
    self->indent = indent;
}

lh_usize_t
lh_json_writer_get_depth(const lh_json_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->depth;
}

lh_void
lh_json_writer_set_depth(lh_json_writer_t *self, lh_usize_t depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

lh_bool_t
lh_json_writer_is_pretty(const lh_json_writer_t *self)
{
    return lh_cast_static(lh_bool_t, !lh_math_is_zero(lh_json_writer_get_indent(self)));
}

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_json_writer_init(lh_json_writer_t *self, lh_str_t *out, lh_usize_t indent)
{
    lh_assert_runtime_ref(out);
    lh_json_writer_set_out(self, out);
    lh_json_writer_set_indent(self, indent);
    lh_json_writer_set_depth(self, 0U);
}

/* ── text ────────────────────────────────────────────────────────────────── */

lh_void
lh_json_writer_put_char(lh_json_writer_t *self, lh_char_t ch)
{
    lh_str_push_back(lh_json_writer_get_out_mut(self), ch);
}

lh_void
lh_json_writer_put_view(lh_json_writer_t *self, lh_str_view_t text)
{
    lh_str_append_view(lh_json_writer_get_out_mut(self), text);
}

lh_void
lh_json_writer_put_newline(lh_json_writer_t *self)
{
    lh_usize_t spaces;

    if (!lh_json_writer_is_pretty(self))
    {
        return;
    }
    lh_json_writer_put_char(self, lh_char_map_lf);
    for (spaces = lh_math_mul(lh_json_writer_get_indent(self), lh_json_writer_get_depth(self));
         !lh_math_is_zero(spaces); spaces = lh_math_sub_one(spaces))
    {
        lh_json_writer_put_char(self, lh_char_map_space);
    }
}

lh_void
lh_json_writer_put_escape(lh_json_writer_t *self, lh_char_t ch)
{
    const lh_byte_t code = lh_cast_static(lh_byte_t, ch);
    lh_char_t name;

    switch (ch)
    {
    case lh_char_map_dquot:
    case lh_char_map_backslash:
        name = ch;
        break;
    case lh_char_map_bs:
        name = lh_char_map_small_b;
        break;
    case lh_char_map_ff:
        name = lh_char_map_small_f;
        break;
    case lh_char_map_lf:
        name = lh_char_map_small_n;
        break;
    case lh_char_map_cr:
        name = lh_char_map_small_r;
        break;
    case lh_char_map_ht:
        name = lh_char_map_small_t;
        break;
    default:
        /* No short name: the code unit in hex. Control characters are below
           0x20, so the first two digits are always zero. */
        lh_json_writer_put_view(self, lh_str_view_lit("\\u00"));
        lh_json_writer_put_char(self, lh_char_from_xdigit(lh_math_bit_shr(code, 4U), lh_bool_true));
        lh_json_writer_put_char(self,
                                lh_char_from_xdigit(lh_math_bit_and(code, 0x0FU), lh_bool_true));
        return;
    }
    lh_json_writer_put_char(self, lh_char_map_backslash);
    lh_json_writer_put_char(self, name);
}

lh_bool_t
lh_json_writer_needs_escape(lh_char_t ch)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(ch, lh_char_map_dquot) ||
                                         lh_math_eq(ch, lh_char_map_backslash) ||
                                         lh_math_lt(lh_cast_static(lh_byte_t, ch),
                                                    lh_cast_static(lh_byte_t, lh_char_map_space)));
}

lh_void
lh_json_writer_put_string(lh_json_writer_t *self, lh_str_view_t text)
{
    const lh_str_cptr chars = lh_str_view_get_data(lh_addr_of(text));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(text));
    lh_str_t *out = lh_json_writer_get_out_mut(self);
    lh_usize_t start = 0U;
    lh_usize_t i;

    lh_json_writer_put_char(self, lh_char_map_dquot);
    for (i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (!lh_json_writer_needs_escape(chars[i]))
        {
            continue;
        }
        /* Everything since the last escape goes in as one run. */
        if (lh_math_gt(i, start))
        {
            lh_str_append(out, chars + start, lh_math_sub(i, start));
        }
        lh_json_writer_put_escape(self, chars[i]);
        start = lh_math_add_one(i);
    }
    if (lh_math_gt(size, start))
    {
        lh_str_append(out, chars + start, lh_math_sub(size, start));
    }
    lh_json_writer_put_char(self, lh_char_map_dquot);
}

/* ── values ──────────────────────────────────────────────────────────────── */

lh_void
lh_json_writer_write_array(lh_json_writer_t *self, const lh_json_node_t *node)
{
    const lh_json_node_t *child = lh_json_node_get_first(node);

    lh_json_writer_put_char(self, lh_char_map_lbracket);
    if (lh_null_eq(child))
    {
        lh_json_writer_put_char(self, lh_char_map_rbracket);
        return;
    }
    lh_json_writer_set_depth(self, lh_math_add_one(lh_json_writer_get_depth(self)));
    for (; lh_null_ne(child); child = lh_json_node_get_next(node, child))
    {
        lh_json_writer_put_newline(self);
        lh_json_writer_write(self, child);
        if (lh_null_ne(lh_json_node_get_next(node, child)))
        {
            lh_json_writer_put_char(self, lh_char_map_comma);
        }
    }
    lh_json_writer_set_depth(self, lh_math_sub_one(lh_json_writer_get_depth(self)));
    lh_json_writer_put_newline(self);
    lh_json_writer_put_char(self, lh_char_map_rbracket);
}

lh_void
lh_json_writer_write_object(lh_json_writer_t *self, const lh_json_node_t *node)
{
    const lh_json_node_t *child = lh_json_node_get_first(node);
    lh_str_view_t key;

    lh_json_writer_put_char(self, lh_char_map_lbrace);
    if (lh_null_eq(child))
    {
        lh_json_writer_put_char(self, lh_char_map_rbrace);
        return;
    }
    lh_json_writer_set_depth(self, lh_math_add_one(lh_json_writer_get_depth(self)));
    for (; lh_null_ne(child); child = lh_json_node_get_next(node, child))
    {
        lh_json_writer_put_newline(self);
        lh_str_as_view(lh_json_node_get_key(child), lh_addr_of(key));
        lh_json_writer_put_string(self, key);
        lh_json_writer_put_char(self, lh_char_map_colon);
        if (lh_json_writer_is_pretty(self))
        {
            lh_json_writer_put_char(self, lh_char_map_space);
        }
        lh_json_writer_write(self, child);
        if (lh_null_ne(lh_json_node_get_next(node, child)))
        {
            lh_json_writer_put_char(self, lh_char_map_comma);
        }
    }
    lh_json_writer_set_depth(self, lh_math_sub_one(lh_json_writer_get_depth(self)));
    lh_json_writer_put_newline(self);
    lh_json_writer_put_char(self, lh_char_map_rbrace);
}

lh_void
lh_json_writer_write(lh_json_writer_t *self, const lh_json_node_t *node)
{
    lh_str_view_t text;

    switch (lh_json_node_get_kind(node))
    {
    case lh_json_kind_bool:
        lh_json_writer_put_view(self, lh_json_node_get_flag(node) ? lh_str_view_lit("true")
                                                                  : lh_str_view_lit("false"));
        break;
    case lh_json_kind_number:
        lh_str_as_view(lh_json_node_get_text(node), lh_addr_of(text));
        lh_json_writer_put_view(self, text);
        break;
    case lh_json_kind_string:
        lh_json_writer_put_string(self, lh_json_node_get_str(node));
        break;
    case lh_json_kind_array:
        lh_json_writer_write_array(self, node);
        break;
    case lh_json_kind_object:
        lh_json_writer_write_object(self, node);
        break;
    default:
        lh_json_writer_put_view(self, lh_str_view_lit("null"));
        break;
    }
}
