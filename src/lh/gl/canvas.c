/**
 * @file canvas.c
 * @brief Drawing-state accessors and initialization.
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

lh_void
lh_gl_canvas_init(lh_gl_canvas_t *self, lh_gl_point_t origin, lh_gl_rect_t clip)
{
    lh_gl_canvas_set_origin(self, origin);
    lh_gl_canvas_set_clip(self, clip);
}
