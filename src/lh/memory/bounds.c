#include <lh/memory/bounds.h>
#include <lh/util/addr.h>
#include <lh/attribute/static.h>
#include <lh/memory/bounds/initializer.h>
#include <lh/memory/view.h>
#include <lh/memory/raw.h>
#include <lh/cast/const.h>
#include <lh/assert.h>

lh_ptr
lh_memory_bounds_get_begin(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_begin(s));
}

lh_ptr
lh_memory_bounds_get_data(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_data(s));
}

lh_ptr
lh_memory_bounds_get_end(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_end(s));
}

lh_memory_view_slice_flags_t
lh_memory_bounds_get_flags(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_flags(s);
}

lh_bool_t
lh_memory_bounds_is_uninitialized(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_uninitialized(s);
}

lh_bool_t
lh_memory_bounds_is_initialized(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_initialized(s);
}

lh_memory_view_slice_direction_t
lh_memory_bounds_get_direction(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_direction(s);
}

lh_bool_t
lh_memory_bounds_is_forward(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_forward(s);
}

lh_bool_t
lh_memory_bounds_is_backward(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_backward(s);
}

lh_bool_t
lh_memory_bounds_is_valid(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_valid(s);
}

lh_ptr
lh_memory_bounds_get_begin_v(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_begin_v(s));
}

lh_ptr
lh_memory_bounds_get_end_v(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_end_v(s));
}

lh_usize_t
lh_memory_bounds_get_size(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_size(s);
}

lh_bool_t
lh_memory_bounds_is_empty(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_empty(s);
}

lh_bool_t
lh_memory_bounds_is_valid_offset(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_valid_offset(s, offset);
}

lh_uoffset_t
lh_memory_bounds_get_offset_from_begin(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_offset_from_begin(s, ptr);
}

lh_uoffset_t
lh_memory_bounds_get_offset_from_end(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_offset_from_end(s, ptr);
}

lh_bool_t
lh_memory_bounds_contains_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_contains_ptr(s, ptr);
}

lh_bool_t
lh_memory_bounds_contains_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_contains_of(s, begin, end);
}

lh_bool_t
lh_memory_bounds_contains_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_contains_range(s, begin, end);
}

lh_bool_t
lh_memory_bounds_contains(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_memory_view_contains(s, o);
}

lh_bool_t
lh_memory_bounds_contains_v(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_memory_view_contains_v(s, o);
}

lh_ptr
lh_memory_bounds_get_ptr_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_ptr_from_begin(s, offset));
}

lh_ptr
lh_memory_bounds_get_ptr_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_ptr_from_end(s, offset));
}

lh_ptr
lh_memory_bounds_get_first_ptr(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_first_ptr(s));
}

lh_ptr
lh_memory_bounds_get_last_ptr(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_last_ptr(s));
}

lh_ptr
lh_memory_bounds_get_ptr(const lh_memory_bounds_t *self, lh_soffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_get_ptr(s, offset));
}

lh_byte_t
lh_memory_bounds_get_value_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_value_from_begin(s, offset);
}

lh_byte_t
lh_memory_bounds_get_value_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_value_from_end(s, offset);
}

lh_byte_t
lh_memory_bounds_get_value(const lh_memory_bounds_t *self, lh_soffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_value(s, offset);
}

lh_byte_t
lh_memory_bounds_get_first_value(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_first_value(s);
}

lh_byte_t
lh_memory_bounds_get_last_value(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_last_value(s);
}

lh_uoffset_t
lh_memory_bounds_get_offset_from_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr,
                                     lh_soffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_get_offset_from_ptr(s, ptr, offset);
}

const lh_ptr
lh_memory_bounds_seek_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr, lh_soffset_t offset)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_seek_ptr(s, ptr, offset);
}

const lh_ptr
lh_memory_bounds_next_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_next_ptr(s, ptr);
}

const lh_ptr
lh_memory_bounds_prev_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_prev_ptr(s, ptr);
}

lh_byte_t
lh_memory_bounds_seek_value(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_seek_value(s, ptr);
}

lh_byte_t
lh_memory_bounds_next_value(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_next_value(s, ptr);
}

lh_byte_t
lh_memory_bounds_prev_value(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_prev_value(s, ptr);
}

lh_bool_t
lh_memory_bounds_overlaps_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_overlaps_of(s, begin, end);
}

lh_bool_t
lh_memory_bounds_overlaps_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_overlaps_range(s, begin, end);
}

lh_bool_t
lh_memory_bounds_overlaps(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_memory_view_overlaps(s, o);
}

lh_bool_t
lh_memory_bounds_overlaps_v(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_memory_view_overlaps_v(s, o);
}

lh_bool_t
lh_memory_bounds_is_multiple_of(const lh_memory_bounds_t *self, lh_usize_t alignment)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_multiple_of(s, alignment);
}

lh_bool_t
lh_memory_bounds_is_begin_aligned(const lh_memory_bounds_t *self, lh_usize_t align)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_begin_aligned(s, align);
}

lh_bool_t
lh_memory_bounds_is_aligned(const lh_memory_bounds_t *self, lh_usize_t align)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_is_aligned(s, align);
}

lh_bool_t
lh_memory_bounds_equals_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_equals_of(s, begin, end);
}

lh_bool_t
lh_memory_bounds_equals_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_memory_view_equals_range(s, begin, end);
}

lh_bool_t
lh_memory_bounds_equals(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_memory_view_equals(s, o);
}

lh_ptr
lh_memory_bounds_copy_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    self_begin = lh_memory_bounds_get_begin_v(self);

    self_end = lh_memory_bounds_get_end_v(self);
    return lh_memory_raw_copy(self_begin, self_end, begin, end);
}

lh_ptr
lh_memory_bounds_copy(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    other_begin = lh_memory_bounds_get_begin_v(other);

    other_end = lh_memory_bounds_get_end_v(other);
    return lh_memory_bounds_copy_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_move_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    self_begin = lh_memory_bounds_get_begin_v(self);

    self_end = lh_memory_bounds_get_end_v(self);
    return lh_memory_raw_move(self_begin, self_end, begin, end);
}

lh_ptr
lh_memory_bounds_move(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    other_begin = lh_memory_bounds_get_begin_v(other);

    other_end = lh_memory_bounds_get_end_v(other);
    return lh_memory_bounds_move_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_find_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_find_range(s, begin, end));
}

lh_ptr
lh_memory_bounds_find(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_cast_const(lh_ptr, lh_memory_view_find(s, o));
}

lh_ptr
lh_memory_bounds_rfind_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_rfind_range(s, begin, end));
}

lh_ptr
lh_memory_bounds_rfind(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_cast_const(lh_ptr, lh_memory_view_rfind(s, o));
}

lh_ptr
lh_memory_bounds_compare_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_compare_range(s, begin, end));
}

lh_ptr
lh_memory_bounds_compare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_cast_const(lh_ptr, lh_memory_view_compare(s, o));
}

lh_ptr
lh_memory_bounds_rcompare_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    return lh_cast_const(lh_ptr, lh_memory_view_rcompare_range(s, begin, end));
}

lh_ptr
lh_memory_bounds_rcompare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    return lh_cast_const(lh_ptr, lh_memory_view_rcompare(s, o));
}

lh_ptr
lh_memory_bounds_fill(lh_memory_bounds_t *self, lh_byte_t value)
{
    lh_ptr begin;
    lh_ptr end;

    begin = lh_memory_bounds_get_begin_v(self);

    end = lh_memory_bounds_get_end_v(self);
    return lh_memory_raw_set(begin, end, value);
}

lh_ptr
lh_memory_bounds_fill_pattern_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    self_begin = lh_memory_bounds_get_begin_v(self);

    self_end = lh_memory_bounds_get_end_v(self);
    return lh_memory_raw_set_pattern(self_begin, self_end, begin, end);
}

lh_ptr
lh_memory_bounds_fill_pattern(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    other_begin = lh_memory_bounds_get_begin_v(other);

    other_end = lh_memory_bounds_get_end_v(other);
    return lh_memory_bounds_fill_pattern_range(self, other_begin, other_end);
}

lh_void
lh_memory_bounds_set(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_set(s, begin, end);
}

lh_void
lh_memory_bounds_assign(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    lh_memory_view_assign(s, o);
}

lh_void
lh_memory_bounds_clear(lh_memory_bounds_t *self)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_clear(s);
}

lh_void
lh_memory_bounds_assign_v(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    lh_memory_view_assign_v(s, o);
}

lh_void
lh_memory_bounds_set_v(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_set_v(s, begin, end);
}

lh_memory_bounds_t
lh_memory_bounds_make(lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_initializer(begin, end);
    return bounds;
}

/* The memory bounds over the memory a view describes: same layout,
   pointers made mutable again. Lets the by-value constructors below reuse
   lh_memory_view's instead of repeating their checks. */
LH_ATTRIBUTE_STATIC
lh_memory_bounds_t
lh_memory_bounds_make_of_view(const lh_memory_view_t *view)
{
    return lh_memory_bounds_make(lh_cast_const(lh_ptr, lh_memory_view_get_begin(view)),
                                 lh_cast_const(lh_ptr, lh_memory_view_get_end(view)));
}

lh_memory_bounds_t
lh_memory_bounds_make_v(lh_ptr begin, lh_ptr end)
{
    const lh_memory_view_t view = lh_memory_view_make_v(begin, end);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_make_by_size(lh_ptr begin, lh_usize_t size)
{
    const lh_memory_view_t view = lh_memory_view_make_by_size(begin, size);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_make_empty(lh_void)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_empty_initializer();
    return bounds;
}

lh_void
lh_memory_bounds_swap(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_t *o = lh_ptr_cast(lh_memory_view_t, other);
    lh_memory_view_swap(s, o);
}

lh_void
lh_memory_bounds_swap_v(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_t *o = lh_ptr_cast(lh_memory_view_t, other);
    lh_memory_view_swap_v(s, o);
}

lh_void
lh_memory_bounds_set_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_set_by_size(s, begin, size);
}

lh_void
lh_memory_bounds_swap_and_clear(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_t *o = lh_ptr_cast(lh_memory_view_t, other);
    lh_memory_view_swap_and_clear(s, o);
}

lh_void
lh_memory_bounds_set_value(const lh_memory_bounds_t *self, lh_uoffset_t offset, lh_byte_t value)
{
    lh_ptr ptr = lh_memory_bounds_get_ptr_from_begin(self, offset);
    lh_ptr_deref(lh_ptr_cast(lh_byte_t, ptr)) = value;
}

lh_void
lh_memory_bounds_init(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_init(s, begin, end);
}

lh_void
lh_memory_bounds_init_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_init_by_size(s, begin, size);
}

lh_void
lh_memory_bounds_init_empty(lh_memory_bounds_t *self)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    lh_memory_view_init_empty(s);
}

lh_void
lh_memory_bounds_init_by_other(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_memory_view_t *s = lh_ptr_cast(lh_memory_view_t, self);
    const lh_memory_view_t *o = lh_ptr_ccast(lh_memory_view_t, other);
    lh_memory_view_init_by_other(s, o);
}

lh_memory_bounds_slice_t
lh_memory_bounds_make_slice(const lh_memory_bounds_t *self)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_slice_t slice = lh_memory_view_make_slice(s);
    return lh_memory_bounds_slice_make(
        lh_cast_const(lh_ptr, lh_memory_view_slice_get_begin(lh_addr_of(slice))),
        lh_cast_const(lh_ptr, lh_memory_view_slice_get_end(lh_addr_of(slice))));
}

lh_memory_bounds_t
lh_memory_bounds_take_first(lh_ptr begin, lh_ptr end, lh_usize_t n)
{
    const lh_memory_view_t view = lh_memory_view_take_first(begin, end, n);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_take_last(lh_ptr begin, lh_ptr end, lh_usize_t n)
{
    const lh_memory_view_t view = lh_memory_view_take_last(begin, end, n);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_make_from_begin(const lh_memory_bounds_t *self, lh_usize_t size)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_make_from_begin(s, size);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_make_from_end(const lh_memory_bounds_t *self, lh_usize_t size)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_make_from_end(s, size);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_make_between(const lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_make_between(s, begin, end);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_make_from_offset(const lh_memory_bounds_t *self, lh_uoffset_t offset,
                                  lh_usize_t size)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_make_from_offset(s, offset, size);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_drop_first(const lh_memory_bounds_t *self, lh_usize_t n)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_drop_first(s, n);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_drop_last(const lh_memory_bounds_t *self, lh_usize_t n)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_drop_last(s, n);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_bounds_t
lh_memory_bounds_trim(const lh_memory_bounds_t *self, lh_usize_t left, lh_usize_t right)
{
    const lh_memory_view_t *s = lh_ptr_ccast(lh_memory_view_t, self);
    const lh_memory_view_t view = lh_memory_view_trim(s, left, right);
    return lh_memory_bounds_make_of_view(lh_addr_of(view));
}

lh_memory_view_t
lh_memory_bounds_as_view(const lh_memory_bounds_t *self)
{
    return lh_memory_view_make(lh_memory_bounds_get_begin(self), lh_memory_bounds_get_end(self));
}
