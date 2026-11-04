/**
 * @file void.c
 * @brief Implementation of `lh/core/web/html/void.h`.
 */

#include <lh/core/web/html/void.h>

lh_bool_t
lh_web_html_void_is_name(lh_str_view_t name)
{
    const lh_str_view_t names[] = {
        lh_str_view_lit("area"), lh_str_view_lit("base"),   lh_str_view_lit("br"),
        lh_str_view_lit("col"),  lh_str_view_lit("embed"),  lh_str_view_lit("hr"),
        lh_str_view_lit("img"),  lh_str_view_lit("input"),  lh_str_view_lit("link"),
        lh_str_view_lit("meta"), lh_str_view_lit("source"), lh_str_view_lit("track"),
        lh_str_view_lit("wbr")};

    return lh_cast_static(
        lh_bool_t, lh_math_ne(lh_str_view_find_in(lh_addr_of(name), names,
                                                  lh_array_ptr_get_size(names), lh_bool_true),
                              LH_STR_VIEW_INVALID));
}
