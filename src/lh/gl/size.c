/**
 * @file size.c
 * @brief Implementation of lh/gl/size.h.
 */

#include <lh/core/math/ops.h>
#include <lh/gl/size.h>

/* -- Accessors ------------------------------------------------------------- */

lh_math_scalar_t
lh_gl_size_get_width(const lh_gl_size_t *self)
{
    return lh_gl_point_get_x(self);
}

lh_math_scalar_t
lh_gl_size_get_height(const lh_gl_size_t *self)
{
    return lh_gl_point_get_y(self);
}

lh_void
lh_gl_size_set_width(lh_gl_size_t *self, lh_math_scalar_t width)
{
    lh_gl_point_set_x(self, width);
}

lh_void
lh_gl_size_set_height(lh_gl_size_t *self, lh_math_scalar_t height)
{
    lh_gl_point_set_y(self, height);
}

/* -- Init and queries, by level -------------------------------------------- */

lh_void
lh_gl_size_init(lh_gl_size_t *self, lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_gl_point_init(self, width, height);
}

lh_void
lh_gl_size_init_empty(lh_gl_size_t *self)
{
    lh_gl_point_init_empty(self);
}

lh_bool_t
lh_gl_size_eq(const lh_gl_size_t *a, const lh_gl_size_t *b)
{
    return lh_gl_point_eq(a, b);
}

lh_bool_t
lh_gl_size_is_at_least(const lh_gl_size_t *self, const lh_gl_size_t *minimum)
{
    return lh_gl_point_is_at_least(self, minimum);
}

lh_bool_t
lh_gl_size_is_less(const lh_gl_size_t *self, const lh_gl_size_t *other)
{
    return lh_gl_point_is_less(self, other);
}

lh_bool_t
lh_gl_size_is_greater(const lh_gl_size_t *self, const lh_gl_size_t *other)
{
    return lh_gl_point_is_greater(self, other);
}

lh_void
lh_gl_size_min(const lh_gl_size_t *a, const lh_gl_size_t *b, lh_gl_size_t *out)
{
    lh_gl_point_min(a, b, out);
}

lh_void
lh_gl_size_max(const lh_gl_size_t *a, const lh_gl_size_t *b, lh_gl_size_t *out)
{
    lh_gl_point_max(a, b, out);
}

lh_void
lh_gl_size_from_extent(const lh_gl_point_t *min, const lh_gl_point_t *max, lh_gl_size_t *out)
{
    lh_gl_size_init(out, lh_gl_point_get_x(max) - lh_gl_point_get_x(min),
                    lh_gl_point_get_y(max) - lh_gl_point_get_y(min));
}

lh_bool_t
lh_gl_size_is_empty(const lh_gl_size_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_gl_size_get_width(self) <= 0 || lh_gl_size_get_height(self) <= 0);
}

lh_math_scalar_t
lh_gl_size_min_side(const lh_gl_size_t *self)
{
    return lh_math_min(lh_gl_size_get_width(self), lh_gl_size_get_height(self));
}

lh_void
lh_gl_size_inset(const lh_gl_size_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                 lh_gl_size_t *out)
{
    lh_gl_size_init(out, lh_gl_size_get_width(self) - dx - dx,
                    lh_gl_size_get_height(self) - dy - dy);
}