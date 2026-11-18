/**
 * @file radio.c
 * @brief Implementation of `lh/gx/radio.h`.
 */

#include <lh/gx/radio.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>
#include <lh/gx/class/initializer.h>

const lh_gx_class_t lh_gx_radio_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, lh_gx_radio_input, lh_null);

const lh_gx_checkbox_t *
lh_gx_radio_get_checkbox(const lh_gx_radio_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->checkbox);
}

lh_gx_checkbox_t *
lh_gx_radio_get_checkbox_mut(lh_gx_radio_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->checkbox);
}

lh_gx_radio_group_t *
lh_gx_radio_get_group_mut(const lh_gx_radio_t *self)
{
    lh_assert_runtime_ref(self);
    return self->group;
}

lh_void
lh_gx_radio_set_group(lh_gx_radio_t *self, lh_gx_radio_group_t *group)
{
    lh_assert_runtime_ref(self);
    self->group = group;
}

lh_gx_radio_t *
lh_gx_radio_from_entity(lh_gx_entity_t *entity)
{
    return lh_ptr_rcast(lh_gx_radio_t, lh_gx_entity_get_content(entity));
}

lh_gx_entity_t *
lh_gx_radio_get_row_mut(lh_gx_radio_t *self)
{
    return lh_gx_checkbox_get_row_mut(lh_gx_radio_get_checkbox_mut(self));
}

lh_bool_t
lh_gx_radio_is_selected(const lh_gx_radio_t *self)
{
    return lh_gx_checkbox_is_checked(lh_gx_radio_get_checkbox(self));
}

lh_void
lh_gx_radio_input(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    lh_gx_radio_t *self = lh_gx_radio_from_entity(entity);
    lh_gx_tap_t *tap = lh_gx_checkbox_get_tap_mut(lh_gx_radio_get_checkbox_mut(self));

    lh_return_if(lh_gx_tap_take(tap, entity, event) != lh_gx_tap_step_tap);
    LH_ATTRIBUTE_UNUSED(lh_gx_radio_group_choose(lh_gx_radio_get_group_mut(self), self));
}

lh_void
lh_gx_radio_init(lh_gx_radio_t *self, lh_gx_radio_group_t *group, const lh_gx_style_t *style,
                 const lh_gx_style_t *mark_style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                 lh_gx_scalar_t width, lh_gx_scalar_t height, const lh_char_t *text)
{
    lh_gx_checkbox_init(lh_gx_radio_get_checkbox_mut(self), style, mark_style, x, y, width, height,
                        text, lh_bool_false, lh_null, lh_null);
    lh_gx_entity_t *row = lh_gx_radio_get_row_mut(self);

    lh_gx_entity_set_class(row, lh_addr_of(lh_gx_radio_class));
    lh_gx_entity_set_content(row, self);
    lh_gx_radio_set_group(self, group);
}
