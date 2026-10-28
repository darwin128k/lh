#include <lh/real/decimal.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/real/bits.h>
#include <lh/real/lemire.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_u64_t
lh_real_decimal_get_mantissa(const lh_real_decimal_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mantissa;
}

lh_void
lh_real_decimal_set_mantissa(lh_real_decimal_t *self, lh_u64_t mantissa)
{
    lh_assert_runtime_ref(self);
    self->mantissa = mantissa;
}

lh_s64_t
lh_real_decimal_get_exponent(const lh_real_decimal_t *self)
{
    lh_assert_runtime_ref(self);
    return self->exponent;
}

lh_void
lh_real_decimal_set_exponent(lh_real_decimal_t *self, lh_s64_t exponent)
{
    lh_assert_runtime_ref(self);
    self->exponent = exponent;
}

lh_bool_t
lh_real_decimal_is_negative(const lh_real_decimal_t *self)
{
    lh_assert_runtime_ref(self);
    return self->negative;
}

lh_void
lh_real_decimal_set_negative(lh_real_decimal_t *self, lh_bool_t negative)
{
    lh_assert_runtime_ref(self);
    self->negative = negative;
}

lh_bool_t
lh_real_decimal_is_truncated(const lh_real_decimal_t *self)
{
    lh_assert_runtime_ref(self);
    return self->truncated;
}

lh_void
lh_real_decimal_set_truncated(lh_real_decimal_t *self, lh_bool_t truncated)
{
    lh_assert_runtime_ref(self);
    self->truncated = truncated;
}

/* ── build ───────────────────────────────────────────────────────────────── */

lh_void
lh_real_decimal_init(lh_real_decimal_t *self)
{
    lh_real_decimal_set_mantissa(self, 0U);
    lh_real_decimal_set_exponent(self, 0);
    lh_real_decimal_set_negative(self, lh_bool_false);
    lh_real_decimal_set_truncated(self, lh_bool_false);
}

lh_void
lh_real_decimal_add_exponent(lh_real_decimal_t *self, lh_s64_t delta)
{
    lh_real_decimal_set_exponent(self, lh_math_add(lh_real_decimal_get_exponent(self), delta));
}

lh_void
lh_real_decimal_push_digit(lh_real_decimal_t *self, lh_byte_t digit, lh_bool_t fraction)
{
    const lh_u64_t mantissa = lh_real_decimal_get_mantissa(self);

    if (lh_math_is_zero(mantissa) && lh_math_is_zero(digit))
    {
        /* A leading zero holds no digit; after the point it still shifts. */
        if (fraction)
        {
            lh_real_decimal_add_exponent(self, -1);
        }
        return;
    }
    /* Every value with 19 digits is below 10^19 < 2^64: no check needed. */
    if (lh_math_lt(mantissa, 1000000000000000000ULL))
    {
        lh_real_decimal_set_mantissa(self, lh_math_add(lh_math_mul(mantissa, 10U), digit));
        if (fraction)
        {
            lh_real_decimal_add_exponent(self, -1);
        }
        return;
    }
    if (!fraction)
    {
        lh_real_decimal_add_exponent(self, 1);
    }
    if (!lh_math_is_zero(digit))
    {
        lh_real_decimal_set_truncated(self, lh_bool_true);
    }
}

lh_bool_t
lh_real_decimal_parse(lh_real_decimal_t *self, const lh_str_ptr str, lh_usize_t str_size)
{
    lh_usize_t pos = 0U;
    lh_bool_t digits = lh_bool_false;

    lh_assert_runtime_if(lh_null_eq(str) && !lh_math_is_zero(str_size),
                         lh_runtime_error_code_null_pointer);
    lh_real_decimal_init(self);

    if (lh_math_lt(pos, str_size) && lh_math_eq(str[pos], lh_char_map_minus))
    {
        lh_real_decimal_set_negative(self, lh_bool_true);
        pos = lh_math_add_one(pos);
    }
    for (; lh_math_lt(pos, str_size) && lh_char_is_digit(str[pos]); pos = lh_math_add_one(pos))
    {
        lh_real_decimal_push_digit(self, lh_char_to_digit(str[pos]), lh_bool_false);
        digits = lh_bool_true;
    }
    if (lh_math_lt(pos, str_size) && lh_math_eq(str[pos], lh_char_map_period))
    {
        for (pos = lh_math_add_one(pos); lh_math_lt(pos, str_size) && lh_char_is_digit(str[pos]);
             pos = lh_math_add_one(pos))
        {
            lh_real_decimal_push_digit(self, lh_char_to_digit(str[pos]), lh_bool_true);
            digits = lh_bool_true;
        }
    }
    if (!digits)
    {
        return lh_bool_false;
    }

    if (lh_math_lt(pos, str_size) &&
        (lh_math_eq(str[pos], lh_char_map_small_e) || lh_math_eq(str[pos], lh_char_map_capital_e)))
    {
        lh_bool_t minus = lh_bool_false;
        lh_s64_t value = 0;
        lh_usize_t start;

        pos = lh_math_add_one(pos);
        if (lh_math_lt(pos, str_size) &&
            (lh_math_eq(str[pos], lh_char_map_plus) || lh_math_eq(str[pos], lh_char_map_minus)))
        {
            minus = lh_cast_static(lh_bool_t, lh_math_eq(str[pos], lh_char_map_minus));
            pos = lh_math_add_one(pos);
        }
        for (start = pos; lh_math_lt(pos, str_size) && lh_char_is_digit(str[pos]);
             pos = lh_math_add_one(pos))
        {
            if (lh_math_lt(value, LH_REAL_DECIMAL_EXPONENT_LIMIT))
            {
                value = lh_math_add(lh_math_mul(value, 10), lh_char_to_digit(str[pos]));
            }
        }
        if (lh_math_eq(pos, start))
        {
            return lh_bool_false;
        }
        lh_real_decimal_add_exponent(self, minus ? lh_math_neg(value) : value);
    }

    return lh_cast_static(lh_bool_t, lh_math_eq(pos, str_size));
}

#if LH_REAL_ENABLED
lh_bool_t
lh_real_decimal_to_real(const lh_real_decimal_t *self, lh_real_t *out)
{
    lh_u64_t fraction = 0U;
    lh_s32_t exponent = 0;

    lh_assert_runtime_ref(out);
    if (!lh_math_is_zero(lh_real_decimal_get_mantissa(self)))
    {
        /* Truncated or not, the first 19 digits decide (see the header). */
        lh_real_lemire_compute(lh_real_decimal_get_exponent(self),
                               lh_real_decimal_get_mantissa(self), lh_addr_of(fraction),
                               lh_addr_of(exponent));
        if (lh_math_ge(exponent, LH_REAL_EXPONENT_MAX))
        {
            return lh_bool_false;
        }
    }
    lh_ptr_deref(out) = lh_real_from_bits(lh_real_bits_make(
        lh_real_decimal_is_negative(self), lh_cast_static(lh_u32_t, exponent), fraction));
    return lh_bool_true;
}
#endif
