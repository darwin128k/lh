/**
 * @file fields.h
 * @brief Macro for declaring memory-bounds fields (`begin` / `size`).
 *
 * Names a contiguous byte span by its first byte and its length:
 * <tt>[begin, begin + size)</tt>.
 */

#ifndef LH_MEMORY_BOUNDS_FIELDS_H
#define LH_MEMORY_BOUNDS_FIELDS_H

#include <lh/size.h>

/**
 * @def lh_memory_bounds_fields(elem_type)
 * @brief Expands to an `elem_type *begin` member and an ::lh_usize_t `size` member.
 *
 * Use inside a struct body when a pointer and a length delimit memory bounds:
 * @c begin is the first byte, @c size the number of bytes. @c size counts
 * bytes whatever @p elem_type is, so the struct can be handed to the
 * ::lh_memory_bounds_t / ::lh_memory_view_t functions as is.
 *
 * @param elem_type Pointee type (not a pointer type); @c begin has type
 *                  `elem_type *`.
 *
 * Example usage:
 * @code{.c}
 * struct lh_memory_bounds {
 *     lh_memory_bounds_fields(void);
 * };
 * @endcode
 */
#define lh_memory_bounds_fields(elem_type)                                                         \
    elem_type *begin;                                                                              \
    lh_usize_t size

#endif /* LH_MEMORY_BOUNDS_FIELDS_H */
