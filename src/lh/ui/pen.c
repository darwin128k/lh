/**
 * @file pen.c
 * @brief Implementation of `lh/ui/pen.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_pen_init(lh_ui_pen_t *self)
{
    lh_ui_pen_init_paint(self, lh_null, lh_ui_scalar(1));
}

lh_void
lh_ui_pen_init_paint(lh_ui_pen_t *self, const lh_ui_paint_t *paint, lh_ui_scalar_t width)
{
    lh_assert_runtime_ref(self);
    lh_ui_pen_set_paint(self, paint);
    lh_ui_pen_set_width(self, width);
}

const lh_ui_paint_t *
lh_ui_pen_get_paint(const lh_ui_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

lh_void
lh_ui_pen_set_paint(lh_ui_pen_t *self, const lh_ui_paint_t *paint)
{
    lh_assert_runtime_ref(self);
    if (lh_null_eq(paint))
    {
        lh_ui_paint_init(lh_addr_of(self->paint));
        return;
    }
    self->paint = lh_ptr_deref(paint);
}

lh_ui_scalar_t
lh_ui_pen_get_width(const lh_ui_pen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_ui_pen_set_width(lh_ui_pen_t *self, lh_ui_scalar_t width)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(width < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->width = width;
}
