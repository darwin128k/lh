/**
 * @file canvas.c
 * @brief Primitives that forward to a backend. Placement comes in with the call.
 */
#include <lh/gl/canvas.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math/ops.h>
#include <lh/gl/color.h>
#include <lh/gl/cover.h>

lh_void
lh_gl_canvas_init(lh_gl_canvas_t *self)
{
    lh_gl_canvas_set_target(self, lh_null);
    lh_gl_canvas_set_backend(self, lh_null);
}

lh_ptr
lh_gl_canvas_get_target(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_gl_canvas_set_target(lh_gl_canvas_t *self, lh_ptr target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

const lh_gl_canvas_backend_t *
lh_gl_canvas_get_backend(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend;
}

lh_void
lh_gl_canvas_set_backend(lh_gl_canvas_t *self, const lh_gl_canvas_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
}

lh_void
lh_gl_canvas_fill_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_rect_t *rect,
                       const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    lh_gl_canvas_fill_rect_fn *fill = lh_gl_canvas_backend_get_fill_rect(backend);

    if (lh_ptr_is_null(fill))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(root), clip) == lh_bool_false)
    {
        return;
    }
    const lh_gl_mask_t *mask = lh_gl_place_get_mask(place);

    if (lh_ptr_is_null(mask))
    {
        fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), color);
        return;
    }
    lh_gl_rect_t hit;

    lh_gl_rect_intersection(lh_addr_of(root), clip, lh_addr_of(hit));
    const lh_gl_point_t *at = lh_gl_rect_get_origin(lh_addr_of(hit));
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(hit));
    lh_gl_scalar_t left = lh_gl_point_get_x(at);
    lh_gl_scalar_t top = lh_gl_point_get_y(at);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    for (lh_gl_scalar_t y = top; y < top + height; ++y)
    {
        for (lh_gl_scalar_t x = left; x < left + width; ++x)
        {
            lh_gl_color_channel_t cover = lh_gl_mask_coverage(mask, x, y);

            if (cover == 0)
            {
                continue;
            }
            lh_gl_rect_t pixel;

            lh_gl_rect_init(lh_addr_of(pixel), x, y, 1, 1);
            if (cover == 255)
            {
                fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(pixel), color);
                continue;
            }
            lh_gl_color_t ink;

            lh_gl_color_scale_alpha(color, cover, lh_addr_of(ink));
            fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(pixel), lh_addr_of(ink));
        }
    }
}

lh_void
lh_gl_canvas_fill_colors(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_scalar_t x,
                         lh_gl_scalar_t y, const lh_gl_color_t *colors, lh_gl_scalar_t count)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(colors);
    if (count <= 0)
    {
        return;
    }
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    if (lh_ptr_is_set(lh_gl_place_get_mask(place)))
    {
        for (lh_gl_scalar_t i = 0; i < count; ++i)
        {
            lh_gl_rect_t pixel;

            lh_gl_rect_init(lh_addr_of(pixel), x + i, y, 1, 1);
            lh_gl_canvas_fill_rect(self, place, lh_addr_of(pixel),
                                   lh_ptr_add_by_offset(const lh_gl_color_t, colors,
                                                        (lh_uaddr_t)i * sizeof(lh_gl_color_t)));
        }
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_scalar_t root_x = x + lh_gl_point_get_x(origin);
    lh_gl_scalar_t root_y = y + lh_gl_point_get_y(origin);
    lh_gl_canvas_fill_colors_fn *fill = lh_gl_canvas_backend_get_fill_colors(backend);

    if (lh_ptr_is_set(fill))
    {
        fill(lh_gl_canvas_get_target(self), lh_gl_place_get_clip(place), root_x, root_y, colors,
             count);
        return;
    }
    for (lh_gl_scalar_t i = 0; i < count; ++i)
    {
        lh_gl_rect_t pixel;

        lh_gl_rect_init(lh_addr_of(pixel), x + i, y, 1, 1);
        lh_gl_canvas_fill_rect(self, place, lh_addr_of(pixel),
                               lh_ptr_add_by_offset(const lh_gl_color_t, colors,
                                                    (lh_uaddr_t)i * sizeof(lh_gl_color_t)));
    }
}

lh_void
lh_gl_canvas_stroke_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_rect_t *rect,
                         const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(pen);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    lh_gl_canvas_stroke_rect_fn *stroke = lh_gl_canvas_backend_get_stroke_rect(backend);

    if (lh_ptr_is_null(stroke))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 0)
    {
        width = 0;
    }
    lh_gl_rect_t bounds;

    lh_gl_rect_inset(lh_addr_of(root), -width, -width, lh_addr_of(bounds));
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    stroke(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), pen);
}

lh_void
lh_gl_canvas_fill_circle(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_point_t center,
                         lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(color);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend) || radius < 1)
    {
        return;
    }
    lh_gl_canvas_fill_circle_fn *fill = lh_gl_canvas_backend_get_fill_circle(backend);

    if (lh_ptr_is_null(fill))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_point_t root;

    lh_gl_point_init(lh_addr_of(root),
                     lh_gl_point_get_x(lh_addr_of(center)) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(lh_addr_of(center)) + lh_gl_point_get_y(origin));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(root)) - radius,
                    lh_gl_point_get_y(lh_addr_of(root)) - radius, radius + radius, radius + radius);
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    fill(lh_gl_canvas_get_target(self), clip, root, radius, color);
}

lh_void
lh_gl_canvas_stroke_circle(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_point_t center,
                           lh_gl_scalar_t radius, const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(pen);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend) || radius < 1)
    {
        return;
    }
    lh_gl_canvas_stroke_circle_fn *stroke = lh_gl_canvas_backend_get_stroke_circle(backend);

    if (lh_ptr_is_null(stroke))
    {
        return;
    }
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 0)
    {
        width = 0;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_point_t root;

    lh_gl_point_init(lh_addr_of(root),
                     lh_gl_point_get_x(lh_addr_of(center)) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(lh_addr_of(center)) + lh_gl_point_get_y(origin));
    lh_gl_scalar_t reach = radius + width;
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(root)) - reach,
                    lh_gl_point_get_y(lh_addr_of(root)) - reach, reach + reach, reach + reach);
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    stroke(lh_gl_canvas_get_target(self), clip, root, radius, pen);
}

lh_void
lh_gl_canvas_fill_round_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place,
                             const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                             const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    lh_gl_canvas_fill_round_rect_fn *fill = lh_gl_canvas_backend_get_fill_round_rect(backend);

    if (lh_ptr_is_null(fill))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(root), clip) == lh_bool_false)
    {
        return;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(root));
    lh_gl_scalar_t half = lh_math_min(lh_gl_size_get_width(size), lh_gl_size_get_height(size)) / 2;

    fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), lh_math_min(radius, half), color);
}

lh_void
lh_gl_canvas_stroke_round_rect(lh_gl_canvas_t *self, const lh_gl_place_t *place,
                               const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                               const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(pen);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    lh_gl_canvas_stroke_round_rect_fn *stroke =
        lh_gl_canvas_backend_get_stroke_round_rect(backend);

    if (lh_ptr_is_null(stroke))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 0)
    {
        width = 0;
    }
    lh_gl_rect_t bounds;

    lh_gl_rect_inset(lh_addr_of(root), -width, -width, lh_addr_of(bounds));
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(root));
    lh_gl_scalar_t half = lh_math_min(lh_gl_size_get_width(size), lh_gl_size_get_height(size)) / 2;

    stroke(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), lh_math_min(radius, half), pen);
}

lh_void
lh_gl_canvas_fill_triangle(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_point_t *a,
                           const lh_gl_point_t *b, const lh_gl_point_t *c,
                           const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    lh_assert_runtime_ref(c);
    lh_assert_runtime_ref(color);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    lh_gl_canvas_fill_triangle_fn *fill = lh_gl_canvas_backend_get_fill_triangle(backend);

    if (lh_ptr_is_null(fill))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_place_get_origin(place);
    lh_gl_point_t root_a;

    lh_gl_point_init(lh_addr_of(root_a), lh_gl_point_get_x(a) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(a) + lh_gl_point_get_y(origin));
    lh_gl_point_t root_b;

    lh_gl_point_init(lh_addr_of(root_b), lh_gl_point_get_x(b) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(b) + lh_gl_point_get_y(origin));
    lh_gl_point_t root_c;

    lh_gl_point_init(lh_addr_of(root_c), lh_gl_point_get_x(c) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(c) + lh_gl_point_get_y(origin));
    lh_gl_scalar_t min_x = lh_math_min(lh_gl_point_get_x(lh_addr_of(root_a)),
                                      lh_math_min(lh_gl_point_get_x(lh_addr_of(root_b)),
                                                  lh_gl_point_get_x(lh_addr_of(root_c))));
    lh_gl_scalar_t min_y = lh_math_min(lh_gl_point_get_y(lh_addr_of(root_a)),
                                      lh_math_min(lh_gl_point_get_y(lh_addr_of(root_b)),
                                                  lh_gl_point_get_y(lh_addr_of(root_c))));
    lh_gl_scalar_t max_x = lh_math_max(lh_gl_point_get_x(lh_addr_of(root_a)),
                                      lh_math_max(lh_gl_point_get_x(lh_addr_of(root_b)),
                                                  lh_gl_point_get_x(lh_addr_of(root_c))));
    lh_gl_scalar_t max_y = lh_math_max(lh_gl_point_get_y(lh_addr_of(root_a)),
                                      lh_math_max(lh_gl_point_get_y(lh_addr_of(root_b)),
                                                  lh_gl_point_get_y(lh_addr_of(root_c))));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), min_x, min_y, max_x - min_x, max_y - min_y);
    const lh_gl_rect_t *clip = lh_gl_place_get_clip(place);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(root_a), lh_addr_of(root_b),
         lh_addr_of(root_c), color);
}

lh_void
lh_gl_canvas_stroke_line(lh_gl_canvas_t *self, const lh_gl_place_t *place, const lh_gl_point_t *a,
                         const lh_gl_point_t *b, const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    lh_assert_runtime_ref(pen);
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 1 || lh_ptr_is_null(lh_gl_canvas_get_backend(self)))
    {
        return;
    }
    const lh_gl_color_t *color = lh_gl_pen_get_color(pen);
    lh_gl_scalar_t x0 = lh_gl_point_get_x(a);
    lh_gl_scalar_t y0 = lh_gl_point_get_y(a);
    lh_gl_scalar_t x1 = lh_gl_point_get_x(b);
    lh_gl_scalar_t y1 = lh_gl_point_get_y(b);
    lh_s64_t dx = lh_math_abs(x1 - x0);
    lh_s64_t step_x = x0 < x1 ? 1 : -1;
    lh_s64_t dy = -lh_math_abs(y1 - y0);
    lh_s64_t step_y = y0 < y1 ? 1 : -1;
    lh_s64_t err = dx + dy;
    lh_gl_scalar_t shift = (width - 1) / 2;

    for (;;)
    {
        lh_gl_rect_t dot;

        lh_gl_rect_init(lh_addr_of(dot), x0 - shift, y0 - shift, width, width);
        lh_gl_canvas_fill_rect(self, place, lh_addr_of(dot), color);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        lh_s64_t twice = err + err;

        if (twice >= dy)
        {
            err += dy;
            x0 = lh_type_cast(lh_gl_scalar_t, x0 + step_x);
        }
        if (twice <= dx)
        {
            err += dx;
            y0 = lh_type_cast(lh_gl_scalar_t, y0 + step_y);
        }
    }
}

lh_void
lh_gl_canvas_fill_sector(lh_gl_canvas_t *self, const lh_gl_place_t *place, lh_gl_point_t center,
                         lh_gl_scalar_t radius, lh_gl_scalar_t start, lh_gl_scalar_t sweep,
                         const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(place);
    lh_assert_runtime_ref(color);
    if (radius < 1 || lh_ptr_is_null(lh_gl_canvas_get_backend(self)))
    {
        return;
    }
    lh_gl_scalar_t left = lh_gl_point_get_x(lh_addr_of(center)) - radius;
    lh_gl_scalar_t top = lh_gl_point_get_y(lh_addr_of(center)) - radius;
    lh_gl_scalar_t right = lh_gl_point_get_x(lh_addr_of(center)) + radius;
    lh_gl_scalar_t bottom = lh_gl_point_get_y(lh_addr_of(center)) + radius;

    for (lh_gl_scalar_t y = top; y < bottom; ++y)
    {
        for (lh_gl_scalar_t x = left; x < right; ++x)
        {
            if (lh_gl_cover_sector(lh_addr_of(center), radius, start, sweep, x, y) == 0)
            {
                continue;
            }
            lh_gl_rect_t dot;

            lh_gl_rect_init(lh_addr_of(dot), x, y, 1, 1);
            lh_gl_canvas_fill_rect(self, place, lh_addr_of(dot), color);
        }
    }
}
