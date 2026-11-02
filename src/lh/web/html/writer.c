/**
 * @file writer.c
 * @brief Implementation of `lh/web/html/writer.h`.
 */

#include <lh/web/html/writer.h>
#include <lh/assert.h>
#include <lh/char/map.h>
#include <lh/web/html/escape.h>
#include <lh/web/html/name.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_str_t *
lh_web_html_writer_get_out_mut(lh_web_html_writer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->out;
}

lh_void
lh_web_html_writer_set_out(lh_web_html_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(self);
    self->out = out;
}

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_web_html_writer_init(lh_web_html_writer_t *self, lh_str_t *out)
{
    lh_assert_runtime_ref(out);
    lh_web_html_writer_set_out(self, out);
}

/* ── text ────────────────────────────────────────────────────────────────── */

lh_void
lh_web_html_writer_put_char(lh_web_html_writer_t *self, lh_char_t ch)
{
    lh_str_push_back(lh_web_html_writer_get_out_mut(self), ch);
}

lh_void
lh_web_html_writer_put_view(lh_web_html_writer_t *self, lh_str_view_t text)
{
    lh_str_append_view(lh_web_html_writer_get_out_mut(self), text);
}

lh_bool_t
lh_web_html_writer_try_put_name(lh_web_html_writer_t *self, const lh_str_t *name)
{
    lh_str_view_t view;

    lh_str_as_view(name, lh_addr_of(view));
    if (!lh_web_html_name_is_valid(view))
    {
        return lh_bool_false;
    }
    lh_web_html_writer_put_view(self, view);
    return lh_bool_true;
}

/* ── markup ──────────────────────────────────────────────────────────────── */

lh_bool_t
lh_web_html_writer_try_write_attr(lh_web_html_writer_t *self, const lh_web_html_attr_t *attr)
{
    lh_str_view_t value;

    lh_web_html_writer_put_char(self, lh_char_map_space);
    if (!lh_web_html_writer_try_put_name(self, lh_web_html_attr_get_name(attr)))
    {
        return lh_bool_false;
    }
    if (lh_web_html_attr_get_bare(attr))
    {
        return lh_bool_true;
    }
    lh_web_html_writer_put_char(self, lh_char_map_equals);
    lh_web_html_writer_put_char(self, lh_char_map_dquot);
    lh_str_as_view(lh_web_html_attr_get_value(attr), lh_addr_of(value));
    lh_web_html_escape_append(lh_web_html_writer_get_out_mut(self), value, lh_bool_true);
    lh_web_html_writer_put_char(self, lh_char_map_dquot);
    return lh_bool_true;
}

lh_bool_t
lh_web_html_writer_try_write_close(lh_web_html_writer_t *self, const lh_web_html_node_t *node)
{
    lh_web_html_writer_put_char(self, lh_char_map_less);
    lh_web_html_writer_put_char(self, lh_char_map_slash);
    if (!lh_web_html_writer_try_put_name(self, lh_web_html_node_get_name(node)))
    {
        return lh_bool_false;
    }
    lh_web_html_writer_put_char(self, lh_char_map_greater);
    return lh_bool_true;
}

lh_bool_t
lh_web_html_writer_try_write_open(lh_web_html_writer_t *self, const lh_web_html_node_t *node)
{
    lh_web_html_writer_put_char(self, lh_char_map_less);
    if (!lh_web_html_writer_try_put_name(self, lh_web_html_node_get_name(node)))
    {
        return lh_bool_false;
    }
    for (const lh_web_html_attr_t *attr = lh_web_html_node_get_first_attr(node); lh_null_ne(attr);
         attr = lh_web_html_node_get_next_attr(node, attr))
    {
        if (!lh_web_html_writer_try_write_attr(self, attr))
        {
            return lh_bool_false;
        }
    }
    lh_web_html_writer_put_char(self, lh_char_map_greater);
    return lh_bool_true;
}

lh_bool_t
lh_web_html_writer_try_write_children(lh_web_html_writer_t *self, const lh_web_html_node_t *node)
{
    for (const lh_web_html_node_t *child = lh_web_html_node_get_first_child(node);
         lh_null_ne(child); child = lh_web_html_node_get_next_child(node, child))
    {
        if (!lh_web_html_writer_try_write(self, child))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_web_html_writer_try_write(lh_web_html_writer_t *self, const lh_web_html_node_t *node)
{
    lh_str_view_t text;

    switch (lh_web_html_node_get_kind(node))
    {
    case lh_web_html_kind_doctype:
        lh_web_html_writer_put_view(self, lh_str_view_lit("<!doctype html>"));
        return lh_bool_true;
    case lh_web_html_kind_element:
        return lh_web_html_writer_try_write_open(self, node) &&
               lh_web_html_writer_try_write_children(self, node) &&
               lh_web_html_writer_try_write_close(self, node);
    case lh_web_html_kind_void:
        return lh_null_eq(lh_web_html_node_get_first_child(node)) &&
               lh_web_html_writer_try_write_open(self, node);
    case lh_web_html_kind_text:
        lh_str_as_view(lh_web_html_node_get_text(node), lh_addr_of(text));
        lh_web_html_escape_append(lh_web_html_writer_get_out_mut(self), text, lh_bool_false);
        return lh_bool_true;
    case lh_web_html_kind_raw:
        lh_str_as_view(lh_web_html_node_get_text(node), lh_addr_of(text));
        lh_web_html_writer_put_view(self, text);
        return lh_bool_true;
    default:
        return lh_bool_false;
    }
}

lh_void
lh_web_html_writer_write(lh_web_html_writer_t *self, const lh_web_html_node_t *node)
{
    lh_runtime_check_ifn(lh_web_html_writer_try_write(self, node),
                         lh_runtime_error_code_invalid_argument, "html tree cannot be written");
}
