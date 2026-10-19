#include <lh/str/parse/uint.h>
#include <lh/char/digit.h>
#include <lh/str/split/next.h>
#include <lh/math.h>
#include <lh/char/map.h>
#include <lh/str/parse/radix.h>

lh_bool_t
lh_str_ptr_parse_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    return lh_str_ptr_parse_radix(str, str_size, LH_CHAR_DIGIT_RADIX, max, out);
}

lh_bool_t
lh_str_ptr_parse_uint(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_assert_runtime_ref(str);

    if (lh_math_gt(str_size, 1U) && lh_math_eq(str[0], lh_char_map_digit_0))
    {
        return lh_bool_false;
    }

    return lh_str_ptr_parse_uint_digits(str, str_size, max, out);
}

lh_bool_t
lh_str_ptr_split_next_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim,
                                  lh_usize_t *pos, lh_uint_t max, lh_uint_t *out,
                                  lh_bool_t want_delim)
{
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    if (!lh_str_ptr_split_next(str, str_size, delim, pos, lh_addr_of(field), lh_addr_of(field_size),
                               lh_addr_of(had_delim)))
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_parse_uint_digits(field, field_size, max, out))
    {
        return lh_bool_false;
    }

    return lh_cast_static(lh_bool_t, lh_math_eq(had_delim, want_delim));
}

lh_usize_t
lh_str_ptr_parse_uint_prefix(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    const lh_usize_t n = lh_str_ptr_count_radix_digits(str, str_size, LH_CHAR_DIGIT_RADIX);

    if (lh_math_is_zero(n) || !lh_str_ptr_parse_uint(str, n, max, out))
    {
        return 0U;
    }
    return n;
}
