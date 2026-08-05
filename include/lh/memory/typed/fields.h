/**
 * @file fields.h
 * @brief Macro for declaring fields of typed memory bounds wrappers.
 */

#ifndef LH_MEMORY_TYPED_FIELDS_H
#define LH_MEMORY_TYPED_FIELDS_H

#include <lh/size.h>

/**
 * @def lh_memory_typed_fields(T)
 * @brief Declares `bounds` and `type_size` members for typed-memory structs.
 *
 * Embeds the given type @p T as the @c bounds field and adds a @c type_size field.
 * The resulting struct inherits all fields from @p T (if any), plus these two new fields.
 *
 * @param T Type of the @c bounds field — must be a struct type with two pointer fields
 *        named @c first and @c second (for example ::lh_memory_bounds_t,
 *        ::lh_memory_view_t). These fields are accessed via the wrapper's
 *        @c bounds member.
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
 * lh_memory_bounds_t bounds;   // inherited from T
 * lh_usize_t type_size;      // size of one element in bytes
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
#define lh_memory_typed_fields(T)                                                                  \
    T bounds;                                                                                      \
    lh_usize_t type_size

#endif /* LH_MEMORY_TYPED_FIELDS_H */
