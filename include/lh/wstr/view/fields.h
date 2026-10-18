/**
 * @file fields.h
 * @brief Macro for declaring wide-string-view fields (`begin` / `size`).
 *
 * Injects the same fields as ::lh_memory_bounds_fields,
 * with a name that reflects non-owning wide string semantics
 * (<tt>[begin, begin + size)</tt>, @c size in bytes, like memory bounds).
 */

#ifndef LH_WSTR_VIEW_FIELDS_H
#define LH_WSTR_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_wstr_view_fields(elem_type)
 * @brief Expands to the same member declarations as ::lh_memory_bounds_fields.
 *
 * Use inside a struct body when a pointer and a byte length delimit a wide string view.
 * Equivalent to `lh_memory_bounds_fields(elem_type)`.
 *
 * @param elem_type Pointee type (not a pointer type); @c begin has type
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
