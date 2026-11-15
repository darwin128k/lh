/**
 * @file pen.c
 * @brief Implementation of `lh/gl/pen.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/pen.h>

const lh_gl_paint_t *
lh_gl_pen_get_paint(const lh_gl_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

lh_void
lh_gl_pen_set_paint_v(lh_gl_pen_t *self, const lh_gl_paint_t *paint)
{
    if (lh_null_eq(paint))
    {
        lh_gl_paint_init(lh_gl_pen_get_paint_mut(self));
        return;
    }
    lh_gl_pen_set_paint(self, lh_ptr_deref(paint));
}

lh_gl_scalar_t
lh_gl_pen_get_width(const lh_gl_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gl_pen_set_width_v(lh_gl_pen_t *self, lh_gl_scalar_t width)
{
    lh_assert_runtime_if(width < lh_gl_scalar(0), lh_runtime_error_code_invalid_argument);
    lh_gl_pen_set_width(self, width);
}

lh_void
lh_gl_pen_init_paint(lh_gl_pen_t *self, const lh_gl_paint_t *paint, lh_gl_scalar_t width)
{
    lh_gl_pen_set_paint_v(self, paint);
    lh_gl_pen_set_width_v(self, width);
}

lh_void
lh_gl_pen_init(lh_gl_pen_t *self)
{
    lh_gl_pen_init_paint(self, lh_null, lh_gl_scalar(1));
}

lh_gl_paint_t *
lh_gl_pen_get_paint_mut(lh_gl_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

lh_void
lh_gl_pen_set_paint(lh_gl_pen_t *self, lh_gl_paint_t paint)
{
    lh_assert_runtime_ref(self);
    self->paint = paint;
}

lh_void
lh_gl_pen_set_width(lh_gl_pen_t *self, lh_gl_scalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}
