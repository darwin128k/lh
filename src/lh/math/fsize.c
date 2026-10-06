/**
 * @file fsize.c
 * @brief Implementation of lh/math/size.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math/fsize.h>
#include <lh/util/return.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_fsize_t
lh_math_fsize_make(lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_math_fsize_t s;
    lh_math_fsize_set_width(lh_addr_of(s), width);
    lh_math_fsize_set_height(lh_addr_of(s), height);
    return s;
}

lh_math_fsize_t
lh_math_fsize_make_empty(void)
{
    return lh_math_fsize_make(0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_fsize_get_width(const lh_math_fsize_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_math_scalar_t
lh_math_fsize_get_height(const lh_math_fsize_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_math_fsize_set_width(lh_math_fsize_t *self, lh_math_scalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_math_fsize_set_height(lh_math_fsize_t *self, lh_math_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_math_size_t
lh_math_fsize_to_size(lh_math_fsize_t self)
{
    return lh_math_size_make(
        lh_cast_static(lh_math_iscalar_t, lh_math_fsize_get_width(lh_addr_of(self))),
        lh_cast_static(lh_math_iscalar_t, lh_math_fsize_get_height(lh_addr_of(self))));
}

lh_math_fsize_t
lh_math_size_to_fsize(lh_math_size_t self)
{
    return lh_math_fsize_make(
        lh_cast_static(lh_math_scalar_t, lh_math_size_get_width(lh_addr_of(self))),
        lh_cast_static(lh_math_scalar_t, lh_math_size_get_height(lh_addr_of(self))));
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_fsize_eq(const lh_math_fsize_t *a, const lh_math_fsize_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_fsize_get_width(a), lh_math_fsize_get_width(b))
        && lh_math_eq(lh_math_fsize_get_height(a), lh_math_fsize_get_height(b));
}
