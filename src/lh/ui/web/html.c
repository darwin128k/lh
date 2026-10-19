/**
 * @file html.c
 * @brief Implementation of `lh/ui/web/html.h`.
 */

#include <lh/ui/web/html.h>
#include <lh/assert/runtime.h>

lh_void
lh_ui_web_html_doctype(lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_t n;

    n.kind = lh_ui_web_html_kind_doctype;
    lh_ptr_deref(out) = n;
}

lh_void
lh_ui_web_html_text(const lh_str_ptr s, lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_t n;

    n.kind = lh_ui_web_html_kind_text;
    n.u.text = s;
    lh_ptr_deref(out) = n;
}

lh_void
lh_ui_web_html_raw(const lh_str_ptr s, lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_t n;

    n.kind = lh_ui_web_html_kind_raw;
    n.u.raw = s;
    lh_ptr_deref(out) = n;
}

lh_void
lh_ui_web_html_tag(const lh_str_ptr name, const lh_ui_web_html_attr_t *attrs, lh_u16_t nattrs,
                   const lh_ui_web_html_t *const *kids, lh_u16_t nkids, lh_bool_t void_el,
                   lh_ui_web_html_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_t n;

    n.kind = lh_ui_web_html_kind_tag;
    n.u.tag.name = name;
    n.u.tag.attrs = attrs;
    n.u.tag.nattrs = nattrs;
    n.u.tag.kids = kids;
    n.u.tag.nkids = nkids;
    n.u.tag.void_el = void_el;
    lh_ptr_deref(out) = n;
}

const lh_str_ptr
lh_ui_web_html_entity_of(lh_char_t c, lh_bool_t in_attr)
{
    switch (c)
    {
    case '&':
        return "&amp;";
    case '<':
        return "&lt;";
    case '>':
        return in_attr ? lh_null : "&gt;";
    case '"':
        return in_attr ? "&quot;" : lh_null;
    default:
        return lh_null;
    }
}

lh_void
lh_ui_web_html_put_escaped(lh_ui_web_buf_t *out, const lh_str_ptr s, lh_bool_t in_attr)
{
    lh_str_cptr at = s;

    if (lh_null_eq(at))
    {
        return;
    }
    for (; lh_ptr_deref(at) != '\0'; ++at)
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
lh_ui_web_html_put_attrs(lh_ui_web_buf_t *out, const lh_ui_web_html_attr_t *attrs, lh_u16_t n)
{
    lh_u16_t i;

    for (i = 0u; i < n; ++i)
    {
        lh_ui_web_buf_lit(out, " ", 1u);
        lh_ui_web_buf_raw(out, attrs[i].name);
        if (attrs[i].value != lh_null)
        {
            lh_ui_web_buf_lit(out, "=\"", 2u);
            lh_ui_web_html_put_escaped(out, attrs[i].value, lh_bool_true);
            lh_ui_web_buf_lit(out, "\"", 1u);
        }
    }
}

lh_void
lh_ui_web_html_open(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    if (out == lh_null || node == lh_null || node->kind != lh_ui_web_html_kind_tag)
    {
        lh_ui_web_buf_fail(out);
        return;
    }
    lh_ui_web_buf_lit(out, "<", 1u);
    lh_ui_web_buf_raw(out, node->u.tag.name);
    lh_ui_web_html_put_attrs(out, node->u.tag.attrs, node->u.tag.nattrs);
    lh_ui_web_buf_lit(out, ">", 1u);
}

lh_void
lh_ui_web_html_close(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    if (out == lh_null || node == lh_null || node->kind != lh_ui_web_html_kind_tag)
    {
        lh_ui_web_buf_fail(out);
        return;
    }
    if (node->u.tag.void_el)
    {
        return;
    }
    lh_ui_web_buf_lit(out, "</", 2u);
    lh_ui_web_buf_raw(out, node->u.tag.name);
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
    switch (node->kind)
    {
    case lh_ui_web_html_kind_doctype:
        lh_ui_web_buf_lit(out, "<!doctype html>", 15u);
        break;
    case lh_ui_web_html_kind_text:
        lh_ui_web_html_put_escaped(out, node->u.text, lh_bool_false);
        break;
    case lh_ui_web_html_kind_raw:
        lh_ui_web_buf_raw(out, node->u.raw);
        break;
    case lh_ui_web_html_kind_tag:
        lh_ui_web_html_open(out, node);
        if (node->u.tag.void_el)
        {
            break;
        }
        for (i = 0u; i < node->u.tag.nkids; ++i)
        {
            lh_ui_web_html_emit(out, node->u.tag.kids[i]);
        }
        lh_ui_web_html_close(out, node);
        break;
    default:
        lh_ui_web_buf_fail(out);
        break;
    }
}
