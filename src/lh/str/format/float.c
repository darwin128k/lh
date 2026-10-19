#include <lh/str/format/float.h>
#include <lh/bit/ops.h>
#include <lh/bit/scan.h>
#include <lh/bool.h>
#include <lh/char/digit.h>
#include <lh/char/map.h>
#include <lh/math.h>
#include <lh/memory/std.h>
#include <lh/numeric/big.h>
#include <lh/numeric/fixed/limits.h>
#include <lh/str/format/put.h>
#include <lh/str/format/uint.h>
#include <lh/util/ptr.h>
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
 *
 * Everyday magnitudes keep the ratio inside u64; that path never touches the
 * 256-bit words.
 */

#define LH_STR_FORMAT_FLOAT_TMP 40U

/** 10^0 .. 10^19 — every power that still fits in u64. */
static const lh_u64_t lh_str_format_float_pow10[20] = {
    1ULL,
    10ULL,
    100ULL,
    1000ULL,
    10000ULL,
    100000ULL,
    1000000ULL,
    10000000ULL,
    100000000ULL,
    1000000000ULL,
    10000000000ULL,
    100000000000ULL,
    1000000000000ULL,
    10000000000000ULL,
    100000000000000ULL,
    1000000000000000ULL,
    10000000000000000ULL,
    100000000000000000ULL,
    1000000000000000000ULL,
    10000000000000000000ULL,
};

lh_void
lh_str_format_float_big_set_u64(lh_numeric_big_t *self, lh_u64_t value)
{
    self->words[0] = lh_cast_static(lh_u32_t, lh_bit_and(value, 0xFFFFFFFFULL));
    self->words[1] = lh_cast_static(lh_u32_t, lh_bit_shr(value, 32U));
    for (lh_usize_t i = 2U; lh_math_lt(i, LH_NUMERIC_BIG_WORDS); i = lh_math_add_one(i))
    {
        self->words[i] = 0U;
    }
}

lh_bool_t
lh_str_format_float_big_as_u64(const lh_numeric_big_t *self, lh_u64_t *out)
{
    for (lh_usize_t i = 2U; lh_math_lt(i, LH_NUMERIC_BIG_WORDS); i = lh_math_add_one(i))
    {
        if (lh_math_ne(self->words[i], 0U))
        {
            return lh_bool_false;
        }
    }
    lh_ptr_deref(out) = lh_bit_or(lh_cast_static(lh_u64_t, self->words[0]),
                                  lh_bit_shl(lh_cast_static(lh_u64_t, self->words[1]), 32U));
    return lh_bool_true;
}

lh_bool_t
lh_str_format_float_u64_mul_pow10(lh_u64_t *x, lh_usize_t k)
{
    lh_u64_t p;

    if (lh_math_ge(k, 20U))
    {
        return lh_bool_false;
    }
    p = lh_str_format_float_pow10[k];
    if (lh_math_gt(lh_ptr_deref(x), lh_math_div(LH_U64_T_MAX, p)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(x) = lh_math_mul(lh_ptr_deref(x), p);
    return lh_bool_true;
}

lh_void
lh_str_format_float_unpack(lh_u32_t bits, lh_u32_t *mantissa, lh_sint_t *e)
{
    const lh_u32_t exp_bits = lh_bit_and(lh_bit_shr(bits, 23U), 0xFFU);
    lh_u32_t man = lh_bit_and(bits, 0x7FFFFFU);

    if (lh_math_ne(exp_bits, 0U))
    {
        lh_bit_set(man, lh_bit_mask(23U));
        lh_ptr_deref(e) = lh_math_sub(lh_cast_static(lh_sint_t, exp_bits), 150);
    }
    else
    {
        lh_ptr_deref(e) = -149;
    }
    lh_ptr_deref(mantissa) = man;
}

lh_bool_t
lh_str_format_float_u32_mul_pow10(lh_u32_t *x, lh_usize_t k)
{
    lh_u32_t p;

    if (lh_math_ge(k, 10U)) /* 10^10 does not fit in u32 */
    {
        return lh_bool_false;
    }
    p = lh_cast_static(lh_u32_t, lh_str_format_float_pow10[k]);
    if (lh_math_gt(lh_ptr_deref(x), lh_math_div(LH_U32_T_MAX, p)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(x) = lh_math_mul(lh_ptr_deref(x), p);
    return lh_bool_true;
}

lh_bool_t
lh_str_format_float_round_digits(lh_sint_t order, lh_usize_t precision, lh_str_ptr digits)
{
    lh_usize_t k = precision;

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
    digits[0] = lh_char_map_digit_1;
    return lh_bool_true;
}

lh_bool_t
lh_str_format_float_scale_u32(lh_u32_t mantissa, lh_sint_t e, lh_u32_t *num, lh_u32_t *den,
                              lh_sint_t *d10_out)
{
    lh_sint_t d10;
    lh_u32_t n;
    lh_u32_t d;

    if (lh_math_lt(e, -31) || lh_math_gt(e, 31))
    {
        return lh_bool_false;
    }

    d10 = lh_str_format_float_floor_log10_pow2(
        lh_math_add(e, lh_cast_static(lh_sint_t, lh_bit_scan_reverse_u32(mantissa))));
    n = mantissa;
    d = 1U;

    if (lh_math_gt(e, 0))
    {
        /* mantissa is ≤24 bits; e≤31 keeps the product in u32 when it fits. */
        if (lh_math_gt(lh_bit_scan_reverse_u32(n), lh_math_sub(31U, lh_cast_static(lh_u32_t, e))))
        {
            return lh_bool_false;
        }
        n = lh_bit_shl(n, lh_cast_static(lh_usize_t, e));
    }
    else if (lh_math_lt(e, 0))
    {
        d = lh_bit_shl(d, lh_cast_static(lh_usize_t, lh_math_neg(e)));
    }

    if (lh_math_gt(d10, 0))
    {
        if (!lh_str_format_float_u32_mul_pow10(lh_addr_of(d), lh_cast_static(lh_usize_t, d10)))
        {
            return lh_bool_false;
        }
    }
    else if (lh_math_lt(d10, 0))
    {
        if (!lh_str_format_float_u32_mul_pow10(lh_addr_of(n),
                                               lh_cast_static(lh_usize_t, lh_math_neg(d10))))
        {
            return lh_bool_false;
        }
    }

    for (;;)
    {
        lh_u32_t next32;

        if (lh_math_gt(d, lh_math_div(LH_U32_T_MAX, 10U)))
        {
            return lh_bool_false;
        }
        next32 = lh_math_mul(d, 10U);
        if (lh_math_lt(n, next32))
        {
            break;
        }
        d = next32;
        d10 = lh_math_add_one(d10);
    }
    while (lh_math_lt(n, d))
    {
        if (lh_math_gt(n, lh_math_div(LH_U32_T_MAX, 10U)))
        {
            return lh_bool_false;
        }
        n = lh_math_mul(n, 10U);
        d10 = lh_math_sub_one(d10);
    }

    if (lh_math_is_zero(d) || lh_math_lt(n, d) || lh_math_ge(d, lh_math_div(LH_U32_T_MAX, 10U)))
    {
        return lh_bool_false;
    }

    lh_ptr_deref(num) = n;
    lh_ptr_deref(den) = d;
    lh_ptr_deref(d10_out) = d10;
    return lh_bool_true;
}

lh_bool_t
lh_str_format_float_get_digits_u32(lh_u32_t num, lh_u32_t den, lh_usize_t precision,
                                   lh_str_ptr digits)
{
    lh_u32_t rem = num;
    lh_sint_t order;

    for (lh_usize_t i = 0U; lh_math_lt(i, precision); i = lh_math_add_one(i))
    {
        const lh_u32_t digit = lh_math_div(rem, den);

        rem = lh_math_mod(rem, den);
        digits[i] = lh_char_from_digit(lh_cast_static(lh_byte_t, digit));
        if (lh_math_lt(lh_math_add_one(i), precision))
        {
            rem = lh_math_mul(rem, 10U);
        }
    }

    rem = lh_math_mul(rem, 2U);
    order = lh_math_lt(rem, den) ? -1 : (lh_math_eq(rem, den) ? 0 : 1);
    return lh_str_format_float_round_digits(order, precision, digits);
}

lh_bool_t
lh_str_format_float_scale_u64(lh_u32_t mantissa, lh_sint_t e, lh_u64_t *num, lh_u64_t *den,
                              lh_sint_t *d10_out)
{
    lh_sint_t d10;
    lh_u64_t n;
    lh_u64_t d;
    lh_bool_t ok;

    if (lh_math_lt(e, -63) || lh_math_gt(e, 63))
    {
        return lh_bool_false;
    }

    d10 = lh_str_format_float_floor_log10_pow2(
        lh_math_add(e, lh_cast_static(lh_sint_t, lh_bit_scan_reverse_u32(mantissa))));
    n = lh_cast_static(lh_u64_t, mantissa);
    d = 1ULL;
    ok = lh_bool_true;

    if (lh_math_gt(e, 0))
    {
        n = lh_bit_shl(n, lh_cast_static(lh_usize_t, e));
    }
    else if (lh_math_lt(e, 0))
    {
        d = lh_bit_shl(d, lh_cast_static(lh_usize_t, lh_math_neg(e)));
    }

    if (lh_math_gt(d10, 0))
    {
        ok = lh_str_format_float_u64_mul_pow10(lh_addr_of(d), lh_cast_static(lh_usize_t, d10));
    }
    else if (lh_math_lt(d10, 0))
    {
        ok = lh_str_format_float_u64_mul_pow10(lh_addr_of(n),
                                               lh_cast_static(lh_usize_t, lh_math_neg(d10)));
    }

    while (ok)
    {
        lh_u64_t next64;

        if (lh_math_gt(d, lh_math_div(LH_U64_T_MAX, 10U)))
        {
            return lh_bool_false;
        }
        next64 = lh_math_mul(d, 10U);
        if (lh_math_lt(n, next64))
        {
            break;
        }
        d = next64;
        d10 = lh_math_add_one(d10);
    }
    while (ok && lh_math_lt(n, d))
    {
        ok = lh_str_format_float_u64_mul_pow10(lh_addr_of(n), 1U);
        if (!ok)
        {
            return lh_bool_false;
        }
        d10 = lh_math_sub_one(d10);
    }

    if (!ok || lh_math_is_zero(d) || lh_math_lt(n, d) ||
        lh_math_ge(d, lh_math_div(LH_U64_T_MAX, 10U)))
    {
        return lh_bool_false;
    }

    lh_ptr_deref(num) = n;
    lh_ptr_deref(den) = d;
    lh_ptr_deref(d10_out) = d10;
    return lh_bool_true;
}

lh_bool_t
lh_str_format_float_get_digits_u64(lh_u64_t num, lh_u64_t den, lh_usize_t precision,
                                   lh_str_ptr digits)
{
    lh_u64_t rem = num;
    lh_sint_t order;

    for (lh_usize_t i = 0U; lh_math_lt(i, precision); i = lh_math_add_one(i))
    {
        const lh_u64_t digit = lh_math_div(rem, den);

        rem = lh_math_mod(rem, den);
        digits[i] = lh_char_from_digit(lh_cast_static(lh_byte_t, digit));
        if (lh_math_lt(lh_math_add_one(i), precision))
        {
            rem = lh_math_mul(rem, 10U);
        }
    }

    rem = lh_math_mul(rem, 2U);
    order = lh_math_lt(rem, den) ? -1 : (lh_math_eq(rem, den) ? 0 : 1);
    return lh_str_format_float_round_digits(order, precision, digits);
}

lh_sint_t
lh_str_format_float_floor_log10_pow2(lh_sint_t x)
{
    const long scaled = lh_cast_static(long, x) * 78913L;

    return (lh_sint_t)(scaled >= 0 ? lh_bit_shr(scaled, 18)
                                   : lh_math_neg(lh_bit_shr(
                                         lh_math_add(lh_math_neg(scaled),
                                                     lh_math_sub_one(lh_bit_shl(1L, 18))),
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
    lh_u32_t mantissa;
    lh_sint_t e;
    lh_sint_t d10;
    lh_u64_t n64;
    lh_u64_t d64;
    lh_numeric_big_t next;

    lh_str_format_float_unpack(bits, lh_addr_of(mantissa), lh_addr_of(e));
    {
        lh_u32_t n32;
        lh_u32_t d32;

        if (lh_str_format_float_scale_u32(mantissa, e, lh_addr_of(n32), lh_addr_of(d32),
                                          lh_addr_of(d10)))
        {
            lh_str_format_float_big_set_u64(num, lh_cast_static(lh_u64_t, n32));
            lh_str_format_float_big_set_u64(den, lh_cast_static(lh_u64_t, d32));
            return d10;
        }
    }
    if (lh_str_format_float_scale_u64(mantissa, e, lh_addr_of(n64), lh_addr_of(d64),
                                      lh_addr_of(d10)))
    {
        lh_str_format_float_big_set_u64(num, n64);
        lh_str_format_float_big_set_u64(den, d64);
        return d10;
    }

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
    lh_u64_t num64;
    lh_u64_t den64;

    if (lh_str_format_float_big_as_u64(num, lh_addr_of(num64)) &&
        lh_str_format_float_big_as_u64(den, lh_addr_of(den64)) && lh_math_ne(den64, 0U) &&
        lh_math_lt(den64, lh_math_div(LH_U64_T_MAX, 10U)))
    {
        return lh_str_format_float_get_digits_u64(num64, den64, precision, digits);
    }

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
            n = lh_str_ptr_put_chars(
                str, n, lh_ptr_add_by_offset_unsafe(const lh_char_t, digits, int_digits),
                lh_math_sub(nd, int_digits));
        }
        return n;
    }
}

lh_usize_t
lh_str_format_float_put_exponent(const lh_str_ptr digits, lh_usize_t nd, lh_sint_t d10,
                                 lh_str_ptr str, lh_usize_t n)
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
    n = lh_str_ptr_put_char(str, n,
                            lh_char_from_digit(lh_cast_static(lh_byte_t, lh_math_div(x, 10))));
    return lh_str_ptr_put_char(str, n,
                               lh_char_from_digit(lh_cast_static(lh_byte_t, lh_math_mod(x, 10))));
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
    lh_u32_t mantissa;
    lh_sint_t e;
    lh_u64_t num64;
    lh_u64_t den64;
    lh_numeric_big_t num;
    lh_numeric_big_t den;

    lh_memory_std_copy(lh_addr_of(bits), lh_addr_of(value), sizeof(bits));
    n = lh_str_format_float_put_special(bits, tmp);
    if (lh_math_ne(n, 0U))
    {
        return lh_str_ptr_format_put(str, str_size, tmp, n);
    }

    /* Exact integers with |v| < 2^24 when %g would use fixed form (not e+XX). */
    {
        const lh_u32_t exp_bits = lh_bit_and(lh_bit_shr(bits, 23U), 0xFFU);
        const lh_u32_t man = lh_bit_and(bits, 0x7FFFFFU);

        if (lh_math_ge(exp_bits, 127U) && lh_math_le(exp_bits, 150U))
        {
            const lh_u32_t shift = lh_math_sub(150U, exp_bits);
            const lh_u32_t sig = lh_bit_or(0x800000U, man);
            const lh_u32_t frac_mask =
                lh_math_is_zero(shift) ? 0U : lh_math_sub(lh_bit_shl(1U, shift), 1U);

            if (lh_math_is_zero(lh_bit_and(sig, frac_mask)))
            {
                const lh_uint_t mag = lh_cast_static(lh_uint_t, lh_bit_shr(sig, shift));
                lh_uint_t t = mag;
                lh_usize_t ndigits = 1U;
                lh_sint_t int_d10;

                while (lh_math_ge(t, 10U))
                {
                    t = lh_math_div(t, 10U);
                    ndigits = lh_math_add_one(ndigits);
                }
                int_d10 = lh_cast_static(lh_sint_t, lh_math_sub_one(ndigits));
                precision = lh_str_format_float_get_precision(digits);
                if (lh_math_ge(int_d10, -4) &&
                    lh_math_lt(int_d10, lh_cast_static(lh_sint_t, precision)))
                {
                    lh_usize_t w;

                    n = 0U;
                    if (lh_math_ne(lh_bit_shr(bits, 31U), 0U))
                    {
                        n = lh_str_ptr_put_char(tmp, n, lh_char_map_minus);
                    }
                    w = lh_str_ptr_format_uint(mag, tmp + n, LH_STR_FORMAT_FLOAT_TMP - n);
                    if (lh_math_is_zero(w))
                    {
                        return 0U;
                    }
                    return lh_str_ptr_format_put(str, str_size, tmp, lh_math_add(n, w));
                }
            }
        }
    }

    precision = lh_str_format_float_get_precision(digits);
    lh_str_format_float_unpack(bits, lh_addr_of(mantissa), lh_addr_of(e));

    /* Prefer u32 (HW div), then u64, then 256-bit big. */
    {
        lh_u32_t num32;
        lh_u32_t den32;

        if (lh_str_format_float_scale_u32(mantissa, e, lh_addr_of(num32), lh_addr_of(den32),
                                          lh_addr_of(d10)))
        {
            if (lh_str_format_float_get_digits_u32(num32, den32, precision, d))
            {
                d10 = lh_math_add_one(d10);
            }
        }
        else if (lh_str_format_float_scale_u64(mantissa, e, lh_addr_of(num64), lh_addr_of(den64),
                                               lh_addr_of(d10)))
        {
            if (lh_str_format_float_get_digits_u64(num64, den64, precision, d))
            {
                d10 = lh_math_add_one(d10);
            }
        }
        else
        {
            d10 = lh_str_format_float_scale(bits, lh_addr_of(num), lh_addr_of(den));
            if (lh_str_format_float_get_digits(lh_addr_of(num), lh_addr_of(den), precision, d))
            {
                d10 = lh_math_add_one(d10);
            }
        }
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
