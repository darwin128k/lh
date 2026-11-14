/**
 * @file canvas.c
 * @brief Drawing state and the primitives that forward to a backend.
 */
#include <lh/gl/canvas.h>

#include <lh/core/assert/runtime.h>

const lh_gl_point_t *
lh_gl_canvas_get_origin(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_void
lh_gl_canvas_set_origin(lh_gl_canvas_t *self, lh_gl_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

const lh_gl_rect_t *
lh_gl_canvas_get_clip(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

lh_void
lh_gl_canvas_set_clip(lh_gl_canvas_t *self, lh_gl_rect_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
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
lh_gl_canvas_init(lh_gl_canvas_t *self, lh_gl_point_t origin, lh_gl_rect_t clip)
{
    lh_gl_canvas_set_origin(self, origin);
    lh_gl_canvas_set_clip(self, clip);
    lh_gl_canvas_set_target(self, lh_null);
    lh_gl_canvas_set_backend(self, lh_null);
}

lh_void
lh_gl_canvas_fill_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, const lh_gl_color_t *color)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(root), clip) == lh_bool_false)
    {
        return;
    }
    fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), color);
}

lh_void
lh_gl_canvas_fill_colors(lh_gl_canvas_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                         const lh_gl_color_t *colors, lh_gl_scalar_t count)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_scalar_t root_x = x + lh_gl_point_get_x(origin);
    lh_gl_scalar_t root_y = y + lh_gl_point_get_y(origin);
    lh_gl_canvas_fill_colors_fn *fill = lh_gl_canvas_backend_get_fill_colors(backend);

    if (lh_ptr_is_set(fill))
    {
        fill(lh_gl_canvas_get_target(self), lh_gl_canvas_get_clip(self), root_x, root_y, colors,
             count);
        return;
    }
    for (lh_gl_scalar_t i = 0; i < count; ++i)
    {
        lh_gl_rect_t pixel;

        lh_gl_rect_init(lh_addr_of(pixel), x + i, y, 1, 1);
        lh_gl_canvas_fill_rect(self, lh_addr_of(pixel),
                               lh_ptr_add_by_offset(const lh_gl_color_t, colors,
                                                    (lh_uaddr_t)i * sizeof(lh_gl_color_t)));
    }
}

lh_void
lh_gl_canvas_stroke_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, const lh_gl_pen_t *pen)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 0)
    {
        width = 0;
    }
    lh_gl_rect_t bounds;

    lh_gl_rect_inset(lh_addr_of(root), -width, -width, lh_addr_of(bounds));
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    stroke(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), pen);
}

lh_void
lh_gl_canvas_fill_circle(lh_gl_canvas_t *self, lh_gl_point_t center, lh_gl_scalar_t radius,
                         const lh_gl_color_t *color)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_point_t root;

    lh_gl_point_init(lh_addr_of(root), lh_gl_point_get_x(lh_addr_of(center)) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(lh_addr_of(center)) + lh_gl_point_get_y(origin));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(root)) - radius,
                    lh_gl_point_get_y(lh_addr_of(root)) - radius, radius + radius, radius + radius);
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    fill(lh_gl_canvas_get_target(self), clip, root, radius, color);
}

lh_void
lh_gl_canvas_stroke_circle(lh_gl_canvas_t *self, lh_gl_point_t center, lh_gl_scalar_t radius,
                           const lh_gl_pen_t *pen)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_point_t root;

    lh_gl_point_init(lh_addr_of(root), lh_gl_point_get_x(lh_addr_of(center)) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(lh_addr_of(center)) + lh_gl_point_get_y(origin));
    lh_gl_scalar_t reach = radius + width;
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(root)) - reach,
                    lh_gl_point_get_y(lh_addr_of(root)) - reach, reach + reach, reach + reach);
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    stroke(lh_gl_canvas_get_target(self), clip, root, radius, pen);
}

lh_void
lh_gl_canvas_fill_round_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect,
                             lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(root), clip) == lh_bool_false)
    {
        return;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(root));
    lh_gl_scalar_t half =
        lh_math_min(lh_gl_size_get_width(size), lh_gl_size_get_height(size)) / 2;

    fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), lh_math_min(radius, half), color);
}

lh_void
lh_gl_canvas_stroke_round_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect,
                               lh_gl_scalar_t radius, const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(pen);
    const lh_gl_canvas_backend_t *backend = lh_gl_canvas_get_backend(self);

    if (lh_ptr_is_null(backend))
    {
        return;
    }
    lh_gl_canvas_stroke_round_rect_fn *stroke = lh_gl_canvas_backend_get_stroke_round_rect(backend);

    if (lh_ptr_is_null(stroke))
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_rect_t root;

    lh_gl_rect_offset(rect, lh_gl_point_get_x(origin), lh_gl_point_get_y(origin), lh_addr_of(root));
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 0)
    {
        width = 0;
    }
    lh_gl_rect_t bounds;

    lh_gl_rect_inset(lh_addr_of(root), -width, -width, lh_addr_of(bounds));
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    if (radius < 0)
    {
        radius = 0;
    }
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(root));
    lh_gl_scalar_t half =
        lh_math_min(lh_gl_size_get_width(size), lh_gl_size_get_height(size)) / 2;

    stroke(lh_gl_canvas_get_target(self), clip, lh_addr_of(root), lh_math_min(radius, half), pen);
}

lh_void
lh_gl_canvas_fill_triangle(lh_gl_canvas_t *self, const lh_gl_point_t *a, const lh_gl_point_t *b,
                           const lh_gl_point_t *c, const lh_gl_color_t *color)
{
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
    const lh_gl_point_t *origin = lh_gl_canvas_get_origin(self);
    lh_gl_point_t root_a;

    lh_gl_point_init(lh_addr_of(root_a),
                     lh_gl_point_get_x(a) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(a) + lh_gl_point_get_y(origin));
    lh_gl_point_t root_b;

    lh_gl_point_init(lh_addr_of(root_b),
                     lh_gl_point_get_x(b) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(b) + lh_gl_point_get_y(origin));
    lh_gl_point_t root_c;

    lh_gl_point_init(lh_addr_of(root_c),
                     lh_gl_point_get_x(c) + lh_gl_point_get_x(origin),
                     lh_gl_point_get_y(c) + lh_gl_point_get_y(origin));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(
        lh_addr_of(bounds),
        lh_math_min(lh_gl_point_get_x(lh_addr_of(root_a)),
                    lh_math_min(lh_gl_point_get_x(lh_addr_of(root_b)),
                                lh_gl_point_get_x(lh_addr_of(root_c)))),
        lh_math_min(lh_gl_point_get_y(lh_addr_of(root_a)),
                    lh_math_min(lh_gl_point_get_y(lh_addr_of(root_b)),
                                lh_gl_point_get_y(lh_addr_of(root_c)))),
        lh_math_max(lh_gl_point_get_x(lh_addr_of(root_a)),
                    lh_math_max(lh_gl_point_get_x(lh_addr_of(root_b)),
                                lh_gl_point_get_x(lh_addr_of(root_c)))) -
            lh_math_min(lh_gl_point_get_x(lh_addr_of(root_a)),
                        lh_math_min(lh_gl_point_get_x(lh_addr_of(root_b)),
                                    lh_gl_point_get_x(lh_addr_of(root_c)))),
        lh_math_max(lh_gl_point_get_y(lh_addr_of(root_a)),
                    lh_math_max(lh_gl_point_get_y(lh_addr_of(root_b)),
                                lh_gl_point_get_y(lh_addr_of(root_c)))) -
            lh_math_min(lh_gl_point_get_y(lh_addr_of(root_a)),
                        lh_math_min(lh_gl_point_get_y(lh_addr_of(root_b)),
                                    lh_gl_point_get_y(lh_addr_of(root_c)))));
    const lh_gl_rect_t *clip = lh_gl_canvas_get_clip(self);

    if (lh_gl_rect_intersects(lh_addr_of(bounds), clip) == lh_bool_false)
    {
        return;
    }
    fill(lh_gl_canvas_get_target(self), clip, lh_addr_of(root_a), lh_addr_of(root_b),
         lh_addr_of(root_c), color);
}
