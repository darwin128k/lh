/**
 * @file pointer.c
 * @brief Implementation of `lh/gl/style/pointer.h`.
 */

#include <lh/gl/style/pointer.h>

#include <lh/core/assert.h>

const struct lh_gl_style *
lh_gl_style_pointer_get_pressed(const lh_gl_style_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_void
lh_gl_style_pointer_set_pressed(lh_gl_style_pointer_t *self, const struct lh_gl_style *pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

lh_gl_scalar_t
lh_gl_style_pointer_get_hit_radius(const lh_gl_style_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hit_radius;
}

lh_void
lh_gl_style_pointer_set_hit_radius(lh_gl_style_pointer_t *self, lh_gl_scalar_t hit_radius)
{
    lh_assert_runtime_ref(self);
    self->hit_radius = hit_radius;
}

lh_void
lh_gl_style_pointer_init(lh_gl_style_pointer_t *self)
{
    lh_gl_style_pointer_set_hit_radius(self, lh_gl_scalar(0));
    lh_gl_style_pointer_set_pressed(self, lh_null);
}
