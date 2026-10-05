/**
 * @file pen.c
 * @brief Implementation of `lh/ui/pen.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>

lh_void
lh_ui_pen_init(lh_ui_pen_t *self, const lh_ui_color_t *color, lh_math_coord_t width)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    lh_ui_paint_init(lh_addr_of(self->paint), color);
    self->width = width;
}

lh_void
lh_ui_pen_init_gradient(lh_ui_pen_t *self, const lh_ui_gradient_t *gradient, lh_math_coord_t width)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(gradient);
    lh_ui_paint_init_gradient(lh_addr_of(self->paint), gradient);
    self->width = width;
}

const lh_ui_paint_t *
lh_ui_pen_get_paint(const lh_ui_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

const lh_ui_color_t *
lh_ui_pen_get_color(const lh_ui_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_paint_get_color(lh_ui_pen_get_paint(self));
}

lh_math_coord_t
lh_ui_pen_get_width(const lh_ui_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}
