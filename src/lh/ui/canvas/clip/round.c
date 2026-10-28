#include <lh/ui/canvas/clip/round.h>
#include <lh/math.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>

lh_void
lh_ui_canvas_clip_round_init(lh_ui_canvas_clip_round_t *self, const lh_ui_rect_t *rect,
                             lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    self->rect = lh_ptr_deref(rect);
    self->radius = lh_ui_radius_clamp(rect, radius);
}

lh_byte_t
lh_ui_canvas_clip_round_coverage(const lh_ui_canvas_clip_round_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return lh_ui_radius_coverage(lh_addr_of(self->rect), self->radius, x, y);
}

lh_void
lh_ui_canvas_clip_round_narrow_row(const lh_ui_canvas_clip_round_t *self, lh_s32_t y, lh_s32_t *lo,
                                   lh_s32_t *hi)
{
    lh_s32_t x0;
    lh_s32_t x1;

    lh_ui_canvas_round_full_span(lh_addr_of(self->rect), self->radius, y, lh_addr_of(x0),
                                 lh_addr_of(x1));
    lh_ptr_deref(lo) = lh_math_max(lh_ptr_deref(lo), x0);
    lh_ptr_deref(hi) = lh_math_min(lh_ptr_deref(hi), x1);
}
