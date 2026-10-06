/**
 * @file fsize.c
 * @brief Implementation of lh/math/fsize.h.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math/fsize.h>
#include <lh/util/return.h>

/* ── init ────────────────────────────────────────────────────────────────── */

lh_void
lh_math_fsize_init(lh_math_fsize_t *self, lh_math_fscalar_t width, lh_math_fscalar_t height)
{
    lh_math_fsize_set_width(self, width);
    lh_math_fsize_set_height(self, height);
}

lh_void
lh_math_fsize_init_empty(lh_math_fsize_t *self)
{
    lh_math_fsize_init(self, 0, 0);
}

lh_math_fsize_t
lh_math_fsize_from_extent(const lh_math_fpoint_t *min, const lh_math_fpoint_t *max)
{
    lh_assert_runtime_ref(min);
    lh_assert_runtime_ref(max);
    lh_math_fsize_t s;
    lh_math_fsize_init(lh_addr_of(s), lh_math_fpoint_get_x(max) - lh_math_fpoint_get_x(min),
                       lh_math_fpoint_get_y(max) - lh_math_fpoint_get_y(min));
    return s;
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_fscalar_t
lh_math_fsize_get_width(const lh_math_fsize_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_math_fscalar_t
lh_math_fsize_get_height(const lh_math_fsize_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_math_fsize_set_width(lh_math_fsize_t *self, lh_math_fscalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_math_fsize_set_height(lh_math_fsize_t *self, lh_math_fscalar_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

/* ── Conversions ─────────────────────────────────────────────────────────── */

lh_math_size_t
lh_math_fsize_to_size(lh_math_fsize_t self)
{
    return ({ lh_math_size_t _v; lh_math_size_init(lh_addr_of(_v), lh_cast_static(lh_math_scalar_t, lh_math_fsize_get_width(lh_addr_of(self))),
        lh_cast_static(lh_math_scalar_t, lh_math_fsize_get_height(lh_addr_of(self)))); _v; });
}

lh_math_fsize_t
lh_math_size_to_fsize(lh_math_size_t self)
{
    return ({ lh_math_fsize_t _v; lh_math_fsize_init(lh_addr_of(_v), lh_cast_static(lh_math_fscalar_t, lh_math_size_get_width(lh_addr_of(self))),
        lh_cast_static(lh_math_fscalar_t, lh_math_size_get_height(lh_addr_of(self)))); _v; });
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

lh_bool_t
lh_math_fsize_is_empty(const lh_math_fsize_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_le(lh_math_fsize_get_width(self), 0)
        || lh_math_le(lh_math_fsize_get_height(self), 0);
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_fsize_t
lh_math_fsize_inset(const lh_math_fsize_t *self, lh_math_fscalar_t dx, lh_math_fscalar_t dy)
{
    lh_assert_runtime_ref(self);
    return ({ lh_math_fsize_t _v; lh_math_fsize_init(lh_addr_of(_v), lh_math_fsize_get_width(self) - dx - dx,
                              lh_math_fsize_get_height(self) - dy - dy); _v; });
}
