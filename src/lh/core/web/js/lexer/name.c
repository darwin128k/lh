/**
 * @file name.c
 * @brief ASCII JavaScript identifier-name classification.
 */

#include <lh/core/web/js/lexer/name.h>

#include <lh/core/char/digit.h>

lh_bool_t
lh_web_js_lexer_is_name_start(lh_char_t ch)
{
    static const lh_char_t special[] = {lh_char_map_dollar, lh_char_map_underscore};
    return (lh_char_is_letter(ch) || lh_str_ptr_contains_char(special, sizeof(special), ch))
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_web_js_lexer_is_name_part(lh_char_t ch)
{
    return (lh_web_js_lexer_is_name_start(ch) || lh_char_is_digit(ch)) ? lh_bool_true
                                                                       : lh_bool_false;
}

lh_bool_t
lh_web_js_lexer_name_is_ascii_identifier(const lh_str_view_t *text)
{
    const lh_usize_t size = lh_str_view_get_size(text);
    if (size == 0u || !lh_web_js_lexer_is_name_start(lh_str_view_get_char_from_begin(text, 0u)))
    {
        return lh_bool_false;
    }
    for (lh_usize_t i = 1u; i < size; i = lh_math_add_one(i))
    {
        if (!lh_web_js_lexer_is_name_part(lh_str_view_get_char_from_begin(text, i)))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}
