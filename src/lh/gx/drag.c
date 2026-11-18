/**
 * @file drag.c
 * @brief Implementation of `lh/gx/drag.h`.
 */

#include <lh/gx/drag.h>

#include <lh/core/util/return.h>

lh_bool_t
lh_gx_drag_is_active(const lh_gx_drag_t *self)
{
    lh_assert_runtime_ref(self);
    return self->active;
}

lh_void
lh_gx_drag_set_active(lh_gx_drag_t *self, lh_bool_t active)
{
    lh_assert_runtime_ref(self);
    self->active = active;
}

const lh_gx_point_t *
lh_gx_drag_get_at(const lh_gx_drag_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->at);
}

lh_void
lh_gx_drag_set_at(lh_gx_drag_t *self, lh_gx_point_t at)
{
    lh_assert_runtime_ref(self);
    self->at = at;
}

lh_void
lh_gx_drag_init(lh_gx_drag_t *self)
{
    lh_gx_point_t at;

    lh_gx_point_init(lh_addr_of(at), 0, 0);
    lh_gx_drag_set_active(self, lh_bool_false);
    lh_gx_drag_set_at(self, at);
}

lh_gx_drag_step_t
lh_gx_drag_take(lh_gx_drag_t *self, const lh_gx_event_t *event)
{
    const lh_gx_event_kind_t kind = lh_gx_event_get_kind(event);

    if (kind == lh_gx_event_kind_press)
    {
        lh_gx_drag_set_active(self, lh_bool_true);
        lh_gx_drag_set_at(self, lh_ptr_deref(lh_gx_event_get_at(event)));
        return lh_gx_drag_step_begin;
    }
    lh_return_if(!lh_gx_drag_is_active(self), lh_gx_drag_step_none);
    if (kind == lh_gx_event_kind_cancel)
    {
        lh_gx_drag_set_active(self, lh_bool_false);
        return lh_gx_drag_step_cancel;
    }
    lh_gx_drag_set_at(self, lh_ptr_deref(lh_gx_event_get_at(event)));
    lh_return_if(kind == lh_gx_event_kind_move, lh_gx_drag_step_move);
    lh_gx_drag_set_active(self, lh_bool_false);
    return lh_gx_drag_step_end;
}
