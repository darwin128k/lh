#include <lh/str/parse/hex.h>
#include <lh/char/xdigit.h>
#include <lh/math.h>
#include <lh/str/parse/radix.h>

lh_bool_t
lh_str_ptr_parse_hex(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    return lh_str_ptr_parse_radix(str, str_size, LH_CHAR_XDIGIT_RADIX, max, out);
}

lh_usize_t
lh_str_ptr_parse_hex_prefix(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    const lh_usize_t n = lh_str_ptr_count_radix_digits(str, str_size, LH_CHAR_XDIGIT_RADIX);

    if (lh_math_is_zero(n) || !lh_str_ptr_parse_hex(str, n, max, out))
    {
        return 0U;
    }
    return n;
}
