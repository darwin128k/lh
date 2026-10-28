/**
 * @file decimal.h
 * @brief A decimal number taken apart: `±mantissa × 10^exponent`
 *        (::lh_real_decimal_t).
 *
 * The first half of reading `-12.5e3` as a number: the text becomes
 * mantissa `125`, exponent `2`, negative — integers only, no rounding yet.
 * ::lh_real_decimal_to_real is the second half. Up to
 * ::LH_REAL_DECIMAL_DIGITS_MAX significant digits are kept, which a 64-bit
 * mantissa holds exactly; further nonzero digits only set `truncated`.
 */

#ifndef LH_REAL_DECIMAL_H
#define LH_REAL_DECIMAL_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/real.h>
#include <lh/real/decimal/fields.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/** @brief Significant digits a decimal keeps: 19, all a ::lh_u64_t always holds. */
#define LH_REAL_DECIMAL_DIGITS_MAX 19U

/**
 * @brief Where an exponent in the text stops counting: anything bigger is
 *        zero or infinity anyway, and the sum must not overflow.
 */
#define LH_REAL_DECIMAL_EXPONENT_LIMIT 100000

/**
 * @struct lh_real_decimal
 * @brief Mantissa, exponent and sign. Fields via ::lh_real_decimal_fields.
 */
struct lh_real_decimal
{
    /** Members: ::lh_real_decimal_fields. */
    lh_real_decimal_fields(lh_u64_t, lh_s64_t, lh_bool_t);
};
/** @copybrief lh_real_decimal */
typedef struct lh_real_decimal lh_real_decimal_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── accessors ───────────────────────────────────────────────────────────── */

/** The significant digits of @p self, as an integer. */
lh_u64_t
lh_real_decimal_get_mantissa(const lh_real_decimal_t *self);

/** Make @p mantissa the significant digits of @p self. */
lh_void
lh_real_decimal_set_mantissa(lh_real_decimal_t *self, lh_u64_t mantissa);

/** The power of ten the mantissa of @p self is scaled by. */
lh_s64_t
lh_real_decimal_get_exponent(const lh_real_decimal_t *self);

/** Make @p exponent the power of ten the mantissa of @p self is scaled by. */
lh_void
lh_real_decimal_set_exponent(lh_real_decimal_t *self, lh_s64_t exponent);

/** True when @p self is negative (`-0` included). */
lh_bool_t
lh_real_decimal_is_negative(const lh_real_decimal_t *self);

/** Make @p negative the sign of @p self. */
lh_void
lh_real_decimal_set_negative(lh_real_decimal_t *self, lh_bool_t negative);

/** True when nonzero digits past the mantissa were dropped. */
lh_bool_t
lh_real_decimal_is_truncated(const lh_real_decimal_t *self);

/** Make @p truncated whether nonzero digits past the mantissa were dropped. */
lh_void
lh_real_decimal_set_truncated(lh_real_decimal_t *self, lh_bool_t truncated);

/* ── build ───────────────────────────────────────────────────────────────── */

/** Set @p self to `+0 × 10^0`. */
lh_void
lh_real_decimal_init(lh_real_decimal_t *self);

/** Scale @p self by `10^delta` more. */
lh_void
lh_real_decimal_add_exponent(lh_real_decimal_t *self, lh_s64_t delta);

/**
 * @brief Append the decimal digit @p digit — of the integer part, or with
 *        @p fraction of the part after the point.
 *
 * Leading zeros are not digits; past ::LH_REAL_DECIMAL_DIGITS_MAX a digit
 * only moves the exponent (integer part) or is dropped (fraction), and a
 * nonzero one sets `truncated`.
 */
lh_void
lh_real_decimal_push_digit(lh_real_decimal_t *self, lh_byte_t digit, lh_bool_t fraction);

/**
 * @brief Take the text at @p str apart into @p self.
 *
 * All of @p str_size must be `-? digits? (. digits?)? ([eE] [+-]? digits)?`
 * with at least one digit before the exponent: `12`, `-0.5`, `.5`, `5.`,
 * `1e-3`. No `+` in front, no spaces, no `inf` or `nan`.
 *
 * @return ::lh_bool_true if all of the text was such a number.
 */
lh_bool_t
lh_real_decimal_parse(lh_real_decimal_t *self, const lh_str_ptr str, lh_usize_t str_size);

#if LH_REAL_ENABLED
/**
 * @brief The ::lh_real_t nearest to @p self, ties to even, into @p out.
 *
 * Eisel–Lemire on the 64-bit mantissa (the algorithm of `fast_float`, Go
 * and Rust): exact — the correctly rounded value — whenever @p self is not
 * truncated. A truncated decimal (more than 19 significant digits) is
 * rounded from its first 19, which is the right answer unless the dropped
 * digits decide a near tie; then it can be one unit in the last place off.
 * Tiny values become subnormals or zero.
 *
 * @return ::lh_bool_false, with @p out untouched, when @p self is too big
 *         for ::lh_real_t (it would round to infinity).
 */
lh_bool_t
lh_real_decimal_to_real(const lh_real_decimal_t *self, lh_real_t *out);
#endif

LH_COMPILER_EXTERN_C_END

#endif /* LH_REAL_DECIMAL_H */
