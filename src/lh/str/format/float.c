#include <lh/str/format/float.h>
#include <lh/bit/ops.h>
#include <lh/bool.h>
#include <lh/memory/std.h>
#include <lh/bit/scan.h>
#include <lh/numeric/big.h>
#include <lh/str/format/put.h>

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
lh_str_ptr_format_float(lh_float_t value, lh_usize_t digits, lh_str_ptr str, lh_usize_t str_size)
{
    lh_u32_t bits;
    lh_char_t tmp[LH_STR_FORMAT_FLOAT_TMP];
    lh_usize_t n = 0U;
    lh_char_t d[LH_STR_FORMAT_FLOAT_DIGITS_MAX];
    lh_usize_t precision;
    lh_usize_t nd;
    lh_usize_t i;
    lh_u32_t mantissa;
    lh_u32_t exp_bits;
    lh_sint_t e;
    lh_sint_t d10;
    lh_bool_t negative;
    lh_numeric_big_t num;
    lh_numeric_big_t den;
    lh_numeric_big_t tmpbig;


    lh_memory_std_copy(lh_addr_of(bits), lh_addr_of(value), sizeof(bits));
    negative = lh_math_ne(lh_bit_shr(bits, 31U), 0U) ? lh_bool_true : lh_bool_false;
    exp_bits = lh_bit_and(lh_bit_shr(bits, 23U), 0xFFU);
    mantissa = lh_bit_and(bits, 0x7FFFFFU);

    if (negative)
    {
        tmp[n++] = '-';
    }

    if (exp_bits == 0xFFU)
    {
        if (mantissa != 0U)
        {
            /* A NaN has no sign worth printing. */
            n = 0U;
            tmp[n++] = 'n';
            tmp[n++] = 'a';
            tmp[n++] = 'n';
        }
        else
        {
            tmp[n++] = 'i';
            tmp[n++] = 'n';
            tmp[n++] = 'f';
        }
        return lh_str_ptr_format_put(str, str_size, tmp, n);
    }
    if (exp_bits == 0U && mantissa == 0U)
    {
        tmp[n++] = '0';
        return lh_str_ptr_format_put(str, str_size, tmp, n);
    }

    precision = digits == 0U ? LH_STR_FORMAT_FLOAT_DIGITS_DEFAULT : digits;
    if (precision > LH_STR_FORMAT_FLOAT_DIGITS_MAX)
    {
        precision = LH_STR_FORMAT_FLOAT_DIGITS_MAX;
    }

    if (exp_bits == 0U)
    {
        e = -149;
    }
    else
    {
        lh_bit_set(mantissa, lh_bit_mask(23U)); /* the implicit leading 1 */
        e = lh_cast_static(lh_sint_t, exp_bits) - 150;
    }

    /* num / den = v / 10^d10, which must land in [1, 10). */
    d10 = lh_str_format_float_floor_log10_pow2(
        e + lh_cast_static(lh_sint_t, lh_bit_scan_reverse_u32(mantissa)));
    lh_numeric_big_set_u32(lh_addr_of(num), mantissa);
    lh_numeric_big_set_u32(lh_addr_of(den), 1U);
    if (e > 0)
    {
        lh_numeric_big_shl(lh_addr_of(num), lh_cast_static(lh_usize_t, e));
    }
    else if (e < 0)
    {
        lh_numeric_big_shl(lh_addr_of(den), lh_cast_static(lh_usize_t, (lh_math_neg(e))));
    }
    for (i = 0U; d10 > 0 && i < lh_cast_static(lh_usize_t, d10); ++i)
    {
        lh_numeric_big_mul_u32(lh_addr_of(den), 10U);
    }
    for (i = 0U; d10 < 0 && i < lh_cast_static(lh_usize_t, (lh_math_neg(d10))); ++i)
    {
        lh_numeric_big_mul_u32(lh_addr_of(num), 10U);
    }
    for (;;)
    {
        tmpbig = den;
        lh_numeric_big_mul_u32(lh_addr_of(tmpbig), 10U);
        if (lh_numeric_big_compare(lh_addr_of(num), lh_addr_of(tmpbig)) < 0)
        {
            break;
        }
        den = tmpbig;
        ++d10;
    }
    while (lh_numeric_big_compare(lh_addr_of(num), lh_addr_of(den)) < 0)
    {
        lh_numeric_big_mul_u32(lh_addr_of(num), 10U);
        --d10;
    }

    /* The digits, one subtraction run at a time. */
    for (i = 0U; i < precision; ++i)
    {
        lh_u32_t digit = 0U;

        while (lh_numeric_big_compare(lh_addr_of(num), lh_addr_of(den)) >= 0)
        {
            lh_numeric_big_sub(lh_addr_of(num), lh_addr_of(den));
            ++digit;
        }
        d[i] = lh_cast_static(lh_char_t, ('0' + digit));
        if (i + 1U < precision)
        {
            lh_numeric_big_mul_u32(lh_addr_of(num), 10U);
        }
    }

    /* Round to nearest, ties to even, on what is left. */
    lh_numeric_big_mul_u32(lh_addr_of(num), 2U);
    i = lh_numeric_big_compare(lh_addr_of(num), lh_addr_of(den)) > 0 ||
                (lh_numeric_big_compare(lh_addr_of(num), lh_addr_of(den)) == 0 &&
                 (lh_bit_and(lh_cast_static(lh_u32_t, (d[precision - 1U] - '0')), 1U) != 0U))
            ? 1U
            : 0U;
    if (i != 0U)
    {
        lh_usize_t k = precision;

        while (k > 0U)
        {
            if (d[k - 1U] == '9')
            {
                d[k - 1U] = '0';
                --k;
            }
            else
            {
                ++d[k - 1U];
                break;
            }
        }
        if (k == 0U)
        {
            /* 999.. rounded up to 1000..: one more power of ten, the same digits. */
            d[0] = '1';
            ++d10;
        }
    }

    nd = precision;
    while (nd > 1U && d[nd - 1U] == '0')
    {
        --nd;
    }

    if (d10 >= -4 && d10 < lh_cast_static(lh_sint_t, precision))
    {
        if (d10 >= 0)
        {
            const lh_usize_t int_digits = lh_cast_static(lh_usize_t, d10) + 1U;

            for (i = 0U; i < int_digits; ++i)
            {
                tmp[n++] = i < nd ? d[i] : '0';
            }
            if (nd > int_digits)
            {
                tmp[n++] = '.';
                for (i = int_digits; i < nd; ++i)
                {
                    tmp[n++] = d[i];
                }
            }
        }
        else
        {
            tmp[n++] = '0';
            tmp[n++] = '.';
            for (i = 0U; i < lh_cast_static(lh_usize_t, (lh_math_neg(d10) - 1)); ++i)
            {
                tmp[n++] = '0';
            }
            for (i = 0U; i < nd; ++i)
            {
                tmp[n++] = d[i];
            }
        }
    }
    else
    {
        int x = d10;

        tmp[n++] = d[0];
        if (nd > 1U)
        {
            tmp[n++] = '.';
            for (i = 1U; i < nd; ++i)
            {
                tmp[n++] = d[i];
            }
        }
        tmp[n++] = 'e';
        if (x < 0)
        {
            tmp[n++] = '-';
            x = lh_math_neg(x);
        }
        else
        {
            tmp[n++] = '+';
        }
        tmp[n++] = lh_cast_static(lh_char_t, ('0' + (x / 10)));
        tmp[n++] = lh_cast_static(lh_char_t, ('0' + (x % 10)));
    }

    return lh_str_ptr_format_put(str, str_size, tmp, n);
}
