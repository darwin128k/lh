/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self, const lh_ui_brush_t *brush, const lh_ui_pen_t *pen)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(brush);
    lh_assert_runtime_ref(pen);
    self->brush = lh_ptr_deref(brush);
    self->pen = lh_ptr_deref(pen);
}

const lh_ui_brush_t *
lh_ui_style_get_brush(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->brush);
}

const lh_ui_pen_t *
lh_ui_style_get_pen(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pen);
}
