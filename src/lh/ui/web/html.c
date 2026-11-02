/**
 * @file html.c
 * @brief Implementation of `lh/ui/web/html.h`.
 */

#include <lh/ui/web/html.h>
#include <lh/char/map.h>
#include <lh/assert/runtime.h>
lh_ui_web_html_kind_t
lh_ui_web_html_get_kind(const lh_ui_web_html_t *self)
{
    return self->kind;
}

lh_void
lh_ui_web_html_set_kind(lh_ui_web_html_t *self, lh_ui_web_html_kind_t value)
{
    self->kind = value;
}

const lh_str_ptr
lh_ui_web_html_get_text(const lh_ui_web_html_t *self)
{
    return self->u.text;
}

lh_void
lh_ui_web_html_set_text(lh_ui_web_html_t *self, const lh_str_ptr value)
{
    self->u.text = value;
}

const lh_str_ptr
lh_ui_web_html_get_raw(const lh_ui_web_html_t *self)
{
    return self->u.raw;
}

lh_void
lh_ui_web_html_set_raw(lh_ui_web_html_t *self, const lh_str_ptr value)
{
    self->u.raw = value;
}

const lh_str_ptr
lh_ui_web_html_get_tag_name(const lh_ui_web_html_t *self)
{
    return self->u.tag.name;
}

lh_void
lh_ui_web_html_set_tag_name(lh_ui_web_html_t *self, const lh_str_ptr value)
{
    self->u.tag.name = value;
}

const lh_ui_web_html_attr_t *
lh_ui_web_html_get_tag_attrs(const lh_ui_web_html_t *self)
{
    return self->u.tag.attrs;
}

lh_void
lh_ui_web_html_set_tag_attrs(lh_ui_web_html_t *self, const lh_ui_web_html_attr_t *value)
{
    self->u.tag.attrs = value;
}

lh_u16_t
lh_ui_web_html_get_tag_nattrs(const lh_ui_web_html_t *self)
{
    return self->u.tag.nattrs;
}

lh_void
lh_ui_web_html_set_tag_nattrs(lh_ui_web_html_t *self, lh_u16_t value)
{
    self->u.tag.nattrs = value;
}

const lh_ui_web_html_t *const *
lh_ui_web_html_get_tag_kids(const lh_ui_web_html_t *self)
{
    return self->u.tag.kids;
}

lh_void
lh_ui_web_html_set_tag_kids(lh_ui_web_html_t *self, const lh_ui_web_html_t *const *value)
{
    self->u.tag.kids = value;
}

lh_u16_t
lh_ui_web_html_get_tag_nkids(const lh_ui_web_html_t *self)
{
    return self->u.tag.nkids;
}

lh_void
lh_ui_web_html_set_tag_nkids(lh_ui_web_html_t *self, lh_u16_t value)
{
    self->u.tag.nkids = value;
}

lh_bool_t
lh_ui_web_html_get_tag_void_el(const lh_ui_web_html_t *self)
{
    return self->u.tag.void_el;
}

lh_void
lh_ui_web_html_set_tag_void_el(lh_ui_web_html_t *self, lh_bool_t value)
{
    self->u.tag.void_el = value;
}

const lh_str_ptr
lh_ui_web_html_entity_of(lh_char_t c, lh_bool_t in_attr)
{
    switch (c)
    {
    case lh_char_map_ampersand:
        return "&amp;";
    case lh_char_map_less:
        return "&lt;";
    case lh_char_map_greater:
        return in_attr ? lh_null : "&gt;";
    case lh_char_map_dquot:
        return in_attr ? "&quot;" : lh_null;
    default:
        return lh_null;
    }
}

lh_void
lh_ui_web_html_doctype(lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_set_kind(out, lh_ui_web_html_kind_doctype);
}

lh_void
lh_ui_web_html_text(const lh_str_ptr s, lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_set_kind(out, lh_ui_web_html_kind_text);
    lh_ui_web_html_set_text(out, s);
}

lh_void
lh_ui_web_html_raw(const lh_str_ptr s, lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_set_kind(out, lh_ui_web_html_kind_raw);
    lh_ui_web_html_set_raw(out, s);
}

lh_void
lh_ui_web_html_tag(const lh_str_ptr name, const lh_ui_web_html_attr_t *attrs, lh_u16_t nattrs,
                   const lh_ui_web_html_t *const *kids, lh_u16_t nkids, lh_bool_t void_el,
                   lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_set_kind(out, lh_ui_web_html_kind_tag);
    lh_ui_web_html_set_tag_name(out, name);
    lh_ui_web_html_set_tag_attrs(out, attrs);
    lh_ui_web_html_set_tag_nattrs(out, nattrs);
    lh_ui_web_html_set_tag_kids(out, kids);
    lh_ui_web_html_set_tag_nkids(out, nkids);
    lh_ui_web_html_set_tag_void_el(out, void_el);
}

lh_void
lh_ui_web_html_put_escaped(lh_ui_web_buf_t *out, const lh_str_ptr s, lh_bool_t in_attr)
{
    lh_str_cptr at = s;

    if (lh_null_eq(at))
    {
        return;
    }
    for (; lh_ptr_deref(at) != lh_char_map_nul; ++at)
    {
        const lh_str_cptr entity = lh_ui_web_html_entity_of(lh_ptr_deref(at), in_attr);

        if (lh_null_ne(entity))
        {
            lh_ui_web_buf_raw(out, entity);
        }
        else
        {
            lh_ui_web_buf_lit(out, at, 1u);
        }
    }
}

lh_void
lh_ui_web_html_close(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    if (out == lh_null || node == lh_null ||
        lh_ui_web_html_get_kind(node) != lh_ui_web_html_kind_tag)
    {
        lh_ui_web_buf_fail(out);
        return;
    }
    if (lh_ui_web_html_get_tag_void_el(node))
    {
        return;
    }
    lh_ui_web_buf_lit(out, "</", 2u);
    lh_ui_web_buf_raw(out, lh_ui_web_html_get_tag_name(node));
    lh_ui_web_buf_lit(out, ">", 1u);
}

lh_void
lh_ui_web_html_put_attrs(lh_ui_web_buf_t *out, const lh_ui_web_html_attr_t *attrs, lh_u16_t n)
{
    for (lh_u16_t i = 0u; i < n; ++i)
    {
        lh_ui_web_buf_lit(out, " ", 1u);
        lh_ui_web_buf_raw(out, lh_ui_web_html_attr_get_name(lh_addr_of(attrs[i])));
        if (lh_ui_web_html_attr_get_value(lh_addr_of(attrs[i])) != lh_null)
        {
            lh_ui_web_buf_lit(out, "=\"", 2u);
            lh_ui_web_html_put_escaped(out, lh_ui_web_html_attr_get_value(lh_addr_of(attrs[i])),
                                       lh_bool_true);
            lh_ui_web_buf_lit(out, "\"", 1u);
        }
    }
}

lh_void
lh_ui_web_html_open(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    if (out == lh_null || node == lh_null ||
        lh_ui_web_html_get_kind(node) != lh_ui_web_html_kind_tag)
    {
        lh_ui_web_buf_fail(out);
        return;
    }
    lh_ui_web_buf_lit(out, "<", 1u);
    lh_ui_web_buf_raw(out, lh_ui_web_html_get_tag_name(node));
    lh_ui_web_html_put_attrs(out, lh_ui_web_html_get_tag_attrs(node),
                             lh_ui_web_html_get_tag_nattrs(node));
    lh_ui_web_buf_lit(out, ">", 1u);
}

lh_void
lh_ui_web_html_emit(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    lh_u16_t i;

    if (out == lh_null || !lh_ui_web_buf_ok(out) || node == lh_null)
    {
        lh_ui_web_buf_fail(out);
        return;
    }
    switch (lh_ui_web_html_get_kind(node))
    {
    case lh_ui_web_html_kind_doctype:
        lh_ui_web_buf_lit(out, "<!doctype html>", 15u);
        break;
    case lh_ui_web_html_kind_text:
        lh_ui_web_html_put_escaped(out, lh_ui_web_html_get_text(node), lh_bool_false);
        break;
    case lh_ui_web_html_kind_raw:
        lh_ui_web_buf_raw(out, lh_ui_web_html_get_raw(node));
        break;
    case lh_ui_web_html_kind_tag:
        lh_ui_web_html_open(out, node);
        if (lh_ui_web_html_get_tag_void_el(node))
        {
            break;
        }
        for (i = 0u; i < lh_ui_web_html_get_tag_nkids(node); ++i)
        {
            lh_ui_web_html_emit(out, lh_ui_web_html_get_tag_kids(node)[i]);
        }
        lh_ui_web_html_close(out, node);
        break;
    default:
        lh_ui_web_buf_fail(out);
        break;
    }
}
