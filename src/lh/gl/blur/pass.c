#include <lh/gl/blur/pass.h>
#include <lh/core/math.h>
#include <lh/gl/blur.h>
#include <lh/gl/radius.h>
#include <lh/core/util/return.h>
#include <lh/core/assert.h>

lh_bool_t
lh_gl_blur_pass_init(const lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *rect, lh_s32_t radius,
                     const lh_u8_t *scratch, const lh_gl_rect_t *shape, lh_gl_scalar_t corner,
                     lh_gl_blur_pass_t *out)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);

    lh_assert_runtime_ref(out);
    lh_gl_blur_pass_set_scratch(out, scratch);
    lh_gl_blur_pass_set_left(out, lh_gl_point_get_x(origin));
    lh_gl_blur_pass_set_top(out, lh_gl_point_get_y(origin));
    lh_gl_blur_pass_set_width(out, lh_gl_size_get_width(size));
    lh_gl_blur_pass_set_height(out, lh_gl_size_get_height(size));
    lh_gl_blur_pass_set_radius(out, radius);
    lh_gl_blur_pass_set_corner(out, lh_null_ne(shape) && corner > lh_gl_scalar(0) ? lh_gl_radius_clamp(shape, corner) : lh_gl_scalar(0));
    lh_gl_blur_pass_set_shape(out, lh_gl_blur_pass_get_corner(out) > lh_gl_scalar(0) ? shape : lh_null);
    lh_return_if(radius < 0 || lh_null_eq(scratch) || lh_gl_blur_pass_get_width(out) <= 0 || lh_gl_blur_pass_get_height(out) <= 0,
                 lh_bool_false);
    lh_gl_blur_band(pixmap, lh_gl_blur_pass_get_top(out), lh_gl_blur_pass_get_height(out), radius, lh_addr_of(out->y0),
                    lh_addr_of(out->y1));
    return lh_bool_true;
}

const lh_u8_t *
lh_gl_blur_pass_get_scratch(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_void
lh_gl_blur_pass_set_scratch(lh_gl_blur_pass_t *self, const lh_u8_t * scratch)
{
    lh_assert_runtime_ref(self);
    self->scratch = scratch;
}

const lh_gl_rect_t *
lh_gl_blur_pass_get_shape(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shape;
}

lh_void
lh_gl_blur_pass_set_shape(lh_gl_blur_pass_t *self, const lh_gl_rect_t * shape)
{
    lh_assert_runtime_ref(self);
    self->shape = shape;
}

lh_gl_scalar_t
lh_gl_blur_pass_get_corner(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_gl_blur_pass_set_corner(lh_gl_blur_pass_t *self, lh_gl_scalar_t corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_s32_t
lh_gl_blur_pass_get_left(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left;
}

lh_void
lh_gl_blur_pass_set_left(lh_gl_blur_pass_t *self, lh_s32_t left)
{
    lh_assert_runtime_ref(self);
    self->left = left;
}

lh_s32_t
lh_gl_blur_pass_get_top(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_void
lh_gl_blur_pass_set_top(lh_gl_blur_pass_t *self, lh_s32_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_s32_t
lh_gl_blur_pass_get_width(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gl_blur_pass_set_width(lh_gl_blur_pass_t *self, lh_s32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_s32_t
lh_gl_blur_pass_get_height(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_gl_blur_pass_set_height(lh_gl_blur_pass_t *self, lh_s32_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_s32_t
lh_gl_blur_pass_get_radius(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_blur_pass_set_radius(lh_gl_blur_pass_t *self, lh_s32_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius;
}

lh_s32_t
lh_gl_blur_pass_get_y0(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y0;
}

lh_void
lh_gl_blur_pass_set_y0(lh_gl_blur_pass_t *self, lh_s32_t y0)
{
    lh_assert_runtime_ref(self);
    self->y0 = y0;
}

lh_s32_t
lh_gl_blur_pass_get_y1(const lh_gl_blur_pass_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y1;
}

lh_void
lh_gl_blur_pass_set_y1(lh_gl_blur_pass_t *self, lh_s32_t y1)
{
    lh_assert_runtime_ref(self);
    self->y1 = y1;
}
