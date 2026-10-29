#include <lh/ui/canvas/sw/shadow/pass.h>
#include <lh/byte/limits.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/ui/blur.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/canvas/sw.h>
#include <lh/util/return.h>
#include <lh/assert.h>

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
    lh_ui_shadow_copy_color(shadow, lh_addr_of(color));
    lh_ui_canvas_sw_shadow_pass_set_rect(out, rect);
    lh_ui_canvas_sw_shadow_pass_set_rgb(out, lh_ui_color_get_argb(lh_addr_of(color)) & 0x00FFFFFFU);
    lh_ui_canvas_sw_shadow_pass_set_corner(out, lh_ui_radius_clamp(rect, radius));
    lh_ui_canvas_sw_shadow_pass_set_rounded(out, lh_ui_canvas_sw_is_rounded(self));
    lh_ui_canvas_sw_shadow_pass_set_left(out, lh_ui_point_get_x(origin) - outset);
    lh_ui_canvas_sw_shadow_pass_set_top(out, lh_ui_point_get_y(origin) - outset);
    lh_ui_canvas_sw_shadow_pass_set_right(out, lh_ui_canvas_sw_shadow_pass_get_left(out) + lh_ui_size_get_width(size) + outset * 2);
    lh_ui_canvas_sw_shadow_pass_set_bottom(out, lh_ui_canvas_sw_shadow_pass_get_top(out) + lh_ui_size_get_height(size) + outset * 2);
    lh_return_if(outset <= 0, lh_bool_false);
    lh_ui_shadow_shape_init(lh_ui_canvas_sw_shadow_pass_get_shape_mut(out), shadow, rect, lh_ui_canvas_sw_shadow_pass_get_corner(out));
    lh_ui_shadow_ramp_init(lh_ui_canvas_sw_shadow_pass_get_ramp_mut(out), lh_ui_canvas_sw_shadow_pass_get_shape_mut(out));
    return lh_bool_true;
}

const lh_ui_shadow_shape_t *
lh_ui_canvas_sw_shadow_pass_get_shape(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shape);
}

lh_ui_shadow_shape_t *
lh_ui_canvas_sw_shadow_pass_get_shape_mut(lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shape);
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_shape(lh_ui_canvas_sw_shadow_pass_t *self, lh_ui_shadow_shape_t shape)
{
    lh_assert_runtime_ref(self);
    self->shape = shape;
}

const lh_ui_shadow_ramp_t *
lh_ui_canvas_sw_shadow_pass_get_ramp(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ramp);
}

lh_ui_shadow_ramp_t *
lh_ui_canvas_sw_shadow_pass_get_ramp_mut(lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ramp);
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_ramp(lh_ui_canvas_sw_shadow_pass_t *self, lh_ui_shadow_ramp_t ramp)
{
    lh_assert_runtime_ref(self);
    self->ramp = ramp;
}

const lh_ui_rect_t *
lh_ui_canvas_sw_shadow_pass_get_rect(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rect;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_rect(lh_ui_canvas_sw_shadow_pass_t *self, const lh_ui_rect_t * rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

lh_ui_scalar_t
lh_ui_canvas_sw_shadow_pass_get_corner(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_corner(lh_ui_canvas_sw_shadow_pass_t *self, lh_ui_scalar_t corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_u32_t
lh_ui_canvas_sw_shadow_pass_get_rgb(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rgb;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_rgb(lh_ui_canvas_sw_shadow_pass_t *self, lh_u32_t rgb)
{
    lh_assert_runtime_ref(self);
    self->rgb = rgb;
}

lh_bool_t
lh_ui_canvas_sw_shadow_pass_is_rounded(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rounded;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_rounded(lh_ui_canvas_sw_shadow_pass_t *self, lh_bool_t rounded)
{
    lh_assert_runtime_ref(self);
    self->rounded = rounded;
}

lh_s32_t
lh_ui_canvas_sw_shadow_pass_get_left(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_left(lh_ui_canvas_sw_shadow_pass_t *self, lh_s32_t left)
{
    lh_assert_runtime_ref(self);
    self->left = left;
}

lh_s32_t
lh_ui_canvas_sw_shadow_pass_get_top(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_top(lh_ui_canvas_sw_shadow_pass_t *self, lh_s32_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_s32_t
lh_ui_canvas_sw_shadow_pass_get_right(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->right;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_right(lh_ui_canvas_sw_shadow_pass_t *self, lh_s32_t right)
{
    lh_assert_runtime_ref(self);
    self->right = right;
}

lh_s32_t
lh_ui_canvas_sw_shadow_pass_get_bottom(const lh_ui_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom;
}

lh_void
lh_ui_canvas_sw_shadow_pass_set_bottom(lh_ui_canvas_sw_shadow_pass_t *self, lh_s32_t bottom)
{
    lh_assert_runtime_ref(self);
    self->bottom = bottom;
}
