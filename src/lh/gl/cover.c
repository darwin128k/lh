/**
 * @file cover.c
 * @brief Pixel-center tests for a rounded rectangle, a circle and a triangle.
 */
#include <lh/gl/cover.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math/isqrt.h>

lh_bool_t
lh_gl_cover_hits(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t slop,
                 lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    lh_gl_point_t point;

    lh_assert_runtime_ref(rect);
    if (slop < 0)
    {
        slop = 0;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    lh_gl_point_init(lh_addr_of(point), x, y);
    if (radius == 0 && slop == 0)
    {
        return lh_gl_rect_contains_point(rect, point);
    }
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    lh_gl_rect_t grown;

    lh_gl_rect_init(lh_addr_of(grown), lh_gl_point_get_x(origin) - slop,
                    lh_gl_point_get_y(origin) - slop, lh_gl_size_get_width(size) + slop + slop,
                    lh_gl_size_get_height(size) + slop + slop);
    if (radius == 0)
    {
        return lh_gl_rect_contains_point(lh_addr_of(grown), point);
    }
    return lh_type_cast(lh_bool_t, lh_gl_cover_round_rect(lh_addr_of(grown), radius + slop, x, y) !=
                                       0);
}

lh_gl_color_channel_t
lh_gl_cover_round_rect(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t x,
                       lh_gl_scalar_t y)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    lh_gl_scalar_t left = lh_gl_point_get_x(origin);
    lh_gl_scalar_t top = lh_gl_point_get_y(origin);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    if (x < left || y < top || x >= left + width || y >= top + height)
    {
        return 0;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    lh_gl_scalar_t limit = lh_math_min(width, height) / 2;

    if (radius > limit)
    {
        radius = limit;
    }
    if (radius == 0)
    {
        return 255;
    }
    lh_gl_scalar_t dx = x - left;
    lh_gl_scalar_t dy = y - top;
    lh_gl_scalar_t cx = left;
    lh_gl_scalar_t cy = top;

    if (dx >= radius && dx < width - radius)
    {
        return 255;
    }
    if (dy >= radius && dy < height - radius)
    {
        return 255;
    }
    if (dx >= radius)
    {
        cx = left + width - radius;
    }
    else
    {
        cx = left + radius;
    }
    if (dy >= radius)
    {
        cy = top + height - radius;
    }
    else
    {
        cy = top + radius;
    }
    lh_s64_t px = 2 * lh_cast_static(lh_s64_t, x) + 1 - 2 * lh_cast_static(lh_s64_t, cx);
    lh_s64_t py = 2 * lh_cast_static(lh_s64_t, y) + 1 - 2 * lh_cast_static(lh_s64_t, cy);
    lh_s64_t reach = 2 * lh_cast_static(lh_s64_t, radius);

    if (px * px + py * py <= reach * reach)
    {
        return 255;
    }
    return 0;
}

lh_bool_t
lh_gl_cover_round_rect_span(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_scalar_t y,
                            lh_gl_scalar_t *x0, lh_gl_scalar_t *x1)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(x0);
    lh_assert_runtime_ref(x1);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    lh_gl_scalar_t left = lh_gl_point_get_x(origin);
    lh_gl_scalar_t top = lh_gl_point_get_y(origin);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(rect));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(rect));

    if (y < top || y >= top + height || width <= 0)
    {
        return lh_bool_false;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    lh_gl_scalar_t limit_r = lh_math_min(width, height) / 2;

    if (radius > limit_r)
    {
        radius = limit_r;
    }
    if (radius == 0 || (y >= top + radius && y < top + height - radius))
    {
        lh_ptr_deref(x0) = left;
        lh_ptr_deref(x1) = left + width;
        return lh_bool_true;
    }
    {
        lh_gl_scalar_t cy = y < top + radius ? top + radius : top + height - radius;
        lh_s64_t py = 2 * lh_cast_static(lh_s64_t, y) + 1 - 2 * lh_cast_static(lh_s64_t, cy);
        lh_s64_t reach = 2 * lh_cast_static(lh_s64_t, radius);
        lh_s64_t remain = reach * reach - py * py;

        if (remain <= 0)
        {
            return lh_bool_false;
        }
        lh_s64_t limit =
            lh_cast_static(lh_s64_t, lh_math_isqrt_u64(lh_cast_static(lh_u64_t, remain)));

        if (limit <= 0)
        {
            return lh_bool_false;
        }
        lh_gl_scalar_t start = left + radius + lh_cast_static(lh_gl_scalar_t, (-limit - 1) / 2);
        lh_gl_scalar_t end =
            left + width - radius + lh_cast_static(lh_gl_scalar_t, (limit - 1) / 2) + 1;

        if (start < left)
        {
            start = left;
        }
        if (end > left + width)
        {
            end = left + width;
        }
        if (start >= end)
        {
            return lh_bool_false;
        }
        lh_ptr_deref(x0) = start;
        lh_ptr_deref(x1) = end;
    }
    return lh_bool_true;
}

lh_gl_color_channel_t
lh_gl_cover_circle(const lh_gl_point_t *center, lh_gl_scalar_t radius, lh_gl_scalar_t x,
                   lh_gl_scalar_t y)
{
    if (radius < 1)
    {
        return 0;
    }
    lh_s64_t px = 2 * lh_cast_static(lh_s64_t, x) + 1 -
                  2 * lh_cast_static(lh_s64_t, lh_gl_point_get_x(center));
    lh_s64_t py = 2 * lh_cast_static(lh_s64_t, y) + 1 -
                  2 * lh_cast_static(lh_s64_t, lh_gl_point_get_y(center));
    lh_s64_t reach = 2 * lh_cast_static(lh_s64_t, radius);

    if (px * px + py * py <= reach * reach)
    {
        return 255;
    }
    return 0;
}

lh_gl_color_channel_t
lh_gl_cover_triangle(const lh_gl_point_t *a, const lh_gl_point_t *b, const lh_gl_point_t *c,
                     lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    lh_s64_t ax = lh_gl_point_get_x(a);
    ax *= 2;
    lh_s64_t ay = lh_gl_point_get_y(a);
    ay *= 2;
    lh_s64_t bx = lh_gl_point_get_x(b);
    bx *= 2;
    lh_s64_t by = lh_gl_point_get_y(b);
    by *= 2;
    lh_s64_t cx = lh_gl_point_get_x(c);
    cx *= 2;
    lh_s64_t cy = lh_gl_point_get_y(c);
    cy *= 2;
    lh_s64_t px = x;
    px = px * 2 + 1;
    lh_s64_t py = y;
    py = py * 2 + 1;
    lh_s64_t ab = (bx - ax) * (py - ay) - (by - ay) * (px - ax);
    lh_s64_t bc = (cx - bx) * (py - by) - (cy - by) * (px - bx);
    lh_s64_t ca = (ax - cx) * (py - cy) - (ay - cy) * (px - cx);

    if (ab == 0 && bc == 0 && ca == 0)
    {
        return 0;
    }
    if ((ab >= 0 && bc >= 0 && ca >= 0) || (ab <= 0 && bc <= 0 && ca <= 0))
    {
        return 255;
    }
    return 0;
}
