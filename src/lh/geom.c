/**
 * @file geom.c
 * @brief Implementation of `lh/geom.h` (point, size, rect) and
 *        `lh/color.h` (RGBA).
 *
 * One file: both modules are tiny, related (rect intersects uses point
 * ops), and pure value arithmetic — splitting them would cost more in
 * indirection than it saves in build parallelism.
 *
 * All ops are `lh_coord_t` / `lh_uchar_t` arithmetic — no allocations, no
 * state. Empty rectangles are represented as `size.width <= 0 ||
 * size.height <= 0`; ::lh_rect_is_empty, ::lh_rect_intersection, and
 * ::lh_rect_width / ::lh_rect_height all share that definition, so an
 * "empty" output is canonical regardless of which operation produced it.
 */

#include <lh/bool.h>
#include <lh/color.h>
#include <lh/geom.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/util/math.h>

/* ── point / size ────────────────────────────────────────────────────────── */

lh_point_t
lh_point_make(lh_coord_t x, lh_coord_t y)
{
    lh_point_t p = { x, y };
    return p;
}

lh_size_t
lh_size_make(lh_coord_t width, lh_coord_t height)
{
    lh_size_t s = { width, height };
    return s;
}

/* ── rect ────────────────────────────────────────────────────────────────── */

lh_rect_t
lh_rect_make(lh_coord_t x, lh_coord_t y, lh_coord_t width, lh_coord_t height)
{
    lh_rect_t r;
    r.origin = lh_point_make(x, y);
    r.size = lh_size_make(width, height);
    return r;
}

lh_rect_t
lh_rect_from_min_max(lh_coord_t x_min, lh_coord_t y_min,
                      lh_coord_t x_max, lh_coord_t y_max)
{
    lh_rect_t r;
    r.origin = lh_point_make(x_min, y_min);
    r.size = lh_size_make(x_max - x_min, y_max - y_min);
    return r;
}

lh_rect_t
lh_rect_zero(void)
{
    return lh_rect_make(0, 0, 0, 0);
}

lh_coord_t
lh_rect_width(const lh_rect_t *self)
{
    if (lh_null_eq(self) || lh_rect_is_empty(self))
    {
        return 0;
    }
    return self->size.width;
}

lh_coord_t
lh_rect_height(const lh_rect_t *self)
{
    if (lh_null_eq(self) || lh_rect_is_empty(self))
    {
        return 0;
    }
    return self->size.height;
}

lh_point_t
lh_rect_origin(const lh_rect_t *self)
{
    if (lh_null_eq(self))
    {
        return lh_point_make(0, 0);
    }
    return self->origin;
}

lh_bool_t
lh_rect_is_empty(const lh_rect_t *self)
{
    if (lh_null_eq(self))
    {
        return lh_bool_true;
    }
    return (self->size.width <= 0) || (self->size.height <= 0);
}

lh_bool_t
lh_rect_contains_point(const lh_rect_t *self, lh_point_t point)
{
    if (lh_rect_is_empty(self))
    {
        return lh_bool_false;
    }
    if (point.x < self->origin.x)
    {
        return lh_bool_false;
    }
    if (point.y < self->origin.y)
    {
        return lh_bool_false;
    }
    if (point.x >= self->origin.x + self->size.width)
    {
        return lh_bool_false;
    }
    if (point.y >= self->origin.y + self->size.height)
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_rect_intersects(const lh_rect_t *a, const lh_rect_t *b)
{
    if (lh_null_eq(a) || lh_null_eq(b))
    {
        return lh_bool_false;
    }
    if (lh_rect_is_empty(a) || lh_rect_is_empty(b))
    {
        return lh_bool_false;
    }
    if (a->origin.x + a->size.width <= b->origin.x)
    {
        return lh_bool_false;
    }
    if (a->origin.y + a->size.height <= b->origin.y)
    {
        return lh_bool_false;
    }
    if (b->origin.x + b->size.width <= a->origin.x)
    {
        return lh_bool_false;
    }
    if (b->origin.y + b->size.height <= a->origin.y)
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_rect_eq(const lh_rect_t *a, const lh_rect_t *b)
{
    if (a == b)
    {
        return lh_bool_true;
    }
    if (lh_null_eq(a) || lh_null_eq(b))
    {
        return lh_bool_false;
    }
    return lh_math_eq(a->origin.x, b->origin.x)
        && lh_math_eq(a->origin.y, b->origin.y)
        && lh_math_eq(a->size.width, b->size.width)
        && lh_math_eq(a->size.height, b->size.height);
}

lh_rect_t
lh_rect_intersection(const lh_rect_t *a, const lh_rect_t *b)
{
    if (lh_null_eq(a) || lh_null_eq(b))
    {
        return lh_rect_zero();
    }
    if (!lh_rect_intersects(a, b))
    {
        return lh_rect_zero();
    }
    lh_coord_t x1 = lh_math_max(a->origin.x, b->origin.x);
    lh_coord_t y1 = lh_math_max(a->origin.y, b->origin.y);
    lh_coord_t x2 = lh_math_min(a->origin.x + a->size.width,
                                  b->origin.x + b->size.width);
    lh_coord_t y2 = lh_math_min(a->origin.y + a->size.height,
                                  b->origin.y + b->size.height);
    return lh_rect_make(x1, y1, x2 - x1, y2 - y1);
}

lh_rect_t
lh_rect_union(const lh_rect_t *a, const lh_rect_t *b)
{
    if (lh_null_eq(a))
    {
        return lh_null_eq(b) ? lh_rect_zero() : *b;
    }
    if (lh_null_eq(b))
    {
        return *a;
    }
    if (lh_rect_is_empty(a))
    {
        return *b;
    }
    if (lh_rect_is_empty(b))
    {
        return *a;
    }
    lh_coord_t x1 = lh_math_min(a->origin.x, b->origin.x);
    lh_coord_t y1 = lh_math_min(a->origin.y, b->origin.y);
    lh_coord_t x2 = lh_math_max(a->origin.x + a->size.width,
                                  b->origin.x + b->size.width);
    lh_coord_t y2 = lh_math_max(a->origin.y + a->size.height,
                                  b->origin.y + b->size.height);
    return lh_rect_make(x1, y1, x2 - x1, y2 - y1);
}

lh_rect_t
lh_rect_offset(const lh_rect_t *self, lh_coord_t dx, lh_coord_t dy)
{
    if (lh_null_eq(self))
    {
        return lh_rect_zero();
    }
    return lh_rect_make(self->origin.x + dx, self->origin.y + dy,
                         self->size.width, self->size.height);
}

lh_rect_t
lh_rect_inset(const lh_rect_t *self, lh_coord_t dx, lh_coord_t dy)
{
    if (lh_null_eq(self))
    {
        return lh_rect_zero();
    }
    lh_coord_t x = self->origin.x + dx;
    lh_coord_t y = self->origin.y + dy;
    lh_coord_t w = self->size.width - dx - dx;
    lh_coord_t h = self->size.height - dy - dy;
    return lh_rect_make(x, y, w, h);
}

/* ── color ──────────────────────────────────────────────────────────────── */

lh_color_t
lh_color_make(lh_uchar_t r, lh_uchar_t g, lh_uchar_t b, lh_uchar_t a)
{
    lh_color_t c = { r, g, b, a };
    return c;
}

lh_color_t
lh_color_from_argb(lh_uint_t argb)
{
    lh_color_t c;
    c.a = lh_cast_static(lh_uchar_t, (argb >> 24) & 0xFFu);
    c.r = lh_cast_static(lh_uchar_t, (argb >> 16) & 0xFFu);
    c.g = lh_cast_static(lh_uchar_t, (argb >> 8) & 0xFFu);
    c.b = lh_cast_static(lh_uchar_t, argb & 0xFFu);
    return c;
}

lh_uint_t
lh_color_to_argb(lh_color_t self)
{
    return ((lh_uint_t)self.a << 24)
        | ((lh_uint_t)self.r << 16)
        | ((lh_uint_t)self.g << 8)
        | ((lh_uint_t)self.b);
}