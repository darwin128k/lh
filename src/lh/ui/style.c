/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/style.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    self->fill = lh_null;
}

const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill;
}

lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_assert_runtime_ref(self);
    self->fill = fill;
}
