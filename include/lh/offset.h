/**
 * @file offset.h
 * @brief Pointer-width offset aliases ::lh_uoffset_t / ::lh_soffset_t.
 *
 * Limits (::LH_UOFFSET_T_MIN, ::LH_UOFFSET_T_MAX, ::LH_UOFFSET_T_SIZE,
 * ::LH_SOFFSET_T_MIN, ::LH_SOFFSET_T_MAX, ::LH_SOFFSET_T_SIZE,
 * ::LH_OFFSET_T_SIZE) live in <lh/offset/limits.h>.
 */

#ifndef LH_OFFSET_H
#define LH_OFFSET_H

#include <lh/addr.h>

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
