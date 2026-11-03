/**
 * @file name.c
 * @brief Implementation of `lh/web/xml/name.h`.
 */

#include <lh/web/xml/name.h>
#include <lh/char/digit.h>
#include <lh/char/letter.h>

lh_bool_t
lh_web_xml_name_is_start_char(lh_char_t ch)
{
    return lh_cast_static(lh_bool_t, lh_char_is_letter(ch) ||
                                         lh_math_eq(ch, lh_char_map_underscore) ||
                                         lh_math_eq(ch, lh_char_map_colon) ||
                                         lh_math_ge(lh_cast_static(lh_byte_t, ch), 0x80U));
}

lh_bool_t
lh_web_xml_name_is_char(lh_char_t ch)
{
    return lh_cast_static(lh_bool_t, lh_web_xml_name_is_start_char(ch) || lh_char_is_digit(ch) ||
                                         lh_math_eq(ch, lh_char_map_minus) ||
                                         lh_math_eq(ch, lh_char_map_period));
}

lh_bool_t
lh_web_xml_name_is_valid(lh_str_view_t name)
{
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(name));

    if (lh_math_is_zero(size) ||
        !lh_web_xml_name_is_start_char(lh_str_view_get_char_from_begin(lh_addr_of(name), 0U)))
    {
        return lh_bool_false;
    }
    for (lh_usize_t i = 1U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (!lh_web_xml_name_is_char(lh_str_view_get_char_from_begin(lh_addr_of(name), i)))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}
