/**
 * @file key.c
 * @brief Implementation of `lh/gl/canvas/sw/shadow/key.h`.
 */

#include <lh/gl/canvas/sw/shadow/key.h>

#include <lh/core/assert.h>

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_width(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_width(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_height(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_height(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_corner(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_corner(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_box_w(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->box_w;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_box_w(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t box_w)
{
    lh_assert_runtime_ref(self);
    self->box_w = box_w;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_box_h(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->box_h;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_box_h(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t box_h)
{
    lh_assert_runtime_ref(self);
    self->box_h = box_h;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_spread(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_spread(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_offset_x(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_x;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_offset_x(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t offset_x)
{
    lh_assert_runtime_ref(self);
    self->offset_x = offset_x;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_offset_y(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_y;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_offset_y(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t offset_y)
{
    lh_assert_runtime_ref(self);
    self->offset_y = offset_y;
}

lh_s32_t
lh_gl_canvas_sw_shadow_key_get_peak(const lh_gl_canvas_sw_shadow_key_t *self)
{
    lh_assert_runtime_ref(self);
    return self->peak;
}

lh_void
lh_gl_canvas_sw_shadow_key_set_peak(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t peak)
{
    lh_assert_runtime_ref(self);
    self->peak = peak;
}

lh_void
lh_gl_canvas_sw_shadow_key_init(lh_gl_canvas_sw_shadow_key_t *self, lh_s32_t width, lh_s32_t height,
                                lh_s32_t corner, lh_s32_t box_w, lh_s32_t box_h, lh_s32_t spread,
                                lh_s32_t offset_x, lh_s32_t offset_y, lh_s32_t peak)
{
    lh_gl_canvas_sw_shadow_key_set_width(self, width);
    lh_gl_canvas_sw_shadow_key_set_height(self, height);
    lh_gl_canvas_sw_shadow_key_set_corner(self, corner);
    lh_gl_canvas_sw_shadow_key_set_box_w(self, box_w);
    lh_gl_canvas_sw_shadow_key_set_box_h(self, box_h);
    lh_gl_canvas_sw_shadow_key_set_spread(self, spread);
    lh_gl_canvas_sw_shadow_key_set_offset_x(self, offset_x);
    lh_gl_canvas_sw_shadow_key_set_offset_y(self, offset_y);
    lh_gl_canvas_sw_shadow_key_set_peak(self, peak);
}

lh_bool_t
lh_gl_canvas_sw_shadow_key_is_equal(const lh_gl_canvas_sw_shadow_key_t *self,
                                    const lh_gl_canvas_sw_shadow_key_t *other)
{
    return lh_type_cast(lh_bool_t, lh_gl_canvas_sw_shadow_key_get_width(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_width(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_height(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_height(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_corner(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_corner(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_box_w(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_box_w(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_box_h(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_box_h(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_spread(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_spread(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_offset_x(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_offset_x(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_offset_y(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_offset_y(other) &&
                                       lh_gl_canvas_sw_shadow_key_get_peak(self) ==
                                           lh_gl_canvas_sw_shadow_key_get_peak(other));
}
