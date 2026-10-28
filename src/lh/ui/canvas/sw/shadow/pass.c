#include <lh/ui/canvas/sw/shadow/pass.h>
#include <lh/byte/limits.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/ui/blur.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/canvas/sw.h>
#include <lh/util/return.h>

lh_bool_t
lh_ui_canvas_sw_shadow_pass_init(const lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect,
                                 lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow,
                                 lh_ui_canvas_sw_shadow_pass_t *out)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
    const lh_ui_point_t *origin = lh_ui_rect_get_origin(rect);
    const lh_s32_t outset = lh_ui_shadow_get_outset(shadow, rect);
    lh_ui_color_t color;

    lh_assert_runtime_ref(out);
    lh_ui_shadow_get_color(shadow, lh_addr_of(color));
    out->rect = rect;
    out->rgb = lh_ui_color_get_argb(lh_addr_of(color)) & 0x00FFFFFFU;
    out->corner = lh_ui_radius_clamp(rect, radius);
    out->rounded = lh_ui_canvas_sw_is_rounded(self);
    out->left = lh_ui_point_get_x(origin) - outset;
    out->top = lh_ui_point_get_y(origin) - outset;
    out->right = out->left + lh_ui_size_get_width(size) + outset * 2;
    out->bottom = out->top + lh_ui_size_get_height(size) + outset * 2;
    lh_return_if(outset <= 0, lh_bool_false);
    lh_ui_shadow_shape_init(lh_addr_of(out->shape), shadow, rect, out->corner);
    lh_ui_shadow_ramp_init(lh_addr_of(out->ramp), lh_addr_of(out->shape));
    return lh_bool_true;
}
