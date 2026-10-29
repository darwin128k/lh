/**
 * @file paint.c
 * @brief Implementation of `lh/ui/paint.h`.
 */
#include <lh/assert.h>

#include <lh/ui/paint.h>
#include <lh/util/return.h>

lh_void
lh_ui_paint_init(lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_set_kind(self, lh_ui_paint_kind_none);
    lh_ui_color_init(lh_ui_paint_get_color_mut(self), 0, 0, 0, 0);
    lh_ui_paint_set_gradient(self, lh_null);
}

lh_void
lh_ui_paint_init_color(lh_ui_paint_t *self, const lh_ui_color_t *color)
{
    lh_ui_paint_init(self);
    if (lh_null_ne(color))
    {
        lh_ui_paint_set_kind(self, lh_ui_paint_kind_solid);
        lh_ui_paint_set_color(self, lh_ptr_deref(color));
    }
}

lh_void
lh_ui_paint_init_copy(lh_ui_paint_t *self, const lh_ui_paint_t *other)
{
    lh_ui_paint_init(self);
    lh_return_if(lh_null_eq(other));
    lh_ptr_deref(self) = lh_ptr_deref(other);
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
    if (lh_ui_paint_get_kind(self) != lh_ui_paint_kind_solid)
    {
        return lh_null;
    }
    return lh_ui_paint_get_color(self);
}

lh_void
lh_ui_paint_init_gradient(lh_ui_paint_t *self, const lh_ui_gradient_t *gradient)
{
    lh_ui_paint_init(self);
    lh_return_if(lh_null_eq(gradient) || lh_ui_gradient_get_stop_count(gradient) == 0);
    lh_ui_paint_set_kind(self, lh_ui_paint_kind_gradient);
    lh_ui_paint_set_gradient(self, gradient);
    lh_ui_paint_set_color(self, lh_ptr_deref(lh_ui_gradient_stop_get_color(lh_ui_gradient_get_stop(gradient, 0))));
}

const lh_ui_gradient_t *
lh_ui_paint_get_gradient(const lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_paint_get_kind(self) == lh_ui_paint_kind_gradient ? lh_ui_paint_get_gradient(self) : lh_null;
}

lh_void
lh_ui_paint_set_kind(lh_ui_paint_t *self, lh_ui_paint_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_ui_color_t *
lh_ui_paint_get_color_mut(lh_ui_paint_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_void
lh_ui_paint_set_color(lh_ui_paint_t *self, lh_ui_color_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

lh_void
lh_ui_paint_set_gradient(lh_ui_paint_t *self, const lh_ui_gradient_t * gradient)
{
    lh_assert_runtime_ref(self);
    self->gradient = gradient;
}
