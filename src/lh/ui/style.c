/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_init(lh_addr_of(self->fill));
}

const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_assert_runtime_ref(self);
    if (lh_null_eq(fill))
    {
        lh_ui_paint_init(lh_addr_of(self->fill));
        return;
    }
    self->fill = lh_ptr_deref(fill);
}

const lh_ui_color_t *
lh_ui_style_get_fill_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color(lh_ui_style_get_fill(self));
}
