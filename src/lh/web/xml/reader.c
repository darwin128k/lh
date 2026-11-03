/**
 * @file reader.c
 * @brief Implementation of `lh/web/xml/reader.h`.
 */

#include <lh/web/xml/reader.h>
#include <lh/assert.h>
#include <lh/char/xdigit.h>
#include <lh/str/utf8.h>
#include <lh/web/xml/char.h>
#include <lh/web/xml/comment.h>
#include <lh/web/xml/entity.h>
#include <lh/web/xml/name.h>

/** What ends a run of plain character data. */
static const lh_char_t m_web_xml_reader_text_stops[] = {lh_char_map_less, lh_char_map_ampersand,
                                                        lh_char_map_cr};

/** What ends a run of plain attribute value, besides its quote. */
static const lh_char_t m_web_xml_reader_value_stops[] = {
    lh_char_map_less, lh_char_map_ampersand, lh_char_map_ht, lh_char_map_lf, lh_char_map_cr};

/* ── accessors ───────────────────────────────────────────────────────────── */
lh_str_cursor_t *
lh_web_xml_reader_get_cursor_mut(lh_web_xml_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->cursor);
}

const lh_str_cursor_t *
lh_web_xml_reader_get_cursor(const lh_web_xml_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->cursor);
}

lh_usize_t
lh_web_xml_reader_get_depth(const lh_web_xml_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->depth;
}

lh_void
lh_web_xml_reader_set_depth(lh_web_xml_reader_t *self, lh_usize_t depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

lh_web_xml_error_t
lh_web_xml_reader_get_error(const lh_web_xml_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->error;
}

lh_void
lh_web_xml_reader_set_error(lh_web_xml_reader_t *self, lh_web_xml_error_t error)
{
    lh_assert_runtime_ref(self);
    self->error = error;
}

/* ── state and characters ────────────────────────────────────────────────── */
lh_void
lh_web_xml_reader_init(lh_web_xml_reader_t *self, lh_str_view_t text)
{
    lh_str_view_skip_bom(lh_addr_of(text));
    lh_str_cursor_init(lh_web_xml_reader_get_cursor_mut(self), text);
    lh_web_xml_reader_set_depth(self, 0U);
    lh_web_xml_reader_set_error(self, lh_web_xml_error_none);
}

lh_bool_t
lh_web_xml_reader_fail(lh_web_xml_reader_t *self, lh_web_xml_error_t error)
{
    lh_web_xml_reader_set_error(self, error);
    return lh_bool_false;
}

lh_bool_t
lh_web_xml_reader_has_error(const lh_web_xml_reader_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ne(lh_web_xml_reader_get_error(self), lh_web_xml_error_none));
}

lh_void
lh_web_xml_reader_leave(lh_web_xml_reader_t *self)
{
    lh_web_xml_reader_set_depth(self, lh_math_sub_one(lh_web_xml_reader_get_depth(self)));
}

lh_bool_t
lh_web_xml_reader_skip_newline(lh_web_xml_reader_t *self)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (lh_str_cursor_skip_char(cursor, lh_char_map_cr))
    {
        lh_str_cursor_skip_char(cursor, lh_char_map_lf);
        return lh_bool_true;
    }
    return lh_str_cursor_skip_char(cursor, lh_char_map_lf);
}

lh_bool_t
lh_web_xml_reader_fail_unexpected(lh_web_xml_reader_t *self)
{
    return lh_web_xml_reader_fail(self, lh_str_cursor_is_end(lh_web_xml_reader_get_cursor(self))
                                            ? lh_web_xml_error_unexpected_end
                                            : lh_web_xml_error_unexpected_char);
}

lh_bool_t
lh_web_xml_reader_enter(lh_web_xml_reader_t *self)
{
    const lh_usize_t depth = lh_math_add_one(lh_web_xml_reader_get_depth(self));

    if (lh_math_gt(depth, LH_WEB_XML_READER_DEPTH_MAX))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_too_deep);
    }
    lh_web_xml_reader_set_depth(self, depth);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read_name_into(lh_web_xml_reader_t *self, lh_str_t *out)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);
    const lh_usize_t start = lh_str_cursor_get_pos(cursor);

    if (lh_str_cursor_is_end(cursor))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_unexpected_end);
    }
    if (!lh_web_xml_name_is_start_char(lh_str_cursor_get_char(cursor)))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_bad_name);
    }
    do
    {
        lh_str_cursor_advance(cursor, 1U);
    } while (!lh_str_cursor_is_end(cursor) &&
             lh_web_xml_name_is_char(lh_str_cursor_get_char(cursor)));

    lh_str_view_t name;

    lh_str_cursor_get_since(cursor, start, lh_addr_of(name));
    lh_str_append_view(out, name);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read_char_reference(lh_web_xml_reader_t *self, lh_str_t *out)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);
    const lh_uint_t radix =
        lh_str_cursor_skip_char(cursor, lh_char_map_small_x) ? LH_CHAR_XDIGIT_RADIX : 10U;
    lh_uint_t code = 0U;
    lh_usize_t digits = 0U;

    while (lh_char_is_radix_digit(lh_str_cursor_get_char(cursor), radix))
    {
        /* Past the last code point the value is wrong anyway; stop growing it
           so a long run of digits cannot wrap it back into range. */
        if (lh_math_le(code, 0x10FFFFU))
        {
            lh_char_digit_accumulate_radix(
                lh_addr_of(code), lh_char_to_xdigit(lh_str_cursor_get_char(cursor)), radix);
        }
        lh_str_cursor_advance(cursor, 1U);
        digits = lh_math_add_one(digits);
    }
    if (lh_math_is_zero(digits) || !lh_str_cursor_skip_char(cursor, lh_char_map_semicolon) ||
        !lh_web_xml_char_is_valid(code))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_bad_reference);
    }

    lh_char_t bytes[4];
    const lh_u32_t size = lh_str_utf8_encode(bytes, 4U, code);

    if (lh_math_is_zero(size))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_bad_reference);
    }
    lh_str_append(out, bytes, size);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_expect_char(lh_web_xml_reader_t *self, lh_char_t ch)
{
    if (lh_str_cursor_skip_char(lh_web_xml_reader_get_cursor_mut(self), ch))
    {
        return lh_bool_true;
    }
    return lh_web_xml_reader_fail_unexpected(self);
}

lh_bool_t
lh_web_xml_reader_read_reference(lh_web_xml_reader_t *self, lh_str_t *out)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (!lh_web_xml_reader_expect_char(self, lh_char_map_ampersand))
    {
        return lh_bool_false;
    }
    if (lh_str_cursor_skip_char(cursor, lh_char_map_hash))
    {
        return lh_web_xml_reader_read_char_reference(self, out);
    }

    const lh_usize_t start = lh_str_cursor_get_pos(cursor);

    while (!lh_str_cursor_is_end(cursor) && lh_web_xml_name_is_char(lh_str_cursor_get_char(cursor)))
    {
        lh_str_cursor_advance(cursor, 1U);
    }

    lh_str_view_t name;

    lh_str_cursor_get_since(cursor, start, lh_addr_of(name));
    const lh_char_t ch = lh_web_xml_entity_get_char(name);

    if (lh_math_eq(ch, lh_char_map_nul) || !lh_str_cursor_skip_char(cursor, lh_char_map_semicolon))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_bad_reference);
    }
    lh_str_push_back(out, ch);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read_text_into(lh_web_xml_reader_t *self, lh_str_t *out)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    for (;;)
    {
        const lh_usize_t start = lh_str_cursor_get_pos(cursor);

        /* Copy a run of plain characters at once: most text has no reference. */
        while (!lh_str_cursor_is_end(cursor) &&
               !lh_str_ptr_contains_char(m_web_xml_reader_text_stops,
                                         lh_array_ptr_get_size(m_web_xml_reader_text_stops),
                                         lh_str_cursor_get_char(cursor)))
        {
            lh_str_cursor_advance(cursor, 1U);
        }

        lh_str_view_t run;

        lh_str_cursor_get_since(cursor, start, lh_addr_of(run));
        lh_str_append_view(out, run);
        if (lh_str_cursor_is_end(cursor) ||
            lh_math_eq(lh_str_cursor_get_char(cursor), lh_char_map_less))
        {
            return lh_bool_true;
        }
        if (lh_web_xml_reader_skip_newline(self))
        {
            lh_str_push_back(out, lh_char_map_lf);
        }
        else if (!lh_web_xml_reader_read_reference(self, out))
        {
            return lh_bool_false;
        }
    }
}

lh_bool_t
lh_web_xml_reader_read_attr_value_into(lh_web_xml_reader_t *self, lh_str_t *out)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);
    const lh_char_t quote = lh_str_cursor_get_char(cursor);

    if ((lh_math_ne(quote, lh_char_map_dquot) && lh_math_ne(quote, lh_char_map_squote)) ||
        lh_str_cursor_is_end(cursor))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }
    lh_str_cursor_advance(cursor, 1U);
    for (;;)
    {
        const lh_usize_t start = lh_str_cursor_get_pos(cursor);

        while (!lh_str_cursor_is_end(cursor) && lh_math_ne(lh_str_cursor_get_char(cursor), quote) &&
               !lh_str_ptr_contains_char(m_web_xml_reader_value_stops,
                                         lh_array_ptr_get_size(m_web_xml_reader_value_stops),
                                         lh_str_cursor_get_char(cursor)))
        {
            lh_str_cursor_advance(cursor, 1U);
        }

        lh_str_view_t run;

        lh_str_cursor_get_since(cursor, start, lh_addr_of(run));
        lh_str_append_view(out, run);
        if (lh_str_cursor_skip_char(cursor, quote))
        {
            return lh_bool_true;
        }
        if (lh_str_cursor_is_end(cursor) ||
            lh_math_eq(lh_str_cursor_get_char(cursor), lh_char_map_less))
        {
            return lh_web_xml_reader_fail_unexpected(self);
        }
        if (lh_math_eq(lh_str_cursor_get_char(cursor), lh_char_map_ampersand))
        {
            if (!lh_web_xml_reader_read_reference(self, out))
            {
                return lh_bool_false;
            }
            continue;
        }
        /* Attribute-value normalization: a tab or a line end is one space. */
        if (!lh_web_xml_reader_skip_newline(self))
        {
            lh_str_cursor_advance(cursor, 1U);
        }
        lh_str_push_back(out, lh_char_map_space);
    }
}

/* ── nodes ───────────────────────────────────────────────────────────────── */
lh_bool_t
lh_web_xml_reader_read_comment(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    const lh_str_view_t end = lh_str_view_lit("-->");
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (!lh_str_cursor_skip_word(cursor, lh_str_view_lit("<!--"), lh_bool_false))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }

    const lh_usize_t start = lh_str_cursor_get_pos(cursor);
    const lh_usize_t size = lh_str_cursor_find(cursor, end, lh_bool_false);

    if (lh_math_eq(size, LH_STR_VIEW_INVALID))
    {
        lh_str_cursor_advance(cursor, lh_str_cursor_get_left(cursor));
        return lh_web_xml_reader_fail(self, lh_web_xml_error_unexpected_end);
    }
    lh_str_cursor_advance(cursor, size);

    lh_str_view_t text;

    lh_str_cursor_get_since(cursor, start, lh_addr_of(text));
    if (!lh_web_xml_comment_is_valid(text))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_bad_comment);
    }
    lh_web_xml_node_set_comment(node, text);
    lh_str_cursor_advance(cursor, lh_str_view_get_size(lh_addr_of(end)));
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read_cdata(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    const lh_str_view_t end = lh_str_view_lit("]]>");
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (!lh_str_cursor_skip_word(cursor, lh_str_view_lit("<![CDATA["), lh_bool_false))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }

    const lh_usize_t start = lh_str_cursor_get_pos(cursor);
    const lh_usize_t size = lh_str_cursor_find(cursor, end, lh_bool_false);

    if (lh_math_eq(size, LH_STR_VIEW_INVALID))
    {
        lh_str_cursor_advance(cursor, lh_str_cursor_get_left(cursor));
        return lh_web_xml_reader_fail(self, lh_web_xml_error_unexpected_end);
    }
    lh_str_cursor_advance(cursor, size);

    lh_str_view_t text;

    lh_str_cursor_get_since(cursor, start, lh_addr_of(text));
    lh_web_xml_node_set_cdata(node, text);
    lh_str_cursor_advance(cursor, lh_str_view_get_size(lh_addr_of(end)));
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read_pi(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    const lh_str_view_t end = lh_str_view_lit("?>");
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (!lh_str_cursor_skip_word(cursor, lh_str_view_lit("<?"), lh_bool_false))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }
    lh_web_xml_node_clear(node);
    lh_web_xml_node_set_kind(node, lh_web_xml_kind_pi);
    if (!lh_web_xml_reader_read_name_into(self, lh_web_xml_node_get_name_mut(node)))
    {
        return lh_bool_false;
    }
    /* The target ends at whitespace or right at `?>`; anything else is glued to it. */
    if (lh_math_is_zero(lh_str_cursor_skip_whitespace(cursor)) &&
        !lh_str_cursor_is_word(cursor, end, lh_bool_false))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }

    const lh_usize_t start = lh_str_cursor_get_pos(cursor);
    const lh_usize_t size = lh_str_cursor_find(cursor, end, lh_bool_false);

    if (lh_math_eq(size, LH_STR_VIEW_INVALID))
    {
        lh_str_cursor_advance(cursor, lh_str_cursor_get_left(cursor));
        return lh_web_xml_reader_fail(self, lh_web_xml_error_unexpected_end);
    }
    lh_str_cursor_advance(cursor, size);

    lh_str_view_t text;

    lh_str_cursor_get_since(cursor, start, lh_addr_of(text));
    lh_str_assign_view(lh_web_xml_node_get_text_mut(node), text);
    lh_str_cursor_advance(cursor, lh_str_view_get_size(lh_addr_of(end)));
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read_doctype_body(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (lh_math_is_zero(lh_str_cursor_skip_whitespace(cursor)))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }

    const lh_usize_t start = lh_str_cursor_get_pos(cursor);
    lh_char_t quote = lh_char_map_nul;
    lh_bool_t in_subset = lh_bool_false;

    /* The declaration ends at the first `>` outside quotes and outside the
       internal subset; the subset ends at the first `]` outside quotes. */
    while (!lh_str_cursor_is_end(cursor))
    {
        const lh_char_t ch = lh_str_cursor_get_char(cursor);

        if (lh_math_ne(quote, lh_char_map_nul))
        {
            if (lh_math_eq(ch, quote))
            {
                quote = lh_char_map_nul;
            }
        }
        else if (lh_math_eq(ch, lh_char_map_dquot) || lh_math_eq(ch, lh_char_map_squote))
        {
            quote = ch;
        }
        else if (lh_math_eq(ch, lh_char_map_lbracket))
        {
            in_subset = lh_bool_true;
        }
        else if (lh_math_eq(ch, lh_char_map_rbracket))
        {
            in_subset = lh_bool_false;
        }
        else if (lh_math_eq(ch, lh_char_map_greater) && !in_subset)
        {
            lh_str_view_t text;

            lh_str_cursor_get_since(cursor, start, lh_addr_of(text));
            lh_web_xml_node_set_doctype(node, text);
            lh_str_cursor_advance(cursor, 1U);
            return lh_bool_true;
        }
        lh_str_cursor_advance(cursor, 1U);
    }
    return lh_web_xml_reader_fail(self, lh_web_xml_error_unexpected_end);
}

lh_bool_t
lh_web_xml_reader_read_doctype(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    if (!lh_str_cursor_skip_word(lh_web_xml_reader_get_cursor_mut(self),
                                 lh_str_view_lit("<!DOCTYPE"), lh_bool_false))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }
    return lh_web_xml_reader_read_doctype_body(self, node);
}

lh_bool_t
lh_web_xml_reader_read_close(lh_web_xml_reader_t *self, const lh_web_xml_node_t *node,
                             lh_bool_t ignore_case)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);
    lh_str_view_t name;

    if (!lh_str_cursor_skip_word(cursor, lh_str_view_lit("</"), lh_bool_false))
    {
        return lh_web_xml_reader_fail_unexpected(self);
    }
    lh_str_as_view(lh_web_xml_node_get_name(node), lh_addr_of(name));
    /* The name must end where the open element's does: `</ab>` does not close `<a>`. */
    if (!lh_str_cursor_skip_word(cursor, name, ignore_case) ||
        (!lh_str_cursor_is_end(cursor) && lh_web_xml_name_is_char(lh_str_cursor_get_char(cursor))))
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_mismatched_tag);
    }
    lh_str_cursor_skip_whitespace(cursor);
    return lh_web_xml_reader_expect_char(self, lh_char_map_greater);
}

lh_bool_t
lh_web_xml_reader_read_attr(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);
    lh_web_xml_attr_t *attr = lh_web_xml_attr_create();
    lh_str_view_t name;

    if (!lh_web_xml_reader_read_name_into(self, lh_web_xml_attr_get_name_mut(attr)))
    {
        lh_web_xml_attr_destroy(attr);
        return lh_bool_false;
    }
    lh_str_as_view(lh_web_xml_attr_get_name(attr), lh_addr_of(name));
    if (lh_null_ne(lh_web_xml_node_find_attr(node, name)))
    {
        lh_web_xml_attr_destroy(attr);
        return lh_web_xml_reader_fail(self, lh_web_xml_error_duplicate_attr);
    }
    /* In the element before its value is read, so a failure still frees it. */
    lh_web_xml_node_push_attr(node, attr);
    lh_str_cursor_skip_whitespace(cursor);
    if (!lh_web_xml_reader_expect_char(self, lh_char_map_equals))
    {
        return lh_bool_false;
    }
    lh_str_cursor_skip_whitespace(cursor);
    return lh_web_xml_reader_read_attr_value_into(self, lh_web_xml_attr_get_value_mut(attr));
}

lh_bool_t
lh_web_xml_reader_read_text(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    lh_web_xml_node_clear(node);
    lh_web_xml_node_set_kind(node, lh_web_xml_kind_text);
    return lh_web_xml_reader_read_text_into(self, lh_web_xml_node_get_text_mut(node));
}

lh_bool_t
lh_web_xml_reader_read_content(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    for (;;)
    {
        if (lh_str_cursor_is_end(cursor))
        {
            return lh_web_xml_reader_fail(self, lh_web_xml_error_unexpected_end);
        }
        if (lh_str_cursor_is_word(cursor, lh_str_view_lit("</"), lh_bool_false))
        {
            return lh_bool_true;
        }

        lh_web_xml_node_t *child = lh_web_xml_node_create();
        lh_bool_t read;

        /* In the tree before it is read, so a failure halfway still frees it. */
        lh_web_xml_node_push_child(node, child);
        if (lh_str_cursor_is_word(cursor, lh_str_view_lit("<!--"), lh_bool_false))
        {
            read = lh_web_xml_reader_read_comment(self, child);
        }
        else if (lh_str_cursor_is_word(cursor, lh_str_view_lit("<![CDATA["), lh_bool_false))
        {
            read = lh_web_xml_reader_read_cdata(self, child);
        }
        else if (lh_str_cursor_is_word(cursor, lh_str_view_lit("<?"), lh_bool_false))
        {
            read = lh_web_xml_reader_read_pi(self, child);
        }
        else if (lh_math_eq(lh_str_cursor_get_char(cursor), lh_char_map_less))
        {
            read = lh_web_xml_reader_read_element(self, child);
        }
        else
        {
            read = lh_web_xml_reader_read_text(self, child);
        }
        if (!read)
        {
            return lh_bool_false;
        }
    }
}

lh_bool_t
lh_web_xml_reader_read_element(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);

    if (!lh_web_xml_reader_expect_char(self, lh_char_map_less))
    {
        return lh_bool_false;
    }
    lh_web_xml_node_clear(node);
    lh_web_xml_node_set_kind(node, lh_web_xml_kind_element);
    if (!lh_web_xml_reader_read_name_into(self, lh_web_xml_node_get_name_mut(node)))
    {
        return lh_bool_false;
    }
    for (;;)
    {
        const lh_usize_t spaces = lh_str_cursor_skip_whitespace(cursor);

        if (lh_str_cursor_skip_word(cursor, lh_str_view_lit("/>"), lh_bool_false))
        {
            lh_web_xml_node_set_kind(node, lh_web_xml_kind_empty);
            return lh_bool_true;
        }
        if (lh_str_cursor_skip_char(cursor, lh_char_map_greater))
        {
            break;
        }
        /* An attribute is set off from the name or the one before it. */
        if (lh_math_is_zero(spaces))
        {
            return lh_web_xml_reader_fail_unexpected(self);
        }
        if (!lh_web_xml_reader_read_attr(self, node))
        {
            return lh_bool_false;
        }
    }
    if (!lh_web_xml_reader_enter(self) || !lh_web_xml_reader_read_content(self, node) ||
        !lh_web_xml_reader_read_close(self, node, lh_bool_false))
    {
        return lh_bool_false;
    }
    lh_web_xml_reader_leave(self);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_reader_read(lh_web_xml_reader_t *self, lh_web_xml_node_t *node)
{
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(self);
    lh_bool_t has_root = lh_bool_false;

    if (lh_web_xml_reader_has_error(self))
    {
        return lh_bool_false;
    }
    lh_web_xml_node_set_document(node);
    for (;;)
    {
        lh_str_cursor_skip_whitespace(cursor);
        if (lh_str_cursor_is_end(cursor))
        {
            break;
        }

        const lh_bool_t is_doctype =
            lh_str_cursor_is_word(cursor, lh_str_view_lit("<!DOCTYPE"), lh_bool_false);
        const lh_bool_t is_comment =
            lh_str_cursor_is_word(cursor, lh_str_view_lit("<!--"), lh_bool_false);
        const lh_bool_t is_pi = lh_str_cursor_is_word(cursor, lh_str_view_lit("<?"), lh_bool_false);

        /* Past the root only comments and processing instructions may follow;
           anything that is not markup is text, which has no place here. */
        if ((has_root && !is_comment && !is_pi) ||
            lh_math_ne(lh_str_cursor_get_char(cursor), lh_char_map_less))
        {
            return lh_web_xml_reader_fail(self, lh_web_xml_error_trailing);
        }

        lh_web_xml_node_t *child = lh_web_xml_node_create();
        lh_bool_t read;

        lh_web_xml_node_push_child(node, child);
        if (is_doctype)
        {
            read = lh_web_xml_reader_read_doctype(self, child);
        }
        else if (is_comment)
        {
            read = lh_web_xml_reader_read_comment(self, child);
        }
        else if (is_pi)
        {
            read = lh_web_xml_reader_read_pi(self, child);
        }
        else
        {
            read = lh_web_xml_reader_read_element(self, child);
            has_root = lh_bool_true;
        }
        if (!read)
        {
            return lh_bool_false;
        }
    }
    if (!has_root)
    {
        return lh_web_xml_reader_fail(self, lh_web_xml_error_no_root);
    }
    return lh_bool_true;
}
