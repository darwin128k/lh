/**
 * @file types.h
 * @brief Portable integer type definitions with explicit signedness.
 *
 * Provides typedefs for unsigned and signed integer types with clear,
 * self-documenting names using the `lh_` prefix.
 *
 * All types carry an explicit signedness prefix:
 *   - `u` — unsigned (`lh_ushort_t` … `lh_ullong_t`)
 *   - `s` — signed   (`lh_sshort_t` … `lh_sllong_t`)
 */

#ifndef LH_NUMERIC_TYPES_H
#define LH_NUMERIC_TYPES_H

/**
 * @typedef lh_short_t
 * @brief Plain short integer type (`short`).
 *
 * Matches the platform ABI and C default semantics.
 * Signedness is signed by definition for plain `short`.
 *
 * Alias for: `short`
 */
typedef short lh_short_t;

/**
 * @typedef lh_int_t
 * @brief Plain integer type (`int`).
 *
 * Matches the platform ABI and C default semantics.
 * Signedness is signed by definition for plain `int`.
 *
 * Alias for: `int`
 */
typedef int lh_int_t;

/**
 * @typedef lh_long_t
 * @brief Plain long integer type (`long`).
 *
 * Matches the platform ABI and C default semantics.
 * Width is architecture-dependent (typically 32 or 64 bits).
 *
 * Alias for: `long`
 */
typedef long lh_long_t;

/**
 * @typedef lh_llong_t
 * @brief Plain long long integer type (`long long`).
 *
 * Matches the platform ABI and C default semantics.
 * Typically 64 bits on mainstream targets.
 *
 * Alias for: `long long`
 */
typedef long long lh_llong_t;

/* ── unsigned ──────────────────────────────────────────────────────────── */

/**
 * @typedef lh_ushort_t
 * @brief 16-bit unsigned integer.
 *
 * Alias for `unsigned short`.
 */
typedef unsigned short lh_ushort_t;

/**
 * @typedef lh_uint_t
 * @brief 32-bit unsigned integer.
 *
 * Alias for `unsigned int`.
 */
typedef unsigned int lh_uint_t;

/**
 * @typedef lh_ulong_t
 * @brief Platform-width unsigned integer (32 or 64 bits).
 *
 * Alias for `unsigned long`.
 * Width is architecture-dependent: 32 bits on ILP32, 64 bits on LP64.
 *
 * @note Prefer ::lh_uint_t or ::lh_ullong_t when a fixed width is required.
 */
typedef unsigned long lh_ulong_t;

/**
 * @typedef lh_ullong_t
 * @brief 64-bit unsigned integer.
 *
 * Alias for `unsigned long long`.
 */
typedef unsigned long long lh_ullong_t;

/* ── signed ────────────────────────────────────────────────────────────── */

/**
 * @typedef lh_sshort_t
 * @brief 16-bit signed integer.
 *
 * Alias for `signed short`.
 */
typedef signed short lh_sshort_t;

/**
 * @typedef lh_sint_t
 * @brief 32-bit signed integer.
 *
 * Alias for `signed int`.
 */
typedef signed int lh_sint_t;

/**
 * @typedef lh_slong_t
 * @brief Platform-width signed integer (32 or 64 bits).
 *
 * Alias for `signed long`.
 * Width is architecture-dependent: 32 bits on ILP32, 64 bits on LP64.
 *
 * @note Prefer ::lh_sint_t or ::lh_sllong_t when a fixed width is required.
 */
typedef signed long lh_slong_t;

/**
 * @typedef lh_sllong_t
 * @brief 64-bit signed integer.
 *
 * Alias for `signed long long`.
 */
typedef signed long long lh_sllong_t;

#endif // LH_NUMERIC_TYPES_H
