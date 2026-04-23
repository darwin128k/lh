#include <lh/memory/typed.h>
#include <lh/memory/range.h>
#include <lh/runtime/check/ref.h>
#include <lh/optional/ref.h>
#include <lh/util/ptr.h>

lh_memory_range_t *lh_memory_typed_get_range(lh_memory_typed_t *self) {
    lh_runtime_check_ref(self);
    return lh_addr_ref(self->range);
}

const lh_memory_range_t *lh_memory_typed_get_crange(const lh_memory_typed_t *self) {
    lh_runtime_check_ref(self);
    return lh_addr_ref(self->range);
}

lh_usize_t lh_memory_typed_get_type_size(const lh_memory_typed_t *self) {
    lh_runtime_check_ref(self);
    return self->type_size;
}

lh_ptr lh_memory_typed_get_begin(const lh_memory_typed_t *self) {
    const lh_memory_range_t *r = lh_memory_typed_get_crange(self);
    return lh_memory_range_get_begin(r);
}

lh_ptr lh_memory_typed_get_end(const lh_memory_typed_t *self) {
    const lh_memory_range_t *r = lh_memory_typed_get_crange(self);
    return lh_memory_range_get_end(r);
}

lh_usize_t lh_memory_typed_get_size(const lh_memory_typed_t *self) {
    const lh_memory_range_t *r = lh_memory_typed_get_crange(self);

    lh_usize_t size = lh_memory_range_get_size(r);
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);

    lh_runtime_check(lh_memory_range_is_multiple_of(r, type_size),
                     lh_runtime_error_code_size_not_multiple_of_type_size);

    return size / type_size;
}

lh_bool_t lh_memory_typed_is_empty(const lh_memory_typed_t *self) {
    return lh_memory_typed_get_size(self) == 0;
}

lh_bool_t lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index) {
    return index < lh_memory_typed_get_size(self);
}

lh_ptr lh_memory_typed_at_ptr_from_front(const lh_memory_typed_t *self, lh_usize_t index) {
    lh_runtime_check(lh_memory_typed_is_valid_index(self, index),
                     lh_runtime_error_code_out_of_range);

    const lh_memory_range_t *r = lh_memory_typed_get_crange(self);
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);

    return lh_memory_range_get_ptr_from_front(r, index * type_size);
}

lh_ptr lh_memory_typed_at_ptr_from_back(const lh_memory_typed_t *self, lh_usize_t index) {
    lh_runtime_check(lh_memory_typed_is_valid_index(self, index),
                     lh_runtime_error_code_out_of_range);

    const lh_memory_range_t *r = lh_memory_typed_get_crange(self);
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);

    return lh_memory_range_get_ptr_from_back(r, index * type_size);
}

lh_ptr lh_memory_typed_at_ptr(const lh_memory_typed_t *self, lh_usize_t index,
                              lh_bool_t from_back) {
    return from_back ? lh_memory_typed_at_ptr_from_back(self, index)
                     : lh_memory_typed_at_ptr_from_front(self, index);
}

lh_byte_t lh_memory_typed_at_from_front(const lh_memory_typed_t *self, lh_usize_t index) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_at_ptr_from_front(self, index)));
}

lh_byte_t lh_memory_typed_at_from_back(const lh_memory_typed_t *self, lh_usize_t index) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_at_ptr_from_back(self, index)));
}

lh_byte_t lh_memory_typed_at(const lh_memory_typed_t *self, lh_usize_t index, lh_bool_t from_back) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_at_ptr(self, index, from_back)));
}

lh_ptr lh_memory_typed_get_front_ptr(const lh_memory_typed_t *self) {
    return lh_memory_typed_at_ptr_from_front(self, 0);
}

lh_ptr lh_memory_typed_get_back_ptr(const lh_memory_typed_t *self) {
    return lh_memory_typed_at_ptr_from_back(self, 0);
}

lh_byte_t lh_memory_typed_get_front(const lh_memory_typed_t *self) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_front_ptr(self)));
}

lh_byte_t lh_memory_typed_get_back(const lh_memory_typed_t *self) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_back_ptr(self)));
}