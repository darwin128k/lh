#include <lh/str/format/float.h>
#include <lh/assert.h>
#include <lh/memory/std.h>

/*
 * A float is m * 2^e with m a 24-bit integer. To get N decimal digits that are
 * right, not close, the ratio v / 10^d10 is built as num / den in a small
 * unsigned big number, and the digits come out one at a time by subtraction:
 * the same schoolbook division a person does, with no floating point in it.
 *
 * Widest case: m << 104 (v near 2^128) against 10^38, and 10^45 * m against
 * 2^149 (the smallest subnormals). Both fit in 192 bits; the number carries
 * 256 so that `* 10` for the next digit never has to check.
 */

#define LH_STR_FORMAT_FLOAT_WORDS 8U
#define LH_STR_FORMAT_FLOAT_TMP 40U

/** An unsigned 256-bit number, as the division above needs it. */
typedef struct lh_str_format_float_big
{
    lh_u32_t w[LH_STR_FORMAT_FLOAT_WORDS]; /**< 32-bit words, least significant first */
} lh_str_format_float_big_t; /**< An unsigned 256-bit number. */

static void
big_set(lh_str_format_float_big_t *b, lh_u32_t value)
{
    lh_usize_t i;

    b->w[0] = value;
    for (i = 1U; i < LH_STR_FORMAT_FLOAT_WORDS; ++i)
    {
        b->w[i] = 0U;
    }
}

static void
big_mul_small(lh_str_format_float_big_t *b, lh_u32_t k)
{
    lh_u64_t carry = 0U;
    lh_usize_t i;

    for (i = 0U; i < LH_STR_FORMAT_FLOAT_WORDS; ++i)
    {
        const lh_u64_t p = lh_cast_static(lh_u64_t, b->w[i])* lh_cast_static(lh_u64_t, k)+ carry;

        b->w[i] = lh_cast_static(lh_u32_t, (p & 0xFFFFFFFFU));
        carry = p >> 32;
    }
}

static void
big_shl(lh_str_format_float_big_t *b, lh_usize_t bits)
{
    const lh_usize_t words = bits / 32U;
    const lh_usize_t rest = bits % 32U;
    lh_usize_t i;

    for (i = LH_STR_FORMAT_FLOAT_WORDS; i > 0U; --i)
    {
        lh_u32_t v = 0U;

        if (i - 1U >= words)
        {
            v = b->w[i - 1U - words] << rest;
            if (rest != 0U && i - 1U > words)
            {
                v |= b->w[i - 2U - words] >> (32U - rest);
            }
        }
        b->w[i - 1U] = v;
    }
}

static int
big_cmp(const lh_str_format_float_big_t *a, const lh_str_format_float_big_t *b)
{
    lh_usize_t i;

    for (i = LH_STR_FORMAT_FLOAT_WORDS; i > 0U; --i)
    {
        if (a->w[i - 1U] != b->w[i - 1U])
        {
            return a->w[i - 1U] < b->w[i - 1U] ? -1 : 1;
        }
    }
    return 0;
}

/* a -= b; the caller guarantees a >= b. */
static void
big_sub(lh_str_format_float_big_t *a, const lh_str_format_float_big_t *b)
{
    lh_u64_t borrow = 0U;
    lh_usize_t i;

    for (i = 0U; i < LH_STR_FORMAT_FLOAT_WORDS; ++i)
    {
        const lh_u64_t x = lh_cast_static(lh_u64_t, a->w[i]);
        const lh_u64_t y = lh_cast_static(lh_u64_t, b->w[i])+ borrow;

        if (x >= y)
        {
            a->w[i] = lh_cast_static(lh_u32_t, (x - y));
            borrow = 0U;
        }
        else
        {
            a->w[i] = lh_cast_static(lh_u32_t, ((x + 0x100000000ULL) - y));
            borrow = 1U;
        }
    }
}

/* Floor of log2(m) for m > 0. */
static int
bit_length_minus_one(lh_u32_t m)
{
    int n = 0;

    while (m > 1U)
    {
        m >>= 1U;
        ++n;
    }
    return n;
}

/* Floor of (x * log10(2)), with log10(2) as 78913 / 2^18 -- plenty for |x| < 200. */
static int
floor_log10_pow2(int x)
{
    const long scaled = lh_cast_static(long, x)* 78913L;

    return (int)(scaled >= 0 ? (scaled >> 18) : -(((-scaled) + ((1L << 18) - 1L)) >> 18));
}

static lh_usize_t
put_text(lh_str_ptr str, lh_usize_t str_size, const lh_char_t *text, lh_usize_t n)
{
    if (n > str_size)
    {
        return 0U;
    }
    lh_memory_std_copy(str, text, n);
    return n;
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
    int e;
    int d10;
    lh_bool_t negative;
    lh_str_format_float_big_t num;
    lh_str_format_float_big_t den;
    lh_str_format_float_big_t tmpbig;

    lh_assert_runtime_ref(str);

    lh_memory_std_copy(&bits, &value, sizeof(bits));
    negative = (bits >> 31) != 0U ? lh_bool_true : lh_bool_false;
    exp_bits = (bits >> 23) & 0xFFU;
    mantissa = bits & 0x7FFFFFU;

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
        return put_text(str, str_size, tmp, n);
    }
    if (exp_bits == 0U && mantissa == 0U)
    {
        tmp[n++] = '0';
        return put_text(str, str_size, tmp, n);
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
        mantissa |= 0x800000U;
        e = lh_cast_static(int, exp_bits)- 150;
    }

    /* num / den = v / 10^d10, which must land in [1, 10). */
    d10 = floor_log10_pow2(e + bit_length_minus_one(mantissa));
    big_set(&num, mantissa);
    big_set(&den, 1U);
    if (e > 0)
    {
        big_shl(&num, lh_cast_static(lh_usize_t, e));
    }
    else if (e < 0)
    {
        big_shl(&den, lh_cast_static(lh_usize_t, (-e)));
    }
    for (i = 0U; d10 > 0 && i < lh_cast_static(lh_usize_t, d10); ++i)
    {
        big_mul_small(&den, 10U);
    }
    for (i = 0U; d10 < 0 && i < lh_cast_static(lh_usize_t, (-d10)); ++i)
    {
        big_mul_small(&num, 10U);
    }
    for (;;)
    {
        tmpbig = den;
        big_mul_small(&tmpbig, 10U);
        if (big_cmp(&num, &tmpbig) < 0)
        {
            break;
        }
        den = tmpbig;
        ++d10;
    }
    while (big_cmp(&num, &den) < 0)
    {
        big_mul_small(&num, 10U);
        --d10;
    }

    /* The digits, one subtraction run at a time. */
    for (i = 0U; i < precision; ++i)
    {
        lh_u32_t digit = 0U;

        while (big_cmp(&num, &den) >= 0)
        {
            big_sub(&num, &den);
            ++digit;
        }
        d[i] = lh_cast_static(lh_char_t, ('0' + digit));
        if (i + 1U < precision)
        {
            big_mul_small(&num, 10U);
        }
    }

    /* Round to nearest, ties to even, on what is left. */
    big_mul_small(&num, 2U);
    i = big_cmp(&num, &den) > 0 ||
                (big_cmp(&num, &den) == 0 && ((lh_cast_static(lh_u32_t, (d[precision - 1U] - '0')) & 1U) != 0U))
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

    if (d10 >= -4 && d10 < lh_cast_static(int, precision))
    {
        if (d10 >= 0)
        {
            const lh_usize_t int_digits = lh_cast_static(lh_usize_t, d10)+ 1U;

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
            for (i = 0U; i < lh_cast_static(lh_usize_t, (-d10 - 1)); ++i)
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
            x = -x;
        }
        else
        {
            tmp[n++] = '+';
        }
        tmp[n++] = lh_cast_static(lh_char_t, ('0' + (x / 10)));
        tmp[n++] = lh_cast_static(lh_char_t, ('0' + (x % 10)));
    }

    return put_text(str, str_size, tmp, n);
}
