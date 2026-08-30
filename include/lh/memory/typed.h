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
lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_uindex_t index);

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TYPED_H */
