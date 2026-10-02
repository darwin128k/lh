/**
 * @file fields.h
 * @brief Member fields of ::lh_array_t.
 */

#ifndef LH_ARRAY_FIELDS_H
#define LH_ARRAY_FIELDS_H

/**
 * @def lh_array_fields(typed_type, size_type)
 * @brief The owned block and how much of it is in use.
 *
 * `typed.bounds` always spans the full allocated capacity; `size` is the
 * number of elements actually in use, from the start of that capacity
 * (`size <= capacity`).
 *
 * @param typed_type Type of `typed` (::lh_memory_typed_allocated_t).
 * @param size_type  Type of `size` (::lh_usize_t).
 */
#define lh_array_fields(typed_type, size_type)                                                     \
    typed_type typed;                                                                              \
    size_type size

#endif /* LH_ARRAY_FIELDS_H */
