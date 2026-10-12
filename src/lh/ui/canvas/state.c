/**
 * @file state.c
 * @brief Implementation of `lh/ui/canvas/state.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas/state.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_canvas_state_init(lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_point_init(lh_addr_of(self->offset), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_rect_init_empty(lh_addr_of(self->clip));
    self->clipped = lh_bool_false;
    self->round_count = 0U;
}

const lh_ui_rect_t *
lh_ui_canvas_state_get_clip(const lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clipped ? lh_addr_of(self->clip) : lh_null;
}

lh_bool_t
lh_ui_canvas_state_has_same_clip(const lh_ui_canvas_state_t *a, const lh_ui_canvas_state_t *b)
{
    lh_return_if(a->clipped != b->clipped || a->round_count != b->round_count, lh_bool_false);
    return !a->clipped || lh_ui_rect_eq(lh_addr_of(a->clip), lh_addr_of(b->clip)) ? lh_bool_true : lh_bool_false;
}

lh_ui_rect_t
lh_ui_canvas_state_to_target(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *rect)
{
    lh_assert_runtime_ref(self);
    return lh_ui_rect_offset(rect, lh_ui_point_get_x(lh_addr_of(self->offset)),
                             lh_ui_point_get_y(lh_addr_of(self->offset)));
}

lh_void
lh_ui_canvas_state_move(lh_ui_canvas_state_t *self, lh_ui_point_t delta)
{
    lh_assert_runtime_ref(self);
    self->offset = lh_ui_point_offset(lh_addr_of(self->offset), lh_ui_point_get_x(lh_addr_of(delta)),
                                      lh_ui_point_get_y(lh_addr_of(delta)));
}

lh_void
lh_ui_canvas_state_set_origin(lh_ui_canvas_state_t *self, lh_ui_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->offset = origin;
}

lh_void
lh_ui_canvas_state_clip_to(lh_ui_canvas_state_t *self, const lh_ui_rect_t *target)
{
    self->clip = lh_ui_canvas_state_cut(self, target);
    self->clipped = lh_bool_true;
}

lh_ui_rect_t
lh_ui_canvas_state_cut(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target)
{
    lh_assert_runtime_ref(self);
    return self->clipped ? lh_ui_rect_intersection(lh_addr_of(self->clip), target) : *target;
}

lh_bool_t
lh_ui_canvas_state_contains(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target)
{
    const lh_ui_rect_t cut = lh_ui_canvas_state_cut(self, target);
    return lh_ui_rect_eq(lh_addr_of(cut), target);
}
