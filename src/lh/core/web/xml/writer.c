/**
 * @file writer.c
 * @brief Implementation of `lh/core/web/xml/writer.h`.
 */

#include <lh/core/web/xml/writer.h>
#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/web/xml/comment.h>
#include <lh/core/web/xml/escape.h>
#include <lh/core/web/xml/name.h>

/* ── accessors ───────────────────────────────────────────────────────────── */
lh_str_t *
lh_web_xml_writer_get_out_mut(lh_web_xml_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->out;
}

lh_void
lh_web_xml_writer_set_out(lh_web_xml_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(self);
    self->out = out;
}

lh_web_xml_dialect_t
lh_web_xml_writer_get_dialect(const lh_web_xml_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dialect;
}

lh_void
lh_web_xml_writer_set_dialect(lh_web_xml_writer_t *self, lh_web_xml_dialect_t dialect)
{
    lh_assert_runtime_ref(self);
    self->dialect = dialect;
}

lh_bool_t
lh_web_xml_writer_is_html(const lh_web_xml_writer_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(lh_web_xml_writer_get_dialect(self), lh_web_xml_dialect_html));
}

/* ── init ────────────────────────────────────────────────────────────────── */
lh_void
lh_web_xml_writer_init(lh_web_xml_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(out);
    lh_web_xml_writer_set_out(self, out);
    lh_web_xml_writer_set_dialect(self, lh_web_xml_dialect_xml);
}

/* ── text ────────────────────────────────────────────────────────────────── */
lh_void
lh_web_xml_writer_put_char(lh_web_xml_writer_t *self, lh_char_t ch)
{
    lh_str_push_back(lh_web_xml_writer_get_out_mut(self), ch);
}

lh_void
lh_web_xml_writer_put_view(lh_web_xml_writer_t *self, lh_str_view_t text)
{
    lh_str_append_view(lh_web_xml_writer_get_out_mut(self), text);
}

lh_void
lh_web_xml_writer_put_escaped(lh_web_xml_writer_t *self, const lh_str_t *text, lh_bool_t in_attr)
{
    lh_str_view_t view;

    lh_str_as_view(text, lh_addr_of(view));
    lh_web_xml_escape_append(lh_web_xml_writer_get_out_mut(self), view, in_attr);
}

lh_void
lh_web_xml_writer_put_str(lh_web_xml_writer_t *self, const lh_str_t *text)
{
    lh_str_view_t view;

    lh_str_as_view(text, lh_addr_of(view));
    lh_web_xml_writer_put_view(self, view);
}

lh_bool_t
lh_web_xml_writer_try_put_name(lh_web_xml_writer_t *self, const lh_str_t *name)
{
    lh_str_view_t view;

    lh_str_as_view(name, lh_addr_of(view));
    if (!lh_web_xml_name_is_valid(view))
    {
        return lh_bool_false;
    }
    lh_web_xml_writer_put_view(self, view);
    return lh_bool_true;
}

/* ── markup ──────────────────────────────────────────────────────────────── */
lh_void
lh_web_xml_writer_write_attr_value(lh_web_xml_writer_t *self, const lh_web_xml_attr_t *attr)
{
    lh_web_xml_writer_put_char(self, lh_char_map_equals);
    lh_web_xml_writer_put_char(self, lh_char_map_dquot);
    lh_web_xml_writer_put_escaped(self, lh_web_xml_attr_get_value(attr), lh_bool_true);
    lh_web_xml_writer_put_char(self, lh_char_map_dquot);
}

lh_bool_t
lh_web_xml_writer_try_write_comment(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    lh_str_view_t text;

    lh_str_as_view(lh_web_xml_node_get_text(node), lh_addr_of(text));
    if (!lh_web_xml_comment_is_valid(text))
    {
        return lh_bool_false;
    }
    lh_web_xml_writer_put_view(self, lh_str_view_lit("<!--"));
    lh_web_xml_writer_put_view(self, text);
    lh_web_xml_writer_put_view(self, lh_str_view_lit("-->"));
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_writer_try_write_cdata(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    const lh_str_view_t end = lh_str_view_lit("]]>");
    lh_str_view_t text;

    lh_str_as_view(lh_web_xml_node_get_text(node), lh_addr_of(text));
    if (lh_str_view_contains(lh_addr_of(text), lh_addr_of(end), lh_bool_false))
    {
        return lh_bool_false;
    }
    lh_web_xml_writer_put_view(self, lh_str_view_lit("<![CDATA["));
    lh_web_xml_writer_put_view(self, text);
    lh_web_xml_writer_put_view(self, end);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_writer_try_write_attr(lh_web_xml_writer_t *self, const lh_web_xml_attr_t *attr)
{
    const lh_bool_t bare = lh_web_xml_attr_get_bare(attr);

    if (bare && !lh_web_xml_writer_is_html(self))
    {
        return lh_bool_false;
    }
    lh_web_xml_writer_put_char(self, lh_char_map_space);
    if (!lh_web_xml_writer_try_put_name(self, lh_web_xml_attr_get_name(attr)))
    {
        return lh_bool_false;
    }
    if (!bare)
    {
        lh_web_xml_writer_write_attr_value(self, attr);
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_writer_try_write_close(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    lh_web_xml_writer_put_char(self, lh_char_map_less);
    lh_web_xml_writer_put_char(self, lh_char_map_slash);
    if (!lh_web_xml_writer_try_put_name(self, lh_web_xml_node_get_name(node)))
    {
        return lh_bool_false;
    }
    lh_web_xml_writer_put_char(self, lh_char_map_greater);
    return lh_bool_true;
}

lh_void
lh_web_xml_writer_write_doctype(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    lh_web_xml_writer_put_view(self, lh_web_xml_writer_is_html(self)
                                         ? lh_str_view_lit("<!doctype ")
                                         : lh_str_view_lit("<!DOCTYPE "));
    lh_web_xml_writer_put_str(self, lh_web_xml_node_get_text(node));
    lh_web_xml_writer_put_char(self, lh_char_map_greater);
}

lh_bool_t
lh_web_xml_writer_try_write_pi(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    const lh_str_view_t end = lh_str_view_lit("?>");
    lh_str_view_t text;

    lh_str_as_view(lh_web_xml_node_get_text(node), lh_addr_of(text));
    if (lh_str_view_contains(lh_addr_of(text), lh_addr_of(end), lh_bool_false))
    {
        return lh_bool_false;
    }
    lh_web_xml_writer_put_char(self, lh_char_map_less);
    lh_web_xml_writer_put_char(self, lh_char_map_question);
    if (!lh_web_xml_writer_try_put_name(self, lh_web_xml_node_get_name(node)))
    {
        return lh_bool_false;
    }
    if (!lh_str_view_is_empty(lh_addr_of(text)))
    {
        lh_web_xml_writer_put_char(self, lh_char_map_space);
        lh_web_xml_writer_put_view(self, text);
    }
    lh_web_xml_writer_put_view(self, end);
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_writer_try_write_open(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    lh_web_xml_writer_put_char(self, lh_char_map_less);
    if (!lh_web_xml_writer_try_put_name(self, lh_web_xml_node_get_name(node)))
    {
        return lh_bool_false;
    }
    for (const lh_web_xml_attr_t *attr = lh_web_xml_node_get_first_attr(node); lh_null_ne(attr);
         attr = lh_web_xml_node_get_next_attr(node, attr))
    {
        if (!lh_web_xml_writer_try_write_attr(self, attr))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_xml_writer_try_write(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    switch (lh_web_xml_node_get_kind(node))
    {
    case lh_web_xml_kind_document:
        return lh_web_xml_writer_try_write_children(self, node);
    case lh_web_xml_kind_element:
        if (!lh_web_xml_writer_try_write_open(self, node))
        {
            return lh_bool_false;
        }
        lh_web_xml_writer_put_char(self, lh_char_map_greater);
        return lh_web_xml_writer_try_write_children(self, node) &&
               lh_web_xml_writer_try_write_close(self, node);
    case lh_web_xml_kind_empty:
        if (lh_null_ne(lh_web_xml_node_get_first_child(node)) ||
            !lh_web_xml_writer_try_write_open(self, node))
        {
            return lh_bool_false;
        }
        /* A void element of HTML ends like a start tag; XML closes it in place. */
        if (!lh_web_xml_writer_is_html(self))
        {
            lh_web_xml_writer_put_char(self, lh_char_map_slash);
        }
        lh_web_xml_writer_put_char(self, lh_char_map_greater);
        return lh_bool_true;
    case lh_web_xml_kind_text:
        lh_web_xml_writer_put_escaped(self, lh_web_xml_node_get_text(node), lh_bool_false);
        return lh_bool_true;
    case lh_web_xml_kind_raw:
        lh_web_xml_writer_put_str(self, lh_web_xml_node_get_text(node));
        return lh_bool_true;
    case lh_web_xml_kind_comment:
        return lh_web_xml_writer_try_write_comment(self, node);
    case lh_web_xml_kind_cdata:
        return !lh_web_xml_writer_is_html(self) && lh_web_xml_writer_try_write_cdata(self, node);
    case lh_web_xml_kind_pi:
        return !lh_web_xml_writer_is_html(self) && lh_web_xml_writer_try_write_pi(self, node);
    case lh_web_xml_kind_doctype:
        lh_web_xml_writer_write_doctype(self, node);
        return lh_bool_true;
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_web_xml_writer_try_write_children(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    for (const lh_web_xml_node_t *child = lh_web_xml_node_get_first_child(node); lh_null_ne(child);
         child = lh_web_xml_node_get_next_child(node, child))
    {
        if (!lh_web_xml_writer_try_write(self, child))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_void
lh_web_xml_writer_write(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node)
{
    lh_runtime_check_ifn(lh_web_xml_writer_try_write(self, node),
                         lh_runtime_error_code_invalid_argument, "xml tree cannot be written");
}
