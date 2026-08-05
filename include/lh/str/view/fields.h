/**
 * @file fields.h
 * @brief Macro for declaring string-view endpoint fields (`first` / `second`).
 *
 * Injects the same pair of pointer fields as ::lh_memory_bounds_fields,
 * with a name that reflects non-owning string slice semantics
 * (half-open <tt>[first, second)</tt> like memory bounds).
 */

#ifndef LH_STR_VIEW_FIELDS_H
#define LH_STR_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_str_view_fields(T)
 * @brief Expands to the same member declarations as ::lh_memory_bounds_fields.
 *
 * Use inside a struct body when two pointers delimit a string view.
 * Equivalent to `lh_memory_bounds_fields(T)` and thus to `lh_interval_bounds_fields(T *)`.
 *
 * @param T Pointee type (not a pointer type); each field has type `T *`.
 *
 * Example usage:
 * @code{.c}
 * struct lh_str_view {
 *     lh_str_view_fields(const char);
 * };
 * @endcode
 *
 * @see lh_memory_bounds_fields
 * @see lh_interval_bounds_fields
 */
#define lh_str_view_fields(T) lh_memory_bounds_fields(T)

#endif /* LH_STR_VIEW_FIELDS_H */
