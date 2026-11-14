/**
 * @file axis.c
 * @brief One scroll offset.
 */
#include <lh/gl/scroll/axis.h>

#include <lh/core/assert/runtime.h>

lh_math_scalar_t
lh_gl_scroll_axis_get_offset(const lh_gl_scroll_axis_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset;
}

lh_void
lh_gl_scroll_axis_set_offset(lh_gl_scroll_axis_t *self, lh_math_scalar_t offset)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
}

lh_void
lh_gl_scroll_axis_init(lh_gl_scroll_axis_t *self, lh_math_scalar_t offset)
{
    lh_gl_scroll_axis_set_offset(self, offset);
}

lh_void
lh_gl_scroll_axis_add(lh_gl_scroll_axis_t *self, lh_math_scalar_t delta)
{
    lh_gl_scroll_axis_set_offset(self, lh_gl_scroll_axis_get_offset(self) + delta);
}
