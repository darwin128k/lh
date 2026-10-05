/**
 * @file gradient.c
 * @brief Implementation of `lh/ui/gradient.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/gradient.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_void
init(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to, lh_ui_gradient_kind_t kind,
     lh_ui_gradient_axis_t axis)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    self->from = lh_ptr_deref(from);
    self->to = lh_ptr_deref(to);
    self->kind = kind;
    self->axis = axis;
}

lh_void
lh_ui_gradient_init_linear(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to,
                           lh_ui_gradient_axis_t axis)
{
    init(self, from, to, lh_ui_gradient_linear, axis);
}

lh_void
lh_ui_gradient_init_radial(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to)
{
    init(self, from, to, lh_ui_gradient_radial, lh_ui_gradient_horizontal);
}

lh_void
lh_ui_gradient_init_angular(lh_ui_gradient_t *self, const lh_ui_color_t *from, const lh_ui_color_t *to)
{
    init(self, from, to, lh_ui_gradient_angular, lh_ui_gradient_horizontal);
}

const lh_ui_color_t *
lh_ui_gradient_get_from(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

const lh_ui_color_t *
lh_ui_gradient_get_to(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->to);
}

lh_ui_gradient_kind_t
lh_ui_gradient_get_kind(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_ui_gradient_axis_t
lh_ui_gradient_get_axis(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}
