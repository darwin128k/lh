/**
 * @file entity.c
 * @brief Implementation of `lh/web/xml/entity.h`.
 */

#include <lh/web/xml/entity.h>
#include <lh/char/map.h>

lh_char_t
lh_web_xml_entity_get_char(lh_str_view_t name)
{
    const lh_str_view_t names[] = {lh_str_view_lit("lt"), lh_str_view_lit("gt"),
                                   lh_str_view_lit("amp"), lh_str_view_lit("quot"),
                                   lh_str_view_lit("apos")};
    const lh_char_t chars[] = {lh_char_map_less, lh_char_map_greater, lh_char_map_ampersand,
                               lh_char_map_dquot, lh_char_map_squote};

    const lh_usize_t index =
        lh_str_view_find_in(lh_addr_of(name), names, lh_array_ptr_get_size(names), lh_bool_false);

    if (lh_math_eq(index, LH_STR_VIEW_INVALID))
    {
        return lh_char_map_nul;
    }
    return chars[index];
}
