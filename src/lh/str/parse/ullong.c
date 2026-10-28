#include <lh/str/parse/ullong.h>
#include <lh/char/digit.h>
#include <lh/math.h>

lh_bool_t
lh_str_ptr_parse_ullong_digits(lh_str_cptr str, lh_usize_t str_size, lh_ullong_t max,
                               lh_ullong_t *out)
{
    lh_ullong_t value = 0U;
    lh_usize_t pos = 0U;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (lh_math_is_zero(str_size))
    {
        return lh_bool_false;
    }

    while (lh_math_lt(pos, str_size) && lh_char_is_digit(str[pos]))
    {
        if (!lh_char_digit_accumulate_ullong(lh_addr_of(value), lh_char_to_digit(str[pos])))
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
lh_str_ptr_parse_ullong(lh_str_cptr str, lh_usize_t str_size, lh_ullong_t max, lh_ullong_t *out)
{
    lh_assert_runtime_ref(str);

    if (lh_math_gt(str_size, 1U) && lh_math_eq(str[0], lh_char_map_digit_0))
    {
        return lh_bool_false;
    }

    return lh_str_ptr_parse_ullong_digits(str, str_size, max, out);
}
