/**
 * @file rect3.c
 * @brief Implementation of lh/math/rect3.h.
 *
 * The 2D rect (`origin` with `x, y` plus `size`) is the core; this file
 * delegates to ::lh_math_rect_* for the 2D parts and adds the `z` and
 * `z_depth` arithmetic.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/util/return.h>
#include <lh/math/rect3.h>
#include <lh/math.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_rect3_t
lh_math_rect3_make(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t z,
                  lh_math_scalar_t width, lh_math_scalar_t height, lh_math_scalar_t z_depth)
{
    lh_math_rect3_t b;
    b.rect = lh_math_rect_make(x, y, width, height);
    lh_math_rect3_set_z(lh_addr_of(b), z);
    lh_math_rect3_set_z_depth(lh_addr_of(b), z_depth);
    return b;
}

lh_math_rect3_t
lh_math_rect3_make_empty(void)
{
    return lh_math_rect3_make(0, 0, 0, 0, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_rect3_get_x(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_x(lh_addr_of(self->rect));
}

lh_math_scalar_t
lh_math_rect3_get_y(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_y(lh_addr_of(self->rect));
}

lh_math_scalar_t
lh_math_rect3_get_z(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return self->z;
}

lh_math_scalar_t
lh_math_rect3_get_size_width(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_size_width(lh_addr_of(self->rect));
}

lh_math_scalar_t
lh_math_rect3_get_size_height(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_size_height(lh_addr_of(self->rect));
}

lh_math_scalar_t
lh_math_rect3_get_z_depth(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return self->z_depth;
}

lh_void
lh_math_rect3_set_z(lh_math_rect3_t *self, lh_math_scalar_t z)
{
    lh_assert_runtime_ref(self);
    self->z = z;
}

lh_void
lh_math_rect3_set_z_depth(lh_math_rect3_t *self, lh_math_scalar_t z_depth)
{
    lh_assert_runtime_ref(self);
    self->z_depth = z_depth;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_math_scalar_t
lh_math_rect3_get_width(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_width(lh_addr_of(self->rect));
}

lh_math_scalar_t
lh_math_rect3_get_height(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_height(lh_addr_of(self->rect));
}

lh_bool_t
lh_math_rect3_is_empty(const lh_math_rect3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_is_empty(lh_addr_of(self->rect))
        || lh_math_rect3_get_z_depth(self) <= 0;
}

lh_bool_t
lh_math_rect3_eq(const lh_math_rect3_t *a, const lh_math_rect3_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_rect3_get_x(a), lh_math_rect3_get_x(b))
        && lh_math_eq(lh_math_rect3_get_y(a), lh_math_rect3_get_y(b))
        && lh_math_eq(lh_math_rect3_get_z(a), lh_math_rect3_get_z(b))
        && lh_math_eq(lh_math_rect3_get_size_width(a), lh_math_rect3_get_size_width(b))
        && lh_math_eq(lh_math_rect3_get_size_height(a), lh_math_rect3_get_size_height(b))
        && lh_math_eq(lh_math_rect3_get_z_depth(a), lh_math_rect3_get_z_depth(b));
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_rect3_t
lh_math_rect3_intersection(const lh_math_rect3_t *a, const lh_math_rect3_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_rect3_is_empty(a) || lh_math_rect3_is_empty(b))
    {
        return lh_math_rect3_make_empty();
    }
    const lh_math_rect_t i2d = lh_math_rect_intersection(lh_addr_of(a->rect), lh_addr_of(b->rect));
    /* Z interval: overlap of [a.z, a.z + a.z_depth) and [b.z, b.z + b.z_depth). */
    const lh_math_scalar_t z1 = lh_math_max(lh_math_rect3_get_z(a), lh_math_rect3_get_z(b));
    const lh_math_scalar_t z2 = lh_math_min(lh_math_rect3_get_z(a) + lh_math_rect3_get_z_depth(a),
                                          lh_math_rect3_get_z(b) + lh_math_rect3_get_z_depth(b));
    if (z2 <= z1)
    {
        return lh_math_rect3_make_empty();
    }
    return lh_math_rect3_make(lh_math_rect_get_x(lh_addr_of(i2d)),
                             lh_math_rect_get_y(lh_addr_of(i2d)),
                             z1,
                             lh_math_rect_get_size_width(lh_addr_of(i2d)),
                             lh_math_rect_get_size_height(lh_addr_of(i2d)),
                             z2 - z1);
}

lh_math_rect3_t
lh_math_rect3_offset(const lh_math_rect3_t *self, lh_math_scalar_t dx,
                     lh_math_scalar_t dy, lh_math_scalar_t dz)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect3_make(lh_math_rect3_get_x(self) + dx,
                             lh_math_rect3_get_y(self) + dy,
                             lh_math_rect3_get_z(self) + dz,
                             lh_math_rect3_get_size_width(self),
                             lh_math_rect3_get_size_height(self),
                             lh_math_rect3_get_z_depth(self));
}