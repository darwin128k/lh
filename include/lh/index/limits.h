/**
 * @file limits.h
 * @brief Min/max/size macros for ::lh_sindex_t / ::lh_uindex_t.
 *
 * Aliases of the matching ::lh_ssize_t / ::lh_usize_t limits from
 * <lh/size/limits.h>. Kept out of <lh/index.h> so the typedef header stays light.
 */

#ifndef LH_INDEX_LIMITS_H
#define LH_INDEX_LIMITS_H

#include <lh/size/limits.h>

#ifndef LH_INDEX_T_SIZE
/**
 * @def LH_INDEX_T_SIZE
 * @brief Size in bytes of ::lh_sindex_t and ::lh_uindex_t.
 *
 * Expands to ::LH_SIZE_T_SIZE.
 */
#    define LH_INDEX_T_SIZE LH_SIZE_T_SIZE
#endif /* LH_INDEX_T_SIZE */

#ifndef LH_SINDEX_T_MIN
/**
 * @def LH_SINDEX_T_MIN
 * @brief Minimum value of ::lh_sindex_t (alias of ::lh_ssize_t lower bound).
 */
#    define LH_SINDEX_T_MIN LH_SSIZE_T_MIN
#endif /* LH_SINDEX_T_MIN */

#ifndef LH_SINDEX_T_MAX
/**
 * @def LH_SINDEX_T_MAX
 * @brief Maximum value of ::lh_sindex_t (alias of ::lh_ssize_t upper bound).
 */
#    define LH_SINDEX_T_MAX LH_SSIZE_T_MAX
#endif /* LH_SINDEX_T_MAX */

#ifndef LH_SINDEX_T_SIZE
/**
 * @def LH_SINDEX_T_SIZE
 * @brief Size of ::lh_sindex_t in bytes.
 *
 * Expands to ::LH_INDEX_T_SIZE.
 */
#    define LH_SINDEX_T_SIZE LH_INDEX_T_SIZE
#endif /* LH_SINDEX_T_SIZE */

#ifndef LH_UINDEX_T_MIN
/**
 * @def LH_UINDEX_T_MIN
 * @brief Minimum value of ::lh_uindex_t (alias of ::lh_usize_t lower bound).
 */
#    define LH_UINDEX_T_MIN LH_USIZE_T_MIN
#endif /* LH_UINDEX_T_MIN */

#ifndef LH_UINDEX_T_MAX
/**
 * @def LH_UINDEX_T_MAX
 * @brief Maximum value of ::lh_uindex_t (alias of ::lh_usize_t upper bound).
 */
#    define LH_UINDEX_T_MAX LH_USIZE_T_MAX
#endif /* LH_UINDEX_T_MAX */

#ifndef LH_UINDEX_T_SIZE
/**
 * @def LH_UINDEX_T_SIZE
 * @brief Size of ::lh_uindex_t in bytes.
 *
 * Expands to ::LH_INDEX_T_SIZE.
 */
#    define LH_UINDEX_T_SIZE LH_INDEX_T_SIZE
#endif /* LH_UINDEX_T_SIZE */

#endif /* LH_INDEX_LIMITS_H */
