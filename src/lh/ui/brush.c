/**
 * @file brush.c
 * @brief Implementation of `lh/ui/brush.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/brush.h>
#include <lh/util/addr.h>

lh_void
lh_ui_brush_init(lh_ui_brush_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    lh_ui_paint_init(lh_addr_of(self->paint), color);
}

lh_void
lh_ui_brush_init_gradient(lh_ui_brush_t *self, const lh_ui_gradient_t *gradient)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(gradient);
    lh_ui_paint_init_gradient(lh_addr_of(self->paint), gradient);
}

const lh_ui_paint_t *
lh_ui_brush_get_paint(const lh_ui_brush_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

const lh_ui_color_t *
lh_ui_brush_get_color(const lh_ui_brush_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_paint_get_color(lh_ui_brush_get_paint(self));
}
