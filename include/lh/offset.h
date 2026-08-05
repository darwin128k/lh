/**
 * @file offset.h
 * @brief Pointer-width offset aliases ::lh_uoffset_t / ::lh_soffset_t.
 */

#ifndef LH_OFFSET_H
#define LH_OFFSET_H

#include <lh/addr.h>

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

/**
 * @typedef lh_uoffset_t
 * @brief Unsigned byte offset type derived from ::lh_uaddr_t.
 *
 * Use this when a value is semantically an offset (distance from a base),
 * not a raw absolute address.
 */
typedef lh_uaddr_t lh_uoffset_t;

/**
 * @typedef lh_soffset_t
 * @brief Signed byte offset type derived from ::lh_saddr_t.
 *
 * Use this for relative shifts that may be negative.
 */
typedef lh_saddr_t lh_soffset_t;

#endif /* LH_OFFSET_H */
