#include <lh/http/char.h>
#include <lh/char/digit.h>
#include <lh/char/letter.h>
#include <lh/util/str/ptr.h>

/* The non-alphanumeric `tchar` of RFC 9110 section 5.6.2, in its order. */
static const lh_char_t lh_http_token_symbols[] = {
    lh_char_map_exclamation, lh_char_map_hash,     lh_char_map_dollar,     lh_char_map_percent,
    lh_char_map_ampersand,   lh_char_map_squote,   lh_char_map_asterisk,   lh_char_map_plus,
    lh_char_map_minus,       lh_char_map_period,   lh_char_map_caret,      lh_char_map_underscore,
    lh_char_map_grave,       lh_char_map_pipe,     lh_char_map_tilde};

lh_bool_t
lh_http_is_token_char(lh_char_t c)
{
    return lh_char_is_letter(c) || lh_char_is_digit(c) ||
           lh_str_ptr_contains_char(lh_http_token_symbols,
                                    lh_array_ptr_get_size(lh_http_token_symbols), c);
}

lh_bool_t
lh_http_is_target_char(lh_char_t c)
{
    const lh_u8_t byte = (lh_u8_t)c;
    return byte >= lh_char_map_exclamation && byte <= lh_char_map_tilde;
}

lh_bool_t
lh_http_is_field_char(lh_char_t c)
{
    const lh_u8_t byte = (lh_u8_t)c;
    return byte == lh_char_map_ht || (byte >= lh_char_map_space && byte != lh_char_map_del);
}
