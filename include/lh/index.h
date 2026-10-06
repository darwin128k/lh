/**
 * @file index.h
 * @brief Element index aliases ::lh_sindex_t / ::lh_uindex_t.
 *
 * Limits (::LH_SINDEX_T_MIN, ::LH_SINDEX_T_MAX, ::LH_SINDEX_T_SIZE,
 * ::LH_UINDEX_T_MIN, ::LH_UINDEX_T_MAX, ::LH_UINDEX_T_SIZE,
 * ::LH_INDEX_T_SIZE) live in <lh/index/limits.h>.
 */

#ifndef LH_INDEX_H
#define LH_INDEX_H

#include <lh/size.h>

/**
 * @typedef lh_sindex_t
 * @brief Signed element index type derived from ::lh_ssize_t.
 *
 * Non-negative values count from the beginning; negative values count from
 * the end (@c -1 is the last element, @c -2 the second-to-last, etc.),
 * matching the @c lh_soffset_t convention used by
 * ::lh_memory_view_get_ptr, ::lh_memory_bounds_slice_get_ptr, and
 * ::lh_memory_view_slice_get_ptr.
 */
typedef lh_ssize_t lh_sindex_t;

/**
 * @typedef lh_uindex_t
 * @brief Unsigned element index type derived from ::lh_usize_t.
 *
 * Always counts from the beginning; there is no from-the-end convention,
 * matching the @c lh_uoffset_t convention used by
 * ::lh_memory_view_get_ptr_from_begin, ::lh_memory_view_get_ptr_from_end,
 * and their bounds/slice equivalents.
 */
typedef lh_usize_t lh_uindex_t;

#endif /* LH_INDEX_H */
