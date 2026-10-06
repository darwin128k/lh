/**
 * @file irect.c
 * @brief Implementation of lh/math/irect.h.
 *
 * All ops are `lh_math_iscalar_t` arithmetic — no allocations, no state. Empty
 * rectangles are represented as `size.width <= 0 || size.height <= 0`:
 * ::lh_math_irect_is_empty, ::lh_math_irect_intersection, and
 * ::lh_math_irect_get_width / ::lh_math_irect_get_height all share that definition, so
 * an "empty" output is canonical regardless of which operation produced it.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math/irect.h>
#include <lh/math.h>
#include <lh/util/return.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_math_irect_t
lh_math_irect_make(lh_math_iscalar_t x, lh_math_iscalar_t y, lh_math_iscalar_t width, lh_math_iscalar_t height)
{
    lh_math_irect_t r;
    lh_math_irect_set_origin(lh_addr_of(r), lh_math_ipoint_make(x, y));
    lh_math_irect_set_size(lh_addr_of(r), lh_math_isize_make(width, height));
    return r;
}

lh_math_irect_t
lh_math_irect_from_min_max(lh_math_iscalar_t x_min, lh_math_iscalar_t y_min,
                          lh_math_iscalar_t x_max, lh_math_iscalar_t y_max)
{
    lh_math_irect_t r;
    lh_math_irect_set_origin(lh_addr_of(r), lh_math_ipoint_make(x_min, y_min));
    lh_math_irect_set_size(lh_addr_of(r), lh_math_isize_make(x_max - x_min, y_max - y_min));
    return r;
}

lh_math_irect_t
lh_math_irect_make_empty(void)
{
    return lh_math_irect_make(0, 0, 0, 0);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_math_ipoint_t
lh_math_irect_get_origin(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->origin;
}

lh_math_isize_t
lh_math_irect_get_size(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_math_iscalar_t
lh_math_irect_get_x(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_ipoint_get_x(lh_addr_of(self->origin));
}

lh_math_iscalar_t
lh_math_irect_get_y(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_ipoint_get_y(lh_addr_of(self->origin));
}

lh_math_iscalar_t
lh_math_irect_get_size_width(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_isize_get_width(lh_addr_of(self->size));
}

lh_math_iscalar_t
lh_math_irect_get_size_height(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_isize_get_height(lh_addr_of(self->size));
}

lh_void
lh_math_irect_set_origin(lh_math_irect_t *self, lh_math_ipoint_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

lh_void
lh_math_irect_set_size(lh_math_irect_t *self, lh_math_isize_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

/* ── Queries ────────────────────────────────────────────────────────────── */

lh_math_iscalar_t
lh_math_irect_get_width(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_math_irect_is_empty(self))
    {
        return 0;
    }
    return lh_math_irect_get_size_width(self);
}

lh_math_iscalar_t
lh_math_irect_get_height(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_math_irect_is_empty(self))
    {
        return 0;
    }
    return lh_math_irect_get_size_height(self);
}

lh_bool_t
lh_math_irect_is_empty(const lh_math_irect_t *self)
{
    lh_assert_runtime_ref(self);
    return (lh_math_irect_get_size_width(self) <= 0)
        || (lh_math_irect_get_size_height(self) <= 0);
}

lh_bool_t
lh_math_irect_contains_point(const lh_math_irect_t *self, lh_math_ipoint_t point)
{
    lh_assert_runtime_ref(self);
    if (lh_math_irect_is_empty(self))
    {
        return lh_bool_false;
    }
    if (lh_math_ipoint_get_x(lh_addr_of(point)) < lh_math_irect_get_x(self))
    {
        return lh_bool_false;
    }
    if (lh_math_ipoint_get_y(lh_addr_of(point)) < lh_math_irect_get_y(self))
    {
        return lh_bool_false;
    }
    if (lh_math_ipoint_get_x(lh_addr_of(point)) >= lh_math_irect_get_x(self) + lh_math_irect_get_size_width(self))
    {
        return lh_bool_false;
    }
    if (lh_math_ipoint_get_y(lh_addr_of(point)) >= lh_math_irect_get_y(self) + lh_math_irect_get_size_height(self))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_math_irect_intersects(const lh_math_irect_t *a, const lh_math_irect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_irect_is_empty(a) || lh_math_irect_is_empty(b))
    {
        return lh_bool_false;
    }
    if (lh_math_irect_get_x(a) + lh_math_irect_get_size_width(a) <= lh_math_irect_get_x(b))
    {
        return lh_bool_false;
    }
    if (lh_math_irect_get_y(a) + lh_math_irect_get_size_height(a) <= lh_math_irect_get_y(b))
    {
        return lh_bool_false;
    }
    if (lh_math_irect_get_x(b) + lh_math_irect_get_size_width(b) <= lh_math_irect_get_x(a))
    {
        return lh_bool_false;
    }
    if (lh_math_irect_get_y(b) + lh_math_irect_get_size_height(b) <= lh_math_irect_get_y(a))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_math_irect_eq(const lh_math_irect_t *a, const lh_math_irect_t *b)
{
    lh_return_if(a == b, lh_bool_true);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    return lh_math_eq(lh_math_irect_get_x(a), lh_math_irect_get_x(b))
        && lh_math_eq(lh_math_irect_get_y(a), lh_math_irect_get_y(b))
        && lh_math_eq(lh_math_irect_get_size_width(a), lh_math_irect_get_size_width(b))
        && lh_math_eq(lh_math_irect_get_size_height(a), lh_math_irect_get_size_height(b));
}

lh_math_irect_t
lh_math_irect_intersection(const lh_math_irect_t *a, const lh_math_irect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (!lh_math_irect_intersects(a, b))
    {
        return lh_math_irect_make_empty();
    }
    lh_math_iscalar_t x1 = lh_math_max(lh_math_irect_get_x(a), lh_math_irect_get_x(b));
    lh_math_iscalar_t y1 = lh_math_max(lh_math_irect_get_y(a), lh_math_irect_get_y(b));
    lh_math_iscalar_t x2 = lh_math_min(lh_math_irect_get_x(a) + lh_math_irect_get_size_width(a),
                                     lh_math_irect_get_x(b) + lh_math_irect_get_size_width(b));
    lh_math_iscalar_t y2 = lh_math_min(lh_math_irect_get_y(a) + lh_math_irect_get_size_height(a),
                                     lh_math_irect_get_y(b) + lh_math_irect_get_size_height(b));
    return lh_math_irect_make(x1, y1, x2 - x1, y2 - y1);
}

lh_math_irect_t
lh_math_irect_union(const lh_math_irect_t *a, const lh_math_irect_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    if (lh_math_irect_is_empty(a))
    {
        return *b;
    }
    if (lh_math_irect_is_empty(b))
    {
        return *a;
    }
    lh_math_iscalar_t x1 = lh_math_min(lh_math_irect_get_x(a), lh_math_irect_get_x(b));
    lh_math_iscalar_t y1 = lh_math_min(lh_math_irect_get_y(a), lh_math_irect_get_y(b));
    lh_math_iscalar_t x2 = lh_math_max(lh_math_irect_get_x(a) + lh_math_irect_get_size_width(a),
                                     lh_math_irect_get_x(b) + lh_math_irect_get_size_width(b));
    lh_math_iscalar_t y2 = lh_math_max(lh_math_irect_get_y(a) + lh_math_irect_get_size_height(a),
                                     lh_math_irect_get_y(b) + lh_math_irect_get_size_height(b));
    return lh_math_irect_make(x1, y1, x2 - x1, y2 - y1);
}

lh_math_irect_t
lh_math_irect_offset(const lh_math_irect_t *self, lh_math_iscalar_t dx, lh_math_iscalar_t dy)
{
    lh_assert_runtime_ref(self);
    return lh_math_irect_make(lh_math_irect_get_x(self) + dx,
                             lh_math_irect_get_y(self) + dy,
                             lh_math_irect_get_size_width(self),
                             lh_math_irect_get_size_height(self));
}

lh_math_irect_t
lh_math_irect_inset(const lh_math_irect_t *self, lh_math_iscalar_t dx, lh_math_iscalar_t dy)
{
    lh_assert_runtime_ref(self);
    lh_math_iscalar_t x = lh_math_irect_get_x(self) + dx;
    lh_math_iscalar_t y = lh_math_irect_get_y(self) + dy;
    lh_math_iscalar_t w = lh_math_irect_get_size_width(self) - dx - dx;
    lh_math_iscalar_t h = lh_math_irect_get_size_height(self) - dy - dy;
    return lh_math_irect_make(x, y, w, h);
}