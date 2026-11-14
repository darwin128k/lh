/**
 * @file point.c
 * @brief Implementation of lh/gl/point.h.
 */

#include <lh/core/assert/runtime.h>
#include <lh/gl/point.h>

/* -- Accessors ------------------------------------------------------------- */

lh_math_scalar_t
lh_gl_point_get_x(const lh_gl_point_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_math_scalar_t
lh_gl_point_get_y(const lh_gl_point_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_gl_point_set_x(lh_gl_point_t *self, lh_math_scalar_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_void
lh_gl_point_set_y(lh_gl_point_t *self, lh_math_scalar_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_void
lh_gl_point_init(lh_gl_point_t *self, lh_math_scalar_t x, lh_math_scalar_t y)
{
    lh_gl_point_set_x(self, x);
    lh_gl_point_set_y(self, y);
}

/* -- Init and queries, by level -------------------------------------------- */

lh_bool_t
lh_gl_point_eq(const lh_gl_point_t *a, const lh_gl_point_t *b)
{
    return lh_cast_static(lh_bool_t, lh_gl_point_get_x(a) == lh_gl_point_get_x(b) &&
                                         lh_gl_point_get_y(a) == lh_gl_point_get_y(b));
}

lh_bool_t
lh_gl_point_is_at_least(const lh_gl_point_t *self, const lh_gl_point_t *minimum)
{
    const lh_math_scalar_t self_x = lh_gl_point_get_x(self);
    const lh_math_scalar_t minimum_x = lh_gl_point_get_x(minimum);

    if (self_x != minimum_x)
    {
        return lh_cast_static(lh_bool_t, self_x > minimum_x);
    }
    return lh_cast_static(lh_bool_t, lh_gl_point_get_y(self) >= lh_gl_point_get_y(minimum));
}

lh_void
lh_gl_point_init_empty(lh_gl_point_t *self)
{
    lh_gl_point_init(self, 0, 0);
}

lh_bool_t
lh_gl_point_is_less(const lh_gl_point_t *self, const lh_gl_point_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_gl_point_is_at_least(self, other));
}

lh_void
lh_gl_point_offset(const lh_gl_point_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                   lh_gl_point_t *out)
{
    lh_gl_point_init(out, lh_gl_point_get_x(self) + dx, lh_gl_point_get_y(self) + dy);
}

lh_void
lh_gl_point_min(const lh_gl_point_t *a, const lh_gl_point_t *b, lh_gl_point_t *out)
{
    const lh_math_scalar_t x = lh_math_min(lh_gl_point_get_x(a), lh_gl_point_get_x(b));
    const lh_math_scalar_t y = lh_math_min(lh_gl_point_get_y(a), lh_gl_point_get_y(b));

    lh_gl_point_init(out, x, y);
}

lh_bool_t
lh_gl_point_is_greater(const lh_gl_point_t *self, const lh_gl_point_t *other)
{
    return lh_gl_point_is_less(other, self);
}

lh_void
lh_gl_point_max(const lh_gl_point_t *a, const lh_gl_point_t *b, lh_gl_point_t *out)
{
    const lh_math_scalar_t x = lh_math_max(lh_gl_point_get_x(a), lh_gl_point_get_x(b));
    const lh_math_scalar_t y = lh_math_max(lh_gl_point_get_y(a), lh_gl_point_get_y(b));

    lh_gl_point_init(out, x, y);
}