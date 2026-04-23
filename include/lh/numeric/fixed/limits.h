/**
 * @file limits.h
 * @brief Min/max/size macros for width-suffixed types (::lh_u8_t … ::lh_s64_t).
 *
 * Each `LH_U8_T_MIN` / `LH_S32_T_MAX` / `LH_U32_T_SIZE` style macro is an alias for the
 * matching limit or size from `lh/numeric/limits.h` (e.g. ::LH_UCHAR_T_MIN,
 * ::LH_SINT_T_MAX, ::LH_UINT_T_SIZE). Include this header when you use
 * ::lh_u32_t-style names and want limits in the same naming scheme.
 */

#ifndef LH_NUMERIC_FIXED_LIMITS_H
#define LH_NUMERIC_FIXED_LIMITS_H

#include <lh/numeric/limits.h>

/* ── unsigned (Nx) ───────────────────────────────────────────────────────── */

/**
 * @def LH_U8_T_MIN
 * @brief Minimum value of ::lh_u8_t (0).
 *
 * Expands to ::LH_UCHAR_T_MIN.
 */
#define LH_U8_T_MIN LH_UCHAR_T_MIN

/**
 * @def LH_U8_T_MAX
 * @brief Maximum value of ::lh_u8_t.
 *
 * Expands to ::LH_UCHAR_T_MAX.
 */
#define LH_U8_T_MAX LH_UCHAR_T_MAX

/**
 * @def LH_U16_T_MIN
 * @brief Minimum value of ::lh_u16_t (0).
 *
 * Expands to ::LH_USHORT_T_MIN.
 */
#define LH_U16_T_MIN LH_USHORT_T_MIN

/**
 * @def LH_U16_T_MAX
 * @brief Maximum value of ::lh_u16_t.
 *
 * Expands to ::LH_USHORT_T_MAX.
 */
#define LH_U16_T_MAX LH_USHORT_T_MAX

/**
 * @def LH_U32_T_MIN
 * @brief Minimum value of ::lh_u32_t (0).
 *
 * Expands to ::LH_UINT_T_MIN.
 */
#define LH_U32_T_MIN LH_UINT_T_MIN

/**
 * @def LH_U32_T_MAX
 * @brief Maximum value of ::lh_u32_t.
 *
 * Expands to ::LH_UINT_T_MAX.
 */
#define LH_U32_T_MAX LH_UINT_T_MAX

/**
 * @def LH_U64_T_MIN
 * @brief Minimum value of ::lh_u64_t (0).
 *
 * Expands to ::LH_ULLONG_T_MIN.
 */
#define LH_U64_T_MIN LH_ULLONG_T_MIN

/**
 * @def LH_U64_T_MAX
 * @brief Maximum value of ::lh_u64_t.
 *
 * Expands to ::LH_ULLONG_T_MAX.
 */
#define LH_U64_T_MAX LH_ULLONG_T_MAX

/**
 * @def LH_U8_T_SIZE
 * @brief Size of ::lh_u8_t in bytes.
 *
 * Expands to ::LH_UCHAR_T_SIZE.
 */
#define LH_U8_T_SIZE LH_UCHAR_T_SIZE

/**
 * @def LH_U16_T_SIZE
 * @brief Size of ::lh_u16_t in bytes.
 *
 * Expands to ::LH_USHORT_T_SIZE.
 */
#define LH_U16_T_SIZE LH_USHORT_T_SIZE

/**
 * @def LH_U32_T_SIZE
 * @brief Size of ::lh_u32_t in bytes.
 *
 * Expands to ::LH_UINT_T_SIZE.
 */
#define LH_U32_T_SIZE LH_UINT_T_SIZE

/**
 * @def LH_U64_T_SIZE
 * @brief Size of ::lh_u64_t in bytes.
 *
 * Expands to ::LH_ULLONG_T_SIZE.
 */
#define LH_U64_T_SIZE LH_ULLONG_T_SIZE

/* ── signed (Nx) ───────────────────────────────────────────────────────── */

/**
 * @def LH_S8_T_MIN
 * @brief Minimum value of ::lh_s8_t.
 *
 * Expands to ::LH_SCHAR_T_MIN.
 */
#define LH_S8_T_MIN LH_SCHAR_T_MIN

/**
 * @def LH_S8_T_MAX
 * @brief Maximum value of ::lh_s8_t.
 *
 * Expands to ::LH_SCHAR_T_MAX.
 */
#define LH_S8_T_MAX LH_SCHAR_T_MAX

/**
 * @def LH_S16_T_MIN
 * @brief Minimum value of ::lh_s16_t.
 *
 * Expands to ::LH_SSHORT_T_MIN.
 */
#define LH_S16_T_MIN LH_SSHORT_T_MIN

/**
 * @def LH_S16_T_MAX
 * @brief Maximum value of ::lh_s16_t.
 *
 * Expands to ::LH_SSHORT_T_MAX.
 */
#define LH_S16_T_MAX LH_SSHORT_T_MAX

/**
 * @def LH_S32_T_MIN
 * @brief Minimum value of ::lh_s32_t.
 *
 * Expands to ::LH_SINT_T_MIN.
 */
#define LH_S32_T_MIN LH_SINT_T_MIN

/**
 * @def LH_S32_T_MAX
 * @brief Maximum value of ::lh_s32_t.
 *
 * Expands to ::LH_SINT_T_MAX.
 */
#define LH_S32_T_MAX LH_SINT_T_MAX

/**
 * @def LH_S64_T_MIN
 * @brief Minimum value of ::lh_s64_t.
 *
 * Expands to ::LH_SLLONG_T_MIN.
 */
#define LH_S64_T_MIN LH_SLLONG_T_MIN

/**
 * @def LH_S64_T_MAX
 * @brief Maximum value of ::lh_s64_t.
 *
 * Expands to ::LH_SLLONG_T_MAX.
 */
#define LH_S64_T_MAX LH_SLLONG_T_MAX

/**
 * @def LH_S8_T_SIZE
 * @brief Size of ::lh_s8_t in bytes.
 *
 * Expands to ::LH_SCHAR_T_SIZE.
 */
#define LH_S8_T_SIZE LH_SCHAR_T_SIZE

/**
 * @def LH_S16_T_SIZE
 * @brief Size of ::lh_s16_t in bytes.
 *
 * Expands to ::LH_SSHORT_T_SIZE.
 */
#define LH_S16_T_SIZE LH_SSHORT_T_SIZE

/**
 * @def LH_S32_T_SIZE
 * @brief Size of ::lh_s32_t in bytes.
 *
 * Expands to ::LH_SINT_T_SIZE.
 */
#define LH_S32_T_SIZE LH_SINT_T_SIZE

/**
 * @def LH_S64_T_SIZE
 * @brief Size of ::lh_s64_t in bytes.
 *
 * Expands to ::LH_SLLONG_T_SIZE.
 */
#define LH_S64_T_SIZE LH_SLLONG_T_SIZE

#endif // LH_NUMERIC_FIXED_LIMITS_H
