/**
 * @file comment.c
 * @brief Implementation of `lh/core/web/xml/comment.h`.
 */

#include <lh/core/web/xml/comment.h>
#include <lh/core/char/map.h>

lh_bool_t
lh_web_xml_comment_is_valid(lh_str_view_t text)
{
    const lh_str_view_t dashes = lh_str_view_lit("--");

    if (lh_str_view_is_empty(lh_addr_of(text)))
    {
        return lh_bool_true;
    }
    return lh_cast_static(
        lh_bool_t, !lh_str_view_contains(lh_addr_of(text), lh_addr_of(dashes), lh_bool_false) &&
                       lh_math_ne(lh_str_view_get_last_char(lh_addr_of(text)), lh_char_map_minus));
}
