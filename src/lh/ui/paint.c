/**
 * @file paint.c
 * @brief Implementation of `lh/ui/paint.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/ui/paint.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_paint_init(lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_ui_paint_kind_none;
    lh_ui_color_init(lh_addr_of(self->color), 0, 0, 0, 0);
}

lh_void
lh_ui_paint_init_color(lh_ui_paint_t *self, const lh_ui_color_t *color)
{
    lh_ui_paint_init(self);
    if (lh_null_ne(color))
    {
        self->kind = lh_ui_paint_kind_solid;
        self->color = lh_ptr_deref(color);
    }
}

lh_ui_paint_kind_t
lh_ui_paint_get_kind(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_bool_t
lh_ui_paint_is_empty(const lh_ui_paint_t *self)
{
    return lh_cast_static(lh_bool_t, lh_ui_paint_get_kind(self) == lh_ui_paint_kind_none);
}

const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    if (self->kind != lh_ui_paint_kind_solid)
    {
        return lh_null;
    }
    return lh_addr_of(self->color);
}
