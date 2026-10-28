/**
 * @file float.h
 * @brief Format a ::lh_float_t as decimal text, `%g` style.
 *
 * Significant digits, not decimals: `lh_str_ptr_format_float(1234.5678f, 6, ...)`
 * is `1234.57`, `0.000012345f` is `1.2345e-05`, and `100000.0f` is `100000`.
 * Trailing zeros (and a bare decimal point) are dropped, as `%g` drops them.
 *
 * Pure integer arithmetic: the value is taken apart into its exact mantissa
 * and exponent and divided as a small big number, so it needs no `snprintf`,
 * no `double`, and no FPU at all, and the digits are the exact ones (not an
 * accumulated float error).
 */

#ifndef LH_STR_FORMAT_FLOAT_H
#define LH_STR_FORMAT_FLOAT_H

#include <lh/bool.h>
#include <lh/numeric/big.h>
#include <lh/numeric/float.h>
#include <lh/str/ptr.h>

/** @brief Significant digits used when the caller passes 0 (the `%g` default). */
#define LH_STR_FORMAT_FLOAT_DIGITS_DEFAULT 6U

/** @brief Most significant digits that mean anything for a single-precision float. */
#define LH_STR_FORMAT_FLOAT_DIGITS_MAX 9U

LH_COMPILER_EXTERN_C_BEGIN

/** Set @p self to @p value. */
lh_void
lh_str_format_float_big_set_u64(lh_numeric_big_t *self, lh_u64_t value);

/** @p self into @p out when it fits 64 bits; false otherwise. */
lh_bool_t
lh_str_format_float_big_as_u64(const lh_numeric_big_t *self, lh_u64_t *out);

/** @p *x *= 10^@p k when the product fits in u64. */
lh_bool_t
lh_str_format_float_u64_mul_pow10(lh_u64_t *x, lh_usize_t k);

/**
 * Unpack a finite non-zero float into mantissa and binary exponent such that
 * value = mantissa * 2^e (mantissa has the implicit 1 for normals).
 */
lh_void
lh_str_format_float_unpack(lh_u32_t bits, lh_u32_t *mantissa, lh_sint_t *e);

/** @p *x *= 10^@p k when the product fits in u32. */
lh_bool_t
lh_str_format_float_u32_mul_pow10(lh_u32_t *x, lh_usize_t k);

/**
 * Round @p digits of a [1,10) ratio after the last remainder compare @p order.
 * Shared by the u32 / u64 digit extractors.
 */
lh_bool_t
lh_str_format_float_round_digits(lh_sint_t order, lh_usize_t precision, lh_str_ptr digits);

/**
 * Scale into [1, 10) using only u32 (hardware UDIV on Cortex-M).
 * Covers typical magnitudes such as ±pi.
 */
lh_bool_t
lh_str_format_float_scale_u32(lh_u32_t mantissa, lh_sint_t e, lh_u32_t *num, lh_u32_t *den,
                              lh_sint_t *d10_out);

/** The first @p precision decimal digits of @p num / @p den into @p digits, rounded half to even;
 * ::lh_str_format_float_get_digits_u64 in 32 bits. */
lh_bool_t
lh_str_format_float_get_digits_u32(lh_u32_t num, lh_u32_t den, lh_usize_t precision,
                                   lh_str_ptr digits);

/**
 * Scale @p mantissa * 2^@p e into num/den in [1, 10) as u64.
 * @return lh_bool_true and writes @p *d10, @p *num, @p *den on success.
 */
lh_bool_t
lh_str_format_float_scale_u64(lh_u32_t mantissa, lh_sint_t e, lh_u64_t *num, lh_u64_t *den,
                              lh_sint_t *d10_out);

/**
 * Digits of @p num / @p den in [1, 10) via native u64 divide.
 * @return lh_bool_true when rounding carried into a new leading 1.
 */
lh_bool_t
lh_str_format_float_get_digits_u64(lh_u64_t num, lh_u64_t den, lh_usize_t precision,
                                   lh_str_ptr digits);

/**
 * @brief Floor of @p x * log10(2): the decimal exponent of 2^@p x.
 *
 * log10(2) is taken as 78913 / 2^18, exact enough for |@p x| < 200 — every
 * binary exponent a single-precision float has.
 *
 * @param x Binary exponent.
 * @return floor(@p x * log10(2)).
 */
lh_sint_t
lh_str_format_float_floor_log10_pow2(lh_sint_t x);

/** @brief The significant digits to print: @p digits, 0 meaning the default, at most the maximum.
 */
lh_usize_t
lh_str_format_float_get_precision(lh_usize_t digits);

/**
 * @brief Write the text of a value that is not a finite non-zero number —
 *        `nan`, `inf` / `-inf`, `0` / `-0` — at @p str.
 *
 * @param bits The float's bits.
 * @param str  Room for four characters.
 * @return Characters written, or @c 0 for a finite non-zero value.
 */
lh_usize_t
lh_str_format_float_put_special(lh_u32_t bits, lh_str_ptr str);

/**
 * @brief Set @p num / @p den to the finite non-zero float @p bits divided by
 *        10^d10, landing in [1, 10).
 * @return d10, the decimal exponent of the value.
 */
lh_sint_t
lh_str_format_float_scale(lh_u32_t bits, lh_numeric_big_t *num, lh_numeric_big_t *den);

/**
 * @brief Write @p precision decimal digits of @p num / @p den (in [1, 10)) at
 *        @p digits, rounded to nearest, ties to even.
 * @return ::lh_bool_true when rounding carried past the first digit
 *         (9.99 → 10.0): the exponent grows by one.
 */
lh_bool_t
lh_str_format_float_get_digits(lh_numeric_big_t *num, const lh_numeric_big_t *den,
                               lh_usize_t precision, lh_str_ptr digits);

/** @brief How many of the @p precision digits are left without trailing zeros (at least 1). */
lh_usize_t
lh_str_format_float_trim(const lh_str_ptr digits, lh_usize_t precision);

/**
 * @brief Write @p nd digits with decimal exponent @p d10 in positional form
 *        (`123.45`, `0.00123`) at @p str from position @p n.
 * @return Position after the last character written.
 */
lh_usize_t
lh_str_format_float_put_fixed(const lh_str_ptr digits, lh_usize_t nd, lh_sint_t d10, lh_str_ptr str,
                              lh_usize_t n);

/**
 * @brief Write @p nd digits with decimal exponent @p d10 in exponent form
 *        (`1.2345e+02`) at @p str from position @p n.
 * @return Position after the last character written.
 */
lh_usize_t
lh_str_format_float_put_exponent(const lh_str_ptr digits, lh_usize_t nd, lh_sint_t d10,
                                 lh_str_ptr str, lh_usize_t n);

/**
 * @brief Format @p value with @p digits significant digits, `%.{digits}g` style.
 *
 * - Fixed notation when the decimal exponent is in `[-4, digits)`, otherwise
 *   `d.ddde+XX` (exponent at least two digits).
 * - Rounds to nearest, ties to even, on the exact binary value.
 * - `0` is `0` (`-0` for a negative zero), infinity is `inf` / `-inf`, a NaN
 *   is `nan`.
 * - No NUL terminator is appended.
 *
 * @param value    Value to format.
 * @param digits   Significant digits: 0 means ::LH_STR_FORMAT_FLOAT_DIGITS_DEFAULT,
 *                 values above ::LH_STR_FORMAT_FLOAT_DIGITS_MAX are clamped to it.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written (at least 1), or 0 if @p str_size was too small
 *         (nothing is written then).
 */
lh_usize_t
lh_str_ptr_format_float(lh_float_t value, lh_usize_t digits, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_FLOAT_H */
