/**
 * @file rect.c
 * @brief Implementation of `lh/math/rect.h`.
 *
 * All ops are `lh_math_coord_t` arithmetic — no allocations, no state. Empty
 * rectangles are represented as `size.width <= 0 || size.height <= 0`:
 * ::lh_math_rect_is_empty, ::lh_math_rect_intersection, and
 * ::lh_math_rect_width / ::lh_math_rect_height all share that definition, so
 * an "empty" output is canonical regardless of which operation produced it.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math/rect.h>
#include <lh/math.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_rect_t
lh_math_rect_make(lh_math_coord_t x, lh_math_coord_t y, lh_math_coord_t width, lh_math_coord_t height)
{
    lh_math_rect_t r;
    lh_math_rect_set_origin(lh_addr_of(r), lh_math_point_make(x, y));
    lh_math_rect_set_size(lh_addr_of(r), lh_math_size_make(width, height));
    return r;
}

lh_math_rect_t
lh_math_rect_from_min_max(lh_math_coord_t x_min, lh_math_coord_t y_min,
                          lh_math_coord_t x_max, lh_math_coord_t y_max)
{
    lh_math_rect_t r;
    lh_math_rect_set_origin(lh_addr_of(r), lh_math_point_make(x_min, y_min));
    lh_math_rect_set_size(lh_addr_of(r), lh_math_size_make(x_max - x_min, y_max - y_min));
    return r;
}

lh_math_rect_t
lh_math_rect_make_empty(void)
{
    return lh_math_rect_make(0, 0, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_point_t
lh_math_rect_get_origin(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->origin;
}

lh_math_size_t
lh_math_rect_get_size(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_math_coord_t
lh_math_rect_get_x(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_point_get_x(lh_addr_of(self->origin));
}

lh_math_coord_t
lh_math_rect_get_y(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_point_get_y(lh_addr_of(self->origin));
}

lh_math_coord_t
lh_math_rect_get_size_width(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_size_get_width(lh_addr_of(self->size));
}

lh_math_coord_t
lh_math_rect_get_size_height(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_size_get_height(lh_addr_of(self->size));
}

lh_void
lh_math_rect_set_origin(lh_math_rect_t *self, lh_math_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

lh_void
lh_math_rect_set_size(lh_math_rect_t *self, lh_math_size_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_math_coord_t
lh_math_rect_width(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_math_rect_is_empty(self))
    {
        return 0;
    }
    return lh_math_rect_get_size_width(self);
}

lh_math_coord_t
lh_math_rect_height(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_math_rect_is_empty(self))
    {
        return 0;
    }
    return lh_math_rect_get_size_height(self);
}

lh_math_point_t
lh_math_rect_origin(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_get_origin(self);
}

lh_bool_t
lh_math_rect_is_empty(const lh_math_rect_t *self)
{
    lh_assert_runtime_ref(self);
    return (lh_math_rect_get_size_width(self) <= 0)
        || (lh_math_rect_get_size_height(self) <= 0);
}

lh_bool_t
lh_math_rect_contains_point(const lh_math_rect_t *self, lh_math_point_t point)
{
    lh_assert_runtime_ref(self);
    if (lh_math_rect_is_empty(self))
    {
        return lh_bool_false;
    }
    if (lh_math_point_get_x(lh_addr_of(point)) < lh_math_rect_get_x(self))
    {
        return lh_bool_false;
    }
    if (lh_math_point_get_y(lh_addr_of(point)) < lh_math_rect_get_y(self))
    {
        return lh_bool_false;
    }
    if (lh_math_point_get_x(lh_addr_of(point)) >= lh_math_rect_get_x(self) + lh_math_rect_get_size_width(self))
    {
        return lh_bool_false;
    }
    if (lh_math_point_get_y(lh_addr_of(point)) >= lh_math_rect_get_y(self) + lh_math_rect_get_size_height(self))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_math_rect_intersects(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_rect_is_empty(a) || lh_math_rect_is_empty(b))
    {
        return lh_bool_false;
    }
    if (lh_math_rect_get_x(a) + lh_math_rect_get_size_width(a) <= lh_math_rect_get_x(b))
    {
        return lh_bool_false;
    }
    if (lh_math_rect_get_y(a) + lh_math_rect_get_size_height(a) <= lh_math_rect_get_y(b))
    {
        return lh_bool_false;
    }
    if (lh_math_rect_get_x(b) + lh_math_rect_get_size_width(b) <= lh_math_rect_get_x(a))
    {
        return lh_bool_false;
    }
    if (lh_math_rect_get_y(b) + lh_math_rect_get_size_height(b) <= lh_math_rect_get_y(a))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_math_rect_eq(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    if (a == b)
    {
        return lh_bool_true;
    }
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_rect_get_x(a), lh_math_rect_get_x(b))
        && lh_math_eq(lh_math_rect_get_y(a), lh_math_rect_get_y(b))
        && lh_math_eq(lh_math_rect_get_size_width(a), lh_math_rect_get_size_width(b))
        && lh_math_eq(lh_math_rect_get_size_height(a), lh_math_rect_get_size_height(b));
}

lh_math_rect_t
lh_math_rect_intersection(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (!lh_math_rect_intersects(a, b))
    {
        return lh_math_rect_make_empty();
    }
    lh_math_coord_t x1 = lh_math_max(lh_math_rect_get_x(a), lh_math_rect_get_x(b));
    lh_math_coord_t y1 = lh_math_max(lh_math_rect_get_y(a), lh_math_rect_get_y(b));
    lh_math_coord_t x2 = lh_math_min(lh_math_rect_get_x(a) + lh_math_rect_get_size_width(a),
                                     lh_math_rect_get_x(b) + lh_math_rect_get_size_width(b));
    lh_math_coord_t y2 = lh_math_min(lh_math_rect_get_y(a) + lh_math_rect_get_size_height(a),
                                     lh_math_rect_get_y(b) + lh_math_rect_get_size_height(b));
    return lh_math_rect_make(x1, y1, x2 - x1, y2 - y1);
}

lh_math_rect_t
lh_math_rect_union(const lh_math_rect_t *a, const lh_math_rect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_rect_is_empty(a))
    {
        return *b;
    }
    if (lh_math_rect_is_empty(b))
    {
        return *a;
    }
    lh_math_coord_t x1 = lh_math_min(lh_math_rect_get_x(a), lh_math_rect_get_x(b));
    lh_math_coord_t y1 = lh_math_min(lh_math_rect_get_y(a), lh_math_rect_get_y(b));
    lh_math_coord_t x2 = lh_math_max(lh_math_rect_get_x(a) + lh_math_rect_get_size_width(a),
                                     lh_math_rect_get_x(b) + lh_math_rect_get_size_width(b));
    lh_math_coord_t y2 = lh_math_max(lh_math_rect_get_y(a) + lh_math_rect_get_size_height(a),
                                     lh_math_rect_get_y(b) + lh_math_rect_get_size_height(b));
    return lh_math_rect_make(x1, y1, x2 - x1, y2 - y1);
}

lh_math_rect_t
lh_math_rect_offset(const lh_math_rect_t *self, lh_math_coord_t dx, lh_math_coord_t dy)
{
    lh_assert_runtime_ref(self);
    return lh_math_rect_make(lh_math_rect_get_x(self) + dx,
                             lh_math_rect_get_y(self) + dy,
                             lh_math_rect_get_size_width(self),
                             lh_math_rect_get_size_height(self));
}

lh_math_rect_t
lh_math_rect_inset(const lh_math_rect_t *self, lh_math_coord_t dx, lh_math_coord_t dy)
{
    lh_assert_runtime_ref(self);
    lh_math_coord_t x = lh_math_rect_get_x(self) + dx;
    lh_math_coord_t y = lh_math_rect_get_y(self) + dy;
    lh_math_coord_t w = lh_math_rect_get_size_width(self) - dx - dx;
    lh_math_coord_t h = lh_math_rect_get_size_height(self) - dy - dy;
    return lh_math_rect_make(x, y, w, h);
}