/**
 * @file frame.c
 * @brief Implementation of `lh/gl/canvas.h`: the frame: begin, a frame of one area, end, clear.
 */

#include <lh/gl/canvas.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_begin(lh_gl_canvas_t *self)
{
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                 lh_null_eq(lh_gl_canvas_backend_get_begin_fn(lh_gl_canvas_get_backend(self))));
    lh_gl_point_init(lh_gl_canvas_get_frame_at_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_canvas_state_set_origin(lh_gl_canvas_get_state_mut(self),
                                  lh_ptr_deref(lh_gl_canvas_get_frame_at(self)));
    lh_gl_rects_init(lh_gl_canvas_get_drawn_mut(self));
    lh_gl_canvas_backend_get_begin_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self));
}

lh_void
lh_gl_canvas_end(lh_gl_canvas_t *self)
{
    /* The frame is over, so the buffer's origin is over with it — even when there
       was no `end` slot to present through. A damage recorded between frames is a
       window rectangle, not a slice of the strip that happened to be drawn last,
       and it would be moved by that origin (::lh_gl_canvas_add_damage): 568 rows
       down for an 800x600 target in 32-row strips, which is a repaint of nothing.
     */
    lh_gl_point_init(lh_gl_canvas_get_frame_at_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                 lh_null_eq(lh_gl_canvas_backend_get_end_fn(lh_gl_canvas_get_backend(self))));
    lh_gl_canvas_backend_get_end_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), lh_gl_canvas_get_drawn_mut(self));
}

lh_void
lh_gl_canvas_clear(lh_gl_canvas_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(color);
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                 lh_null_eq(lh_gl_canvas_backend_get_clear_fn(lh_gl_canvas_get_backend(self))));
    lh_gl_canvas_backend_get_clear_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), color);
    lh_gl_rect_t area;
    lh_gl_rect_init(lh_addr_of(area), lh_gl_scalar(0), lh_gl_scalar(0),
                    lh_gl_size_get_width(lh_gl_canvas_get_size_mut(self)),
                    lh_gl_size_get_height(lh_gl_canvas_get_size_mut(self)));
    lh_gl_canvas_add_damage(self, lh_addr_of(area));
}

lh_void
lh_gl_canvas_begin_area(lh_gl_canvas_t *self, const lh_gl_rect_t *area)
{
    lh_assert_runtime_ref(area);
    /* Without the slot the frame is the whole target, which is exactly what
       begin draws: the area is then only a slice of a full frame, and the
       picture is the same, just no smaller buffer and no faster. */
    if (lh_null_eq(lh_gl_canvas_get_backend(self)) ||
        lh_null_eq(lh_gl_canvas_backend_get_begin_area_fn(lh_gl_canvas_get_backend(self))))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_canvas_begin(self));
        return;
    }
    const lh_gl_point_t *at = lh_gl_rect_get_origin(area);
    lh_gl_canvas_set_frame_at(self, lh_ptr_deref(at));
    lh_gl_point_t origin;
    /* (0, 0) of the buffer is the area corner, so the space the backend draws
       in is the target space moved by minus the area origin. */
    lh_gl_point_init(lh_addr_of(origin), lh_math_neg(lh_gl_point_get_x(at)),
                     lh_math_neg(lh_gl_point_get_y(at)));
    lh_gl_canvas_state_set_origin(lh_gl_canvas_get_state_mut(self), origin);
    lh_gl_rects_init(lh_gl_canvas_get_drawn_mut(self));
    lh_gl_canvas_backend_get_begin_area_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), area);
}
