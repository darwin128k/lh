#include <lh/str/format/float.h>
#include <lh/bit/ops.h>
#include <lh/bit/scan.h>
#include <lh/bool.h>
#include <lh/char/digit.h>
#include <lh/char/map.h>
#include <lh/math.h>
#include <lh/memory/std.h>
#include <lh/numeric/big.h>
#include <lh/str/format/put.h>
#include <lh/util/str/ptr.h>

/*
 * A float is m * 2^e with m a 24-bit integer. To get N decimal digits that are
 * right, not close, the ratio v / 10^d10 is built as num / den in a small
 * unsigned big number, and the digits come out one at a time by subtraction:
 * the same schoolbook division a person does, with no floating point in it.
 *
 * Widest case: m << 104 (v near 2^128) against 10^38, and 10^45 * m against
 * 2^149 (the smallest subnormals). Both fit in 192 bits; ::lh_numeric_big_t
 * carries 256 so that `* 10` for the next digit never has to check.
 */

#define LH_STR_FORMAT_FLOAT_TMP 40U

lh_sint_t
lh_str_format_float_floor_log10_pow2(lh_sint_t x)
{
    const long scaled = lh_cast_static(long, x) * 78913L;

    return (lh_sint_t)(scaled >= 0
                           ? lh_bit_shr(scaled, 18)
                           : lh_math_neg(lh_bit_shr(
                                 lh_math_add(lh_math_neg(scaled), lh_math_sub_one(lh_bit_shl(1L, 18))),
                                 18)));
}

lh_usize_t
lh_str_format_float_get_precision(lh_usize_t digits)
{
    return lh_math_is_zero(digits) ? LH_STR_FORMAT_FLOAT_DIGITS_DEFAULT
                                   : lh_math_min(digits, LH_STR_FORMAT_FLOAT_DIGITS_MAX);
}

lh_usize_t
lh_str_format_float_put_special(lh_u32_t bits, lh_str_ptr str)
{
    const lh_u32_t exp_bits = lh_bit_and(lh_bit_shr(bits, 23U), 0xFFU);
    const lh_u32_t mantissa = lh_bit_and(bits, 0x7FFFFFU);
    lh_usize_t n = 0U;

    if (lh_math_eq(exp_bits, 0xFFU) && lh_math_ne(mantissa, 0U))
    {
        /* A NaN has no sign worth printing. */
        return lh_str_ptr_put_chars(str, 0U, "nan", 3U);
    }
    if (lh_math_ne(lh_bit_shr(bits, 31U), 0U) &&
        (lh_math_eq(exp_bits, 0xFFU) || (lh_math_is_zero(exp_bits) && lh_math_is_zero(mantissa))))
    {
        n = lh_str_ptr_put_char(str, n, lh_char_map_minus);
    }
    if (lh_math_eq(exp_bits, 0xFFU))
    {
        return lh_str_ptr_put_chars(str, n, "inf", 3U);
    }
    if (lh_math_is_zero(exp_bits) && lh_math_is_zero(mantissa))
    {
        return lh_str_ptr_put_char(str, n, lh_char_map_digit_0);
    }
    return 0U;
}

lh_sint_t
lh_str_format_float_scale(lh_u32_t bits, lh_numeric_big_t *num, lh_numeric_big_t *den)
{
    const lh_u32_t exp_bits = lh_bit_and(lh_bit_shr(bits, 23U), 0xFFU);
    lh_u32_t mantissa = lh_bit_and(bits, 0x7FFFFFU);
    lh_sint_t e = -149;
    lh_sint_t d10;
    lh_numeric_big_t next;

    if (lh_math_ne(exp_bits, 0U))
    {
        lh_bit_set(mantissa, lh_bit_mask(23U)); /* the implicit leading 1 */
        e = lh_math_sub(lh_cast_static(lh_sint_t, exp_bits), 150);
    }

    /* num / den = v / 10^d10, which must land in [1, 10). */
    d10 = lh_str_format_float_floor_log10_pow2(
        lh_math_add(e, lh_cast_static(lh_sint_t, lh_bit_scan_reverse_u32(mantissa))));
    lh_numeric_big_set_u32(num, mantissa);
    lh_numeric_big_set_u32(den, 1U);
    if (lh_math_gt(e, 0))
    {
        lh_numeric_big_shl(num, lh_cast_static(lh_usize_t, e));
    }
    else if (lh_math_lt(e, 0))
    {
        lh_numeric_big_shl(den, lh_cast_static(lh_usize_t, lh_math_neg(e)));
    }
    for (lh_sint_t i = 0; lh_math_lt(i, d10); i = lh_math_add_one(i))
    {
        lh_numeric_big_mul_u32(den, 10U);
    }
    for (lh_sint_t i = 0; lh_math_gt(i, d10); i = lh_math_sub_one(i))
    {
        lh_numeric_big_mul_u32(num, 10U);
    }
    for (;;)
    {
        next = lh_ptr_deref(den);
        lh_numeric_big_mul_u32(lh_addr_of(next), 10U);
        if (lh_math_lt(lh_numeric_big_compare(num, lh_addr_of(next)), 0))
        {
            break;
        }
        lh_ptr_deref(den) = next;
        d10 = lh_math_add_one(d10);
    }
    while (lh_math_lt(lh_numeric_big_compare(num, den), 0))
    {
        lh_numeric_big_mul_u32(num, 10U);
        d10 = lh_math_sub_one(d10);
    }
    return d10;
}

lh_bool_t
lh_str_format_float_get_digits(lh_numeric_big_t *num, const lh_numeric_big_t *den,
                               lh_usize_t precision, lh_str_ptr digits)
{
    lh_sint_t order;
    lh_usize_t k = precision;

    /* The digits, one subtraction run at a time. */
    for (lh_usize_t i = 0U; lh_math_lt(i, precision); i = lh_math_add_one(i))
    {
        lh_byte_t digit = 0U;

        while (lh_math_ge(lh_numeric_big_compare(num, den), 0))
        {
            lh_numeric_big_sub(num, den);
            digit = lh_math_add_one(digit);
        }
        digits[i] = lh_char_from_digit(digit);
        if (lh_math_lt(lh_math_add_one(i), precision))
        {
            lh_numeric_big_mul_u32(num, 10U);
        }
    }

    /* Round to nearest, ties to even, on what is left. */
    lh_numeric_big_mul_u32(num, 2U);
    order = lh_numeric_big_compare(num, den);
    if (lh_math_lt(order, 0) ||
        (lh_math_eq(order, 0) &&
         lh_math_is_zero(lh_bit_and(lh_char_to_digit(digits[lh_math_sub_one(precision)]), 1U))))
    {
        return lh_bool_false;
    }
    while (lh_math_gt(k, 0U) && lh_math_eq(digits[lh_math_sub_one(k)], lh_char_map_digit_9))
    {
        k = lh_math_sub_one(k);
        digits[k] = lh_char_map_digit_0;
    }
    if (lh_math_gt(k, 0U))
    {
        k = lh_math_sub_one(k);
        digits[k] = lh_char_from_digit(lh_math_add_one(lh_char_to_digit(digits[k])));
        return lh_bool_false;
    }
    /* 999.. rounded up to 1000..: one more power of ten, the same digits. */
    digits[0] = lh_char_map_digit_1;
    return lh_bool_true;
}

lh_usize_t
lh_str_format_float_trim(const lh_str_ptr digits, lh_usize_t precision)
{
    lh_usize_t nd = precision;

    while (lh_math_gt(nd, 1U) && lh_math_eq(digits[lh_math_sub_one(nd)], lh_char_map_digit_0))
    {
        nd = lh_math_sub_one(nd);
    }
    return nd;
}

lh_usize_t
lh_str_format_float_put_fixed(const lh_str_ptr digits, lh_usize_t nd, lh_sint_t d10, lh_str_ptr str,
                              lh_usize_t n)
{
    if (lh_math_lt(d10, 0))
    {
        n = lh_str_ptr_put_char(str, n, lh_char_map_digit_0);
        n = lh_str_ptr_put_char(str, n, lh_char_map_period);
        n = lh_str_ptr_put_run(str, n, lh_char_map_digit_0,
                               lh_cast_static(lh_usize_t, lh_math_sub_one(lh_math_neg(d10))));
        return lh_str_ptr_put_chars(str, n, digits, nd);
    }

    {
        const lh_usize_t int_digits = lh_math_add_one(lh_cast_static(lh_usize_t, d10));

        n = lh_str_ptr_put_chars(str, n, digits, lh_math_min(nd, int_digits));
        if (lh_math_gt(int_digits, nd))
        {
            return lh_str_ptr_put_run(str, n, lh_char_map_digit_0, lh_math_sub(int_digits, nd));
        }
        if (lh_math_gt(nd, int_digits))
        {
            n = lh_str_ptr_put_char(str, n, lh_char_map_period);
            n = lh_str_ptr_put_chars(str, n,
                                     lh_ptr_add_by_offset_unsafe(const lh_char_t, digits, int_digits),
                                     lh_math_sub(nd, int_digits));
        }
        return n;
    }
}

lh_usize_t
lh_str_format_float_put_exponent(const lh_str_ptr digits, lh_usize_t nd, lh_sint_t d10, lh_str_ptr str,
                                 lh_usize_t n)
{
    const lh_sint_t x = lh_math_lt(d10, 0) ? lh_math_neg(d10) : d10;

    n = lh_str_ptr_put_char(str, n, digits[0]);
    if (lh_math_gt(nd, 1U))
    {
        n = lh_str_ptr_put_char(str, n, lh_char_map_period);
        n = lh_str_ptr_put_chars(str, n, lh_ptr_add_by_offset_unsafe(const lh_char_t, digits, 1U),
                                 lh_math_sub_one(nd));
    }
    n = lh_str_ptr_put_char(str, n, lh_char_map_small_e);
    n = lh_str_ptr_put_char(str, n, lh_math_lt(d10, 0) ? lh_char_map_minus : lh_char_map_plus);
    n = lh_str_ptr_put_char(str, n, lh_char_from_digit(lh_cast_static(lh_byte_t, lh_math_div(x, 10))));
    return lh_str_ptr_put_char(str, n, lh_char_from_digit(lh_cast_static(lh_byte_t, lh_math_mod(x, 10))));
}

lh_usize_t
lh_str_ptr_format_float(lh_float_t value, lh_usize_t digits, lh_str_ptr str, lh_usize_t str_size)
{
    lh_u32_t bits;
    lh_char_t tmp[LH_STR_FORMAT_FLOAT_TMP];
    lh_char_t d[LH_STR_FORMAT_FLOAT_DIGITS_MAX];
    lh_usize_t n;
    lh_usize_t precision;
    lh_usize_t nd;
    lh_sint_t d10;
    lh_numeric_big_t num;
    lh_numeric_big_t den;

    lh_memory_std_copy(lh_addr_of(bits), lh_addr_of(value), sizeof(bits));
    n = lh_str_format_float_put_special(bits, tmp);
    if (lh_math_ne(n, 0U))
    {
        return lh_str_ptr_format_put(str, str_size, tmp, n);
    }

    precision = lh_str_format_float_get_precision(digits);
    d10 = lh_str_format_float_scale(bits, lh_addr_of(num), lh_addr_of(den));
    if (lh_str_format_float_get_digits(lh_addr_of(num), lh_addr_of(den), precision, d))
    {
        d10 = lh_math_add_one(d10);
    }
    nd = lh_str_format_float_trim(d, precision);

    if (lh_math_ne(lh_bit_shr(bits, 31U), 0U))
    {
        n = lh_str_ptr_put_char(tmp, n, lh_char_map_minus);
    }
    n = lh_math_ge(d10, -4) && lh_math_lt(d10, lh_cast_static(lh_sint_t, precision))
            ? lh_str_format_float_put_fixed(d, nd, d10, tmp, n)
            : lh_str_format_float_put_exponent(d, nd, d10, tmp, n);
    return lh_str_ptr_format_put(str, str_size, tmp, n);
}
