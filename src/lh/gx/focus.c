/**
 * @file focus.c
 * @brief Implementation of `lh/gx/focus.h`.
 */

#include <lh/gx/focus.h>

#include <lh/core/cast/const.h>
#include <lh/core/util/return.h>

lh_gx_entity_t *
lh_gx_focus_get_focused(const lh_gx_focus_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focused;
}

lh_void
lh_gx_focus_set_focused(lh_gx_focus_t *self, lh_gx_entity_t *entity)
{
    lh_assert_runtime_ref(self);
    self->focused = entity;
}

lh_gx_entity_t *
lh_gx_focus_find(lh_gx_entity_t *entity)
{
    for (const lh_gx_entity_t *up = entity; lh_null_ne(up); up = lh_gx_entity_get_owner(up))
    {
        /* The owners are the caller's, as the entity is. */
        lh_return_if(lh_gx_entity_can_take_keys(up), lh_cast_const(lh_gx_entity_t *, up));
    }
    return lh_null;
}

lh_void
lh_gx_focus_send_to(lh_gx_entity_t *entity, lh_gx_event_kind_t kind)
{
    lh_gx_event_t event;

    lh_gx_event_init_kind(lh_addr_of(event), kind);
    lh_gx_entity_take_key(entity, lh_addr_of(event));
}

lh_void
lh_gx_focus_init(lh_gx_focus_t *self)
{
    lh_gx_focus_set_focused(self, lh_null);
}

lh_void
lh_gx_focus_move(lh_gx_focus_t *self, lh_gx_entity_t *entity)
{
    lh_gx_entity_t *before = lh_gx_focus_get_focused(self);

    lh_return_if(lh_ptr_eq(before, entity));
    lh_gx_focus_set_focused(self, entity);
    if (lh_null_ne(before))
    {
        lh_gx_focus_send_to(before, lh_gx_event_kind_blur);
    }
    lh_return_if(lh_null_eq(entity));
    lh_gx_focus_send_to(entity, lh_gx_event_kind_focus);
}

lh_bool_t
lh_gx_focus_take(const lh_gx_focus_t *self, const lh_gx_event_t *event)
{
    lh_gx_entity_t *focused = lh_gx_focus_get_focused(self);

    lh_return_if(lh_null_eq(focused), lh_bool_false);
    lh_gx_entity_take_key(focused, event);
    return lh_bool_true;
}

lh_void
lh_gx_focus_press(lh_gx_focus_t *self, lh_gx_entity_t *entity)
{
    lh_gx_focus_move(self, lh_gx_focus_find(entity));
}
