/**
 * @file fields.h
 * @brief Macro for declaring fields of typed memory.
 */

#ifndef LH_MEMORY_TYPED_FIELDS_H
#define LH_MEMORY_TYPED_FIELDS_H

#include <lh/memory/bounds/fields.h>

/**
 * @def lh_memory_typed_fields(elem_type)
 * @brief The fields of ::lh_memory_bounds_fields plus `type_size`.
 *
 * A typed struct starts with exactly the fields of memory bounds, so it can be
 * handed to the ::lh_memory_bounds_t functions as is; `type_size` is the size
 * of one element in bytes. The element count is not stored: it is the byte
 * size divided by `type_size`.
 *
 * @param elem_type Pointee type of @c begin (not a pointer type).
 *
 * Example usage:
 * @code{.c}
 * struct lh_memory_typed {
 *     lh_memory_typed_fields(lh_void);
 * };
 * typedef struct lh_memory_typed lh_memory_typed_t;
 * @endcode
 */
#define lh_memory_typed_fields(elem_type)                                                          \
    lh_memory_bounds_fields(elem_type);                                                            \
    lh_usize_t type_size

#endif /* LH_MEMORY_TYPED_FIELDS_H */
