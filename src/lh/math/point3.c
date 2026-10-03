/**
 * @file point3.c
 * @brief Implementation of `lh/math/point3.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math/point3.h>
#include <lh/math.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_point3_t
lh_math_point3_make(lh_math_coord_t x, lh_math_coord_t y, lh_math_coord_t z)
{
    lh_math_point3_t p;
    lh_math_point3_set_x(lh_addr_of(p), x);
    lh_math_point3_set_y(lh_addr_of(p), y);
    lh_math_point3_set_z(lh_addr_of(p), z);
    return p;
}

lh_math_point3_t
lh_math_point3_make_empty(void)
{
    return lh_math_point3_make(0, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_coord_t
lh_math_point3_get_x(const lh_math_point3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_point_get_x(lh_addr_of(self->point));
}

lh_math_coord_t
lh_math_point3_get_y(const lh_math_point3_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_point_get_y(lh_addr_of(self->point));
}

lh_math_coord_t
lh_math_point3_get_z(const lh_math_point3_t *self)
{
    lh_assert_runtime_ref(self);
    return self->z;
}

lh_void
lh_math_point3_set_x(lh_math_point3_t *self, lh_math_coord_t x)
{
    lh_assert_runtime_ref(self);
    lh_math_point_set_x(lh_addr_of(self->point), x);
}

lh_void
lh_math_point3_set_y(lh_math_point3_t *self, lh_math_coord_t y)
{
    lh_assert_runtime_ref(self);
    lh_math_point_set_y(lh_addr_of(self->point), y);
}

lh_void
lh_math_point3_set_z(lh_math_point3_t *self, lh_math_coord_t z)
{
    lh_assert_runtime_ref(self);
    self->z = z;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_bool_t
lh_math_point3_eq(const lh_math_point3_t *a, const lh_math_point3_t *b)
{
    if (a == b)
    {
        return lh_bool_true;
    }
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_point3_get_x(a), lh_math_point3_get_x(b))
        && lh_math_eq(lh_math_point3_get_y(a), lh_math_point3_get_y(b))
        && lh_math_eq(lh_math_point3_get_z(a), lh_math_point3_get_z(b));
}

/* ── Set ops ────────────────────────────────────────────────────────────── */

lh_math_point3_t
lh_math_point3_offset(const lh_math_point3_t *self, lh_math_coord_t dx,
                     lh_math_coord_t dy, lh_math_coord_t dz)
{
    lh_assert_runtime_ref(self);
    return lh_math_point3_make(lh_math_point3_get_x(self) + dx,
                             lh_math_point3_get_y(self) + dy,
                             lh_math_point3_get_z(self) + dz);
}