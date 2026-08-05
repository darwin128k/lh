/**
 * @file fields.h
 * @brief Macro for memory-view endpoint fields (`first` / `second`, const-qualified).
 */

#ifndef LH_MEMORY_VIEW_FIELDS_H
#define LH_MEMORY_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_memory_view_fields(T)
 * @brief Expands to the same member declarations as ::lh_memory_bounds_fields.
 *
 * Use inside a struct body when two pointers delimit a const view of memory
 * (half-open <tt>[first, second)</tt>).
 * Equivalent to `lh_memory_bounds_fields(const T)`
 * and thus to `lh_interval_bounds_fields(const T *)`.
 */
#define lh_memory_view_fields(T) lh_memory_bounds_fields(const T)

#endif /* LH_MEMORY_VIEW_FIELDS_H */
