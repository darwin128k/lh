/**
 * @file types.h
 * @brief Width-suffixed integer aliases (::lh_u8_t … ::lh_s64_t).
 *
 * Maps common “Nx” names onto the lh numeric and character typedefs from
 * `lh/char.h` and `lh/numeric/types.h`.
 * Use these when you want the bit width in the type name; use ::lh_uint_t-style
 * names when you prefer signedness-first naming.
 *
 * @note C does not guarantee exact bit widths for `short` / `int` / `long long`
 *       on every exotic platform; these aliases follow the same assumptions as
 *       ::lh_ushort_t, ::lh_uint_t, ::lh_ullong_t and the 8-bit ::lh_uchar_t / ::lh_schar_t
 *       documented in this library.
 */

#ifndef LH_NUMERIC_FIXED_TYPES_H
#define LH_NUMERIC_FIXED_TYPES_H

#include <lh/char.h>
#include <lh/numeric/types.h>

/**
 * @typedef lh_u8_t
 * @brief 8-bit unsigned integer.
 *
 * Alias for ::lh_uchar_t (`unsigned char`).
 */
typedef lh_uchar_t lh_u8_t;

/**
 * @typedef lh_u16_t
 * @brief 16-bit unsigned integer.
 *
 * Alias for ::lh_ushort_t (`unsigned short`).
 */
typedef lh_ushort_t lh_u16_t;

/**
 * @typedef lh_u32_t
 * @brief 32-bit unsigned integer.
 *
 * Alias for ::lh_uint_t (`unsigned int`).
 */
typedef lh_uint_t lh_u32_t;

/**
 * @typedef lh_u64_t
 * @brief 64-bit unsigned integer.
 *
 * Alias for ::lh_ullong_t (`unsigned long long`).
 */
typedef lh_ullong_t lh_u64_t;

/**
 * @typedef lh_s8_t
 * @brief 8-bit signed integer.
 *
 * Alias for ::lh_schar_t (`signed char`).
 */
typedef lh_schar_t lh_s8_t;

/**
 * @typedef lh_s16_t
 * @brief 16-bit signed integer.
 *
 * Alias for ::lh_sshort_t (`signed short`).
 */
typedef lh_sshort_t lh_s16_t;

/**
 * @typedef lh_s32_t
 * @brief 32-bit signed integer.
 *
 * Alias for ::lh_sint_t (`signed int`).
 */
typedef lh_sint_t lh_s32_t;

/**
 * @typedef lh_s64_t
 * @brief 64-bit signed integer.
 *
 * Alias for ::lh_sllong_t (`signed long long`).
 */
typedef lh_sllong_t lh_s64_t;

#endif // LH_NUMERIC_FIXED_TYPES_H
