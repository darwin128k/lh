#include <lh/core/str/format/real.h>
#include <lh/core/memory/std.h>
#include <lh/core/util/return.h>

#if LH_REAL_ENABLED

#    include <lh/core/assert.h>
#    include <lh/core/cast/static.h>
#    include <lh/core/char/map.h>
#    include <lh/core/math/ops.h>
#    include <lh/core/real/bits.h>
#    include <lh/core/real/grisu.h>
#    include <lh/core/str/format/put.h>
#    include <lh/core/str/format/uint.h>

lh_usize_t
lh_str_ptr_format_real_layout(const lh_str_ptr digits, lh_usize_t count, lh_s32_t point,
                              lh_str_ptr str, lh_usize_t str_size)
{
    const lh_s32_t n = lh_cast_static(lh_s32_t, count);
    lh_usize_t length = 0U;

    lh_assert_runtime_ref(digits);
    lh_assert_runtime_if(lh_math_is_zero(count) || lh_math_gt(count, LH_REAL_GRISU_DIGITS_MAX),
                         lh_runtime_error_code_invalid_argument);

    lh_char_t text[LH_STR_FORMAT_REAL_SIZE_MAX];
    if (lh_math_le(n, point) && lh_math_le(point, 21))
    {
        /* An integer: the digits, then zeros up to the point. 1e21 is the
           first that goes to exponent notation. */
        for (lh_s32_t i = 0; lh_math_lt(i, point); i = lh_math_add_one(i))
        {
            text[length] = lh_math_lt(i, n) ? digits[i] : lh_char_map_digit_0;
            length = lh_math_add_one(length);
        }
    }
    else if (lh_math_gt(point, 0) && lh_math_le(point, 21))
    {
        /* The point falls inside the digits. */
        for (lh_s32_t i = 0; lh_math_lt(i, n); i = lh_math_add_one(i))
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
        for (lh_s32_t i = point; lh_math_lt(i, 0); i = lh_math_add_one(i))
        {
            text[length] = lh_char_map_digit_0;
            length = lh_math_add_one(length);
        }
        for (lh_s32_t i = 0; lh_math_lt(i, n); i = lh_math_add_one(i))
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
            for (lh_s32_t i = 1; lh_math_lt(i, n); i = lh_math_add_one(i))
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

    lh_assert_runtime_ref(str);
    if (!lh_real_bits_is_finite(bits))
    {
        return 0U;
    }

    lh_char_t text[LH_STR_FORMAT_REAL_SIZE_MAX];
    text[0] = lh_char_map_minus;
    lh_usize_t length = negative ? 1U : 0U;
    if (lh_math_is_zero(magnitude))
    {
        /* Zero has no digits for Grisu to find. */
        text[length] = lh_char_map_digit_0;
        return lh_str_ptr_format_put(str, str_size, text, lh_math_add_one(length));
    }

    /* Grisu wants the magnitude; the sign is already written. */
    lh_char_t digits[LH_REAL_GRISU_DIGITS_MAX];
    lh_usize_t count;
    lh_s32_t k;
    lh_real_grisu(lh_real_from_bits(magnitude), digits, lh_addr_of(count), lh_addr_of(k));
    length =
        lh_math_add(length, lh_str_ptr_format_real_layout(
                                digits, count, lh_math_add(lh_cast_static(lh_s32_t, count), k),
                                text + length, lh_math_sub(LH_STR_FORMAT_REAL_SIZE_MAX, length)));
    return lh_str_ptr_format_put(str, str_size, text, length);
}

#endif /* LH_REAL_ENABLED */

lh_usize_t
lh_str_ptr_format_real_fixed(lh_real_t value, lh_usize_t decimals, lh_str_ptr str,
                             lh_usize_t str_size)
{
    const lh_u64_t bits = lh_real_to_bits(value);
    const lh_u64_t magnitude = lh_real_bits_get_magnitude(bits);
    lh_usize_t count = 0U;
    lh_s32_t k = 0;
    lh_usize_t pos = 0U;
    lh_bool_t carry = lh_bool_false;

    lh_assert_runtime_ref(str);
    lh_return_ifn(lh_real_bits_is_finite(bits), 0U);
    lh_char_t digits[LH_REAL_GRISU_DIGITS_MAX];
    if (!lh_math_is_zero(magnitude))
    {
        lh_real_grisu(lh_real_from_bits(magnitude), digits, lh_addr_of(count), lh_addr_of(k));
    }
    /* digits[0 .. count) with the point `point` digits from the left. */
    lh_s32_t point = lh_math_add(lh_cast_static(lh_s32_t, count), k);
    lh_s32_t keep = lh_math_add(point, lh_cast_static(lh_s32_t, decimals));
    if (lh_math_lt(keep, lh_cast_static(lh_s32_t, count)))
    {
        /* Round half away from zero at the last digit kept. */
        carry = lh_math_ge(keep, 0) && lh_math_ge(digits[keep], lh_char_map_digit_5)
                    ? lh_bool_true
                    : lh_bool_false;
        if (lh_math_lt(keep, 0))
        {
            count = 0U;
        }
        else
        {
            count = lh_cast_static(lh_usize_t, keep);
        }
        for (lh_usize_t i = count; carry && lh_math_gt(i, 0U); --i)
        {
            if (lh_math_eq(digits[i - 1U], lh_char_map_digit_9))
            {
                digits[i - 1U] = lh_char_map_digit_0;
            }
            else
            {
                digits[i - 1U] = lh_cast_static(lh_char_t, digits[i - 1U] + 1);
                carry = lh_bool_false;
            }
        }
        if (carry)
        {
            /* 9.99 -> 10.0: one more digit on the left. */
            lh_memory_std_move(digits + 1, digits, count);
            digits[0] = lh_char_map_digit_1;
            count = lh_math_add_one(count);
            point = lh_math_add_one(point);
        }
    }
    {
        /* All or nothing, as every formatter: the size first, then the text. */
        const lh_bool_t negative = lh_real_bits_is_negative(bits) && !lh_math_is_zero(magnitude)
                                       ? lh_bool_true
                                       : lh_bool_false;
        const lh_usize_t whole = lh_math_gt(point, 0) ? lh_cast_static(lh_usize_t, point) : 1U;
        const lh_usize_t size = (negative ? 1U : 0U) + whole +
                                (lh_math_is_zero(decimals) ? 0U : lh_math_add_one(decimals));

        lh_return_if(lh_math_gt(size, str_size), 0U);
        if (negative)
        {
            str[pos++] = lh_char_map_minus;
        }
        if (lh_math_le(point, 0))
        {
            str[pos++] = lh_char_map_digit_0;
        }
        for (lh_s32_t i = 0; lh_math_lt(i, point); ++i)
        {
            str[pos++] =
                lh_math_lt(i, lh_cast_static(lh_s32_t, count)) ? digits[i] : lh_char_map_digit_0;
        }
        lh_return_if(lh_math_is_zero(decimals), pos);
        str[pos++] = lh_char_map_period;
        for (lh_s32_t i = point;
             lh_math_lt(i, lh_math_add(point, lh_cast_static(lh_s32_t, decimals))); ++i)
        {
            str[pos++] = lh_math_ge(i, 0) && lh_math_lt(i, lh_cast_static(lh_s32_t, count))
                             ? digits[i]
                             : lh_char_map_digit_0;
        }
    }
    return pos;
}
