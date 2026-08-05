/**
 * @file types.h
 * @brief Interval bounds struct types for all standard numeric types.
 *
 * Provides a complete set of interval bounds structures covering all integer
 * types defined in @ref char.h and @ref types.h.
 *
 * Each struct stores a homogeneous pair of boundary values (members @c first
 * and @c second) via ::lh_interval_bounds_fields; for intervals read them as
 * the lower and upper endpoints in the usual order.
 *
 * These structs are intended to be used as the @c bounds
 * member inside full interval structs built with ::lh_interval_fields.
 *
 * Types are organized in two groups:
 *   - **unsigned** — @c u-prefixed types (::lh_uchar_t … ::lh_ullong_t)
 *   - **signed** — @c s-prefixed types (::lh_schar_t … ::lh_sllong_t)
 */

#ifndef LH_NUMERIC_INTERVAL_BOUNDS_H
#define LH_NUMERIC_INTERVAL_BOUNDS_H

#include <lh/char.h>
#include <lh/interval/bounds/fields.h>
#include <lh/numeric/types.h>

/* ── unsigned ──────────────────────────────────────────────────────────── */

/**
 * @struct lh_uchar_interval_bounds
 * @brief Interval bounds with 8-bit unsigned endpoints.
 *
 * Both @c first and @c second are ::lh_uchar_t values (unsigned char,
 * range 0..255).
 * Suitable for byte-range or color-channel intervals.
 *
 * @see lh_uchar_t
 * @see lh_interval_bounds_fields
 */
struct lh_uchar_interval_bounds
{
    /** Endpoints of type ::lh_uchar_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_uchar_t);
};

/**
 * @struct lh_ushort_interval_bounds
 * @brief Interval bounds with 16-bit unsigned endpoints.
 *
 * Both @c first and @c second are ::lh_ushort_t values (unsigned short,
 * exactly 2 bytes, range 0..65535).
 *
 * @see lh_ushort_t
 * @see lh_interval_bounds_fields
 */
struct lh_ushort_interval_bounds
{
    /** Endpoints of type ::lh_ushort_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_ushort_t);
};

/**
 * @struct lh_uint_interval_bounds
 * @brief Interval bounds with 32-bit unsigned endpoints.
 *
 * Both @c first and @c second are ::lh_uint_t values (unsigned int,
 * exactly 4 bytes, range 0..4294967295).
 *
 * @see lh_uint_t
 * @see lh_interval_bounds_fields
 */
struct lh_uint_interval_bounds
{
    /** Endpoints of type ::lh_uint_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_uint_t);
};

/**
 * @struct lh_ulong_interval_bounds
 * @brief Interval bounds with platform-width unsigned endpoints.
 *
 * Both @c first and @c second are ::lh_ulong_t values (unsigned long).
 * Width is architecture-dependent: 32 bits on LP32/ILP32, 64 bits on LP64.
 *
 * @note Prefer ::lh_uint_interval_bounds or ::lh_ullong_interval_bounds
 *       when a fixed-width type is required.
 *
 * @see lh_ulong_t
 * @see lh_interval_bounds_fields
 */
struct lh_ulong_interval_bounds
{
    /** Endpoints of type ::lh_ulong_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_ulong_t);
};

/**
 * @struct lh_ullong_interval_bounds
 * @brief Interval bounds with 64-bit unsigned endpoints.
 *
 * Both @c first and @c second are ::lh_ullong_t values (unsigned long long,
 * exactly 8 bytes).
 * Suitable for large-range or file-offset intervals.
 *
 * @see lh_ullong_t
 * @see lh_interval_bounds_fields
 */
struct lh_ullong_interval_bounds
{
    /** Endpoints of type ::lh_ullong_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_ullong_t);
};

/* ── signed (explicit) ─────────────────────────────────────────────────── */

/**
 * @struct lh_schar_interval_bounds
 * @brief Interval bounds with 8-bit signed endpoints.
 *
 * Both @c first and @c second are ::lh_schar_t values (signed char,
 * range -128..+127).
 * Suitable for small signed delta or offset ranges.
 *
 * @see lh_schar_t
 * @see lh_interval_bounds_fields
 */
struct lh_schar_interval_bounds
{
    /** Endpoints of type ::lh_schar_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_schar_t);
};

/**
 * @struct lh_sshort_interval_bounds
 * @brief Interval bounds with 16-bit signed endpoints.
 *
 * Both @c first and @c second are ::lh_sshort_t values (signed short,
 * exactly 2 bytes, range -32768..+32767).
 *
 * @see lh_sshort_t
 * @see lh_interval_bounds_fields
 */
struct lh_sshort_interval_bounds
{
    /** Endpoints of type ::lh_sshort_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_sshort_t);
};

/**
 * @struct lh_sint_interval_bounds
 * @brief Interval bounds with 32-bit signed endpoints.
 *
 * Both @c first and @c second are ::lh_sint_t values (signed int,
 * exactly 4 bytes, range -2147483648..+2147483647).
 *
 * @see lh_sint_t
 * @see lh_interval_bounds_fields
 */
struct lh_sint_interval_bounds
{
    /** Endpoints of type ::lh_sint_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_sint_t);
};

/**
 * @struct lh_slong_interval_bounds
 * @brief Interval bounds with platform-width signed endpoints.
 *
 * Both @c first and @c second are ::lh_slong_t values (signed long).
 * Width is architecture-dependent: 32 bits on ILP32, 64 bits on LP64.
 *
 * @note Prefer ::lh_sint_interval_bounds or ::lh_sllong_interval_bounds
 *       when a fixed-width type is required.
 *
 * @see lh_slong_t
 * @see lh_interval_bounds_fields
 */
struct lh_slong_interval_bounds
{
    /** Endpoints of type ::lh_slong_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_slong_t);
};

/**
 * @struct lh_sllong_interval_bounds
 * @brief Interval bounds with 64-bit signed endpoints.
 *
 * Both @c first and @c second are ::lh_sllong_t values (signed long long,
 * exactly 8 bytes, range -2^63..+2^63-1).
 *
 * @see lh_sllong_t
 * @see lh_interval_bounds_fields
 */
struct lh_sllong_interval_bounds
{
    /** Endpoints of type ::lh_sllong_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_sllong_t);
};

/* ── typedefs ──────────────────────────────────────────────────────────── */

/** @typedef lh_uchar_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_uchar_interval_bounds. */
typedef struct lh_uchar_interval_bounds lh_uchar_interval_bounds_t;

/** @typedef lh_ushort_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_ushort_interval_bounds. */
typedef struct lh_ushort_interval_bounds lh_ushort_interval_bounds_t;

/** @typedef lh_uint_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_uint_interval_bounds. */
typedef struct lh_uint_interval_bounds lh_uint_interval_bounds_t;

/** @typedef lh_ulong_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_ulong_interval_bounds. */
typedef struct lh_ulong_interval_bounds lh_ulong_interval_bounds_t;

/** @typedef lh_ullong_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_ullong_interval_bounds. */
typedef struct lh_ullong_interval_bounds lh_ullong_interval_bounds_t;

/** @typedef lh_schar_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_schar_interval_bounds. */
typedef struct lh_schar_interval_bounds lh_schar_interval_bounds_t;

/** @typedef lh_sshort_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_sshort_interval_bounds. */
typedef struct lh_sshort_interval_bounds lh_sshort_interval_bounds_t;

/** @typedef lh_sint_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_sint_interval_bounds. */
typedef struct lh_sint_interval_bounds lh_sint_interval_bounds_t;

/** @typedef lh_slong_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_slong_interval_bounds. */
typedef struct lh_slong_interval_bounds lh_slong_interval_bounds_t;

/** @typedef lh_sllong_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_sllong_interval_bounds. */
typedef struct lh_sllong_interval_bounds lh_sllong_interval_bounds_t;

#endif /* LH_NUMERIC_INTERVAL_BOUNDS_H */
