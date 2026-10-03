/**
 * @file limits.h
 * @brief Compile-time numeric limit constants for character types.
 *
 * Declares a `static const` full-range closed interval for the character
 * type aliases declared in <lh/char.h> (::lh_char_t, ::lh_uchar_t,
 * ::lh_schar_t) and provides convenience macros (::LH_UCHAR_T_MIN,
 * ::LH_UCHAR_T_MAX, ::LH_UCHAR_T_SIZE, ::LH_SCHAR_T_MIN, ::LH_SCHAR_T_MAX,
 * ::LH_SCHAR_T_SIZE, ::LH_CHAR_T_MIN, ::LH_CHAR_T_MAX, ::LH_CHAR_T_SIZE)
 * for minimum and maximum representable values and for size in bytes.
 *
 * For each numeric interval type, `bounds` is declared with
 * ::lh_interval_bounds_fields (members `first` / `second`). The `*_T_MIN` /
 * `*_T_MAX` macros for those types expand to `…INTERVAL.bounds.first` and
 * `…INTERVAL.bounds.second` respectively (minimum at `first`, maximum at
 * `second`). ::lh_char_t limits use ::lh_numeric_limit_min /
 * ::lh_numeric_limit_max instead, because `char` signedness is
 * implementation-defined.
 *
 * @note All intervals are initialized with ::lh_interval_flags_closed.
 *
 * @see <lh/char/interval/types.h> for the interval struct types.
 * @see <lh/numeric/limits.h> for the non-character numeric limits.
 */

#ifndef LH_CHAR_LIMITS_H
#define LH_CHAR_LIMITS_H

#include <lh/char/interval/types.h>
#include <lh/numeric/interval/initializer.h>

/* ── unsigned interval ─────────────────────────────────────────────────── */

/**
 * @var LH_UCHAR_T_INTERVAL
 * @brief Full-range closed interval for ::lh_uchar_t.
 *
 * Covers the complete value range of ::lh_uchar_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_unsigned.
 */
static const lh_uchar_interval_t LH_UCHAR_T_INTERVAL =
    lh_numeric_interval_initializer_unsigned(lh_uchar_t, lh_interval_flags_closed);

/* ── signed interval ───────────────────────────────────────────────────── */

/**
 * @var LH_SCHAR_T_INTERVAL
 * @brief Full-range closed interval for ::lh_schar_t.
 *
 * Covers the complete value range of ::lh_schar_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_signed.
 */
static const lh_schar_interval_t LH_SCHAR_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_schar_t, lh_interval_flags_closed);

/* ── sizes ──────────────────────────────────────────────────────────────── */

#ifndef LH_UCHAR_T_SIZE
/**
 * @def LH_UCHAR_T_SIZE
 * @brief Size of ::lh_uchar_t in bytes.
 *
 * Expands to `lh_type_size(lh_uchar_t)`.
 */
#    define LH_UCHAR_T_SIZE lh_type_size(lh_uchar_t)
#endif /* LH_UCHAR_T_SIZE */

#ifndef LH_SCHAR_T_SIZE
/**
 * @def LH_SCHAR_T_SIZE
 * @brief Size of ::lh_schar_t in bytes.
 *
 * Expands to `lh_type_size(lh_schar_t)`.
 */
#    define LH_SCHAR_T_SIZE lh_type_size(lh_schar_t)
#endif /* LH_SCHAR_T_SIZE */

#ifndef LH_CHAR_T_SIZE
/**
 * @def LH_CHAR_T_SIZE
 * @brief Size of ::lh_char_t in bytes.
 */
#    define LH_CHAR_T_SIZE lh_type_size(lh_char_t)
#endif /* LH_CHAR_T_SIZE */

/* ── unsigned limits ───────────────────────────────────────────────────── */

#ifndef LH_UCHAR_T_MIN
/**
 * @def LH_UCHAR_T_MIN
 * @brief Minimum value of ::lh_uchar_t (0).
 *
 * Expands to `LH_UCHAR_T_INTERVAL.bounds.first`.
 */
#    define LH_UCHAR_T_MIN LH_UCHAR_T_INTERVAL.bounds.first
#endif /* LH_UCHAR_T_MIN */

#ifndef LH_UCHAR_T_MAX
/**
 * @def LH_UCHAR_T_MAX
 * @brief Maximum value of ::lh_uchar_t.
 *
 * Expands to `LH_UCHAR_T_INTERVAL.bounds.second`.
 */
#    define LH_UCHAR_T_MAX LH_UCHAR_T_INTERVAL.bounds.second
#endif /* LH_UCHAR_T_MAX */

/* ── signed limits ─────────────────────────────────────────────────────── */

#ifndef LH_SCHAR_T_MIN
/**
 * @def LH_SCHAR_T_MIN
 * @brief Minimum value of ::lh_schar_t.
 *
 * Expands to `LH_SCHAR_T_INTERVAL.bounds.first`.
 */
#    define LH_SCHAR_T_MIN LH_SCHAR_T_INTERVAL.bounds.first
#endif /* LH_SCHAR_T_MIN */

#ifndef LH_SCHAR_T_MAX
/**
 * @def LH_SCHAR_T_MAX
 * @brief Maximum value of ::lh_schar_t.
 *
 * Expands to `LH_SCHAR_T_INTERVAL.bounds.second`.
 */
#    define LH_SCHAR_T_MAX LH_SCHAR_T_INTERVAL.bounds.second
#endif /* LH_SCHAR_T_MAX */

#ifndef LH_CHAR_T_MIN
/**
 * @def LH_CHAR_T_MIN
 * @brief Minimum value of ::lh_char_t (auto signedness).
 *
 * Expands to `lh_numeric_limit_min(lh_char_t)`.
 */
#    define LH_CHAR_T_MIN lh_numeric_limit_min(lh_char_t)
#endif /* LH_CHAR_T_MIN */

#ifndef LH_CHAR_T_MAX
/**
 * @def LH_CHAR_T_MAX
 * @brief Maximum value of ::lh_char_t (auto signedness).
 *
 * Expands to `lh_numeric_limit_max(lh_char_t)`.
 */
#    define LH_CHAR_T_MAX lh_numeric_limit_max(lh_char_t)
#endif /* LH_CHAR_T_MAX */

#endif /* LH_CHAR_LIMITS_H */