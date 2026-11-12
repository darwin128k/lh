/**
 * @file reader.c
 * @brief Implementation of `lh/core/web/html/reader.h`.
 */

#include <lh/core/web/html/reader.h>
#include <lh/core/assert.h>
#include <lh/core/char/space.h>
#include <lh/core/web/html/rawtext.h>
#include <lh/core/web/html/void.h>

/** What ends an unquoted attribute value, besides whitespace and `&`. */
static const lh_char_t m_web_html_reader_unquoted_stops[] = {
    lh_char_map_dquot,   lh_char_map_squote, lh_char_map_equals,   lh_char_map_less,
    lh_char_map_greater, lh_char_map_grave,  lh_char_map_ampersand};

/* ── accessors ───────────────────────────────────────────────────────────── */
lh_web_xml_reader_t *
lh_web_html_reader_get_xml_mut(lh_web_html_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->xml);
}

const lh_web_xml_reader_t *
lh_web_html_reader_get_xml(const lh_web_html_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->xml);
}

lh_web_xml_error_t
lh_web_html_reader_get_error(const lh_web_html_reader_t *self)
{
    return lh_web_xml_reader_get_error(lh_web_html_reader_get_xml(self));
}

/* ── init ────────────────────────────────────────────────────────────────── */
lh_void
lh_web_html_reader_init(lh_web_html_reader_t *self, lh_str_view_t text)
{
    lh_web_xml_reader_init(lh_web_html_reader_get_xml_mut(self), text);
}

/* ── steps ───────────────────────────────────────────────────────────────── */
lh_bool_t
lh_web_html_reader_read_unquoted_into(lh_web_html_reader_t *self, lh_str_t *out)
{
    lh_web_xml_reader_t *xml = lh_web_html_reader_get_xml_mut(self);
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(xml);
    const lh_usize_t first = lh_str_cursor_get_pos(cursor);

    for (;;)
    {
        const lh_usize_t start = lh_str_cursor_get_pos(cursor);

        while (!lh_str_cursor_is_end(cursor) &&
               !lh_char_is_whitespace(lh_str_cursor_get_char(cursor)) &&
               !lh_str_ptr_contains_char(m_web_html_reader_unquoted_stops,
                                         lh_array_ptr_get_size(m_web_html_reader_unquoted_stops),
                                         lh_str_cursor_get_char(cursor)))
        {
            lh_str_cursor_advance(cursor, 1U);
        }

        lh_str_view_t run;

        lh_str_cursor_get_since(cursor, start, lh_addr_of(run));
        lh_str_append_view(out, run);
        if (lh_str_cursor_is_end(cursor) ||
            lh_math_ne(lh_str_cursor_get_char(cursor), lh_char_map_ampersand))
        {
            break;
        }
        if (!lh_web_xml_reader_read_reference(xml, out))
        {
            return lh_bool_false;
        }
    }
    if (lh_math_eq(lh_str_cursor_get_pos(cursor), first))
    {
        return lh_web_xml_reader_fail_unexpected(xml);
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_html_reader_read_rawtext(lh_web_html_reader_t *self, lh_web_xml_node_t *node)
{
    lh_web_xml_reader_t *xml = lh_web_html_reader_get_xml_mut(self);
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(xml);
    const lh_usize_t start = lh_str_cursor_get_pos(cursor);

    /* The body ends at `</name`, in any case; nothing inside it is markup. */
    lh_str_t end;

    lh_str_init(lh_addr_of(end));
    lh_str_append_view(lh_addr_of(end), lh_str_view_lit("</"));
    lh_str_append_str(lh_addr_of(end), lh_web_xml_node_get_name(node));

    lh_str_view_t view;

    lh_str_as_view(lh_addr_of(end), lh_addr_of(view));

    const lh_usize_t size = lh_str_cursor_find(cursor, view, lh_bool_true);

    lh_str_deinit(lh_addr_of(end));
    if (lh_math_eq(size, LH_STR_VIEW_INVALID))
    {
        lh_str_cursor_advance(cursor, lh_str_cursor_get_left(cursor));
        return lh_web_xml_reader_fail(xml, lh_web_xml_error_unexpected_end);
    }
    lh_str_cursor_advance(cursor, size);
    if (!lh_math_is_zero(size))
    {
        lh_str_view_t body;

        lh_str_cursor_get_since(cursor, start, lh_addr_of(body));
        lh_web_xml_node_push_child(node, lh_web_xml_node_create_raw(body));
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_html_reader_read_attr(lh_web_html_reader_t *self, lh_web_xml_node_t *node)
{
    lh_web_xml_reader_t *xml = lh_web_html_reader_get_xml_mut(self);
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(xml);
    lh_web_xml_attr_t *attr = lh_web_xml_attr_create();

    if (!lh_web_xml_reader_read_name_into(xml, lh_web_xml_attr_get_name_mut(attr)))
    {
        lh_web_xml_attr_destroy(attr);
        return lh_bool_false;
    }
    lh_str_view_t name;
    lh_str_as_view(lh_web_xml_attr_get_name(attr), lh_addr_of(name));
    if (lh_null_ne(lh_web_xml_node_find_attr(node, name)))
    {
        lh_web_xml_attr_destroy(attr);
        return lh_web_xml_reader_fail(xml, lh_web_xml_error_duplicate_attr);
    }
    /* In the element before its value is read, so a failure still frees it. */
    lh_web_xml_node_push_attr(node, attr);

    const lh_usize_t after_name = lh_str_cursor_get_pos(cursor);

    lh_str_cursor_skip_whitespace(cursor);
    if (!lh_str_cursor_skip_char(cursor, lh_char_map_equals))
    {
        /* A bare attribute: the whitespace after it belongs to the tag. */
        lh_str_cursor_set_pos(cursor, after_name);
        lh_web_xml_attr_set_bare(attr, lh_bool_true);
        return lh_bool_true;
    }
    lh_str_cursor_skip_whitespace(cursor);

    const lh_char_t quote = lh_str_cursor_get_char(cursor);

    if (lh_math_eq(quote, lh_char_map_dquot) || lh_math_eq(quote, lh_char_map_squote))
    {
        return lh_web_xml_reader_read_attr_value_into(xml, lh_web_xml_attr_get_value_mut(attr));
    }
    return lh_web_html_reader_read_unquoted_into(self, lh_web_xml_attr_get_value_mut(attr));
}

lh_bool_t
lh_web_html_reader_read_element(lh_web_html_reader_t *self, lh_web_xml_node_t *node)
{
    lh_web_xml_reader_t *xml = lh_web_html_reader_get_xml_mut(self);
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(xml);

    if (!lh_web_xml_reader_expect_char(xml, lh_char_map_less))
    {
        return lh_bool_false;
    }
    lh_web_xml_node_clear(node);
    lh_web_xml_node_set_kind(node, lh_web_xml_kind_element);
    if (!lh_web_xml_reader_read_name_into(xml, lh_web_xml_node_get_name_mut(node)))
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
            return lh_web_xml_reader_fail_unexpected(xml);
        }
        if (!lh_web_html_reader_read_attr(self, node))
        {
            return lh_bool_false;
        }
    }

    lh_str_view_t name;

    lh_str_as_view(lh_web_xml_node_get_name(node), lh_addr_of(name));
    if (lh_web_html_void_is_name(name))
    {
        lh_web_xml_node_set_kind(node, lh_web_xml_kind_empty);
        return lh_bool_true;
    }
    if (!lh_web_xml_reader_enter(xml))
    {
        return lh_bool_false;
    }
    if (lh_web_html_rawtext_is_name(name) ? !lh_web_html_reader_read_rawtext(self, node)
                                          : !lh_web_html_reader_read_content(self, node))
    {
        return lh_bool_false;
    }
    if (!lh_web_xml_reader_read_close(xml, node, lh_bool_true))
    {
        return lh_bool_false;
    }
    lh_web_xml_reader_leave(xml);
    return lh_bool_true;
}

lh_bool_t
lh_web_html_reader_read_content(lh_web_html_reader_t *self, lh_web_xml_node_t *node)
{
    lh_web_xml_reader_t *xml = lh_web_html_reader_get_xml_mut(self);
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(xml);

    for (;;)
    {
        if (lh_str_cursor_is_end(cursor))
        {
            return lh_web_xml_reader_fail(xml, lh_web_xml_error_unexpected_end);
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
            read = lh_web_xml_reader_read_comment(xml, child);
        }
        else if (lh_math_eq(lh_str_cursor_get_char(cursor), lh_char_map_less))
        {
            read = lh_web_html_reader_read_element(self, child);
        }
        else
        {
            read = lh_web_xml_reader_read_text(xml, child);
        }
        if (!read)
        {
            return lh_bool_false;
        }
    }
}

lh_bool_t
lh_web_html_reader_read(lh_web_html_reader_t *self, lh_web_xml_node_t *node)
{
    lh_web_xml_reader_t *xml = lh_web_html_reader_get_xml_mut(self);
    lh_str_cursor_t *cursor = lh_web_xml_reader_get_cursor_mut(xml);

    if (lh_web_xml_reader_has_error(xml))
    {
        return lh_bool_false;
    }
    lh_web_xml_node_set_document(node);
    for (;;)
    {
        lh_str_cursor_skip_whitespace(cursor);
        if (lh_str_cursor_is_end(cursor))
        {
            return lh_bool_true;
        }
        if (lh_math_ne(lh_str_cursor_get_char(cursor), lh_char_map_less))
        {
            return lh_web_xml_reader_fail(xml, lh_web_xml_error_trailing);
        }

        lh_web_xml_node_t *child = lh_web_xml_node_create();
        lh_bool_t read;

        lh_web_xml_node_push_child(node, child);
        if (lh_str_cursor_skip_word(cursor, lh_str_view_lit("<!doctype"), lh_bool_true))
        {
            read = lh_web_xml_reader_read_doctype_body(xml, child);
        }
        else if (lh_str_cursor_is_word(cursor, lh_str_view_lit("<!--"), lh_bool_false))
        {
            read = lh_web_xml_reader_read_comment(xml, child);
        }
        else
        {
            read = lh_web_html_reader_read_element(self, child);
        }
        if (!read)
        {
            return lh_bool_false;
        }
    }
}
