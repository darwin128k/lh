#include <lh/memory/bounds/slice.h>
#include <lh/runtime/check/ref.h>
#include <lh/util/interval.h>
#include <lh/optional/ref.h>
#include <lh/util/ptr.h>

lh_void
lh_memory_bounds_slice_unpack(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end) {
    lh_runtime_check_ref(self);

    lh_optional_ref(begin) {
        lh_ptr_deref(begin) = self->first;
    }

    lh_optional_ref(end) {
        lh_ptr_deref(end) = self->second;
    }
}

lh_ptr
lh_memory_bounds_slice_get_begin(const lh_memory_bounds_slice_t *self) {
    lh_ptr begin;
    lh_memory_bounds_slice_unpack(self, lh_addr_of(begin), lh_null);
    return begin;
}

lh_ptr
lh_memory_bounds_slice_get_end(const lh_memory_bounds_slice_t *self) {
    lh_ptr end;
    lh_memory_bounds_slice_unpack(self, lh_null, lh_addr_of(end));
    return end;
}

lh_memory_bounds_slice_flags_t
lh_memory_bounds_slice_get_flags(const lh_memory_bounds_slice_t *self) {
    lh_memory_bounds_slice_flags_t flags = lh_memory_bounds_slice_flags_uninitialized;

    lh_void *begin, *end;
    lh_memory_bounds_slice_unpack(self, lh_addr_of(begin), lh_addr_of(end));

    if (!lh_ptr_is_null(begin)) {
        lh_bit_set(flags, lh_memory_bounds_slice_flags_initialized_begin);
    }

    if (!lh_ptr_is_null(end)) {
        lh_bit_set(flags, lh_memory_bounds_slice_flags_initialized_end);
    }

    return flags;
}

lh_bool_t
lh_memory_bounds_slice_is_uninitialized(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_get_flags(self) == lh_memory_bounds_slice_flags_uninitialized;
}

lh_bool_t
lh_memory_bounds_slice_is_initialized(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_get_flags(self) == lh_memory_bounds_slice_flags_initialized;
}

lh_memory_bounds_slice_direction_t
lh_memory_bounds_slice_get_direction(const lh_memory_bounds_slice_t *self) {
    lh_memory_bounds_slice_direction_t direction = lh_memory_bounds_slice_direction_unknown;
    if (lh_memory_bounds_slice_is_initialized(self)) {
        lh_void *begin, *end;
        lh_memory_bounds_slice_unpack(self, lh_addr_of(begin), lh_addr_of(end));

        if (lh_interval_closed_is_valid(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end))) {
            direction = lh_memory_bounds_slice_direction_forward;
        } else {
            direction = lh_memory_bounds_slice_direction_backward;
        }
    }
    return direction;
}

lh_bool_t
lh_memory_bounds_slice_is_forward_direction(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_get_direction(self) == lh_memory_bounds_slice_direction_forward;
}

lh_bool_t
lh_memory_bounds_slice_is_backward_direction(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_get_direction(self) == lh_memory_bounds_slice_direction_backward;
}

lh_bool_t
lh_memory_bounds_slice_is_valid(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_is_forward_direction(self);
}

lh_void
lh_memory_bounds_slice_unpack_v(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end) {
    lh_runtime_check(lh_memory_bounds_slice_is_valid(self),
                     lh_runtime_error_code_invalid_memory_range);
    lh_memory_bounds_slice_unpack(self, begin, end);
}

lh_ptr
lh_memory_bounds_slice_get_begin_v(const lh_memory_bounds_slice_t *self) {
    lh_ptr begin;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(begin), lh_null);
    return begin;
}

lh_ptr
lh_memory_bounds_slice_get_end_v(const lh_memory_bounds_slice_t *self) {
    lh_ptr end;
    lh_memory_bounds_slice_unpack_v(self, lh_null, lh_addr_of(end));
    return end;
}

lh_usize_t
lh_memory_bounds_slice_get_size(const lh_memory_bounds_slice_t *self) {
    lh_void *begin, *end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_closed_get_size(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_bounds_slice_is_empty(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_is_uninitialized(self) ||
           lh_math_is_zero(lh_memory_bounds_slice_get_size(self));
}

lh_bool_t
lh_memory_bounds_slice_is_valid_offset(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset) {
    const lh_usize_t size = lh_memory_bounds_slice_get_size(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_bool_t
lh_memory_bounds_slice_contains_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr) {
    lh_void *begin, *end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_closed_contains_value(begin, end, ptr);
}

lh_bool_t
lh_memory_bounds_slice_contains_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                   const lh_ptr end) {
    lh_void *self_begin, *self_end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_closed_contains_range(self_begin, self_end, begin, end);
}

lh_bool_t
lh_memory_bounds_slice_contains(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other) {
    lh_void *other_begin, *other_end;
    lh_memory_bounds_slice_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_slice_contains_of(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_slice_get_ptr_from_begin(const lh_memory_bounds_slice_t *self,
                                          lh_uoffset_t offset) {
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_slice_get_begin(self), offset);
}

lh_ptr
lh_memory_bounds_slice_get_ptr_from_end(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset) {
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_slice_get_end(self),
                                       -lh_type_cast(lh_soffset_t, offset));
}

lh_ptr
lh_memory_bounds_slice_get_ptr(const lh_memory_bounds_slice_t *self, lh_soffset_t offset) {
    if (lh_math_ge(offset, 0)) {
        return lh_memory_bounds_slice_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, offset));
    } else {
        return lh_memory_bounds_slice_get_ptr_from_end(self,
                                                       lh_type_cast(lh_uoffset_t, -offset - 1));
    }
}

lh_byte_t
lh_memory_bounds_slice_get_value_from_begin(const lh_memory_bounds_slice_t *self,
                                            lh_uoffset_t offset) {
    return lh_ptr_deref(
        lh_ptr_cast(lh_byte_t, lh_memory_bounds_slice_get_ptr_from_begin(self, offset)));
}

lh_byte_t
lh_memory_bounds_slice_get_value_from_end(const lh_memory_bounds_slice_t *self,
                                          lh_uoffset_t offset) {
    return lh_ptr_deref(
        lh_ptr_cast(lh_byte_t, lh_memory_bounds_slice_get_ptr_from_end(self, offset)));
}

lh_byte_t
lh_memory_bounds_slice_get_value(const lh_memory_bounds_slice_t *self, lh_soffset_t offset) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_bounds_slice_get_ptr(self, offset)));
}

lh_byte_t
lh_memory_bounds_slice_get_begin_value(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_get_value_from_begin(self, 0);
}

lh_byte_t
lh_memory_bounds_slice_get_end_value(const lh_memory_bounds_slice_t *self) {
    return lh_memory_bounds_slice_get_value_from_end(self, 0);
}
