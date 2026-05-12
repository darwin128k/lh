/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_memory_view_slice_t.
 */

#ifndef LH_MEMORY_VIEW_SLICE_INITIALIZER_H
#define LH_MEMORY_VIEW_SLICE_INITIALIZER_H

#include <lh/interval/bounds/initializer.h>
#include <lh/null.h>

#define lh_memory_view_slice_initializer(begin, end) lh_interval_bounds_initializer(begin, end)

#define lh_memory_view_slice_initializer_empty() lh_memory_view_slice_initializer(lh_null, lh_null)

#endif // LH_MEMORY_VIEW_SLICE_INITIALIZER_H
