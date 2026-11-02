/**
 * @file escape.c
 * @brief Implementation of `lh/web/html/escape.h`.
 */

#include <lh/web/html/escape.h>
#include <lh/char/map.h>

lh_str_view_t
lh_web_html_escape_get_entity(lh_char_t ch, lh_bool_t in_attr)
{
    lh_str_view_t none;

    switch (ch)
    {
    case lh_char_map_ampersand:
        return lh_str_view_lit("&amp;");
    case lh_char_map_less:
        return lh_str_view_lit("&lt;");
    case lh_char_map_greater:
        if (!in_attr)
        {
            return lh_str_view_lit("&gt;");
        }
        break;
    case lh_char_map_dquot:
        if (in_attr)
        {
            return lh_str_view_lit("&quot;");
        }
        break;
    default:
        break;
    }
    lh_str_view_init_empty(lh_addr_of(none));
    return none;
}

lh_bool_t
lh_web_html_escape_is_needed(lh_char_t ch, lh_bool_t in_attr)
{
    const lh_str_view_t entity = lh_web_html_escape_get_entity(ch, in_attr);

    return lh_cast_static(lh_bool_t, !lh_math_is_zero(lh_str_view_get_size(lh_addr_of(entity))));
}

lh_void
lh_web_html_escape_append(lh_str_t *out, lh_str_view_t text, lh_bool_t in_attr)
{
    const lh_str_cptr chars = lh_str_view_get_data(lh_addr_of(text));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(text));
    lh_usize_t start = 0U;

    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (!lh_web_html_escape_is_needed(chars[i], in_attr))
        {
            continue;
        }
        /* Everything since the last escape goes in as one run. */
        if (lh_math_gt(i, start))
        {
            lh_str_append(out, chars + start, lh_math_sub(i, start));
        }
        lh_str_append_view(out, lh_web_html_escape_get_entity(chars[i], in_attr));
        start = lh_math_add_one(i);
    }
    if (lh_math_gt(size, start))
    {
        lh_str_append(out, chars + start, lh_math_sub(size, start));
    }
}
