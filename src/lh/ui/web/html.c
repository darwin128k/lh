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

/** Append @p s as element text: `&`, `<` and `>` escaped. */
static lh_void
html_put_esc_text(lh_ui_web_buf_t *out, const lh_str_ptr s)
{
    if (s == lh_null)
    {
        return;
    }
    while (lh_ptr_deref(s) != '\0')
    {
        lh_char_t c;

        c = lh_ptr_deref(s++);
        if (c == '&')
        {
            lh_ui_web_buf_lit(out, "&amp;", 5u);
        }
        else if (c == '<')
        {
            lh_ui_web_buf_lit(out, "&lt;", 4u);
        }
        else if (c == '>')
        {
            lh_ui_web_buf_lit(out, "&gt;", 4u);
        }
        else
        {
            lh_ui_web_buf_lit(out, lh_addr_of(c), 1u);
        }
    }
}

/** Append @p s as an attribute value: `&`, `"` and `<` escaped. */
static lh_void
html_put_esc_attr(lh_ui_web_buf_t *out, const lh_str_ptr s)
{
    if (s == lh_null)
    {
        return;
    }
    while (lh_ptr_deref(s) != '\0')
    {
        lh_char_t c;

        c = lh_ptr_deref(s++);
        if (c == '&')
        {
            lh_ui_web_buf_lit(out, "&amp;", 5u);
        }
        else if (c == '"')
        {
            lh_ui_web_buf_lit(out, "&quot;", 6u);
        }
        else if (c == '<')
        {
            lh_ui_web_buf_lit(out, "&lt;", 4u);
        }
        else
        {
            lh_ui_web_buf_lit(out, lh_addr_of(c), 1u);
        }
    }
}

/** Append @p n attributes as ` name="value"` (or bare ` name`). */
static lh_void
html_put_attrs(lh_ui_web_buf_t *out, const lh_ui_web_html_attr_t *attrs, lh_u16_t n)
{
    lh_u16_t i;

    for (i = 0u; i < n; ++i)
    {
        lh_ui_web_buf_lit(out, " ", 1u);
        lh_ui_web_buf_raw(out, attrs[i].name);
        if (attrs[i].value != lh_null)
        {
            lh_ui_web_buf_lit(out, "=\"", 2u);
            html_put_esc_attr(out, attrs[i].value);
            lh_ui_web_buf_lit(out, "\"", 1u);
        }
    }
}

/** Append the opening tag of @p node with its attributes. */
static lh_void
html_open_tag(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    lh_ui_web_buf_lit(out, "<", 1u);
    lh_ui_web_buf_raw(out, node->u.tag.name);
    html_put_attrs(out, node->u.tag.attrs, node->u.tag.nattrs);
    lh_ui_web_buf_lit(out, ">", 1u);
}

lh_void
lh_ui_web_html_open(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node)
{
    if (out == lh_null || node == lh_null || node->kind != lh_ui_web_html_kind_tag)
    {
        lh_ui_web_buf_fail(out);
        return;
    }
    html_open_tag(out, node);
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
        html_put_esc_text(out, node->u.text);
        break;
    case lh_ui_web_html_kind_raw:
        lh_ui_web_buf_raw(out, node->u.raw);
        break;
    case lh_ui_web_html_kind_tag:
        html_open_tag(out, node);
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
