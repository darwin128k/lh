/**
 * @file motion.c
 * @brief Implementation of `lh/gl/style/motion.h`.
 */

#include <lh/gl/style/motion.h>

#include <lh/core/assert.h>

lh_u16_t
lh_gl_style_motion_get_transition(const lh_gl_style_motion_t *self)
{
    lh_assert_runtime_ref(self);
    return self->transition;
}

lh_void
lh_gl_style_motion_set_transition(lh_gl_style_motion_t *self, lh_u16_t transition)
{
    lh_assert_runtime_ref(self);
    self->transition = transition;
}

lh_gl_easing_t
lh_gl_style_motion_get_easing(const lh_gl_style_motion_t *self)
{
    lh_assert_runtime_ref(self);
    return self->easing;
}

lh_void
lh_gl_style_motion_set_easing(lh_gl_style_motion_t *self, lh_gl_easing_t easing)
{
    lh_assert_runtime_ref(self);
    self->easing = easing;
}

lh_void
lh_gl_style_motion_init(lh_gl_style_motion_t *self)
{
    /* Every style reached by a switch changes at once unless a caller asks for
       a time of its own. */
    lh_gl_style_motion_set_transition(self, 0);
    lh_gl_style_motion_set_easing(self, lh_gl_easing_ease);
}
