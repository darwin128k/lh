/**
 * @file paint.c
 * @brief Implementation of `lh/ui/paint.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/paint.h>
#include <lh/util/addr.h>

lh_void
lh_ui_paint_init(lh_ui_paint_t *self)
{
    lh_ui_paint_init_color(self, lh_null);
}

lh_void
lh_ui_paint_init_color(lh_ui_paint_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}
