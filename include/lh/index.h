/**
 * @file index.h
 * @brief Element index aliases ::lh_sindex_t / ::lh_uindex_t.
 */

#ifndef LH_INDEX_H
#define LH_INDEX_H

#include <lh/size.h>

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
