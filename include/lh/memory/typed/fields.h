/**
 * @file fields.h
 * @brief Macro for declaring fields of typed memory bounds wrappers.
 */

#ifndef LH_MEMORY_TYPED_FIELDS_H
#define LH_MEMORY_TYPED_FIELDS_H

#include <lh/size.h>

/**
 * @def lh_memory_typed_fields(bounds_type)
 * @brief Declares `bounds`, `type_size`, and `size` members for typed-memory structs.
 *
 * @param bounds_type Type of the @c bounds field — a struct with two pointer
 *        fields named @c first and @c second (for example ::lh_memory_bounds_t,
 *        ::lh_memory_view_t).
 *
 * Example usage — creates a typed wrapper around lh_memory_bounds_t:
 * @code{.c}
 * typedef struct lh_memory_typed {
 *     lh_memory_typed_fields(lh_memory_bounds_t);
 * } lh_memory_typed_t;
 * @endcode
 *
 * The resulting struct has:
 * @code{.c}
 * lh_memory_bounds_t bounds;
 * lh_usize_t type_size;      // size of one element in bytes
 * lh_usize_t size;           // element count — a cached fact, not derived from bounds on
 *                            // read; every mutator that changes bounds or type_size is
 *                            // responsible for keeping this in sync (see typed.c)
 * @endcode
 *
 * Example usage — creates a typed wrapper around a custom bounds type:
 * @code{.c}
 * typedef struct my_bounds {
 *     void *begin;
 *     void *end;
 * } my_bounds_t;
 *
 * typedef struct my_typed {
 *     lh_memory_typed_fields(my_bounds_t);
 * } my_typed_t;
 * @endcode
 */
#define lh_memory_typed_fields(bounds_type)                                                        \
    bounds_type bounds;                                                                            \
    lh_usize_t type_size;                                                                          \
    lh_usize_t size

#endif /* LH_MEMORY_TYPED_FIELDS_H */
