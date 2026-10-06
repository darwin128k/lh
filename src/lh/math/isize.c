/**
 * @file isize.c
 * @brief Implementation of lh/math/isize.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/util/return.h>
#include <lh/math/isize.h>
#include <lh/math.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_isize_t
lh_math_isize_make(lh_math_iscalar_t width, lh_math_iscalar_t height)
{
    lh_math_isize_t s;
    lh_math_isize_set_width(lh_addr_of(s), width);
    lh_math_isize_set_height(lh_addr_of(s), height);
    return s;
}

lh_math_isize_t
lh_math_isize_make_empty(void)
{
    return lh_math_isize_make(0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_iscalar_t
lh_math_isize_get_width(const lh_math_isize_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_math_iscalar_t
lh_math_isize_get_height(const lh_math_isize_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_math_isize_set_width(lh_math_isize_t *self, lh_math_iscalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_math_isize_set_height(lh_math_isize_t *self, lh_math_iscalar_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_isize_eq(const lh_math_isize_t *a, const lh_math_isize_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_isize_get_width(a), lh_math_isize_get_width(b))
        && lh_math_eq(lh_math_isize_get_height(a), lh_math_isize_get_height(b));
}