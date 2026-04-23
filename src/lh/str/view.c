#include <lh/null.h>
#include <lh/str/view.h>
#include <lh/util/str/raw.h>

lh_void lh_str_view_pack(lh_str_view_t *self, const lh_str_ptr *begin, const lh_str_ptr *end) {
    lh_memory_view_pack(self, lh_ptr_ccast(lh_ptr, begin), lh_ptr_ccast(lh_ptr, end));
}

lh_void lh_str_view_unpack(const lh_str_view_t *self, const lh_str_ptr *begin,
                           const lh_str_ptr *end) {
    lh_memory_view_unpack_v(self, lh_ptr_ccast(lh_ptr, begin), lh_ptr_ccast(lh_ptr, end));
}

lh_void lh_str_view_init(lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end) {
    lh_memory_view_init(self, begin, end);
}

lh_void lh_str_view_init_by_size(lh_str_view_t *self, const lh_str_ptr begin, lh_usize_t size) {
    lh_memory_view_init_by_size(self, begin, size);
}

lh_void lh_str_view_init_by_empty(lh_str_view_t *self) {
    lh_str_view_init(self, lh_null, lh_null);
}

lh_void lh_str_view_init_by_other(lh_str_view_t *self, const lh_str_view_t *other) {
    lh_memory_view_init_by_other(self, other);
}

const lh_ptr lh_str_view_get_begin(const lh_str_view_t *self) {
    return lh_memory_view_get_begin(self);
}

const lh_ptr lh_str_view_get_end(const lh_str_view_t *self) {
    return lh_memory_view_get_end(self);
}

lh_void lh_str_view_assign(lh_str_view_t *self, const lh_str_view_t *other) {
    lh_memory_view_assign_v(self, other);
}

lh_void lh_str_view_clear(lh_str_view_t *self) {
    lh_memory_view_clear(self);
}

lh_void lh_str_view_swap(lh_str_view_t *self, lh_str_view_t *other) {
    lh_memory_view_swap(self, other);
}

lh_void lh_str_view_exchange(lh_str_view_t *self, lh_str_view_t *other) {
    lh_memory_view_exchange(self, other);
}

lh_bool_t lh_str_view_is_empty(const lh_str_view_t *self) {
    return lh_memory_view_is_empty(self);
}

lh_str_view_t lh_str_make(const lh_str_ptr begin, const lh_str_ptr end) {
    return lh_memory_view_make_v(begin, end);
}

lh_str_view_t lh_str_make_by_size(const lh_str_ptr begin, lh_usize_t size) {
    return lh_memory_view_make_by_size(begin, size);
}

lh_bool_t lh_str_is_sliceable(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size) {
    return lh_memory_view_is_sliceable(self, offset, size);
}

lh_str_view_t lh_str_slice(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size) {
    return lh_memory_view_slice(self, offset, size);
}

lh_usize_t lh_str_get_size_bytes(const lh_str_view_t *self) {
    return lh_memory_view_get_size(self);
}

lh_usize_t lh_str_get_size(const lh_str_view_t *self) {
    return lh_str_get_size_bytes(self) / sizeof(lh_char_t);
}

const lh_str_ptr lh_str_get_ptr(const lh_str_view_t *self, lh_uoffset_t offset, lh_bool_t is_back) {
    return lh_memory_view_get_ptr(self, offset, is_back);
}

lh_char_t lh_str_get_value(const lh_str_view_t *self, lh_uoffset_t offset, lh_bool_t is_back) {
    return lh_type_cast(lh_char_t, lh_memory_view_get_value(self, offset, is_back));
}

const lh_str_ptr lh_str_get_front_ptr(const lh_str_view_t *self) {
    return lh_memory_view_get_front_ptr(self);
}

const lh_str_ptr lh_str_get_back_ptr(const lh_str_view_t *self) {
    return lh_memory_view_get_back_ptr(self);
}

lh_char_t lh_str_get_front(const lh_str_view_t *self) {
    return lh_type_cast(lh_char_t, lh_memory_view_get_front(self));
}

lh_char_t lh_str_get_back(const lh_str_view_t *self) {
    return lh_type_cast(lh_char_t, lh_memory_view_get_back(self));
}

const lh_str_ptr lh_str_next_ptr(const lh_str_view_t *self, const lh_str_ptr ptr) {
    return lh_type_cast(const lh_str_ptr, lh_memory_view_next_ptr(self, ptr));
}

const lh_str_ptr lh_str_prev_ptr(const lh_str_view_t *self, const lh_str_ptr ptr) {
    return lh_type_cast(const lh_str_ptr, lh_memory_view_prev_ptr(self, ptr));
}

lh_char_t lh_str_next_value(const lh_str_view_t *self, const lh_str_ptr ptr) {
    return lh_type_cast(lh_char_t, lh_memory_view_next_value(self, ptr));
}

lh_char_t lh_str_prev_value(const lh_str_view_t *self, const lh_str_ptr ptr) {
    return lh_type_cast(lh_char_t, lh_memory_view_prev_value(self, ptr));
}

lh_bool_t lh_str_contains_ptr(const lh_str_view_t *self, const lh_str_ptr ptr) {
    return lh_memory_view_contains_ptr(self, ptr);
}

lh_bool_t lh_str_contains_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                const lh_str_ptr end) {
    return lh_memory_view_contains_range(self, begin, end);
}

lh_bool_t lh_str_contains(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_contains(self, other);
}

lh_bool_t lh_str_equals_range(const lh_str_view_t *self, const lh_ptr begin, const lh_ptr end) {
    return lh_memory_view_equals_range(self, begin, end);
}

lh_bool_t lh_str_equals(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_equals(self, other);
}

lh_bool_t lh_str_overlaps_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                const lh_str_ptr end) {
    return lh_memory_view_overlaps_range(self, begin, end);
}

lh_bool_t lh_str_overlaps(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_overlaps(self, other);
}

lh_void lh_str_view_set(lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end) {
    lh_memory_view_set(self, begin, end);
}

const lh_str_ptr lh_str_find_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                   const lh_str_ptr end) {
    return lh_memory_view_find_range(self, begin, end);
}

const lh_str_ptr lh_str_find(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_find(self, other);
}

const lh_str_ptr lh_str_rfind_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                    const lh_str_ptr end) {
    return lh_memory_view_rfind_range(self, begin, end);
}

const lh_str_ptr lh_str_rfind(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_rfind(self, other);
}

const lh_str_ptr lh_str_compare_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                      const lh_str_ptr end) {
    return lh_memory_view_compare_range(self, begin, end);
}

const lh_str_ptr lh_str_compare(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_compare(self, other);
}

const lh_str_ptr lh_str_rcompare_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                       const lh_str_ptr end) {
    return lh_memory_view_rcompare_range(self, begin, end);
}

const lh_str_ptr lh_str_rcompare(const lh_str_view_t *self, const lh_str_view_t *other) {
    return lh_memory_view_rcompare(self, other);
}

const lh_ptr lh_memory_view_find_char(const lh_memory_view_t *self, lh_char_t ch) {
    const lh_str_ptr str = lh_str_view_get_begin(self);
    lh_usize_t size = lh_str_get_size(self);
    return lh_str_raw_find_of_char(str, size, ch);
}