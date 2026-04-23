/**
 * @file fields.h
 * @brief Macro for declaring memory-range endpoint fields (`first` / `second`).
 *
 * Semantically names a contiguous byte span as a half-open pointer interval
 * <tt>[first, second)</tt>, using ::lh_interval_bounds_fields (same layout as
 * ::lh_pair_fields of two pointers).
 */

#ifndef LH_MEMORY_RANGE_FIELDS_H
#define LH_MEMORY_RANGE_FIELDS_H

#include <lh/interval/bounds/fields.h>

/**
 * @def lh_memory_range_fields(T)
 * @brief Expands to two `T *` members via ::lh_interval_bounds_fields (`first`, `second`).
 *
 * Use inside a struct body when two pointers delimit a memory range
 * (@c first inclusive begin, @c second exclusive end).
 * Equivalent to `lh_interval_bounds_fields(T *)`.
 *
 * @param T Pointee type (not a pointer type); each field has type `T *`.
 *
 * Example usage:
 * @code{.c}
 * struct lh_memory_range {
 *     lh_memory_range_fields(void);
 * };
 * @endcode
 *
 * @see lh_interval_bounds_fields
 */
#define lh_memory_range_fields(T) lh_interval_bounds_fields(T *)

#endif // LH_MEMORY_RANGE_FIELDS_H
