/**
 * @file limits.h
 * @brief Min/max/size macros for ::lh_byte_t.
 *
 * Aliases of the matching ::lh_u8_t limits from <lh/numeric/fixed/limits.h>
 * (::LH_U8_T_MIN, ::LH_U8_T_MAX, ::LH_U8_T_SIZE). Kept out of <lh/byte.h> so
 * the light typedef header stays cheap for wide includes.
 */

#ifndef LH_BYTE_LIMITS_H
#define LH_BYTE_LIMITS_H

#include <lh/numeric/fixed/limits.h>

#ifndef LH_BYTE_T_MIN
/**
 * @def LH_BYTE_T_MIN
 * @brief Minimum value of ::lh_byte_t (0).
 *
 * Expands to ::LH_U8_T_MIN.
 */
#    define LH_BYTE_T_MIN LH_U8_T_MIN
#endif /* LH_BYTE_T_MIN */

#ifndef LH_BYTE_T_MAX
/**
 * @def LH_BYTE_T_MAX
 * @brief Maximum value of ::lh_byte_t.
 *
 * Expands to ::LH_U8_T_MAX.
 */
#    define LH_BYTE_T_MAX LH_U8_T_MAX
#endif /* LH_BYTE_T_MAX */

#ifndef LH_BYTE_T_SIZE
/**
 * @def LH_BYTE_T_SIZE
 * @brief Size of ::lh_byte_t in bytes.
 *
 * Expands to ::LH_U8_T_SIZE.
 */
#    define LH_BYTE_T_SIZE LH_U8_T_SIZE
#endif /* LH_BYTE_T_SIZE */

#endif /* LH_BYTE_LIMITS_H */
