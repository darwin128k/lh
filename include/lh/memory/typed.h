#ifndef LH_MEMORY_TYPED_H
#define LH_MEMORY_TYPED_H

#include <lh/memory/typed/fields.h>
#include <lh/memory/bounds.h>
#include <lh/index.h>

typedef struct lh_memory_typed
{
    lh_memory_typed_fields(lh_memory_bounds_t);
} lh_memory_typed_t;

LH_COMPILER_EXTERN_C_BEGIN

LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_t *
lh_memory_typed_get_bounds(lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
const lh_memory_bounds_t *
lh_memory_typed_get_bounds_as_const(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_data(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_size_of_bytes(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_multiple_of(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_uindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_uoffset_t
lh_memory_typed_get_offset_from_index(const lh_memory_typed_t *self, lh_uindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_uindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_memory_typed_get_last_index(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_memory_typed_get_index_from_end(const lh_memory_typed_t *self, lh_uindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_ptr_from_end(const lh_memory_typed_t *self, lh_uindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_first_ptr(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_last_ptr(const lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_sindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_memory_typed_get_index_from_offset(const lh_memory_typed_t *self, lh_uoffset_t offset);

LH_ATTRIBUTE_SYMBOL
lh_uindex_t
lh_memory_typed_get_index_from_ptr(const lh_memory_typed_t *self, const lh_ptr ptr);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set_v(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_swap(lh_memory_typed_t *self, lh_memory_typed_t *other);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_swap_v(lh_memory_typed_t *self, lh_memory_typed_t *other);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_retype(lh_memory_typed_t *self, lh_usize_t type_size);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_clear(lh_memory_typed_t *self);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_init_empty(lh_memory_typed_t *self, lh_usize_t type_size);

LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_t
lh_memory_typed_get_value_bounds(const lh_memory_typed_t *self, lh_uindex_t index);

LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_t
lh_memory_typed_get_range_bounds(const lh_memory_typed_t *self, lh_uindex_t index, lh_usize_t count);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set_value(lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr value);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_get_value_into(const lh_memory_typed_t *self, lh_uindex_t index, lh_ptr dst);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_swap_values(lh_memory_typed_t *self, lh_uindex_t i, lh_uindex_t j, lh_ptr scratch);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_typed_value_equals(const lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr other);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_set_values(lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr values,
                           lh_usize_t count);

LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_typed_move_within(lh_memory_typed_t *self, lh_uindex_t dst_index, lh_uindex_t src_index,
                     lh_usize_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TYPED_H */
