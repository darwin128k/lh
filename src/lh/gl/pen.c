/**
 * @file pen.c
 * @brief Stroke color and thickness.
 */
#include <lh/gl/pen.h>

#include <lh/core/assert/runtime.h>

const lh_gl_color_t *
lh_gl_pen_get_color(const lh_gl_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_void
lh_gl_pen_set_color(lh_gl_pen_t *self, lh_gl_color_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

lh_gl_scalar_t
lh_gl_pen_get_width(const lh_gl_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gl_pen_set_width(lh_gl_pen_t *self, lh_gl_scalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_gl_pen_init(lh_gl_pen_t *self, lh_gl_color_t color, lh_gl_scalar_t width)
{
    lh_gl_pen_set_color(self, color);
    lh_gl_pen_set_width(self, width);
}
