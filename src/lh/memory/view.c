#include <lh/memory/range.h>
#include <lh/memory/view.h>
#include <lh/memory/view/initializer.h>
#include <lh/optional/ref.h>
#include <lh/runtime/check/ref.h>
#include <lh/runtime/try.h>
#include <lh/util/algorithm.h>
#include <lh/util/ptr.h>

void lh_memory_view_pack(lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end) {
    lh_runtime_check_ref(self);

    lh_optional_ref(begin) {
        self->first = lh_ptr_deref(begin);
    }

    lh_optional_ref(end) {
        self->second = lh_ptr_deref(end);
    }
}

void lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end) {
    lh_memory_view_pack(self, lh_addr_ref(begin), lh_addr_ref(end));
}

void lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end) {
    lh_memory_view_pack(self, lh_addr_ref(begin), lh_addr_ref(end));
}

void lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size) {
    lh_runtime_check(begin, lh_runtime_error_code_invalid_argument);
    const lh_ptr end = lh_ptr_add_by_offset(const lh_void, begin, size);
    lh_memory_view_init(self, begin, end);
}

void lh_memory_view_unpack(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end) {
    lh_runtime_check_ref(self);

    lh_optional_ref(begin) {
        lh_ptr_deref(begin) = self->first;
    }

    lh_optional_ref(end) {
        lh_ptr_deref(end) = self->second;
    }
}

const lh_ptr lh_memory_view_get_begin(const lh_memory_view_t *self) {
    return lh_memory_range_get_begin(lh_ptr_ccast(lh_memory_range_t, self));
}

const lh_ptr lh_memory_view_get_end(const lh_memory_view_t *self) {
    return lh_memory_range_get_end(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_void lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other) {
    const lh_ptr other_begin;
    const lh_ptr other_end;

    lh_memory_view_unpack(other, lh_addr_ref(other_begin), lh_addr_ref(other_end));
    lh_memory_view_init(self, other_begin, other_end);
}

lh_void lh_memory_view_clear(lh_memory_view_t *self) {
    const lh_memory_view_t other = lh_memory_view_empty_initializer();
    lh_memory_view_assign(self, &other);
}

lh_void lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other) {
    lh_runtime_check_ref(self);
    lh_runtime_check_ref(other);

    lh_algorithm_swap(lh_memory_view_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

lh_void lh_memory_view_exchange(lh_memory_view_t *self, lh_memory_view_t *other) {
    lh_memory_view_clear(self);
    lh_memory_view_swap(self, other);
}

lh_memory_view_t lh_memory_view_clone(const lh_memory_view_t *self) {
    lh_memory_view_t v;
    lh_memory_view_unpack(self, lh_addr_ref(v.first), lh_addr_ref(v.second));
    return v;
}

lh_void lh_memory_view_dup(const lh_memory_view_t *self, lh_memory_view_t *other) {
    const lh_memory_view_t v = lh_memory_view_clone(self);
    lh_memory_view_assign(other, lh_addr_ref(v));
}

lh_void lh_memory_view_dup_v(const lh_memory_view_t *self, lh_memory_view_t *other) {
    const lh_memory_view_t v = lh_memory_view_clone(self);
    lh_memory_view_assign_v(other, lh_addr_ref(v));
}

lh_memory_view_t lh_memory_view_clone_v(const lh_memory_view_t *self) {
    lh_memory_view_t v;
    lh_memory_view_dup_v(self, lh_addr_ref(v));
    return v;
}

void lh_memory_view_init_by_other(lh_memory_view_t *self, const lh_memory_view_t *other) {
    lh_memory_view_assign(self, other);
}

void lh_memory_view_init_by_empty(lh_memory_view_t *self) {
    lh_memory_view_t other = lh_memory_view_empty_initializer();
    lh_memory_view_assign(self, lh_addr_ref(other));
}

lh_memory_range_state_t lh_memory_view_get_state(const lh_memory_view_t *self) {
    return lh_memory_range_get_state(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_bool_t lh_memory_view_is_uninitialized(const lh_memory_view_t *self) {
    return lh_memory_range_is_uninitialized(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_bool_t lh_memory_view_has_data(const lh_memory_view_t *self) {
    return lh_memory_range_has_data(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_bool_t lh_memory_view_is_empty(const lh_memory_view_t *self) {
    return lh_memory_range_is_empty(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_bool_t lh_memory_view_is_valid(const lh_memory_view_t *self) {
    return lh_memory_range_is_valid(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_bool_t lh_memory_view_is_invalid(const lh_memory_view_t *self) {
    return lh_memory_range_is_invalid(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_memory_view_t lh_memory_view_make(const lh_ptr begin, const lh_ptr end) {
    lh_memory_view_t view = lh_memory_view_initializer(begin, end);
    return view;
}

lh_memory_view_t lh_memory_view_make_by_empty(lh_void) {
    lh_memory_view_t view = lh_memory_view_empty_initializer();
    return view;
}

lh_memory_view_t lh_memory_view_make_by_size(const lh_ptr begin, lh_usize_t size) {
    lh_memory_view_t view = lh_memory_view_empty_initializer();
    lh_memory_view_init_by_size(&view, begin, size);
    return view;
}

lh_memory_view_t lh_memory_view_make_v(const lh_ptr begin, const lh_ptr end) {
    lh_memory_view_t view = lh_memory_view_make(begin, end);
    lh_runtime_check_if(lh_memory_view_is_invalid(&view),
                        lh_runtime_error_code_invalid_memory_range);
    return view;
}

lh_memory_view_t lh_memory_view_make_or_empty(const lh_ptr begin, const lh_ptr end) {
    lh_runtime_try(e) {
        lh_memory_view_t view = lh_memory_view_make_v(begin, end);
        lh_runtime_try_return(view);
    }
    return lh_memory_view_make_by_empty();
}

lh_bool_t lh_memory_view_is_sliceable(const lh_memory_view_t *self, lh_uoffset_t offset,
                                      lh_uoffset_t size) {
    return lh_memory_range_is_sliceable(lh_ptr_ccast(lh_memory_range_t, self), offset, size);
}

lh_memory_view_t lh_memory_view_slice(const lh_memory_view_t *self, lh_uoffset_t offset,
                                      lh_uoffset_t size) {
    lh_runtime_check(lh_memory_view_is_sliceable(self, offset, size),
                     lh_runtime_error_code_out_of_range);

    const lh_ptr begin = lh_memory_view_get_ptr(self, offset, lh_bool_false);
    const lh_ptr end = lh_ptr_add_by_offset_unsafe(const lh_void, begin, size);
    return lh_memory_view_make_v(begin, end);
}

lh_memory_view_t lh_memory_view_slice_or_empty(const lh_memory_view_t *self, lh_uoffset_t offset,
                                               lh_uoffset_t size) {
    lh_runtime_try(e) {
        lh_memory_view_t view = lh_memory_view_slice(self, offset, size);
        lh_runtime_try_return(view);
    }
    return lh_memory_view_make_by_empty();
}

void lh_memory_view_unpack_v(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end) {
    lh_runtime_check_if(lh_memory_view_is_invalid(self),
                        lh_runtime_error_code_invalid_memory_range);
    lh_memory_view_unpack(self, begin, end);
}

lh_saddr_t lh_memory_view_diff(const lh_memory_view_t *self) {
    return lh_memory_range_diff(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_usize_t lh_memory_view_get_size(const lh_memory_view_t *self) {
    return lh_memory_range_get_size(lh_ptr_ccast(lh_memory_range_t, self));
}

lh_bool_t lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align) {
    return lh_memory_range_is_begin_aligned(lh_ptr_ccast(lh_memory_range_t, self), align);
}

lh_bool_t lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align) {
    return lh_memory_range_is_aligned(lh_ptr_ccast(lh_memory_range_t, self), align);
}

lh_bool_t lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t multiple) {
    return lh_memory_range_is_multiple_of(lh_ptr_ccast(lh_memory_range_t, self), multiple);
}

lh_bool_t lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset) {
    return lh_memory_range_is_valid_offset(lh_ptr_ccast(lh_memory_range_t, self), offset);
}

const lh_ptr lh_memory_view_get_ptr_from_front(const lh_memory_view_t *self, lh_uoffset_t offset) {
    return lh_memory_range_get_ptr_from_front(lh_ptr_ccast(lh_memory_range_t, self), offset);
}

const lh_ptr lh_memory_view_get_ptr_from_back(const lh_memory_view_t *self, lh_uoffset_t offset) {
    return lh_memory_range_get_ptr_from_back(lh_ptr_ccast(lh_memory_range_t, self), offset);
}

const lh_ptr lh_memory_view_get_ptr(const lh_memory_view_t *self, lh_uoffset_t offset,
                                    lh_bool_t from_back) {
    return lh_memory_range_get_ptr(lh_ptr_ccast(lh_memory_range_t, self), offset, from_back);
}

lh_byte_t lh_memory_view_get_value_from_front(const lh_memory_view_t *self, lh_uoffset_t offset) {
    return lh_memory_range_get_value_from_front(lh_ptr_ccast(lh_memory_range_t, self), offset);
}

lh_byte_t lh_memory_view_get_value_from_back(const lh_memory_view_t *self, lh_uoffset_t offset) {
    return lh_memory_range_get_value_from_back(lh_ptr_ccast(lh_memory_range_t, self), offset);
}

lh_byte_t lh_memory_view_get_value(const lh_memory_view_t *self, lh_uoffset_t offset,
                                   lh_bool_t from_back) {
    return lh_memory_range_get_value(lh_ptr_ccast(lh_memory_range_t, self), offset, from_back);
}

const lh_ptr lh_memory_view_get_front_ptr(const lh_memory_view_t *self) {
    return lh_memory_range_get_ptr(lh_ptr_ccast(lh_memory_range_t, self), 0, lh_bool_false);
}

lh_byte_t lh_memory_view_get_front_value(const lh_memory_view_t *self) {
    return lh_memory_range_get_value(lh_ptr_ccast(lh_memory_range_t, self), 0, lh_bool_false);
}

const lh_ptr lh_memory_view_get_back_ptr(const lh_memory_view_t *self) {
    return lh_memory_range_get_ptr(lh_ptr_ccast(lh_memory_range_t, self), 0, lh_bool_true);
}

lh_byte_t lh_memory_view_get_back_value(const lh_memory_view_t *self) {
    return lh_memory_range_get_value(lh_ptr_ccast(lh_memory_range_t, self), 0, lh_bool_true);
}

const lh_ptr lh_memory_view_next_ptr(const lh_memory_view_t *self, const lh_ptr ptr) {
    return lh_memory_range_next_ptr(lh_ptr_ccast(lh_memory_range_t, self),
                                    lh_cast_const(lh_ptr, ptr));
}

const lh_ptr lh_memory_view_prev_ptr(const lh_memory_view_t *self, const lh_ptr ptr) {
    return lh_memory_range_prev_ptr(lh_ptr_ccast(lh_memory_range_t, self),
                                    lh_cast_const(lh_ptr, ptr));
}

lh_byte_t lh_memory_view_next_value(const lh_memory_view_t *self, const lh_ptr ptr) {
    return lh_memory_range_next_value(lh_ptr_ccast(lh_memory_range_t, self),
                                      lh_cast_const(lh_ptr, ptr));
}

lh_byte_t lh_memory_view_prev_value(const lh_memory_view_t *self, const lh_ptr ptr) {
    return lh_memory_range_prev_value(lh_ptr_ccast(lh_memory_range_t, self),
                                      lh_cast_const(lh_ptr, ptr));
}

lh_bool_t lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr) {
    return lh_memory_range_contains_ptr(lh_ptr_ccast(lh_memory_range_t, self), ptr);
}

lh_bool_t lh_memory_view_contains_range(const lh_memory_view_t *self, const lh_ptr begin,
                                        const lh_ptr end) {
    return lh_memory_range_contains_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

lh_bool_t lh_memory_view_contains(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_contains(lh_ptr_ccast(lh_memory_range_t, self),
                                    lh_ptr_ccast(lh_memory_range_t, other));
}

lh_bool_t lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin,
                                      const lh_ptr end) {
    return lh_memory_range_equals_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

lh_bool_t lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_equals(lh_ptr_ccast(lh_memory_range_t, self),
                                  lh_ptr_ccast(lh_memory_range_t, other));
}

lh_bool_t lh_memory_view_overlaps_range(const lh_memory_view_t *self, const lh_ptr begin,
                                        const lh_ptr end) {
    return lh_memory_range_overlaps_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

lh_bool_t lh_memory_view_overlaps(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_overlaps(lh_ptr_ccast(lh_memory_range_t, self),
                                    lh_ptr_ccast(lh_memory_range_t, other));
}

lh_void lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other) {
    lh_runtime_check_if(lh_memory_view_is_invalid(other),
                        lh_runtime_error_code_invalid_memory_range);
    lh_memory_view_assign(self, other);
}

void lh_memory_view_init_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end) {
    const lh_memory_view_t view = lh_memory_view_make(begin, end);
    lh_memory_view_assign_v(self, &view);
}

lh_void lh_memory_view_init_by_size_or_clear(lh_memory_view_t *self, const lh_ptr begin,
                                             lh_usize_t size) {
    lh_runtime_try(e) {
        lh_memory_view_init_by_size(self, begin, size);
        lh_runtime_try_finalize();
    }
    lh_runtime_catch {
        lh_memory_view_clear(self);
    }
}

const lh_ptr lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin,
                                       const lh_ptr end) {
    return lh_memory_range_find_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

const lh_ptr lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_find(lh_ptr_ccast(lh_memory_range_t, self),
                                lh_ptr_ccast(lh_memory_range_t, other));
}

const lh_ptr lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin,
                                        const lh_ptr end) {
    return lh_memory_range_rfind_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

const lh_ptr lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_rfind(lh_ptr_ccast(lh_memory_range_t, self),
                                 lh_ptr_ccast(lh_memory_range_t, other));
}

const lh_ptr lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin,
                                          const lh_ptr end) {
    return lh_memory_range_compare_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

const lh_ptr lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_compare(lh_ptr_ccast(lh_memory_range_t, self),
                                   lh_ptr_ccast(lh_memory_range_t, other));
}

const lh_ptr lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin,
                                           const lh_ptr end) {
    return lh_memory_range_rcompare_range(lh_ptr_ccast(lh_memory_range_t, self), begin, end);
}

const lh_ptr lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other) {
    return lh_memory_range_rcompare(lh_ptr_ccast(lh_memory_range_t, self),
                                    lh_ptr_ccast(lh_memory_range_t, other));
}
