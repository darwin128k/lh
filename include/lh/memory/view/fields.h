/**
 * @file fields.h
 * @brief Macro for memory-view fields (`begin` / `size`, const-qualified).
 */

#ifndef LH_MEMORY_VIEW_FIELDS_H
#define LH_MEMORY_VIEW_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_memory_view_fields(elem_type)
 * @brief Expands to the same member declarations as ::lh_memory_bounds_fields
 *        with a const pointee.
 *
 * Use inside a struct body when a pointer and a byte length delimit a const
 * view of memory, <tt>[begin, begin + size)</tt>.
 * Equivalent to `lh_memory_bounds_fields(const elem_type)`.
 *
 * @param elem_type Pointee type (not a pointer type).
 */
#define lh_memory_view_fields(elem_type) lh_memory_bounds_fields(const elem_type)

#endif /* LH_MEMORY_VIEW_FIELDS_H */
