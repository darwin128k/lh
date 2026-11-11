/**
 * @file fields.h
 * @brief Member fields of ::lh_array_t.
 */

#ifndef LH_ARRAY_FIELDS_H
#define LH_ARRAY_FIELDS_H

/**
 * @def lh_array_fields(typed_type, size_type, allocator_type)
 * @brief The owned block, how much of it is in use, and who makes it.
 *
 * `typed` always spans the full allocated capacity; `size` is the
 * number of elements actually in use, from the start of that capacity
 * (`size <= capacity`). `allocator` makes, grows and frees the block: the
 * runtime allocator, or the one the caller named at init.
 *
 * @param typed_type     Type of `typed` (::lh_memory_typed_allocated_t).
 * @param size_type      Type of `size` (::lh_usize_t).
 * @param allocator_type Type of `allocator` (::lh_memory_allocator_t *).
 */
#define lh_array_fields(typed_type, size_type, allocator_type)                                     \
    typed_type typed;                                                                              \
    size_type size;                                                                                \
    allocator_type allocator

#endif /* LH_ARRAY_FIELDS_H */
