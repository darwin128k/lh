/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_view_slice_t.
 */

#ifndef LH_MEMORY_VIEW_SLICE_INITIALIZER_H
#define LH_MEMORY_VIEW_SLICE_INITIALIZER_H

#include <lh/null.h>

/** @brief Initializer of a slice from @p begin to @p end (both ends inclusive). */
#define lh_memory_view_slice_initializer(begin, end) lh_interval_bounds_initializer(begin, end)

/** @brief Initializer of an empty slice: both ends ::lh_null. */
#define lh_memory_view_slice_initializer_empty() lh_memory_view_slice_initializer(lh_null, lh_null)

#endif /* LH_MEMORY_VIEW_SLICE_INITIALIZER_H */
