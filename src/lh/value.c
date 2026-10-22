#include <lh/value.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/math/ops.h>
#include <lh/null.h>
#include <lh/runtime/error.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_value_kind_t
lh_value_get_kind(const lh_value_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_value_set_kind(lh_value_t *self, lh_value_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_value_data_t *
lh_value_get_data_mut(lh_value_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->data);
}

const lh_value_data_t *
lh_value_get_data(const lh_value_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->data);
}

/* The storage of @p self after checking it holds a @p kind. */
const lh_value_data_t *
lh_value_get_data_of(const lh_value_t *self, lh_value_kind_t kind)
{
    lh_assert_runtime_if(!lh_value_has_kind(self, kind), lh_runtime_error_code_invalid_argument,
                         "value holds another kind");
    return lh_value_get_data(self);
}

lh_bool_t
lh_value_has_kind(const lh_value_t *self, lh_value_kind_t kind)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_value_get_kind(self), kind));
}

lh_bool_t
lh_value_is_none(const lh_value_t *self)
{
    return lh_value_has_kind(self, lh_value_kind_none);
}

/* ── init / make ─────────────────────────────────────────────────────────── */

lh_void
lh_value_init(lh_value_t *self)
{
    lh_value_set_kind(self, lh_value_kind_none);
    lh_value_get_data_mut(self)->p = lh_null;
}

lh_void
lh_value_assign(lh_value_t *self, const lh_value_t *other)
{
    lh_value_set_kind(self, lh_value_get_kind(other));
    lh_ptr_deref(lh_value_get_data_mut(self)) = lh_ptr_deref(lh_value_get_data(other));
}

lh_value_t
lh_value_make_none(lh_void)
{
    lh_value_t self;

    lh_value_init(lh_addr_of(self));
    return self;
}

lh_value_t
lh_value_make_bool(lh_bool_t value)
{
    lh_value_t self;

    lh_value_set_kind(lh_addr_of(self), lh_value_kind_bool);
    lh_value_get_data_mut(lh_addr_of(self))->b = value;
    return self;
}

lh_value_t
lh_value_make_sint(lh_sllong_t value)
{
    lh_value_t self;

    lh_value_set_kind(lh_addr_of(self), lh_value_kind_sint);
    lh_value_get_data_mut(lh_addr_of(self))->i = value;
    return self;
}

lh_value_t
lh_value_make_uint(lh_ullong_t value)
{
    lh_value_t self;

    lh_value_set_kind(lh_addr_of(self), lh_value_kind_uint);
    lh_value_get_data_mut(lh_addr_of(self))->u = value;
    return self;
}

#if LH_REAL_ENABLED
lh_value_t
lh_value_make_real(lh_real_t value)
{
    lh_value_t self;

    lh_value_set_kind(lh_addr_of(self), lh_value_kind_real);
    lh_value_get_data_mut(lh_addr_of(self))->r = value;
    return self;
}
#endif

lh_value_t
lh_value_make_str(lh_str_view_t value)
{
    lh_value_t self;

    lh_value_set_kind(lh_addr_of(self), lh_value_kind_str);
    lh_value_get_data_mut(lh_addr_of(self))->s = value;
    return self;
}

lh_value_t
lh_value_make_ptr(lh_ptr value)
{
    lh_value_t self;

    lh_value_set_kind(lh_addr_of(self), lh_value_kind_ptr);
    lh_value_get_data_mut(lh_addr_of(self))->p = value;
    return self;
}

/* ── read (kind checked) ─────────────────────────────────────────────────── */

lh_bool_t
lh_value_get_bool(const lh_value_t *self)
{
    return lh_value_get_data_of(self, lh_value_kind_bool)->b;
}

lh_sllong_t
lh_value_get_sint(const lh_value_t *self)
{
    return lh_value_get_data_of(self, lh_value_kind_sint)->i;
}

lh_ullong_t
lh_value_get_uint(const lh_value_t *self)
{
    return lh_value_get_data_of(self, lh_value_kind_uint)->u;
}

#if LH_REAL_ENABLED
lh_real_t
lh_value_get_real(const lh_value_t *self)
{
    return lh_value_get_data_of(self, lh_value_kind_real)->r;
}
#endif

lh_str_view_t
lh_value_get_str(const lh_value_t *self)
{
    return lh_value_get_data_of(self, lh_value_kind_str)->s;
}

lh_ptr
lh_value_get_ptr(const lh_value_t *self)
{
    return lh_value_get_data_of(self, lh_value_kind_ptr)->p;
}

/* ── read with a fallback ────────────────────────────────────────────────── */

lh_bool_t
lh_value_get_bool_or(const lh_value_t *self, lh_bool_t fallback)
{
    return lh_value_has_kind(self, lh_value_kind_bool) ? lh_value_get_bool(self) : fallback;
}

lh_sllong_t
lh_value_get_sint_or(const lh_value_t *self, lh_sllong_t fallback)
{
    return lh_value_has_kind(self, lh_value_kind_sint) ? lh_value_get_sint(self) : fallback;
}

lh_ullong_t
lh_value_get_uint_or(const lh_value_t *self, lh_ullong_t fallback)
{
    return lh_value_has_kind(self, lh_value_kind_uint) ? lh_value_get_uint(self) : fallback;
}

#if LH_REAL_ENABLED
lh_real_t
lh_value_get_real_or(const lh_value_t *self, lh_real_t fallback)
{
    return lh_value_has_kind(self, lh_value_kind_real) ? lh_value_get_real(self) : fallback;
}
#endif

lh_str_view_t
lh_value_get_str_or(const lh_value_t *self, lh_str_view_t fallback)
{
    return lh_value_has_kind(self, lh_value_kind_str) ? lh_value_get_str(self) : fallback;
}

lh_ptr
lh_value_get_ptr_or(const lh_value_t *self, lh_ptr fallback)
{
    return lh_value_has_kind(self, lh_value_kind_ptr) ? lh_value_get_ptr(self) : fallback;
}

/* ── compare ─────────────────────────────────────────────────────────────── */

lh_bool_t
lh_value_equals(const lh_value_t *self, const lh_value_t *other)
{
    const lh_value_kind_t kind = lh_value_get_kind(self);

    if (lh_math_ne(kind, lh_value_get_kind(other)))
    {
        return lh_bool_false;
    }
    switch (kind)
    {
    case lh_value_kind_none:
        return lh_bool_true;
    case lh_value_kind_bool:
        return lh_cast_static(lh_bool_t,
                              lh_bool_eq(lh_value_get_bool(self), lh_value_get_bool(other)));
    case lh_value_kind_sint:
        return lh_cast_static(lh_bool_t,
                              lh_math_eq(lh_value_get_sint(self), lh_value_get_sint(other)));
    case lh_value_kind_uint:
        return lh_cast_static(lh_bool_t,
                              lh_math_eq(lh_value_get_uint(self), lh_value_get_uint(other)));
#if LH_REAL_ENABLED
    case lh_value_kind_real:
        return lh_cast_static(lh_bool_t,
                              lh_math_eq(lh_value_get_real(self), lh_value_get_real(other)));
#endif
    case lh_value_kind_str:
    {
        const lh_str_view_t a = lh_value_get_str(self);
        const lh_str_view_t b = lh_value_get_str(other);

        return lh_str_view_equals(lh_addr_of(a), lh_addr_of(b), lh_bool_false);
    }
    case lh_value_kind_ptr:
        return lh_cast_static(lh_bool_t,
                              lh_math_eq(lh_value_get_ptr(self), lh_value_get_ptr(other)));
    default:
        return lh_bool_false;
    }
}
