/**
 * @file rect.c
 * @brief Implementation of lh/gl/rect.h.
 *
 * A thin composition of ::lh_gl_point_t and ::lh_gl_size_t: only the four
 * level-1 accessors name a field, everything else uses them and the methods of
 * the parts. Empty rectangles follow ::lh_gl_size_is_empty on the size.
 */

#include <lh/core/assert/runtime.h>
#include <lh/gl/rect.h>

/* -- Accessors ------------------------------------------------------------- */

lh_gl_point_t *
lh_gl_rect_get_origin_mut(lh_gl_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

const lh_gl_point_t *
lh_gl_rect_get_origin(const lh_gl_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_gl_size_t *
lh_gl_rect_get_size_mut(lh_gl_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_gl_size_t *
lh_gl_rect_get_size(const lh_gl_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

/* -- Init and queries, by level -------------------------------------------- */

lh_void
lh_gl_rect_set_origin(lh_gl_rect_t *self, lh_gl_point_t origin)
{
    lh_ptr_deref(lh_gl_rect_get_origin_mut(self)) = origin;
}

lh_void
lh_gl_rect_set_size(lh_gl_rect_t *self, lh_gl_size_t size)
{
    lh_ptr_deref(lh_gl_rect_get_size_mut(self)) = size;
}

lh_void
lh_gl_rect_get_end(const lh_gl_rect_t *self, lh_gl_point_t *out)
{
    const lh_gl_size_t *size = lh_gl_rect_get_size(self);

    lh_gl_point_offset(lh_gl_rect_get_origin(self), lh_gl_size_get_width(size),
                       lh_gl_size_get_height(size), out);
}

lh_void
lh_gl_rect_get_origin_min(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out)
{
    lh_gl_point_min(lh_gl_rect_get_origin(a), lh_gl_rect_get_origin(b), out);
}

lh_void
lh_gl_rect_get_origin_max(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out)
{
    lh_gl_point_max(lh_gl_rect_get_origin(a), lh_gl_rect_get_origin(b), out);
}

lh_bool_t
lh_gl_rect_is_empty(const lh_gl_rect_t *self)
{
    return lh_gl_size_is_empty(lh_gl_rect_get_size(self));
}

lh_bool_t
lh_gl_rect_eq(const lh_gl_rect_t *a, const lh_gl_rect_t *b)
{
    return lh_cast_static(lh_bool_t,
                          lh_gl_point_eq(lh_gl_rect_get_origin(a), lh_gl_rect_get_origin(b)) &&
                              lh_gl_size_eq(lh_gl_rect_get_size(a), lh_gl_rect_get_size(b)));
}

lh_bool_t
lh_gl_rect_is_at_least(const lh_gl_rect_t *self, const lh_gl_rect_t *minimum)
{
    const lh_gl_point_t *self_origin = lh_gl_rect_get_origin(self);
    const lh_gl_point_t *minimum_origin = lh_gl_rect_get_origin(minimum);

    if (!lh_gl_point_is_at_least(self_origin, minimum_origin))
    {
        return lh_bool_false;
    }
    if (lh_gl_point_is_greater(self_origin, minimum_origin))
    {
        return lh_bool_true;
    }
    return lh_gl_size_is_at_least(lh_gl_rect_get_size(self), lh_gl_rect_get_size(minimum));
}

lh_void
lh_gl_rect_init_origin_size(lh_gl_rect_t *self, lh_gl_point_t origin, lh_gl_size_t size)
{
    lh_gl_rect_set_origin(self, origin);
    lh_gl_rect_set_size(self, size);
}

lh_void
lh_gl_rect_get_end_min(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out)
{
    lh_gl_point_t end_a;

    lh_gl_rect_get_end(a, lh_addr_of(end_a));
    lh_gl_point_t end_b;

    lh_gl_rect_get_end(b, lh_addr_of(end_b));
    lh_gl_point_min(lh_addr_of(end_a), lh_addr_of(end_b), out);
}

lh_void
lh_gl_rect_get_end_max(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_point_t *out)
{
    lh_gl_point_t end_a;

    lh_gl_rect_get_end(a, lh_addr_of(end_a));
    lh_gl_point_t end_b;

    lh_gl_rect_get_end(b, lh_addr_of(end_b));
    lh_gl_point_max(lh_addr_of(end_a), lh_addr_of(end_b), out);
}

lh_bool_t
lh_gl_rect_contains_point(const lh_gl_rect_t *self, lh_gl_point_t point)
{
    if (lh_gl_rect_is_empty(self))
    {
        return lh_bool_false;
    }
    lh_gl_point_t end;

    lh_gl_rect_get_end(self, lh_addr_of(end));
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(self);
    const lh_math_scalar_t x = lh_gl_point_get_x(lh_addr_of(point));
    const lh_math_scalar_t y = lh_gl_point_get_y(lh_addr_of(point));

    return lh_cast_static(lh_bool_t, x >= lh_gl_point_get_x(origin) &&
                                         y >= lh_gl_point_get_y(origin) &&
                                         x < lh_gl_point_get_x(lh_addr_of(end)) &&
                                         y < lh_gl_point_get_y(lh_addr_of(end)));
}

lh_bool_t
lh_gl_rect_is_less(const lh_gl_rect_t *self, const lh_gl_rect_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_gl_rect_is_at_least(self, other));
}

lh_void
lh_gl_rect_init(lh_gl_rect_t *self, lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width,
                lh_math_scalar_t height)
{
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), x, y);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), width, height);
    lh_gl_rect_init_origin_size(self, origin, size);
}

lh_bool_t
lh_gl_rect_is_greater(const lh_gl_rect_t *self, const lh_gl_rect_t *other)
{
    return lh_gl_rect_is_less(other, self);
}

lh_void
lh_gl_rect_offset(const lh_gl_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                  lh_gl_rect_t *out)
{
    lh_gl_point_t moved;

    lh_gl_point_offset(lh_gl_rect_get_origin(self), dx, dy, lh_addr_of(moved));
    lh_gl_rect_init_origin_size(out, moved, lh_ptr_deref(lh_gl_rect_get_size(self)));
}

lh_void
lh_gl_rect_init_empty(lh_gl_rect_t *self)
{
    lh_gl_rect_init(self, 0, 0, 0, 0);
}

lh_void
lh_gl_rect_from_extent(const lh_gl_point_t *min, const lh_gl_point_t *max, lh_gl_rect_t *out)
{
    lh_gl_size_t size;

    lh_gl_size_from_extent(min, max, lh_addr_of(size));
    if (lh_gl_size_is_empty(lh_addr_of(size)))
    {
        lh_gl_rect_init_empty(out);
        return;
    }
    lh_gl_rect_init_origin_size(out, lh_ptr_deref(min), size);
}

lh_void
lh_gl_rect_from_min_max(lh_math_scalar_t x_min, lh_math_scalar_t y_min, lh_math_scalar_t x_max,
                        lh_math_scalar_t y_max, lh_gl_rect_t *out)
{
    lh_gl_point_t min;

    lh_gl_point_init(lh_addr_of(min), x_min, y_min);
    lh_gl_point_t max;

    lh_gl_point_init(lh_addr_of(max), x_max, y_max);
    lh_gl_rect_from_extent(lh_addr_of(min), lh_addr_of(max), out);
}

lh_void
lh_gl_rect_intersection(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_rect_t *out)
{
    if (lh_gl_rect_is_empty(a) || lh_gl_rect_is_empty(b))
    {
        lh_gl_rect_init_empty(out);
        return;
    }
    lh_gl_point_t origin;

    lh_gl_rect_get_origin_max(a, b, lh_addr_of(origin));
    lh_gl_point_t end;

    lh_gl_rect_get_end_min(a, b, lh_addr_of(end));
    lh_gl_rect_from_extent(lh_addr_of(origin), lh_addr_of(end), out);
}

lh_void
lh_gl_rect_union(const lh_gl_rect_t *a, const lh_gl_rect_t *b, lh_gl_rect_t *out)
{
    if (lh_gl_rect_is_empty(a))
    {
        lh_gl_rect_init_origin_size(out, lh_ptr_deref(lh_gl_rect_get_origin(b)),
                                    lh_ptr_deref(lh_gl_rect_get_size(b)));
        return;
    }
    if (lh_gl_rect_is_empty(b))
    {
        lh_gl_rect_init_origin_size(out, lh_ptr_deref(lh_gl_rect_get_origin(a)),
                                    lh_ptr_deref(lh_gl_rect_get_size(a)));
        return;
    }
    lh_gl_point_t origin;

    lh_gl_rect_get_origin_min(a, b, lh_addr_of(origin));
    lh_gl_point_t end;

    lh_gl_rect_get_end_max(a, b, lh_addr_of(end));
    lh_gl_rect_from_extent(lh_addr_of(origin), lh_addr_of(end), out);
}

lh_bool_t
lh_gl_rect_intersects(const lh_gl_rect_t *a, const lh_gl_rect_t *b)
{
    lh_gl_rect_t overlap;

    lh_gl_rect_intersection(a, b, lh_addr_of(overlap));
    return lh_cast_static(lh_bool_t, !lh_gl_rect_is_empty(lh_addr_of(overlap)));
}

lh_void
lh_gl_rect_inset(const lh_gl_rect_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy,
                 lh_gl_rect_t *out)
{
    lh_gl_size_t size;

    lh_gl_size_inset(lh_gl_rect_get_size(self), dx, dy, lh_addr_of(size));
    lh_gl_rect_offset(self, dx, dy, out);
    lh_gl_rect_set_size(out, size);
}