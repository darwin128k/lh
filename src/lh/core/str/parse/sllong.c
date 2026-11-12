#include <lh/core/str/parse/sllong.h>
#include <lh/core/assert.h>
#include <lh/core/char/map.h>
#include <lh/core/math.h>
#include <lh/core/str/parse/ullong.h>

lh_bool_t
lh_str_ptr_parse_sllong(lh_str_cptr str, lh_usize_t str_size, lh_sllong_t min, lh_sllong_t max,
                        lh_sllong_t *out)
{
    /* The magnitude of the most negative value is one past the largest positive
       one, so a negative number is parsed up to that and negated as
       -(magnitude - 1) - 1, which never leaves the type. */
    const lh_ullong_t magnitude_max =
        lh_math_add_one(lh_cast_static(lh_ullong_t, lh_numeric_limit_max(lh_sllong_t)));

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    const lh_bool_t negative = lh_cast_static(lh_bool_t, lh_math_gt(str_size, 0U) &&
                                                             lh_math_eq(str[0], lh_char_map_minus));
    /* The digits after the sign; a negative number reaches one further than a positive one. */
    const lh_usize_t sign = negative ? 1U : 0U;
    lh_ullong_t magnitude;
    if (!lh_str_ptr_parse_ullong(str + sign, str_size - sign,
                                 negative ? magnitude_max : lh_math_sub_one(magnitude_max),
                                 lh_addr_of(magnitude)))
    {
        return lh_bool_false;
    }
    /* -magnitude without overflow: -(magnitude - 1) - 1 reaches the minimum. */
    const lh_sllong_t value =
        !negative ? lh_cast_static(lh_sllong_t, magnitude)
        : lh_math_is_zero(magnitude)
            ? 0
            : lh_math_sub_one(lh_math_neg(lh_cast_static(lh_sllong_t, lh_math_sub_one(magnitude))));

    if (lh_math_lt(value, min) || lh_math_gt(value, max))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = value;
    return lh_bool_true;
}
