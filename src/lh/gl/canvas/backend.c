/**
 * @file backend.c
 * @brief Accessors for the canvas drawing-target table.
 */
#include <lh/gl/canvas/backend.h>

#include <lh/core/assert/runtime.h>

lh_gl_canvas_fill_rect_fn *
lh_gl_canvas_backend_get_fill_rect(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_rect;
}

lh_void
lh_gl_canvas_backend_set_fill_rect(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_rect_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->fill_rect = fn;
}

lh_gl_canvas_stroke_rect_fn *
lh_gl_canvas_backend_get_stroke_rect(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stroke_rect;
}

lh_void
lh_gl_canvas_backend_set_stroke_rect(lh_gl_canvas_backend_t *self, lh_gl_canvas_stroke_rect_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->stroke_rect = fn;
}

lh_gl_canvas_fill_circle_fn *
lh_gl_canvas_backend_get_fill_circle(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_circle;
}

lh_void
lh_gl_canvas_backend_set_fill_circle(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_circle_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->fill_circle = fn;
}

lh_gl_canvas_stroke_circle_fn *
lh_gl_canvas_backend_get_stroke_circle(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stroke_circle;
}

lh_void
lh_gl_canvas_backend_set_stroke_circle(lh_gl_canvas_backend_t *self,
                                       lh_gl_canvas_stroke_circle_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->stroke_circle = fn;
}

lh_gl_canvas_fill_round_rect_fn *
lh_gl_canvas_backend_get_fill_round_rect(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_round_rect;
}

lh_void
lh_gl_canvas_backend_set_fill_round_rect(lh_gl_canvas_backend_t *self,
                                         lh_gl_canvas_fill_round_rect_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->fill_round_rect = fn;
}

lh_gl_canvas_stroke_round_rect_fn *
lh_gl_canvas_backend_get_stroke_round_rect(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stroke_round_rect;
}

lh_void
lh_gl_canvas_backend_set_stroke_round_rect(lh_gl_canvas_backend_t *self,
                                           lh_gl_canvas_stroke_round_rect_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->stroke_round_rect = fn;
}

lh_gl_canvas_fill_triangle_fn *
lh_gl_canvas_backend_get_fill_triangle(const lh_gl_canvas_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill_triangle;
}

lh_void
lh_gl_canvas_backend_set_fill_triangle(lh_gl_canvas_backend_t *self,
                                       lh_gl_canvas_fill_triangle_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->fill_triangle = fn;
}

lh_void
lh_gl_canvas_backend_init(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_rect_fn *fill_rect,
                          lh_gl_canvas_stroke_rect_fn *stroke_rect,
                          lh_gl_canvas_fill_circle_fn *fill_circle,
                          lh_gl_canvas_stroke_circle_fn *stroke_circle,
                          lh_gl_canvas_fill_round_rect_fn *fill_round_rect,
                          lh_gl_canvas_stroke_round_rect_fn *stroke_round_rect,
                          lh_gl_canvas_fill_triangle_fn *fill_triangle)
{
    lh_gl_canvas_backend_set_fill_rect(self, fill_rect);
    lh_gl_canvas_backend_set_stroke_rect(self, stroke_rect);
    lh_gl_canvas_backend_set_fill_circle(self, fill_circle);
    lh_gl_canvas_backend_set_stroke_circle(self, stroke_circle);
    lh_gl_canvas_backend_set_fill_round_rect(self, fill_round_rect);
    lh_gl_canvas_backend_set_stroke_round_rect(self, stroke_round_rect);
    lh_gl_canvas_backend_set_fill_triangle(self, fill_triangle);
}
