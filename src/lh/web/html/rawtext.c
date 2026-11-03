/**
 * @file rawtext.c
 * @brief Implementation of `lh/web/html/rawtext.h`.
 */

#include <lh/web/html/rawtext.h>

lh_bool_t
lh_web_html_rawtext_is_name(lh_str_view_t name)
{
    const lh_str_view_t names[] = {lh_str_view_lit("script"), lh_str_view_lit("style")};

    return lh_cast_static(
        lh_bool_t, lh_math_ne(lh_str_view_find_in(lh_addr_of(name), names,
                                                  lh_array_ptr_get_size(names), lh_bool_true),
                              LH_STR_VIEW_INVALID));
}
