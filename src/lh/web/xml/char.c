/**
 * @file char.c
 * @brief Implementation of `lh/web/xml/char.h`.
 */

#include <lh/web/xml/char.h>
#include <lh/char/map.h>
#include <lh/math.h>

lh_bool_t
lh_web_xml_char_is_valid(lh_u32_t code)
{
    if (lh_math_lt(code, lh_cast_static(lh_u32_t, lh_char_map_space)))
    {
        return lh_cast_static(lh_bool_t, lh_math_eq(code, lh_char_map_ht) ||
                                             lh_math_eq(code, lh_char_map_lf) ||
                                             lh_math_eq(code, lh_char_map_cr));
    }
    if (lh_math_le(code, 0xD7FFU))
    {
        return lh_bool_true;
    }
    if (lh_math_le(code, 0xFFFDU))
    {
        return lh_cast_static(lh_bool_t, lh_math_ge(code, 0xE000U));
    }
    return lh_cast_static(lh_bool_t, lh_math_ge(code, 0x10000U) && lh_math_le(code, 0x10FFFFU));
}
