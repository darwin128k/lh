/**
 * @file limits.h
 * @brief Min/max/size macros for ::lh_uoffset_t / ::lh_soffset_t.
 *
 * Aliases of the matching ::lh_uaddr_t / ::lh_saddr_t limits from <lh/addr.h>.
 * Kept out of <lh/offset.h> so the typedef header stays light.
 */

#ifndef LH_OFFSET_LIMITS_H
#define LH_OFFSET_LIMITS_H

#include <lh/addr.h>

#ifndef LH_OFFSET_T_SIZE
/**
 * @def LH_OFFSET_T_SIZE
 * @brief Size in bytes of ::lh_uoffset_t and ::lh_soffset_t.
 *
 * Expands to ::LH_ADDR_T_SIZE.
 */
#    define LH_OFFSET_T_SIZE LH_ADDR_T_SIZE
#endif /* LH_OFFSET_T_SIZE */

#ifndef LH_UOFFSET_T_MIN
/**
 * @def LH_UOFFSET_T_MIN
 * @brief Minimum value of ::lh_uoffset_t (alias of ::lh_uaddr_t lower bound).
 */
#    define LH_UOFFSET_T_MIN LH_UADDR_T_MIN
#endif /* LH_UOFFSET_T_MIN */

#ifndef LH_UOFFSET_T_MAX
/**
 * @def LH_UOFFSET_T_MAX
 * @brief Maximum value of ::lh_uoffset_t (alias of ::lh_uaddr_t upper bound).
 */
#    define LH_UOFFSET_T_MAX LH_UADDR_T_MAX
#endif /* LH_UOFFSET_T_MAX */

#ifndef LH_UOFFSET_T_SIZE
/**
 * @def LH_UOFFSET_T_SIZE
 * @brief Size of ::lh_uoffset_t in bytes.
 *
 * Expands to ::LH_OFFSET_T_SIZE.
 */
#    define LH_UOFFSET_T_SIZE LH_OFFSET_T_SIZE
#endif /* LH_UOFFSET_T_SIZE */

#ifndef LH_SOFFSET_T_MIN
/**
 * @def LH_SOFFSET_T_MIN
 * @brief Minimum value of ::lh_soffset_t (alias of ::lh_saddr_t lower bound).
 */
#    define LH_SOFFSET_T_MIN LH_SADDR_T_MIN
#endif /* LH_SOFFSET_T_MIN */

#ifndef LH_SOFFSET_T_MAX
/**
 * @def LH_SOFFSET_T_MAX
 * @brief Maximum value of ::lh_soffset_t (alias of ::lh_saddr_t upper bound).
 */
#    define LH_SOFFSET_T_MAX LH_SADDR_T_MAX
#endif /* LH_SOFFSET_T_MAX */

#ifndef LH_SOFFSET_T_SIZE
/**
 * @def LH_SOFFSET_T_SIZE
 * @brief Size of ::lh_soffset_t in bytes.
 *
 * Expands to ::LH_OFFSET_T_SIZE.
 */
#    define LH_SOFFSET_T_SIZE LH_OFFSET_T_SIZE
#endif /* LH_SOFFSET_T_SIZE */

#endif /* LH_OFFSET_LIMITS_H */
