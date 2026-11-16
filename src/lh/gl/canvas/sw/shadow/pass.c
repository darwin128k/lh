#include <lh/gl/canvas/sw/shadow/pass.h>
#include <lh/core/byte/limits.h>
#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/gl/blur.h>
#include <lh/gl/canvas/round.h>
#include <lh/gl/canvas/sw.h>
#include <lh/core/util/return.h>
#include <lh/core/assert.h>

lh_bool_t
lh_gl_canvas_sw_shadow_pass_init_bounds(const lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect,
                                        lh_gl_scalar_t radius, const lh_gl_shadow_t *shadow,
                                        lh_gl_canvas_sw_shadow_pass_t *out)
{
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_s32_t outset = lh_gl_shadow_get_outset(shadow, rect);

    lh_assert_runtime_ref(out);
    lh_gl_color_t color;
    lh_gl_shadow_copy_color(shadow, lh_addr_of(color));
    lh_gl_canvas_sw_shadow_pass_set_rect(out, rect);
    lh_gl_canvas_sw_shadow_pass_set_rgb(out, lh_gl_color_get_argb(lh_addr_of(color)) & 0x00FFFFFFU);
    lh_gl_canvas_sw_shadow_pass_set_corner(out, lh_gl_radius_clamp(rect, radius));
    lh_gl_canvas_sw_shadow_pass_set_rounded(out, lh_gl_canvas_sw_is_rounded(self));
    lh_gl_canvas_sw_shadow_pass_set_left(out, lh_gl_point_get_x(origin) - outset);
    lh_gl_canvas_sw_shadow_pass_set_top(out, lh_gl_point_get_y(origin) - outset);
    lh_gl_canvas_sw_shadow_pass_set_right(
        out, lh_gl_canvas_sw_shadow_pass_get_left(out) + lh_gl_size_get_width(size) + outset * 2);
    lh_gl_canvas_sw_shadow_pass_set_bottom(
        out, lh_gl_canvas_sw_shadow_pass_get_top(out) + lh_gl_size_get_height(size) + outset * 2);
    return outset > 0 ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_gl_canvas_sw_shadow_pass_init(const lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect,
                                 lh_gl_scalar_t radius, const lh_gl_shadow_t *shadow,
                                 lh_gl_canvas_sw_shadow_pass_t *out)
{
    lh_return_if(!lh_gl_canvas_sw_shadow_pass_init_bounds(self, rect, radius, shadow, out),
                 lh_bool_false);
    lh_gl_shadow_shape_init(lh_gl_canvas_sw_shadow_pass_get_shape_mut(out), shadow, rect,
                            lh_gl_canvas_sw_shadow_pass_get_corner(out));
    lh_gl_shadow_ramp_init(lh_gl_canvas_sw_shadow_pass_get_ramp_mut(out),
                           lh_gl_canvas_sw_shadow_pass_get_shape_mut(out));
    return lh_bool_true;
}

const lh_gl_shadow_shape_t *
lh_gl_canvas_sw_shadow_pass_get_shape(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shape);
}

lh_gl_shadow_shape_t *
lh_gl_canvas_sw_shadow_pass_get_shape_mut(lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shape);
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_shape(lh_gl_canvas_sw_shadow_pass_t *self, lh_gl_shadow_shape_t shape)
{
    lh_assert_runtime_ref(self);
    self->shape = shape;
}

const lh_gl_shadow_ramp_t *
lh_gl_canvas_sw_shadow_pass_get_ramp(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ramp);
}

lh_gl_shadow_ramp_t *
lh_gl_canvas_sw_shadow_pass_get_ramp_mut(lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->ramp);
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_ramp(lh_gl_canvas_sw_shadow_pass_t *self, lh_gl_shadow_ramp_t ramp)
{
    lh_assert_runtime_ref(self);
    self->ramp = ramp;
}

const lh_gl_rect_t *
lh_gl_canvas_sw_shadow_pass_get_rect(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rect;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_rect(lh_gl_canvas_sw_shadow_pass_t *self, const lh_gl_rect_t * rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

lh_gl_scalar_t
lh_gl_canvas_sw_shadow_pass_get_corner(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_corner(lh_gl_canvas_sw_shadow_pass_t *self, lh_gl_scalar_t corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_u32_t
lh_gl_canvas_sw_shadow_pass_get_rgb(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rgb;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_rgb(lh_gl_canvas_sw_shadow_pass_t *self, lh_u32_t rgb)
{
    lh_assert_runtime_ref(self);
    self->rgb = rgb;
}

lh_bool_t
lh_gl_canvas_sw_shadow_pass_is_rounded(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rounded;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_rounded(lh_gl_canvas_sw_shadow_pass_t *self, lh_bool_t rounded)
{
    lh_assert_runtime_ref(self);
    self->rounded = rounded;
}

lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_left(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_left(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t left)
{
    lh_assert_runtime_ref(self);
    self->left = left;
}

lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_top(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_top(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_right(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->right;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_right(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t right)
{
    lh_assert_runtime_ref(self);
    self->right = right;
}

lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_bottom(const lh_gl_canvas_sw_shadow_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom;
}

lh_void
lh_gl_canvas_sw_shadow_pass_set_bottom(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t bottom)
{
    lh_assert_runtime_ref(self);
    self->bottom = bottom;
}
