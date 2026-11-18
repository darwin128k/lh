/**
 * @file pointer.c
 * @brief Implementation of `lh/gx/pointer.h`.
 */

#include <lh/gx/pointer.h>

#include <lh/core/util/return.h>
#include <lh/gx/hit.h>

lh_gx_entity_t *
lh_gx_pointer_get_captured(const lh_gx_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->captured;
}

lh_void
lh_gx_pointer_set_captured(lh_gx_pointer_t *self, lh_gx_entity_t *entity)
{
    lh_assert_runtime_ref(self);
    self->captured = entity;
}

lh_void
lh_gx_pointer_send_to(lh_gx_entity_t *entity, lh_gx_event_kind_t kind, const lh_gx_point_t *at)
{
    lh_return_if(lh_null_eq(entity));
    lh_gx_event_t event;

    lh_gx_event_init(lh_addr_of(event), kind, lh_ptr_deref(at));
    lh_gx_entity_take_input(entity, lh_addr_of(event));
}

lh_void
lh_gx_pointer_init(lh_gx_pointer_t *self)
{
    lh_gx_pointer_set_captured(self, lh_null);
}

lh_void
lh_gx_pointer_send(const lh_gx_pointer_t *self, lh_gx_event_kind_t kind, const lh_gx_point_t *at)
{
    lh_gx_pointer_send_to(lh_gx_pointer_get_captured(self), kind, at);
}

lh_void
lh_gx_pointer_end(lh_gx_pointer_t *self, lh_gx_event_kind_t kind, const lh_gx_point_t *at)
{
    lh_gx_entity_t *entity = lh_gx_pointer_get_captured(self);

    lh_gx_pointer_set_captured(self, lh_null);
    lh_gx_pointer_send_to(entity, kind, at);
}

lh_void
lh_gx_pointer_cancel(lh_gx_pointer_t *self)
{
    lh_gx_point_t nowhere;

    lh_gx_point_init(lh_addr_of(nowhere), 0, 0);
    lh_gx_pointer_end(self, lh_gx_event_kind_cancel, lh_addr_of(nowhere));
}

lh_void
lh_gx_pointer_move(const lh_gx_pointer_t *self, const lh_gx_point_t *at)
{
    lh_gx_pointer_send(self, lh_gx_event_kind_move, at);
}

lh_void
lh_gx_pointer_release(lh_gx_pointer_t *self, const lh_gx_point_t *at)
{
    lh_gx_pointer_end(self, lh_gx_event_kind_release, at);
}

lh_void
lh_gx_pointer_press(lh_gx_pointer_t *self, lh_gx_entity_t *top, const lh_gx_point_t *at)
{
    lh_gx_pointer_cancel(self);
    lh_gx_pointer_set_captured(self, lh_gx_hit_find(top, at));
    lh_gx_pointer_send(self, lh_gx_event_kind_press, at);
}

lh_void
lh_gx_pointer_take(lh_gx_pointer_t *self, lh_gx_entity_t *top, const lh_gx_event_t *event)
{
    const lh_gx_point_t *at = lh_gx_event_get_at(event);

    switch (lh_gx_event_get_kind(event))
    {
    case lh_gx_event_kind_press:
        lh_gx_pointer_press(self, top, at);
        break;
    case lh_gx_event_kind_move:
        lh_gx_pointer_move(self, at);
        break;
    case lh_gx_event_kind_release:
        lh_gx_pointer_release(self, at);
        break;
    case lh_gx_event_kind_cancel:
    default:
        lh_gx_pointer_cancel(self);
        break;
    }
}
