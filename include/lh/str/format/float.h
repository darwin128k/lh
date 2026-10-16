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

#include <lh/numeric/float.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Significant digits used when the caller passes 0 (the `%g` default). */
#define LH_STR_FORMAT_FLOAT_DIGITS_DEFAULT 6U

/** @brief Most significant digits that mean anything for a single-precision float. */
#define LH_STR_FORMAT_FLOAT_DIGITS_MAX 9U

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
