#include <lh/memory/range.h>
#include <lh/memory/range/initializer.h>
#include <lh/memory/raw.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/runtime/check/ref.h>
#include <lh/runtime/try.h>
#include <lh/util/algorithm.h>
#include <lh/util/interval.h>
#include <lh/util/ptr.h>

void lh_memory_range_pack(lh_memory_range_t *self, lh_ptr *begin, lh_ptr *end) {
    lh_runtime_check_ref(self);

    lh_optional_ref(begin) {
        self->first = lh_ptr_deref(begin);
    }

    lh_optional_ref(end) {
        self->second = lh_ptr_deref(end);
    }
}

void lh_memory_range_set(lh_memory_range_t *self, lh_ptr begin, lh_ptr end) {
    lh_memory_range_pack(self, lh_addr_ref(begin), lh_addr_ref(end));
}

void lh_memory_range_unpack(const lh_memory_range_t *self, lh_ptr *begin, lh_ptr *end) {
    lh_runtime_check_ref(self);

    lh_optional_ref(begin) {
        lh_ptr_deref(begin) = self->first;
    }

    lh_optional_ref(end) {
        lh_ptr_deref(end) = self->second;
    }
}

lh_ptr lh_memory_range_get_begin(const lh_memory_range_t *self) {
    lh_ptr begin;
    lh_memory_range_unpack(self, lh_addr_ref(begin), lh_null);
    return begin;
}

lh_ptr lh_memory_range_get_end(const lh_memory_range_t *self) {
    lh_ptr end;
    lh_memory_range_unpack(self, lh_null, lh_addr_ref(end));
    return end;
}

lh_void lh_memory_range_assign(lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    lh_memory_range_set(self, other_begin, other_end);
}

lh_void lh_memory_range_clear(lh_memory_range_t *self) {
    lh_memory_range_t other = lh_memory_range_empty_initializer();
    lh_memory_range_assign(self, &other);
}

lh_void lh_memory_range_swap(lh_memory_range_t *self, lh_memory_range_t *other) {
    lh_runtime_check_ref(self);
    lh_runtime_check_ref(other);

    lh_algorithm_swap(lh_memory_range_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

lh_void lh_memory_range_exchange(lh_memory_range_t *self, lh_memory_range_t *other) {
    lh_memory_range_clear(self);
    lh_memory_range_swap(self, other);
}

void lh_memory_range_set_by_size(lh_memory_range_t *self, lh_ptr begin, lh_usize_t size) {
    lh_runtime_check(begin, lh_runtime_error_code_invalid_argument);
    lh_ptr end = lh_ptr_add_by_offset(lh_void, begin, size);
    lh_memory_range_set(self, begin, end);
}

void lh_memory_range_init(lh_memory_range_t *self, lh_void *begin, lh_void *end) {
    lh_memory_range_set(self, begin, end);
}

void lh_memory_range_init_by_size(lh_memory_range_t *self, lh_ptr begin, lh_usize_t size) {
    lh_memory_range_set_by_size(self, begin, size);
}

void lh_memory_range_init_by_other(lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_memory_range_assign(self, other);
}

lh_memory_range_state_t lh_memory_range_get_state(const lh_memory_range_t *self) {
    lh_ptr begin;
    lh_ptr end;

    lh_memory_range_state_t state = lh_memory_range_state_empty;
    lh_memory_range_unpack(self, lh_addr_ref(begin), lh_addr_ref(end));

    if (lh_ptr_is_null(begin)) {
        lh_bit_set(state, lh_memory_range_state_null_begin);
    }

    if (lh_ptr_is_null(end)) {
        lh_bit_set(state, lh_memory_range_state_null_end);
    }

    if (state == lh_memory_range_state_empty) {
        if (lh_ptr_lt(begin, end)) {
            state = lh_memory_range_state_has_data;
        } else if (lh_ptr_gt(begin, end)) {
            state = lh_memory_range_state_reversed;
        }
    }

    return state;
}

lh_bool_t lh_memory_range_is_uninitialized(const lh_memory_range_t *self) {
    const lh_memory_range_state_t state = lh_memory_range_get_state(self);
    return state == lh_memory_range_state_uninitialized;
}

lh_bool_t lh_memory_range_has_data(const lh_memory_range_t *self) {
    const lh_memory_range_state_t state = lh_memory_range_get_state(self);
    return state == lh_memory_range_state_has_data;
}

lh_bool_t lh_memory_range_is_empty(const lh_memory_range_t *self) {
    const lh_memory_range_state_t state = lh_memory_range_get_state(self);
    return state == lh_memory_range_state_empty;
}

lh_bool_t lh_memory_range_is_valid(const lh_memory_range_t *self) {
    return lh_memory_range_is_empty(self) || lh_memory_range_has_data(self);
}

lh_bool_t lh_memory_range_is_invalid(const lh_memory_range_t *self) {
    return !lh_memory_range_is_valid(self);
}

lh_memory_range_t lh_memory_range_make(lh_ptr begin, lh_ptr end) {
    lh_memory_range_t range = lh_memory_range_initializer(begin, end);
    return range;
}

lh_memory_range_t lh_memory_range_make_v(lh_ptr begin, lh_ptr end) {
    lh_memory_range_t range = lh_memory_range_make(begin, end);
    lh_runtime_check_if(lh_memory_range_is_invalid(&range),
                        lh_runtime_error_code_invalid_memory_range);
    return range;
}

lh_memory_range_t lh_memory_range_make_by_size(lh_ptr begin, lh_usize_t size) {
    lh_ptr end = lh_ptr_add_by_offset(lh_void, begin, size);
    return lh_memory_range_make_v(begin, end);
}

lh_bool_t lh_memory_range_is_sliceable(const lh_memory_range_t *self, lh_uoffset_t offset,
                                       lh_uoffset_t size) {
    const lh_usize_t self_size = lh_memory_range_get_size(self);
    if (lh_interval_closed_is_add_overflow(offset, size, LH_UADDR_T_MIN, self_size)) {
        return lh_bool_false;
    }

    const lh_uoffset_t end_offset = lh_math_add(offset, size);
    return lh_math_le(end_offset, self_size);
}

lh_memory_range_t lh_memory_range_slice(const lh_memory_range_t *self, lh_uoffset_t offset,
                                        lh_uoffset_t size) {
    lh_runtime_check(lh_memory_range_is_sliceable(self, offset, size),
                     lh_runtime_error_code_out_of_range);

    lh_ptr begin = lh_memory_range_get_ptr(self, offset, lh_bool_false);
    lh_ptr end = lh_ptr_add_by_offset_unsafe(lh_void, begin, size);
    return lh_memory_range_make_v(begin, end);
}

void lh_memory_range_unpack_v(const lh_memory_range_t *self, lh_ptr *begin, lh_ptr *end) {
    lh_runtime_check_if(lh_memory_range_is_invalid(self),
                        lh_runtime_error_code_invalid_memory_range);
    lh_memory_range_unpack(self, begin, end);
}

lh_saddr_t lh_memory_range_diff(const lh_memory_range_t *self) {
    lh_ptr begin;
    lh_ptr end;

    lh_memory_range_unpack(self, lh_addr_ref(begin), lh_addr_ref(end));
    return lh_ptr_sdiff(end, begin);
}

lh_usize_t lh_memory_range_get_size(const lh_memory_range_t *self) {
    lh_runtime_check_if(lh_memory_range_is_invalid(self),
                        lh_runtime_error_code_invalid_memory_range);
    return lh_type_cast(lh_usize_t, lh_memory_range_diff(self));
}

lh_bool_t lh_memory_range_is_begin_aligned(const lh_memory_range_t *self, lh_usize_t align) {
    lh_ptr begin = lh_memory_range_get_begin(self);
    lh_runtime_check(lh_math_is_power_of_two(align), lh_runtime_error_code_not_power_of_two);
    return lh_ptr_is_aligned(begin, align);
}

lh_bool_t lh_memory_range_is_aligned(const lh_memory_range_t *self, lh_usize_t align) {
    const lh_bool_t is_begin_aligned = lh_memory_range_is_begin_aligned(self, align);
    lh_ptr end = lh_memory_range_get_end(self);
    return is_begin_aligned && lh_ptr_is_aligned(end, align);
}

lh_bool_t lh_memory_range_is_multiple_of(const lh_memory_range_t *self, lh_usize_t multiple) {
    const lh_usize_t size = lh_memory_range_get_size(self);
    return lh_math_is_multiple_of(size, multiple);
}

lh_bool_t lh_memory_range_is_valid_offset(const lh_memory_range_t *self, lh_uoffset_t offset) {
    const lh_usize_t size = lh_memory_range_get_size(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_ptr lh_memory_range_get_ptr_from_front(const lh_memory_range_t *self, lh_uoffset_t offset) {
    lh_runtime_check(lh_memory_range_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);

    lh_ptr begin = lh_memory_range_get_begin(self);
    return lh_ptr_add_by_offset_unsafe(lh_void, begin, offset);
}

lh_ptr lh_memory_range_get_ptr_from_back(const lh_memory_range_t *self, lh_uoffset_t offset) {
    const lh_usize_t size = lh_memory_range_get_size(self);
    return lh_memory_range_get_ptr_from_front(self, size - (offset + 1));
}

lh_ptr lh_memory_range_get_ptr(const lh_memory_range_t *self, lh_uoffset_t offset,
                               lh_bool_t is_back) {
    return is_back ? lh_memory_range_get_ptr_from_back(self, offset)
                   : lh_memory_range_get_ptr_from_front(self, offset);
}

lh_byte_t lh_memory_range_get_value_from_front(const lh_memory_range_t *self, lh_uoffset_t offset) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_range_get_ptr_from_front(self, offset)));
}

lh_byte_t lh_memory_range_get_value_from_back(const lh_memory_range_t *self, lh_uoffset_t offset) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_range_get_ptr_from_back(self, offset)));
}

lh_byte_t lh_memory_range_get_value(const lh_memory_range_t *self, lh_uoffset_t offset,
                                    lh_bool_t is_back) {
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_range_get_ptr(self, offset, is_back)));
}

lh_void lh_memory_range_set_value(lh_memory_range_t *self, lh_uoffset_t offset, lh_byte_t value,
                                  lh_bool_t is_back) {
    lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_range_get_ptr(self, offset, is_back))) = value;
}

lh_ptr lh_memory_range_get_front_ptr(const lh_memory_range_t *self) {
    return lh_memory_range_get_ptr(self, 0, lh_bool_false);
}

lh_byte_t lh_memory_range_get_front(const lh_memory_range_t *self) {
    return lh_memory_range_get_value(self, 0, lh_bool_false);
}

lh_ptr lh_memory_range_get_back_ptr(const lh_memory_range_t *self) {
    return lh_memory_range_get_ptr(self, 0, lh_bool_true);
}

lh_byte_t lh_memory_range_get_back(const lh_memory_range_t *self) {
    return lh_memory_range_get_value(self, 0, lh_bool_true);
}

lh_ptr lh_memory_range_next_ptr(const lh_memory_range_t *self, lh_ptr ptr) {
    if (!lh_memory_range_contains_ptr(self, ptr)) {
        return lh_cast_const(lh_ptr, lh_null);
    }

    lh_ptr next = lh_ptr_add_by_offset_unsafe(lh_void, ptr, 1);
    return lh_memory_range_contains_ptr(self, next) ? next : lh_cast_const(lh_ptr, lh_null);
}

lh_ptr lh_memory_range_prev_ptr(const lh_memory_range_t *self, lh_ptr ptr) {
    if (!lh_memory_range_contains_ptr(self, ptr)) {
        return lh_cast_const(lh_ptr, lh_null);
    }

    lh_ptr prev = lh_ptr_add_by_offset_unsafe(lh_void, ptr, -1);
    return lh_memory_range_contains_ptr(self, prev) ? prev : lh_cast_const(lh_ptr, lh_null);
}

lh_byte_t lh_memory_range_next_value(const lh_memory_range_t *self, lh_ptr ptr) {
    const lh_ptr next = lh_memory_range_next_ptr(self, ptr);
    lh_runtime_check(next, lh_runtime_error_code_null_pointer_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, next));
}

lh_byte_t lh_memory_range_prev_value(const lh_memory_range_t *self, lh_ptr ptr) {
    const lh_ptr prev = lh_memory_range_prev_ptr(self, ptr);
    lh_runtime_check(prev, lh_runtime_error_code_null_pointer_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, prev));
}

lh_bool_t lh_memory_range_contains_ptr(const lh_memory_range_t *self, const lh_ptr ptr) {
    lh_ptr begin;
    lh_ptr end;

    lh_memory_range_unpack_v(self, lh_addr_ref(begin), lh_addr_ref(end));
    return lh_interval_ropen_contains_value(begin, end, ptr);
}

lh_bool_t lh_memory_range_contains_range(const lh_memory_range_t *self, const lh_ptr begin,
                                         const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_interval_ropen_contains_range(self_begin, self_end, begin, end);
}

lh_bool_t lh_memory_range_contains(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_contains_range(self, other_begin, other_end);
}

lh_bool_t lh_memory_range_equals_range(const lh_memory_range_t *self, const lh_ptr begin,
                                       const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_ptr_eq(self_begin, begin) && lh_ptr_eq(self_end, end);
}

lh_bool_t lh_memory_range_equals(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_equals_range(self, other_begin, other_end);
}

lh_bool_t lh_memory_range_overlaps_range(const lh_memory_range_t *self, const lh_ptr begin,
                                         const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_interval_ropen_overlaps_range(self_begin, self_end, begin, end);
}

lh_bool_t lh_memory_range_overlaps(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_overlaps_range(self, other_begin, other_end);
}

lh_void lh_memory_range_assign_v(lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_runtime_check_if(lh_memory_range_is_invalid(other),
                        lh_runtime_error_code_invalid_memory_range);
    lh_memory_range_assign(self, other);
}

lh_void lh_memory_range_set_v(lh_memory_range_t *self, lh_ptr begin, lh_ptr end) {
    const lh_memory_range_t range = lh_memory_range_initializer(begin, end);
    lh_memory_range_assign_v(self, &range);
}

lh_void lh_memory_range_set_by_size_or_clear(lh_memory_range_t *self, lh_ptr begin,
                                             lh_usize_t size) {
    lh_runtime_try(e) {
        lh_memory_range_set_by_size(self, begin, size);
        lh_runtime_try_finalize();
    }
    lh_runtime_catch {
        lh_memory_range_clear(self);
    }
}

lh_ptr lh_memory_range_copy_range(lh_memory_range_t *self, const lh_ptr begin, const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_memory_raw_copy(self_begin, self_end, begin, end);
}

lh_ptr lh_memory_range_copy(lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_copy_range(self, other_begin, other_end);
}

lh_ptr lh_memory_range_move_range(lh_memory_range_t *self, const lh_ptr begin, const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_memory_raw_move(self_begin, self_end, begin, end);
}

lh_ptr lh_memory_range_move(lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_move_range(self, other_begin, other_end);
}

lh_ptr lh_memory_range_find_range(const lh_memory_range_t *self, const lh_ptr begin,
                                  const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_find(self_begin, self_end, begin, end));
}

lh_ptr lh_memory_range_find(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_find_range(self, other_begin, other_end);
}

lh_ptr lh_memory_range_rfind_range(const lh_memory_range_t *self, const lh_ptr begin,
                                   const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_rfind(self_begin, self_end, begin, end));
}

lh_ptr lh_memory_range_rfind(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_rfind_range(self, other_begin, other_end);
}

lh_ptr lh_memory_range_compare_range(const lh_memory_range_t *self, const lh_ptr begin,
                                     const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_compare(self_begin, self_end, begin, end));
}

lh_ptr lh_memory_range_compare(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_compare_range(self, other_begin, other_end);
}

lh_ptr lh_memory_range_rcompare_range(const lh_memory_range_t *self, const lh_ptr begin,
                                      const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_rcompare(self_begin, self_end, begin, end));
}

lh_ptr lh_memory_range_rcompare(const lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_rcompare_range(self, other_begin, other_end);
}

lh_ptr lh_memory_range_fill(lh_memory_range_t *self, lh_byte_t value) {
    lh_ptr begin;
    lh_ptr end;

    lh_memory_range_unpack_v(self, lh_addr_ref(begin), lh_addr_ref(end));
    return lh_memory_raw_set(begin, end, value);
}

lh_ptr lh_memory_range_fill_pattern_range(lh_memory_range_t *self, const lh_ptr begin,
                                          const lh_ptr end) {
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_range_unpack_v(self, lh_addr_ref(self_begin), lh_addr_ref(self_end));
    return lh_memory_raw_set_pattern(self_begin, self_end, begin, end);
}

lh_ptr lh_memory_range_fill_pattern(lh_memory_range_t *self, const lh_memory_range_t *other) {
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_range_unpack_v(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    return lh_memory_range_fill_pattern_range(self, other_begin, other_end);
}