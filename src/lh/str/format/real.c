#include <lh/str/format/real.h>

#if LH_REAL_ENABLED

#    include <lh/assert.h>
#    include <lh/cast/static.h>
#    include <lh/char/map.h>
#    include <lh/math/ops.h>
#    include <lh/real/bits.h>
#    include <lh/real/grisu.h>
#    include <lh/str/format/put.h>
#    include <lh/str/format/uint.h>

lh_usize_t
lh_str_ptr_format_real_layout(const lh_str_ptr digits, lh_usize_t count, lh_s32_t point,
                              lh_str_ptr str, lh_usize_t str_size)
{
    lh_char_t text[LH_STR_FORMAT_REAL_SIZE_MAX];
    const lh_s32_t n = lh_cast_static(lh_s32_t, count);
    lh_usize_t length = 0U;
    lh_s32_t i;

    lh_assert_runtime_ref(digits);
    lh_assert_runtime_if(lh_math_is_zero(count) || lh_math_gt(count, LH_REAL_GRISU_DIGITS_MAX),
                         lh_runtime_error_code_invalid_argument);

    if (lh_math_le(n, point) && lh_math_le(point, 21))
    {
        /* An integer: the digits, then zeros up to the point. 1e21 is the
           first that goes to exponent notation. */
        for (i = 0; lh_math_lt(i, point); i = lh_math_add_one(i))
        {
            text[length] = lh_math_lt(i, n) ? digits[i] : lh_char_map_digit_0;
            length = lh_math_add_one(length);
        }
    }
    else if (lh_math_gt(point, 0) && lh_math_le(point, 21))
    {
        /* The point falls inside the digits. */
        for (i = 0; lh_math_lt(i, n); i = lh_math_add_one(i))
        {
            if (lh_math_eq(i, point))
            {
                text[length] = lh_char_map_period;
                length = lh_math_add_one(length);
            }
            text[length] = digits[i];
            length = lh_math_add_one(length);
        }
    }
    else if (lh_math_gt(point, -6) && lh_math_le(point, 0))
    {
        /* Below one but not below 1e-6: "0.", zeros, then the digits. */
        text[0] = lh_char_map_digit_0;
        text[1] = lh_char_map_period;
        length = 2U;
        for (i = point; lh_math_lt(i, 0); i = lh_math_add_one(i))
        {
            text[length] = lh_char_map_digit_0;
            length = lh_math_add_one(length);
        }
        for (i = 0; lh_math_lt(i, n); i = lh_math_add_one(i))
        {
            text[length] = digits[i];
            length = lh_math_add_one(length);
        }
    }
    else
    {
        /* Exponent notation: one digit, the rest after a point, e±(point-1). */
        const lh_s32_t exponent = lh_math_sub_one(point);

        text[length] = digits[0];
        length = lh_math_add_one(length);
        if (lh_math_gt(n, 1))
        {
            text[length] = lh_char_map_period;
            length = lh_math_add_one(length);
            for (i = 1; lh_math_lt(i, n); i = lh_math_add_one(i))
            {
                text[length] = digits[i];
                length = lh_math_add_one(length);
            }
        }
        text[length] = lh_char_map_small_e;
        text[lh_math_add_one(length)] =
            lh_math_is_negative(exponent) ? lh_char_map_minus : lh_char_map_plus;
        length = lh_math_add(length, 2U);
        length = lh_math_add(
            length,
            lh_str_ptr_format_uint(lh_cast_static(lh_uint_t, lh_math_abs(exponent)), text + length,
                                   lh_math_sub(LH_STR_FORMAT_REAL_SIZE_MAX, length)));
    }
    return lh_str_ptr_format_put(str, str_size, text, length);
}

lh_usize_t
lh_str_ptr_format_real(lh_real_t value, lh_str_ptr str, lh_usize_t str_size)
{
    const lh_u64_t bits = lh_real_to_bits(value);
    const lh_u64_t magnitude = lh_real_bits_get_magnitude(bits);
    const lh_bool_t negative = lh_real_bits_is_negative(bits);
    lh_char_t text[LH_STR_FORMAT_REAL_SIZE_MAX];
    lh_char_t digits[LH_REAL_GRISU_DIGITS_MAX];
    lh_usize_t count;
    lh_usize_t length;
    lh_s32_t k;

    lh_assert_runtime_ref(str);
    if (!lh_real_bits_is_finite(bits))
    {
        return 0U;
    }

    text[0] = lh_char_map_minus;
    length = negative ? 1U : 0U;
    if (lh_math_is_zero(magnitude))
    {
        /* Zero has no digits for Grisu to find. */
        text[length] = lh_char_map_digit_0;
        return lh_str_ptr_format_put(str, str_size, text, lh_math_add_one(length));
    }

    /* Grisu wants the magnitude; the sign is already written. */
    lh_real_grisu(lh_real_from_bits(magnitude), digits, lh_addr_of(count), lh_addr_of(k));
    length =
        lh_math_add(length, lh_str_ptr_format_real_layout(
                                digits, count, lh_math_add(lh_cast_static(lh_s32_t, count), k),
                                text + length, lh_math_sub(LH_STR_FORMAT_REAL_SIZE_MAX, length)));
    return lh_str_ptr_format_put(str, str_size, text, length);
}

#endif /* LH_REAL_ENABLED */
