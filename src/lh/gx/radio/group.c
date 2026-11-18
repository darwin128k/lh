/**
 * @file group.c
 * @brief Implementation of `lh/gx/radio/group.h`.
 */

#include <lh/gx/radio/group.h>

#include <lh/core/util/return.h>
#include <lh/gx/radio.h>

struct lh_gx_radio *
lh_gx_radio_group_get_selected(const lh_gx_radio_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->selected;
}

lh_void
lh_gx_radio_group_set_selected(lh_gx_radio_group_t *self, struct lh_gx_radio *radio)
{
    lh_assert_runtime_ref(self);
    self->selected = radio;
}

lh_gx_radio_group_change_fn *
lh_gx_radio_group_get_on_change(const lh_gx_radio_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->on_change;
}

lh_void
lh_gx_radio_group_set_on_change(lh_gx_radio_group_t *self, lh_gx_radio_group_change_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->on_change = fn;
}

lh_ptr
lh_gx_radio_group_get_context(const lh_gx_radio_group_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gx_radio_group_set_context(lh_gx_radio_group_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_void
lh_gx_radio_group_init(lh_gx_radio_group_t *self, lh_gx_radio_group_change_fn *on_change,
                       lh_ptr context)
{
    lh_gx_radio_group_set_selected(self, lh_null);
    lh_gx_radio_group_set_on_change(self, on_change);
    lh_gx_radio_group_set_context(self, context);
}

lh_void
lh_gx_radio_group_change(lh_gx_radio_group_t *self)
{
    lh_gx_radio_group_change_fn *on_change = lh_gx_radio_group_get_on_change(self);
    lh_gx_radio_t *selected = lh_gx_radio_group_get_selected(self);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, on_change)) || lh_null_eq(selected));
    on_change(self, selected, lh_gx_radio_group_get_context(self));
}

lh_void
lh_gx_radio_group_select(lh_gx_radio_group_t *self, struct lh_gx_radio *radio)
{
    lh_gx_radio_t *before = lh_gx_radio_group_get_selected(self);

    if (lh_null_ne(before))
    {
        lh_gx_checkbox_check(lh_gx_radio_get_checkbox_mut(before), lh_bool_false);
    }
    lh_gx_radio_group_set_selected(self, radio);
    lh_return_if(lh_null_eq(radio));
    lh_gx_checkbox_check(lh_gx_radio_get_checkbox_mut(radio), lh_bool_true);
}

lh_bool_t
lh_gx_radio_group_choose(lh_gx_radio_group_t *self, struct lh_gx_radio *radio)
{
    lh_return_if(lh_ptr_eq(lh_gx_radio_group_get_selected(self), radio), lh_bool_false);
    lh_gx_radio_group_select(self, radio);
    lh_gx_radio_group_change(self);
    return lh_bool_true;
}
