/**
 * @file fields.h
 * @brief Macro for declaring string-view fields (`begin` / `size`).
 *
 * Injects the same fields as ::lh_memory_bounds_fields,
 * with a name that reflects non-owning string semantics
 * (<tt>[begin, begin + size)</tt>, @c size in bytes, like memory bounds).
 */

#ifndef LH_STR_VIEW_FIELDS_H
#define LH_STR_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_str_view_fields(elem_type)
 * @brief Expands to the same member declarations as ::lh_memory_bounds_fields.
 *
 * Use inside a struct body when a pointer and a byte length delimit a string view.
 * Equivalent to `lh_memory_bounds_fields(elem_type)`.
 *
 * @param elem_type Pointee type (not a pointer type); @c begin has type
 *                  `elem_type *`.
 *
 * Example usage:
 * @code{.c}
 * struct lh_str_view {
 *     lh_str_view_fields(const char);
 * };
 * @endcode
 *
 * @see lh_memory_bounds_fields
 */
#define lh_str_view_fields(elem_type) lh_memory_bounds_fields(elem_type)

#endif /* LH_STR_VIEW_FIELDS_H */
