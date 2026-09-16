/**
 * @file fields.h
 * @brief Macro for declaring wide-string-view endpoint fields (`first` / `second`).
 *
 * Injects the same pair of pointer fields as ::lh_memory_bounds_fields,
 * with a name that reflects non-owning wide string slice semantics
 * (half-open <tt>[first, second)</tt> like memory bounds).
 */

#ifndef LH_WSTR_VIEW_FIELDS_H
#define LH_WSTR_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_wstr_view_fields(elem_type)
 * @brief Expands to the same member declarations as ::lh_memory_bounds_fields.
 *
 * Use inside a struct body when two pointers delimit a wide string view.
 * Equivalent to `lh_memory_bounds_fields(elem_type)` and thus to
 * `lh_interval_bounds_fields(elem_type *)`.
 *
 * @param elem_type Pointee type (not a pointer type); each field has type
 *                  `elem_type *`. Typically `const lh_wchar_t`.
 *
 * Example usage:
 * @code{.c}
 * struct lh_wstr_view {
 *     lh_wstr_view_fields(const lh_wchar_t);
 * };
 * @endcode
 *
 * @see lh_memory_bounds_fields
 * @see lh_str_view_fields
 */
#define lh_wstr_view_fields(elem_type) lh_memory_bounds_fields(elem_type)

#endif /* LH_WSTR_VIEW_FIELDS_H */
