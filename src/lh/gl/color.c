/**
 * @file color.c
 * @brief 8-bit straight RGBA.
 */
#include <lh/gl/color.h>

#include <lh/core/assert/runtime.h>

lh_gl_color_channel_t
lh_gl_color_get_r(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_gl_color_channel_t
lh_gl_color_get_g(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->g;
}

lh_gl_color_channel_t
lh_gl_color_get_b(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->b;
}

lh_gl_color_channel_t
lh_gl_color_get_a(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->a;
}

lh_void
lh_gl_color_set_r(lh_gl_color_t *self, lh_gl_color_channel_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_void
lh_gl_color_set_g(lh_gl_color_t *self, lh_gl_color_channel_t g)
{
    lh_assert_runtime_ref(self);
    self->g = g;
}

lh_void
lh_gl_color_set_b(lh_gl_color_t *self, lh_gl_color_channel_t b)
{
    lh_assert_runtime_ref(self);
    self->b = b;
}

lh_void
lh_gl_color_set_a(lh_gl_color_t *self, lh_gl_color_channel_t a)
{
    lh_assert_runtime_ref(self);
    self->a = a;
}

lh_void
lh_gl_color_init(lh_gl_color_t *self, lh_gl_color_channel_t r, lh_gl_color_channel_t g,
                 lh_gl_color_channel_t b, lh_gl_color_channel_t a)
{
    lh_gl_color_set_r(self, r);
    lh_gl_color_set_g(self, g);
    lh_gl_color_set_b(self, b);
    lh_gl_color_set_a(self, a);
}
