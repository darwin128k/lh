#include <lh/memory/typed.h>
#include <lh/memory/bounds.h>
#include <lh/memory/typed/initializer.h>
#include <lh/optional/ref.h>
#include <lh/runtime/check/ref.h>
#include <lh/runtime/try.h>
#include <lh/util/interval.h>
#include <lh/util/ptr.h>

lh_memory_bounds_t *
lh_memory_typed_get_bounds(lh_memory_typed_t *self)
{
    lh_runtime_check_ref(self);
    return lh_addr_of(self->bounds);
}

const lh_memory_bounds_t *
lh_memory_typed_get_cbounds(const lh_memory_typed_t *self)
{
    lh_runtime_check_ref(self);
    return lh_addr_of(self->bounds);
}

/* ── pack / unpack ────────────────────────────────────────────────────────── */

lh_void
lh_memory_typed_pack(lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                     const lh_usize_t *type_size)
{
    lh_memory_bounds_t *r = lh_memory_typed_get_bounds(self);
    lh_memory_bounds_pack(r, begin, end);

    lh_optional_ref(type_size)
    {
        self->type_size = lh_ptr_deref(type_size);
    }
}

lh_void
lh_memory_typed_pack_from_bounds(lh_memory_typed_t *self, lh_memory_bounds_t *bounds,
                                 const lh_usize_t *type_size)
{
    lh_runtime_check_ref(bounds);
    lh_memory_typed_pack(self, lh_addr_of(bounds->first), lh_addr_of(bounds->second), type_size);
}

lh_void
lh_memory_typed_unpack(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                       lh_usize_t *type_size)
{
    const lh_memory_bounds_t *r = lh_memory_typed_get_cbounds(self);
    lh_memory_bounds_unpack(r, begin, end);

    lh_optional_ref(type_size)
    {
        lh_ptr_deref(type_size) = self->type_size;
    }
}

void
lh_memory_typed_unpack_v(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                         lh_usize_t *type_size)
{
    lh_runtime_check_if(lh_memory_typed_is_invalid(self),
                        lh_runtime_error_code_invalid_memory_range);
    lh_memory_typed_unpack(self, begin, end, type_size);
}

lh_void
lh_memory_typed_unpack_to_bounds(const lh_memory_typed_t *self, lh_memory_bounds_t *bounds,
                                 lh_usize_t *type_size)
{
    lh_runtime_check_ref(bounds);
    lh_memory_typed_unpack(self, lh_addr_of(bounds->first), lh_addr_of(bounds->second), type_size);
}

/* ── set / init / assign ──────────────────────────────────────────────────── */

lh_void
lh_memory_typed_set(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_pack(self, lh_addr_of(begin), lh_addr_of(end), lh_addr_of(type_size));
}

lh_void
lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;
    lh_usize_t other_type_size;

    lh_memory_typed_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end),
                           lh_addr_of(other_type_size));
    lh_memory_typed_set(self, other_begin, other_end, other_type_size);
}

lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_runtime_check_if(lh_memory_typed_is_invalid(other),
                        lh_runtime_error_code_invalid_memory_range);
    lh_memory_typed_assign(self, other);
}

lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    return lh_memory_typed_set(self, begin, end, type_size);
}

lh_void
lh_memory_typed_set_by_size(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t size,
                            lh_usize_t type_size)
{
    lh_runtime_check(begin, lh_runtime_error_code_invalid_argument);
    lh_ptr end = lh_ptr_add_by_offset(lh_void, begin, size);
    lh_memory_typed_set(self, begin, end, type_size);
}

lh_void
lh_memory_typed_init_by_size(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t size,
                             lh_usize_t type_size)
{
    lh_memory_typed_set_by_size(self, begin, size, type_size);
}

lh_void
lh_memory_typed_init_by_empty(lh_memory_typed_t *self, lh_usize_t type_size)
{
    lh_memory_typed_t other = lh_memory_typed_empty_initializer(type_size);
    lh_memory_typed_assign(self, lh_addr_of(other));
}

/* ── clone / dup / validated pack ─────────────────────────────────────────── */

lh_memory_typed_t
lh_memory_typed_clone(const lh_memory_typed_t *self)
{
    lh_memory_typed_t t;
    lh_memory_typed_unpack_to_bounds(self, lh_addr_of(t.bounds), lh_addr_of(t.type_size));
    return t;
}

lh_void
lh_memory_typed_dup(const lh_memory_typed_t *self, lh_memory_typed_t *other)
{
    const lh_memory_typed_t t = lh_memory_typed_clone(self);
    lh_memory_typed_assign(other, lh_addr_of(t));
}

lh_void
lh_memory_typed_dup_v(const lh_memory_typed_t *self, lh_memory_typed_t *other)
{
    const lh_memory_typed_t t = lh_memory_typed_clone(self);
    lh_memory_typed_assign_v(other, lh_addr_of(t));
}

lh_memory_typed_t
lh_memory_typed_clone_v(const lh_memory_typed_t *self)
{
    lh_memory_typed_t t;
    lh_memory_typed_dup_v(self, lh_addr_of(t));
    return t;
}

lh_memory_typed_t
lh_memory_typed_make(lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_t t;
    lh_memory_typed_init(lh_addr_of(t), begin, end, type_size);
    return t;
}

lh_memory_typed_t
lh_memory_typed_make_v(lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_t t = lh_memory_typed_make(begin, end, type_size);
    lh_runtime_check_if(lh_memory_typed_is_invalid(lh_addr_of(t)),
                        lh_runtime_error_code_invalid_memory_range);
    return t;
}

lh_memory_typed_t
lh_memory_typed_make_by_empty(lh_usize_t type_size)
{
    lh_memory_typed_t t;
    lh_memory_typed_init_by_empty(lh_addr_of(t), type_size);
    return t;
}

lh_memory_typed_t
lh_memory_typed_make_or_empty(lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_runtime_try(e)
    {
        lh_memory_typed_t t = lh_memory_typed_make_v(begin, end, type_size);
        lh_runtime_try_return(t);
    }
    return lh_memory_typed_make_by_empty(type_size);
}

lh_void
lh_memory_typed_pack_v(lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                       const lh_usize_t *type_size)
{
    lh_memory_typed_t t = lh_memory_typed_clone_v(self);
    lh_memory_typed_pack(lh_addr_of(t), begin, end, type_size);
    lh_memory_typed_assign_v(self, lh_addr_of(t));
}

/* ── classification / geometry ────────────────────────────────────────────── */

lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self)
{
    const lh_memory_bounds_t *r = lh_memory_typed_get_cbounds(self);
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_memory_bounds_is_multiple_of(r, type_size);
}

lh_bool_t
lh_memory_typed_is_invalid(const lh_memory_typed_t *self)
{
    return !lh_memory_typed_is_valid(self);
}

/* ── element access ───────────────────────────────────────────────────────── */

lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self)
{
    lh_ptr begin;
    lh_memory_typed_unpack(self, lh_addr_of(begin), lh_null, lh_null);
    return begin;
}

lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self)
{
    lh_ptr end;
    lh_memory_typed_unpack(self, lh_null, lh_addr_of(end), lh_null);
    return end;
}

lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self)
{
    lh_usize_t size;
    lh_memory_typed_unpack(self, lh_null, lh_null, lh_addr_of(size));
    return size;
}

lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self)
{
    lh_runtime_check_if(lh_memory_typed_is_invalid(self),
                        lh_runtime_error_code_size_not_multiple_of_type_size);

    const lh_memory_bounds_t *r = lh_memory_typed_get_cbounds(self);
    lh_usize_t size = lh_memory_bounds_get_size(r);
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);

    return size / type_size;
}

lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self)
{
    lh_usize_t size = lh_memory_typed_get_size(self);
    return lh_math_is_zero(size);
}

lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_usize_t size = lh_memory_typed_get_size(self);
    return lh_math_lt(index, size);
}

lh_bool_t
lh_memory_typed_is_sliceable(const lh_memory_typed_t *self, lh_usize_t offset, lh_usize_t size)
{
    const lh_memory_bounds_t *r = lh_memory_typed_get_cbounds(self);
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);

    if (lh_interval_closed_is_mul_overflow(offset, type_size, LH_USIZE_T_MIN, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }

    if (lh_interval_closed_is_mul_overflow(size, type_size, LH_USIZE_T_MIN, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }

    return lh_memory_bounds_is_sliceable(r, lh_math_mul(offset, type_size),
                                         lh_math_mul(size, type_size));
}

lh_memory_typed_t
lh_memory_typed_slice(const lh_memory_typed_t *self, lh_usize_t offset, lh_usize_t size)
{
    lh_runtime_check(lh_memory_typed_is_sliceable(self, offset, size),
                     lh_runtime_error_code_out_of_range);

    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    lh_memory_bounds_t r =
        lh_memory_bounds_slice(lh_memory_typed_get_cbounds(self), lh_math_mul(offset, type_size),
                               lh_math_mul(size, type_size));

    lh_memory_typed_t t;
    lh_memory_typed_pack_from_bounds(lh_addr_of(t), lh_addr_of(r), lh_addr_of(type_size));
    return t;
}

lh_memory_typed_t
lh_memory_typed_slice_or_empty(const lh_memory_typed_t *self, lh_usize_t offset, lh_usize_t size)
{
    lh_runtime_try(e)
    {
        lh_memory_typed_t t = lh_memory_typed_slice(self, offset, size);
        lh_runtime_try_return(t);
    }
    return lh_memory_typed_make_by_empty(lh_memory_typed_get_type_size(self));
}

lh_ptr
lh_memory_typed_get_ptr_from_front(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_runtime_check(lh_memory_typed_is_valid_index(self, index),
                     lh_runtime_error_code_out_of_range);

    const lh_memory_bounds_t *r = lh_memory_typed_get_cbounds(self);
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);

    return lh_memory_bounds_get_ptr_from_front(r, index * type_size);
}

lh_ptr
lh_memory_typed_get_ptr_from_back(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_usize_t n = lh_memory_typed_get_size(self);
    return lh_memory_typed_get_ptr_from_front(self, n - index - 1);
}

lh_ptr
lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_usize_t index, lh_bool_t from_back)
{
    return from_back ? lh_memory_typed_get_ptr_from_back(self, index)
                     : lh_memory_typed_get_ptr_from_front(self, index);
}

lh_byte_t
lh_memory_typed_get_value_from_front(const lh_memory_typed_t *self, lh_usize_t index)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_ptr_from_front(self, index)));
}

lh_byte_t
lh_memory_typed_get_value_from_back(const lh_memory_typed_t *self, lh_usize_t index)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_ptr_from_back(self, index)));
}

lh_byte_t
lh_memory_typed_get_value(const lh_memory_typed_t *self, lh_usize_t index, lh_bool_t from_back)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_ptr(self, index, from_back)));
}

lh_ptr
lh_memory_typed_get_front_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_ptr_from_front(self, 0);
}

lh_ptr
lh_memory_typed_get_back_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_ptr_from_back(self, 0);
}

lh_byte_t
lh_memory_typed_get_front_value(const lh_memory_typed_t *self)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_front_ptr(self)));
}

lh_byte_t
lh_memory_typed_get_back_value(const lh_memory_typed_t *self)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_back_ptr(self)));
}