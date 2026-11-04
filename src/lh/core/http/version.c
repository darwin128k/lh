#include <lh/core/http/version.h>
#include <lh/core/char/digit.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_http_parse_version(lh_str_view_t text, lh_u8_t *minor)
{
    const lh_str_view_t prefix = lh_str_view_initializer("HTTP/1.", 7);
    lh_return_if(
        lh_str_view_get_size(lh_addr_of(text)) != 8 ||
            !lh_str_view_starts_with(lh_addr_of(text), lh_addr_of(prefix), lh_bool_false) ||
            !lh_char_is_digit(lh_str_view_get_char_from_begin(lh_addr_of(text), 7)),
        lh_bool_false);
    *minor = lh_char_to_digit(lh_str_view_get_char_from_begin(lh_addr_of(text), 7));
    return lh_bool_true;
}
