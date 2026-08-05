/**
 * @file limits.h
 * @brief Compile-time numeric limit constants for all built-in integer types.
 *
 * Declares a `static const` full-range closed interval for the built-in integer
 * type aliases, including plain (`lh_char_t`, `lh_short_t`, `lh_int_t`,
 * `lh_long_t`, `lh_llong_t`) and explicit signed/unsigned variants, and provides
 * convenience macros (::LH_UCHAR_T_MIN, ::LH_UINT_T_MAX, ::LH_SINT_T_SIZE, …)
 * for minimum and maximum representable values and for size in bytes.
 *
 * For each numeric interval type, `bounds` is declared with
 * ::lh_interval_bounds_fields (members `first` / `second`). The `*_T_MIN` /
 * `*_T_MAX` macros for those types expand to `…INTERVAL.bounds.first` and
 * `…INTERVAL.bounds.second` respectively (minimum at `first`, maximum at
 * `second`). ::lh_char_t limits use ::lh_numeric_limit_min / ::lh_numeric_limit_max
 * instead, because `char` signedness is implementation-defined.
 *
 * @note All intervals are initialized with ::lh_interval_flags_closed.
 */

#ifndef LH_NUMERIC_LIMITS_H
#define LH_NUMERIC_LIMITS_H

#include <lh/numeric/interval/initializer.h>
#include <lh/numeric/interval/types.h>

/* ── unsigned intervals ─────────────────────────────────────────────────── */

/**
 * @var LH_UCHAR_T_INTERVAL
 * @brief Full-range closed interval for ::lh_uchar_t.
 *
 * Covers the complete value range of ::lh_uchar_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_unsigned.
 */
static const lh_uchar_interval_t LH_UCHAR_T_INTERVAL =
    lh_numeric_interval_initializer_unsigned(lh_uchar_t, lh_interval_flags_closed);

/**
 * @var LH_USHORT_T_INTERVAL
 * @brief Full-range closed interval for ::lh_ushort_t.
 *
 * Covers the complete value range of ::lh_ushort_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_unsigned.
 */
static const lh_ushort_interval_t LH_USHORT_T_INTERVAL =
    lh_numeric_interval_initializer_unsigned(lh_ushort_t, lh_interval_flags_closed);

/**
 * @var LH_UINT_T_INTERVAL
 * @brief Full-range closed interval for ::lh_uint_t.
 *
 * Covers the complete value range of ::lh_uint_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_unsigned.
 */
static const lh_uint_interval_t LH_UINT_T_INTERVAL =
    lh_numeric_interval_initializer_unsigned(lh_uint_t, lh_interval_flags_closed);

/**
 * @var LH_ULONG_T_INTERVAL
 * @brief Full-range closed interval for ::lh_ulong_t.
 *
 * Covers the complete value range of ::lh_ulong_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_unsigned.
 */
static const lh_ulong_interval_t LH_ULONG_T_INTERVAL =
    lh_numeric_interval_initializer_unsigned(lh_ulong_t, lh_interval_flags_closed);

/**
 * @var LH_ULLONG_T_INTERVAL
 * @brief Full-range closed interval for ::lh_ullong_t.
 *
 * Covers the complete value range of ::lh_ullong_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_unsigned.
 */
static const lh_ullong_interval_t LH_ULLONG_T_INTERVAL =
    lh_numeric_interval_initializer_unsigned(lh_ullong_t, lh_interval_flags_closed);

/* ── signed intervals ───────────────────────────────────────────────────── */

/**
 * @var LH_SCHAR_T_INTERVAL
 * @brief Full-range closed interval for ::lh_schar_t.
 *
 * Covers the complete value range of ::lh_schar_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_signed.
 */
static const lh_schar_interval_t LH_SCHAR_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_schar_t, lh_interval_flags_closed);

/**
 * @var LH_SSHORT_T_INTERVAL
 * @brief Full-range closed interval for ::lh_sshort_t.
 *
 * Covers the complete value range of ::lh_sshort_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_signed.
 */
static const lh_sshort_interval_t LH_SSHORT_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_sshort_t, lh_interval_flags_closed);

/**
 * @var LH_SINT_T_INTERVAL
 * @brief Full-range closed interval for ::lh_sint_t.
 *
 * Covers the complete value range of ::lh_sint_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_signed.
 */
static const lh_sint_interval_t LH_SINT_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_sint_t, lh_interval_flags_closed);

/**
 * @var LH_SLONG_T_INTERVAL
 * @brief Full-range closed interval for ::lh_slong_t.
 *
 * Covers the complete value range of ::lh_slong_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_signed.
 */
static const lh_slong_interval_t LH_SLONG_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_slong_t, lh_interval_flags_closed);

/**
 * @var LH_SLLONG_T_INTERVAL
 * @brief Full-range closed interval for ::lh_sllong_t.
 *
 * Covers the complete value range of ::lh_sllong_t with closed bounds.
 * Initialized via ::lh_numeric_interval_initializer_signed.
 */
static const lh_sllong_interval_t LH_SLLONG_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_sllong_t, lh_interval_flags_closed);

/**
 * @var LH_SHORT_T_INTERVAL
 * @brief Full-range closed interval for ::lh_short_t.
 */
static const lh_sshort_interval_t LH_SHORT_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_short_t, lh_interval_flags_closed);

/**
 * @var LH_INT_T_INTERVAL
 * @brief Full-range closed interval for ::lh_int_t.
 */
static const lh_sint_interval_t LH_INT_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_int_t, lh_interval_flags_closed);

/**
 * @var LH_LONG_T_INTERVAL
 * @brief Full-range closed interval for ::lh_long_t.
 */
static const lh_slong_interval_t LH_LONG_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_long_t, lh_interval_flags_closed);

/**
 * @var LH_LLONG_T_INTERVAL
 * @brief Full-range closed interval for ::lh_llong_t.
 */
static const lh_sllong_interval_t LH_LLONG_T_INTERVAL =
    lh_numeric_interval_initializer_signed(lh_llong_t, lh_interval_flags_closed);

/* ── sizes ──────────────────────────────────────────────────────────────── */

#ifndef LH_CHAR_T_SIZE
/**
 * @def LH_CHAR_T_SIZE
 * @brief Size of ::lh_char_t in bytes.
 */
#    define LH_CHAR_T_SIZE lh_type_size(lh_char_t)
#endif /* LH_CHAR_T_SIZE */

#ifndef LH_UCHAR_T_SIZE
/**
 * @def LH_UCHAR_T_SIZE
 * @brief Size of ::lh_uchar_t in bytes.
 *
 * Expands to `lh_type_size(lh_uchar_t)`.
 */
#    define LH_UCHAR_T_SIZE lh_type_size(lh_uchar_t)
#endif /* LH_UCHAR_T_SIZE */

#ifndef LH_USHORT_T_SIZE
/**
 * @def LH_USHORT_T_SIZE
 * @brief Size of ::lh_ushort_t in bytes.
 *
 * Expands to `lh_type_size(lh_ushort_t)`.
 */
#    define LH_USHORT_T_SIZE lh_type_size(lh_ushort_t)
#endif /* LH_USHORT_T_SIZE */

#ifndef LH_UINT_T_SIZE
/**
 * @def LH_UINT_T_SIZE
 * @brief Size of ::lh_uint_t in bytes.
 *
 * Expands to `lh_type_size(lh_uint_t)`.
 */
#    define LH_UINT_T_SIZE lh_type_size(lh_uint_t)
#endif /* LH_UINT_T_SIZE */

#ifndef LH_ULONG_T_SIZE
/**
 * @def LH_ULONG_T_SIZE
 * @brief Size of ::lh_ulong_t in bytes.
 *
 * Expands to `lh_type_size(lh_ulong_t)`.
 */
#    define LH_ULONG_T_SIZE lh_type_size(lh_ulong_t)
#endif /* LH_ULONG_T_SIZE */

#ifndef LH_ULLONG_T_SIZE
/**
 * @def LH_ULLONG_T_SIZE
 * @brief Size of ::lh_ullong_t in bytes.
 *
 * Expands to `lh_type_size(lh_ullong_t)`.
 */
#    define LH_ULLONG_T_SIZE lh_type_size(lh_ullong_t)
#endif /* LH_ULLONG_T_SIZE */

#ifndef LH_SCHAR_T_SIZE
/**
 * @def LH_SCHAR_T_SIZE
 * @brief Size of ::lh_schar_t in bytes.
 *
 * Expands to `lh_type_size(lh_schar_t)`.
 */
#    define LH_SCHAR_T_SIZE lh_type_size(lh_schar_t)
#endif /* LH_SCHAR_T_SIZE */

#ifndef LH_SHORT_T_SIZE
/**
 * @def LH_SHORT_T_SIZE
 * @brief Size of ::lh_short_t in bytes.
 */
#    define LH_SHORT_T_SIZE lh_type_size(lh_short_t)
#endif /* LH_SHORT_T_SIZE */

#ifndef LH_SSHORT_T_SIZE
/**
 * @def LH_SSHORT_T_SIZE
 * @brief Size of ::lh_sshort_t in bytes.
 *
 * Expands to `lh_type_size(lh_sshort_t)`.
 */
#    define LH_SSHORT_T_SIZE lh_type_size(lh_sshort_t)
#endif /* LH_SSHORT_T_SIZE */

#ifndef LH_SINT_T_SIZE
/**
 * @def LH_SINT_T_SIZE
 * @brief Size of ::lh_sint_t in bytes.
 *
 * Expands to `lh_type_size(lh_sint_t)`.
 */
#    define LH_SINT_T_SIZE lh_type_size(lh_sint_t)
#endif /* LH_SINT_T_SIZE */

#ifndef LH_INT_T_SIZE
/**
 * @def LH_INT_T_SIZE
 * @brief Size of ::lh_int_t in bytes.
 */
#    define LH_INT_T_SIZE lh_type_size(lh_int_t)
#endif /* LH_INT_T_SIZE */

#ifndef LH_SLONG_T_SIZE
/**
 * @def LH_SLONG_T_SIZE
 * @brief Size of ::lh_slong_t in bytes.
 *
 * Expands to `lh_type_size(lh_slong_t)`.
 */
#    define LH_SLONG_T_SIZE lh_type_size(lh_slong_t)
#endif /* LH_SLONG_T_SIZE */

#ifndef LH_LONG_T_SIZE
/**
 * @def LH_LONG_T_SIZE
 * @brief Size of ::lh_long_t in bytes.
 */
#    define LH_LONG_T_SIZE lh_type_size(lh_long_t)
#endif /* LH_LONG_T_SIZE */

#ifndef LH_SLLONG_T_SIZE
/**
 * @def LH_SLLONG_T_SIZE
 * @brief Size of ::lh_sllong_t in bytes.
 *
 * Expands to `lh_type_size(lh_sllong_t)`.
 */
#    define LH_SLLONG_T_SIZE lh_type_size(lh_sllong_t)
#endif /* LH_SLLONG_T_SIZE */

#ifndef LH_LLONG_T_SIZE
/**
 * @def LH_LLONG_T_SIZE
 * @brief Size of ::lh_llong_t in bytes.
 */
#    define LH_LLONG_T_SIZE lh_type_size(lh_llong_t)
#endif /* LH_LLONG_T_SIZE */

/* ── unsigned limits ────────────────────────────────────────────────────── */

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

#ifndef LH_USHORT_T_MIN
/**
 * @def LH_USHORT_T_MIN
 * @brief Minimum value of ::lh_ushort_t (0).
 *
 * Expands to `LH_USHORT_T_INTERVAL.bounds.first`.
 */
#    define LH_USHORT_T_MIN LH_USHORT_T_INTERVAL.bounds.first
#endif /* LH_USHORT_T_MIN */

#ifndef LH_USHORT_T_MAX
/**
 * @def LH_USHORT_T_MAX
 * @brief Maximum value of ::lh_ushort_t.
 *
 * Expands to `LH_USHORT_T_INTERVAL.bounds.second`.
 */
#    define LH_USHORT_T_MAX LH_USHORT_T_INTERVAL.bounds.second
#endif /* LH_USHORT_T_MAX */

#ifndef LH_UINT_T_MIN
/**
 * @def LH_UINT_T_MIN
 * @brief Minimum value of ::lh_uint_t (0).
 *
 * Expands to `LH_UINT_T_INTERVAL.bounds.first`.
 */
#    define LH_UINT_T_MIN LH_UINT_T_INTERVAL.bounds.first
#endif /* LH_UINT_T_MIN */

#ifndef LH_UINT_T_MAX
/**
 * @def LH_UINT_T_MAX
 * @brief Maximum value of ::lh_uint_t.
 *
 * Expands to `LH_UINT_T_INTERVAL.bounds.second`.
 */
#    define LH_UINT_T_MAX LH_UINT_T_INTERVAL.bounds.second
#endif /* LH_UINT_T_MAX */

#ifndef LH_ULONG_T_MIN
/**
 * @def LH_ULONG_T_MIN
 * @brief Minimum value of ::lh_ulong_t (0).
 *
 * Expands to `LH_ULONG_T_INTERVAL.bounds.first`.
 */
#    define LH_ULONG_T_MIN LH_ULONG_T_INTERVAL.bounds.first
#endif /* LH_ULONG_T_MIN */

#ifndef LH_ULONG_T_MAX
/**
 * @def LH_ULONG_T_MAX
 * @brief Maximum value of ::lh_ulong_t.
 *
 * Expands to `LH_ULONG_T_INTERVAL.bounds.second`.
 */
#    define LH_ULONG_T_MAX LH_ULONG_T_INTERVAL.bounds.second
#endif /* LH_ULONG_T_MAX */

#ifndef LH_ULLONG_T_MIN
/**
 * @def LH_ULLONG_T_MIN
 * @brief Minimum value of ::lh_ullong_t (0).
 *
 * Expands to `LH_ULLONG_T_INTERVAL.bounds.first`.
 */
#    define LH_ULLONG_T_MIN LH_ULLONG_T_INTERVAL.bounds.first
#endif /* LH_ULLONG_T_MIN */

#ifndef LH_ULLONG_T_MAX
/**
 * @def LH_ULLONG_T_MAX
 * @brief Maximum value of ::lh_ullong_t.
 *
 * Expands to `LH_ULLONG_T_INTERVAL.bounds.second`.
 */
#    define LH_ULLONG_T_MAX LH_ULLONG_T_INTERVAL.bounds.second
#endif /* LH_ULLONG_T_MAX */

/* ── signed limits ──────────────────────────────────────────────────────── */

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

#ifndef LH_SHORT_T_MIN
/**
 * @def LH_SHORT_T_MIN
 * @brief Minimum value of ::lh_short_t.
 *
 * Expands to `LH_SHORT_T_INTERVAL.bounds.first`.
 */
#    define LH_SHORT_T_MIN LH_SHORT_T_INTERVAL.bounds.first
#endif /* LH_SHORT_T_MIN */

#ifndef LH_SHORT_T_MAX
/**
 * @def LH_SHORT_T_MAX
 * @brief Maximum value of ::lh_short_t.
 *
 * Expands to `LH_SHORT_T_INTERVAL.bounds.second`.
 */
#    define LH_SHORT_T_MAX LH_SHORT_T_INTERVAL.bounds.second
#endif /* LH_SHORT_T_MAX */

#ifndef LH_SSHORT_T_MIN
/**
 * @def LH_SSHORT_T_MIN
 * @brief Minimum value of ::lh_sshort_t.
 *
 * Expands to `LH_SSHORT_T_INTERVAL.bounds.first`.
 */
#    define LH_SSHORT_T_MIN LH_SSHORT_T_INTERVAL.bounds.first
#endif /* LH_SSHORT_T_MIN */

#ifndef LH_SSHORT_T_MAX
/**
 * @def LH_SSHORT_T_MAX
 * @brief Maximum value of ::lh_sshort_t.
 *
 * Expands to `LH_SSHORT_T_INTERVAL.bounds.second`.
 */
#    define LH_SSHORT_T_MAX LH_SSHORT_T_INTERVAL.bounds.second
#endif /* LH_SSHORT_T_MAX */

#ifndef LH_SINT_T_MIN
/**
 * @def LH_SINT_T_MIN
 * @brief Minimum value of ::lh_sint_t.
 *
 * Expands to `LH_SINT_T_INTERVAL.bounds.first`.
 */
#    define LH_SINT_T_MIN LH_SINT_T_INTERVAL.bounds.first
#endif /* LH_SINT_T_MIN */

#ifndef LH_SINT_T_MAX
/**
 * @def LH_SINT_T_MAX
 * @brief Maximum value of ::lh_sint_t.
 *
 * Expands to `LH_SINT_T_INTERVAL.bounds.second`.
 */
#    define LH_SINT_T_MAX LH_SINT_T_INTERVAL.bounds.second
#endif /* LH_SINT_T_MAX */

#ifndef LH_INT_T_MIN
/**
 * @def LH_INT_T_MIN
 * @brief Minimum value of ::lh_int_t.
 *
 * Expands to `LH_INT_T_INTERVAL.bounds.first`.
 */
#    define LH_INT_T_MIN LH_INT_T_INTERVAL.bounds.first
#endif /* LH_INT_T_MIN */

#ifndef LH_INT_T_MAX
/**
 * @def LH_INT_T_MAX
 * @brief Maximum value of ::lh_int_t.
 *
 * Expands to `LH_INT_T_INTERVAL.bounds.second`.
 */
#    define LH_INT_T_MAX LH_INT_T_INTERVAL.bounds.second
#endif /* LH_INT_T_MAX */

#ifndef LH_SLONG_T_MIN
/**
 * @def LH_SLONG_T_MIN
 * @brief Minimum value of ::lh_slong_t.
 *
 * Expands to `LH_SLONG_T_INTERVAL.bounds.first`.
 */
#    define LH_SLONG_T_MIN LH_SLONG_T_INTERVAL.bounds.first
#endif /* LH_SLONG_T_MIN */

#ifndef LH_SLONG_T_MAX
/**
 * @def LH_SLONG_T_MAX
 * @brief Maximum value of ::lh_slong_t.
 *
 * Expands to `LH_SLONG_T_INTERVAL.bounds.second`.
 */
#    define LH_SLONG_T_MAX LH_SLONG_T_INTERVAL.bounds.second
#endif /* LH_SLONG_T_MAX */

#ifndef LH_LONG_T_MIN
/**
 * @def LH_LONG_T_MIN
 * @brief Minimum value of ::lh_long_t.
 *
 * Expands to `LH_LONG_T_INTERVAL.bounds.first`.
 */
#    define LH_LONG_T_MIN LH_LONG_T_INTERVAL.bounds.first
#endif /* LH_LONG_T_MIN */

#ifndef LH_LONG_T_MAX
/**
 * @def LH_LONG_T_MAX
 * @brief Maximum value of ::lh_long_t.
 *
 * Expands to `LH_LONG_T_INTERVAL.bounds.second`.
 */
#    define LH_LONG_T_MAX LH_LONG_T_INTERVAL.bounds.second
#endif /* LH_LONG_T_MAX */

#ifndef LH_SLLONG_T_MIN
/**
 * @def LH_SLLONG_T_MIN
 * @brief Minimum value of ::lh_sllong_t.
 *
 * Expands to `LH_SLLONG_T_INTERVAL.bounds.first`.
 */
#    define LH_SLLONG_T_MIN LH_SLLONG_T_INTERVAL.bounds.first
#endif /* LH_SLLONG_T_MIN */

#ifndef LH_SLLONG_T_MAX
/**
 * @def LH_SLLONG_T_MAX
 * @brief Maximum value of ::lh_sllong_t.
 *
 * Expands to `LH_SLLONG_T_INTERVAL.bounds.second`.
 */
#    define LH_SLLONG_T_MAX LH_SLLONG_T_INTERVAL.bounds.second
#endif /* LH_SLLONG_T_MAX */

#ifndef LH_LLONG_T_MIN
/**
 * @def LH_LLONG_T_MIN
 * @brief Minimum value of ::lh_llong_t.
 *
 * Expands to `LH_LLONG_T_INTERVAL.bounds.first`.
 */
#    define LH_LLONG_T_MIN LH_LLONG_T_INTERVAL.bounds.first
#endif /* LH_LLONG_T_MIN */

#ifndef LH_LLONG_T_MAX
/**
 * @def LH_LLONG_T_MAX
 * @brief Maximum value of ::lh_llong_t.
 *
 * Expands to `LH_LLONG_T_INTERVAL.bounds.second`.
 */
#    define LH_LLONG_T_MAX LH_LLONG_T_INTERVAL.bounds.second
#endif /* LH_LLONG_T_MAX */

#endif /* LH_NUMERIC_LIMITS_H */
