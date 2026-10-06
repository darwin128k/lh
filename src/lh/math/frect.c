/**
 * @file frect.c
 * @brief Implementation of lh/math/rect.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math.h>
#include <lh/math/frect.h>
#include <lh/util/return.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_frect_t
lh_math_frect_make(lh_math_scalar_t x, lh_math_scalar_t y,
                         lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_math_frect_t r;
    lh_math_frect_set_origin(lh_addr_of(r), lh_math_fpoint_make(x, y));
    lh_math_frect_set_size(lh_addr_of(r), lh_math_fsize_make(width, height));
    return r;
}

lh_math_frect_t
lh_math_frect_make_empty(void)
{
    return lh_math_frect_make(0, 0, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_fpoint_t
lh_math_frect_get_origin(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->origin;
}

lh_math_fsize_t
lh_math_frect_get_size(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_math_scalar_t
lh_math_frect_get_x(const lh_math_frect_t *self)
{
    lh_math_fpoint_t origin;
    lh_assert_runtime_ref(self);
    origin = lh_math_frect_get_origin(self);
    return lh_math_fpoint_get_x(lh_addr_of(origin));
}

lh_math_scalar_t
lh_math_frect_get_y(const lh_math_frect_t *self)
{
    lh_math_fpoint_t origin;
    lh_assert_runtime_ref(self);
    origin = lh_math_frect_get_origin(self);
    return lh_math_fpoint_get_y(lh_addr_of(origin));
}

lh_math_scalar_t
lh_math_frect_get_size_width(const lh_math_frect_t *self)
{
    lh_math_fsize_t size;
    lh_assert_runtime_ref(self);
    size = lh_math_frect_get_size(self);
    return lh_math_fsize_get_width(lh_addr_of(size));
}

lh_math_scalar_t
lh_math_frect_get_size_height(const lh_math_frect_t *self)
{
    lh_math_fsize_t size;
    lh_assert_runtime_ref(self);
    size = lh_math_frect_get_size(self);
    return lh_math_fsize_get_height(lh_addr_of(size));
}

lh_void
lh_math_frect_set_origin(lh_math_frect_t *self, lh_math_fpoint_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

lh_void
lh_math_frect_set_size(lh_math_frect_t *self, lh_math_fsize_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_math_rect_t
lh_math_frect_to_rect(lh_math_frect_t self)
{
    lh_math_rect_t r;
    lh_math_rect_set_origin(lh_addr_of(r),
                            lh_math_fpoint_to_point(lh_math_frect_get_origin(lh_addr_of(self))));
    lh_math_rect_set_size(lh_addr_of(r),
                          lh_math_fsize_to_size(lh_math_frect_get_size(lh_addr_of(self))));
    return r;
}

lh_math_frect_t
lh_math_rect_to_frect(lh_math_rect_t self)
{
    lh_math_frect_t r;
    lh_math_frect_set_origin(lh_addr_of(r),
                                   lh_math_point_to_fpoint(lh_math_rect_get_origin(lh_addr_of(self))));
    lh_math_frect_set_size(lh_addr_of(r),
                                 lh_math_size_to_fsize(lh_math_rect_get_size(lh_addr_of(self))));
    return r;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_frect_get_width(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_math_frect_is_empty(self), 0);
    return lh_math_frect_get_size_width(self);
}

lh_math_scalar_t
lh_math_frect_get_height(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_math_frect_is_empty(self), 0);
    return lh_math_frect_get_size_height(self);
}

lh_bool_t
lh_math_frect_is_empty(const lh_math_frect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_le(lh_math_frect_get_size_width(self), 0)
        || lh_math_le(lh_math_frect_get_size_height(self), 0);
}

lh_bool_t
lh_math_frect_eq(const lh_math_frect_t *a, const lh_math_frect_t *b)
{
    lh_math_fpoint_t origin_a;
    lh_math_fpoint_t origin_b;
    lh_math_fsize_t size_a;
    lh_math_fsize_t size_b;
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    origin_a = lh_math_frect_get_origin(a);
    origin_b = lh_math_frect_get_origin(b);
    size_a = lh_math_frect_get_size(a);
    size_b = lh_math_frect_get_size(b);
    return lh_math_fpoint_eq(lh_addr_of(origin_a), lh_addr_of(origin_b))
        && lh_math_fsize_eq(lh_addr_of(size_a), lh_addr_of(size_b));
}
