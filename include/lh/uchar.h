/**
 * @file uchar.h
 * @brief Unicode character width aliases.
 */

#ifndef LH_UCHAR_H
#define LH_UCHAR_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_uchar8_t
 * @brief 8-bit alias for single-byte character data.
 */
typedef lh_u8_t lh_uchar8_t;

/**
 * @typedef lh_uchar16_t
 * @brief 16-bit alias for UTF-16 code units.
 */
typedef lh_u16_t lh_uchar16_t;

/**
 * @typedef lh_uchar32_t
 * @brief 32-bit alias for UTF-32 code points and wide Unicode scalars.
 */
typedef lh_u32_t lh_uchar32_t;

/**
 * @typedef lh_schar8_t
 * @brief 8-bit signed character alias for small code units or legacy encodings.
 */
typedef lh_s8_t lh_schar8_t;

/**
 * @typedef lh_schar16_t
 * @brief 16-bit signed character alias mirroring UTF-16 halves.
 */
typedef lh_s16_t lh_schar16_t;

/**
 * @typedef lh_schar32_t
 * @brief 32-bit signed alias aligned with UTF-32 scalars.
 */
typedef lh_s32_t lh_schar32_t;

#endif // LH_UCHAR_H
