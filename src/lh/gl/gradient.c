/**
 * @file gradient.c
 * @brief Mix two stored colors along `0..255`.
 */
#include <lh/gl/gradient.h>

#include <lh/core/assert/runtime.h>

const lh_gl_color_t *
lh_gl_gradient_get_from(const lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

lh_gl_color_t *
lh_gl_gradient_get_from_mut(lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

const lh_gl_color_t *
lh_gl_gradient_get_to(const lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->to);
}

lh_gl_color_t *
lh_gl_gradient_get_to_mut(lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->to);
}

lh_void
lh_gl_gradient_set_from(lh_gl_gradient_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    self->from = lh_ptr_deref(color);
}

lh_void
lh_gl_gradient_set_to(lh_gl_gradient_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    self->to = lh_ptr_deref(color);
}

lh_void
lh_gl_gradient_init(lh_gl_gradient_t *self, const lh_gl_color_t *from, const lh_gl_color_t *to)
{
    lh_gl_gradient_set_from(self, from);
    lh_gl_gradient_set_to(self, to);
}

lh_void
lh_gl_gradient_color_at(const lh_gl_gradient_t *self, lh_gl_color_channel_t frac,
                        lh_gl_color_t *out)
{
    lh_gl_color_mix(lh_gl_gradient_get_from(self), lh_gl_gradient_get_to(self), frac, out);
}
