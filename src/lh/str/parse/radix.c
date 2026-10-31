#include <lh/str/parse/radix.h>
#include <lh/char/xdigit.h>
#include <lh/math.h>

lh_usize_t
lh_str_ptr_count_radix_digits(lh_str_cptr str, lh_usize_t str_size, lh_uint_t radix)
{
    lh_usize_t n = 0U;

    lh_assert_runtime_ref(str);
    while (lh_math_lt(n, str_size) && lh_char_is_radix_digit(str[n], radix))
    {
        n = lh_math_add_one(n);
    }
    return n;
}

lh_bool_t
lh_str_ptr_parse_radix(lh_str_cptr str, lh_usize_t str_size, lh_uint_t radix, lh_uint_t max,
                       lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (lh_math_is_zero(str_size))
    {
        return lh_bool_false;
    }

    while (lh_math_lt(pos, str_size) && lh_char_is_radix_digit(str[pos], radix))
    {
        if (!lh_char_digit_accumulate_radix(lh_addr_of(value), lh_char_to_xdigit(str[pos]), radix))
        {
            return lh_bool_false;
        }
        if (lh_math_gt(value, max))
        {
            return lh_bool_false;
        }
        pos = lh_math_add_one(pos);
    }

    if (lh_math_ne(pos, str_size))
    {
        return lh_bool_false;
    }

    lh_ptr_deref(out) = value;
    return lh_bool_true;
}

lh_bool_t
lh_str_ptr_parse_radix_ullong(lh_str_cptr str, lh_usize_t str_size, lh_uint_t radix,
                              lh_ullong_t max, lh_ullong_t *out)
{
    lh_ullong_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (lh_math_is_zero(str_size))
    {
        return lh_bool_false;
    }

    while (lh_math_lt(pos, str_size) && lh_char_is_radix_digit(str[pos], radix))
    {
        if (!lh_char_digit_accumulate_radix_ullong(lh_addr_of(value), lh_char_to_xdigit(str[pos]),
                                                   radix))
        {
            return lh_bool_false;
        }
        if (lh_math_gt(value, max))
        {
            return lh_bool_false;
        }
        pos = lh_math_add_one(pos);
    }

    if (lh_math_ne(pos, str_size))
    {
        return lh_bool_false;
    }

    lh_ptr_deref(out) = value;
    return lh_bool_true;
}
