#include <lh/null.h>
#include <lh/str/view.h>
#include <lh/util/str/raw.h>

lh_void
lh_str_view_pack(lh_str_view_t *self, const lh_str_ptr *begin, const lh_str_ptr *end)
{
    lh_memory_view_pack(self, lh_ptr_ccast(lh_ptr, begin), lh_ptr_ccast(lh_ptr, end));
}

lh_void
lh_str_view_unpack(const lh_str_view_t *self, const lh_str_ptr *begin, const lh_str_ptr *end)
{
    lh_memory_view_unpack_v(self, lh_ptr_ccast(lh_ptr, begin), lh_ptr_ccast(lh_ptr, end));
}

lh_void
lh_str_view_unpack_v(const lh_str_view_t *self, const lh_str_ptr *begin, const lh_str_ptr *end)
{
    lh_memory_view_unpack_v(self, lh_ptr_ccast(lh_ptr, begin), lh_ptr_ccast(lh_ptr, end));
}

lh_void
lh_str_view_init(lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    lh_memory_view_init(self, begin, end);
}

lh_void
lh_str_view_init_by_size(lh_str_view_t *self, const lh_str_ptr begin, lh_usize_t size)
{
    lh_memory_view_init_by_size(self, begin, size);
}

lh_void
lh_str_view_init_by_empty(lh_str_view_t *self)
{
    lh_str_view_init(self, lh_null, lh_null);
}

lh_void
lh_str_view_init_by_other(lh_str_view_t *self, const lh_str_view_t *other)
{
    lh_memory_view_init_by_other(self, other);
}

const lh_ptr
lh_str_view_get_begin(const lh_str_view_t *self)
{
    return lh_memory_view_get_begin(self);
}

const lh_ptr
lh_str_view_get_end(const lh_str_view_t *self)
{
    return lh_memory_view_get_end(self);
}

lh_void
lh_str_view_assign(lh_str_view_t *self, const lh_str_view_t *other)
{
    lh_memory_view_assign_v(self, other);
}

lh_void
lh_str_view_clear(lh_str_view_t *self)
{
    lh_memory_view_clear(self);
}

lh_void
lh_str_view_swap(lh_str_view_t *self, lh_str_view_t *other)
{
    lh_memory_view_swap(self, other);
}

lh_void
lh_str_view_exchange(lh_str_view_t *self, lh_str_view_t *other)
{
    lh_memory_view_exchange(self, other);
}

lh_str_view_t
lh_str_view_clone(const lh_str_view_t *self)
{
    return lh_memory_view_clone(self);
}

lh_void
lh_str_view_dup(const lh_str_view_t *self, lh_str_view_t *other)
{
    lh_memory_view_dup(self, other);
}

lh_void
lh_str_view_dup_v(const lh_str_view_t *self, lh_str_view_t *other)
{
    lh_memory_view_dup_v(self, other);
}

lh_str_view_t
lh_str_view_clone_v(const lh_str_view_t *self)
{
    return lh_memory_view_clone_v(self);
}

lh_memory_bounds_state_t
lh_str_view_get_state(const lh_str_view_t *self)
{
    return lh_memory_view_get_state(self);
}

lh_bool_t
lh_str_view_is_uninitialized(const lh_str_view_t *self)
{
    return lh_memory_view_is_uninitialized(self);
}

lh_bool_t
lh_str_view_has_data(const lh_str_view_t *self)
{
    return lh_memory_view_has_data(self);
}

lh_bool_t
lh_str_view_is_empty(const lh_str_view_t *self)
{
    return lh_memory_view_is_empty(self);
}

lh_bool_t
lh_str_view_is_valid(const lh_str_view_t *self)
{
    return lh_memory_view_is_valid(self);
}

lh_bool_t
lh_str_view_is_invalid(const lh_str_view_t *self)
{
    return lh_memory_view_is_invalid(self);
}

lh_str_view_t
lh_str_make(const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_make_v(begin, end);
}

lh_str_view_t
lh_str_make_by_empty(lh_void)
{
    return lh_memory_view_make_by_empty();
}

lh_str_view_t
lh_str_make_by_size(const lh_str_ptr begin, lh_usize_t size)
{
    return lh_memory_view_make_by_size(begin, size);
}

lh_str_view_t
lh_str_make_or_empty(const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_make_or_empty(begin, end);
}

lh_bool_t
lh_str_is_sliceable(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size)
{
    return lh_memory_view_is_sliceable(self, offset, size);
}

lh_str_view_t
lh_str_slice(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size)
{
    return lh_memory_view_slice(self, offset, size);
}

lh_str_view_t
lh_str_slice_or_empty(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size)
{
    return lh_memory_view_slice_or_empty(self, offset, size);
}

lh_usize_t
lh_str_get_size_bytes(const lh_str_view_t *self)
{
    return lh_memory_view_get_size(self);
}

lh_usize_t
lh_str_get_size(const lh_str_view_t *self)
{
    return lh_str_get_size_bytes(self) / sizeof(lh_char_t);
}

lh_saddr_t
lh_str_view_diff(const lh_str_view_t *self)
{
    return lh_memory_view_diff(self);
}

lh_bool_t
lh_str_view_is_begin_aligned(const lh_str_view_t *self, lh_usize_t align)
{
    return lh_memory_view_is_begin_aligned(self, align);
}

lh_bool_t
lh_str_view_is_aligned(const lh_str_view_t *self, lh_usize_t align)
{
    return lh_memory_view_is_aligned(self, align);
}

lh_bool_t
lh_str_view_is_multiple_of(const lh_str_view_t *self, lh_usize_t multiple)
{
    return lh_memory_view_is_multiple_of(self, multiple);
}

lh_bool_t
lh_str_view_is_valid_offset(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_memory_view_is_valid_offset(self, offset);
}

const lh_str_ptr
lh_str_get_ptr(const lh_str_view_t *self, lh_uoffset_t offset, lh_bool_t from_back)
{
    return lh_memory_view_get_ptr(self, offset, from_back);
}

const lh_str_ptr
lh_str_get_ptr_from_front(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_memory_view_get_ptr_from_front(self, offset);
}

const lh_str_ptr
lh_str_get_ptr_from_back(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_memory_view_get_ptr_from_back(self, offset);
}

lh_char_t
lh_str_get_value(const lh_str_view_t *self, lh_uoffset_t offset, lh_bool_t from_back)
{
    return lh_type_cast(lh_char_t, lh_memory_view_get_value(self, offset, from_back));
}

lh_char_t
lh_str_get_value_from_front(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_type_cast(lh_char_t, lh_memory_view_get_value_from_front(self, offset));
}

lh_char_t
lh_str_get_value_from_back(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_type_cast(lh_char_t, lh_memory_view_get_value_from_back(self, offset));
}

const lh_str_ptr
lh_str_get_front_ptr(const lh_str_view_t *self)
{
    return lh_memory_view_get_front_ptr(self);
}

const lh_str_ptr
lh_str_get_back_ptr(const lh_str_view_t *self)
{
    return lh_memory_view_get_back_ptr(self);
}

lh_char_t
lh_str_get_front_value(const lh_str_view_t *self)
{
    return lh_type_cast(lh_char_t, lh_memory_view_get_front_value(self));
}

lh_char_t
lh_str_get_back_value(const lh_str_view_t *self)
{
    return lh_type_cast(lh_char_t, lh_memory_view_get_back_value(self));
}

const lh_str_ptr
lh_str_next_ptr(const lh_str_view_t *self, const lh_str_ptr ptr)
{
    return lh_type_cast(const lh_str_ptr, lh_memory_view_next_ptr(self, ptr));
}

const lh_str_ptr
lh_str_prev_ptr(const lh_str_view_t *self, const lh_str_ptr ptr)
{
    return lh_type_cast(const lh_str_ptr, lh_memory_view_prev_ptr(self, ptr));
}

lh_char_t
lh_str_next_value(const lh_str_view_t *self, const lh_str_ptr ptr)
{
    return lh_type_cast(lh_char_t, lh_memory_view_next_value(self, ptr));
}

lh_char_t
lh_str_prev_value(const lh_str_view_t *self, const lh_str_ptr ptr)
{
    return lh_type_cast(lh_char_t, lh_memory_view_prev_value(self, ptr));
}

lh_bool_t
lh_str_contains_ptr(const lh_str_view_t *self, const lh_str_ptr ptr)
{
    return lh_memory_view_contains_ptr(self, ptr);
}

lh_bool_t
lh_str_contains_range(const lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_contains_range(self, begin, end);
}

lh_bool_t
lh_str_contains(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_contains(self, other);
}

lh_bool_t
lh_str_equals_range(const lh_str_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_equals_range(self, begin, end);
}

lh_bool_t
lh_str_equals(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_equals(self, other);
}

lh_bool_t
lh_str_overlaps_range(const lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_overlaps_range(self, begin, end);
}

lh_bool_t
lh_str_overlaps(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_overlaps(self, other);
}

lh_void
lh_str_view_set(lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    lh_memory_view_set(self, begin, end);
}

const lh_str_ptr
lh_str_find_range(const lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_find_range(self, begin, end);
}

const lh_str_ptr
lh_str_find(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_find(self, other);
}

const lh_str_ptr
lh_str_rfind_range(const lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_rfind_range(self, begin, end);
}

const lh_str_ptr
lh_str_rfind(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_rfind(self, other);
}

const lh_str_ptr
lh_str_compare_range(const lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_compare_range(self, begin, end);
}

const lh_str_ptr
lh_str_compare(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_compare(self, other);
}

const lh_str_ptr
lh_str_rcompare_range(const lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end)
{
    return lh_memory_view_rcompare_range(self, begin, end);
}

const lh_str_ptr
lh_str_rcompare(const lh_str_view_t *self, const lh_str_view_t *other)
{
    return lh_memory_view_rcompare(self, other);
}

const lh_ptr
lh_memory_view_find_char(const lh_memory_view_t *self, lh_char_t ch)
{
    const lh_str_ptr str = lh_str_view_get_begin(self);
    lh_usize_t size = lh_str_get_size(self);
    return lh_str_raw_find_of_char(str, size, ch);
}