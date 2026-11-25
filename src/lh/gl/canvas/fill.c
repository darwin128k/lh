/**
 * @file fill.c
 * @brief Implementation of `lh/gl/canvas.h`: the fills: a rect and a rounded rect, on the target or
 * through the clip.
 */

#include <lh/gl/canvas.h>

#include <lh/core/assert.h>
#include <lh/gl/canvas/round.h>
#include <lh/gl/radius.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_fill_target_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *target,
                              const lh_gl_color_t *color)
{
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                 lh_null_eq(lh_gl_canvas_backend_get_fill_rect_fn(lh_gl_canvas_get_backend(self))));
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state_mut(self), target, lh_addr_of(cut));
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)));
    lh_gl_canvas_add_damage(self, lh_addr_of(cut));
    if (lh_gl_canvas_is_cutting(self))
    {
        lh_gl_canvas_send_cut(self, lh_addr_of(cut), color);
        return;
    }
    lh_gl_canvas_backend_get_fill_rect_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), target, color);
}

lh_bool_t
lh_gl_canvas_can_fill_round(const lh_gl_canvas_t *self, const lh_gl_rect_t *target)
{
    lh_return_if(
        lh_null_eq(lh_gl_canvas_get_backend(self)) ||
            lh_null_eq(lh_gl_canvas_backend_get_fill_round_rect_fn(lh_gl_canvas_get_backend(self))),
        lh_bool_false);
    /* A rounded box is a shape and not a reading: it is drawn whole and cut by
       whoever clips, so a backend that clips may have it even when the clip
       cuts it. Keeping that shortcut here and not in ::lh_gl_canvas_can_send_whole
       is the whole difference between the two kinds of slot. */
    lh_return_if(!lh_gl_canvas_is_cutting(self), lh_bool_true);
    return lh_gl_canvas_can_send_whole(self, target);
}

lh_void
lh_gl_canvas_fill_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), rect, lh_addr_of(target));
    lh_gl_canvas_fill_target_rect(self, lh_addr_of(target), color);
}

lh_bool_t
lh_gl_canvas_try_fill_round(lh_gl_canvas_t *self, const lh_gl_rect_t *target, lh_gl_scalar_t radius,
                            const lh_gl_color_t *color)
{
    lh_return_if(!lh_gl_canvas_can_fill_round(self, target), lh_bool_false);
    return lh_gl_canvas_backend_get_fill_round_rect_fn(lh_gl_canvas_get_backend(self))(
        lh_gl_canvas_get_context(self), target, radius, color);
}

lh_void
lh_gl_canvas_fill_target_round_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *target,
                                    lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    if (radius <= lh_gl_scalar(0))
    {
        lh_gl_canvas_fill_target_rect(self, target, color);
        return;
    }
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state_mut(self), target, lh_addr_of(cut));
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)));
    lh_gl_canvas_add_damage(self, lh_addr_of(cut));
    lh_return_if(lh_gl_canvas_try_fill_round(self, target, radius, color));
    lh_gl_canvas_fill_round_rect_by_rects(self, target, radius, color);
}

lh_void
lh_gl_canvas_fill_round_rect(lh_gl_canvas_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                             const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), rect, lh_addr_of(target));
    lh_gl_canvas_fill_target_round_rect(self, lh_addr_of(target), lh_gl_radius_clamp(rect, radius),
                                        color);
}
