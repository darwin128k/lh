/**
 * @file paint.c
 * @brief Implementation of `lh/ui/paint.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/paint.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_paint_init(lh_ui_paint_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    self->kind = lh_ui_paint_solid;
    self->color = lh_ptr_deref(color);
    lh_ui_gradient_init_linear(lh_addr_of(self->gradient), color, color, lh_ui_gradient_horizontal);
}

lh_void
lh_ui_paint_init_gradient(lh_ui_paint_t *self, const lh_ui_gradient_t *gradient)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(gradient);
    self->kind = lh_ui_paint_gradient;
    self->color = lh_ptr_deref(lh_ui_gradient_get_from(gradient));
    self->gradient = lh_ptr_deref(gradient);
}

lh_ui_paint_kind_t
lh_ui_paint_get_kind(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(self->kind != lh_ui_paint_solid, lh_runtime_error_code_invalid_argument);
    return lh_addr_of(self->color);
}

const lh_ui_gradient_t *
lh_ui_paint_get_gradient(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(self->kind != lh_ui_paint_gradient, lh_runtime_error_code_invalid_argument);
    return lh_addr_of(self->gradient);
}
