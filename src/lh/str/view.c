#include <lh/str/view.h>
#include <lh/util/ptr.h>
#include <lh/cast/static.h>
#include <lh/util/str/raw.h>

lh_void
lh_str_init_by_size(lh_str_view_t *self, lh_str_cptr data, lh_usize_t size)
{
    lh_memory_view_init_by_size(self, data, size);
}

lh_void
lh_str_view_init(lh_str_view_t *self, lh_str_cptr data)
{
    lh_str_init_by_size(self, data, lh_str_raw_len(data));
}

lh_void
lh_str_view_init_empty(lh_str_view_t *self)
{
    lh_memory_view_init_empty(self);
}

lh_void
lh_str_view_init_by_other(lh_str_view_t *self, const lh_str_view_t *other)
{
    lh_memory_view_init_by_other(self, other);
}

lh_str_cptr 
lh_str_view_get_begin(const lh_str_view_t *self)
{
    return lh_ptr_cast(lh_char_t, lh_memory_view_get_begin_v(self));
}

lh_str_cptr 
lh_str_view_get_end(const lh_str_view_t *self)
{
    return lh_ptr_cast(lh_char_t, lh_memory_view_get_end_v(self));
}

lh_str_cptr 
lh_str_view_get_data(const lh_str_view_t *self)
{
    return lh_ptr_cast(lh_char_t, lh_memory_view_get_data(self));
}

lh_usize_t
lh_str_view_get_size(const lh_str_view_t *self)
{
    return lh_memory_view_get_size(self);
}

lh_usize_t
lh_str_view_find_char(const lh_str_view_t *self, lh_char_t ch)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_find_of_char(data, lh_str_view_get_size(self), ch);
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_usize_t
lh_str_view_rfind_char(const lh_str_view_t *self, lh_char_t ch)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_rfind_of_char(data, lh_str_view_get_size(self), ch);
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_bool_t
lh_str_view_contains_char(const lh_str_view_t *self, lh_char_t ch)
{
    return lh_str_raw_contains_char(lh_str_view_get_data(self), lh_str_view_get_size(self), ch);
}

lh_usize_t
lh_str_view_find(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case)
{
    lh_usize_t result = lh_str_raw_index_of_by_size(
        lh_str_view_get_data(self), lh_str_view_get_size(self), lh_str_view_get_data(other),
        lh_str_view_get_size(other), ignore_case);

    return result == LH_STR_RAW_INVALID ? LH_STR_VIEW_INVALID : result;
}

lh_usize_t
lh_str_view_rfind(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr =
        lh_str_raw_rfind(data, lh_str_view_get_size(self), lh_str_view_get_data(other),
                         lh_str_view_get_size(other), ignore_case);
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_bool_t
lh_str_view_contains(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case)
{
    return lh_str_view_find(self, other, ignore_case) != LH_STR_VIEW_INVALID;
}

lh_usize_t
lh_str_view_find_of(const lh_str_view_t *self, const lh_str_view_t *chars)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_find_of_chars(
        data, lh_str_view_get_size(self),
        lh_str_view_get_data(chars), lh_str_view_get_size(chars));
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_usize_t
lh_str_view_rfind_of(const lh_str_view_t *self, const lh_str_view_t *chars)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_rfind_of_chars(
        data, lh_str_view_get_size(self),
        lh_str_view_get_data(chars), lh_str_view_get_size(chars));
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_usize_t
lh_str_view_find_not_of(const lh_str_view_t *self, const lh_str_view_t *chars)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_find_not_of_chars(
        data, lh_str_view_get_size(self),
        lh_str_view_get_data(chars), lh_str_view_get_size(chars));
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_usize_t
lh_str_view_rfind_not_of(const lh_str_view_t *self, const lh_str_view_t *chars)
{
    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_rfind_not_of_chars(
        data, lh_str_view_get_size(self),
        lh_str_view_get_data(chars), lh_str_view_get_size(chars));
    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, data) : LH_STR_VIEW_INVALID;
}

lh_str_cptr
lh_str_view_compare(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case)
{
    return lh_str_raw_compare(lh_str_view_get_data(self), lh_str_view_get_size(self),
                              lh_str_view_get_data(other), lh_str_view_get_size(other),
                              ignore_case);
}

lh_str_cptr
lh_str_view_rcompare(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case)
{
    return lh_str_raw_rcompare(lh_str_view_get_data(self), lh_str_view_get_size(self),
                               lh_str_view_get_data(other), lh_str_view_get_size(other),
                               ignore_case);
}

lh_bool_t
lh_str_view_equals(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case)
{
    lh_usize_t size = lh_str_view_get_size(self);
    lh_usize_t other_size = lh_str_view_get_size(other);

    if (size != other_size)
    {
        return lh_bool_false;
    }

    return !lh_ptr_is_set(lh_str_raw_compare(lh_str_view_get_data(self), size,
                                             lh_str_view_get_data(other), other_size, ignore_case));
}

lh_bool_t
lh_str_view_starts_with(const lh_str_view_t *self, const lh_str_view_t *other,
                        lh_bool_t ignore_case)
{
    return lh_str_raw_starts_with_by_size(lh_str_view_get_data(self), lh_str_view_get_size(self),
                                         lh_str_view_get_data(other), lh_str_view_get_size(other),
                                         ignore_case);
}

lh_bool_t
lh_str_view_ends_with(const lh_str_view_t *self, const lh_str_view_t *other,
                      lh_bool_t ignore_case)
{
    return lh_str_raw_ends_with_by_size(lh_str_view_get_data(self), lh_str_view_get_size(self),
                                       lh_str_view_get_data(other), lh_str_view_get_size(other),
                                       ignore_case);
}

lh_usize_t
lh_str_view_find_null_terminator(const lh_str_view_t *self)
{
    lh_usize_t size = lh_str_view_get_size(self);

    lh_str_cptr data = lh_str_view_get_data(self);
    lh_str_cptr ptr = lh_str_raw_find_of_null_terminator_by_size(data, size);

    return lh_ptr_is_set(ptr) ? lh_ptr_udiff(ptr, lh_str_view_get_data(self)) : LH_STR_VIEW_INVALID;
}

lh_bool_t
lh_str_view_is_null_terminated(const lh_str_view_t *self)
{
    return lh_str_view_find_null_terminator(self) != LH_STR_VIEW_INVALID;
}

lh_char_t
lh_str_view_get_first_char(const lh_str_view_t *self)
{
    return lh_cast_static(lh_char_t, lh_memory_view_get_first_value(self));
}

lh_char_t
lh_str_view_get_last_char(const lh_str_view_t *self)
{
    return lh_cast_static(lh_char_t, lh_memory_view_get_last_value(self));
}

lh_char_t
lh_str_view_get_char_from_begin(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_cast_static(lh_char_t, lh_memory_view_get_value_from_begin(self, offset));
}

lh_char_t
lh_str_view_get_char_from_end(const lh_str_view_t *self, lh_uoffset_t offset)
{
    return lh_cast_static(lh_char_t, lh_memory_view_get_value_from_end(self, offset));
}

lh_char_t
lh_str_view_get_char_by_offset(const lh_str_view_t *self, lh_soffset_t offset)
{
    return lh_cast_static(lh_char_t, lh_memory_view_get_value_by_offset(self, offset));
}

lh_bool_t
lh_str_view_is_empty(const lh_str_view_t *self)
{
    return lh_memory_view_is_empty(self);
}

lh_void
lh_str_view_clear(lh_str_view_t *self)
{
    lh_memory_view_clear(self);
}

lh_void
lh_str_view_assign(lh_str_view_t *self, const lh_str_view_t *other)
{
    lh_memory_view_assign_v(self, other);
}

lh_void
lh_str_view_swap(lh_str_view_t *self, lh_str_view_t *other)
{
    lh_memory_view_swap(self, other);
}

lh_void
lh_str_view_swap_and_clear(lh_str_view_t *self, lh_str_view_t *other)
{
    lh_memory_view_swap_and_clear(self, other);
}
