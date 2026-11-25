/**
 * @file transition.c
 * @brief Implementation of `lh/gl/view.h`: transitions: a style reached over time, and the tick
 * that moves them.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_gl_transition_t *
lh_gl_view_slot_of(const lh_gl_view_t *self, const lh_gl_entity_t *entity)
{
    for (int i = 0; i < LH_GL_VIEW_TRANSITION_MAX; ++i)
    {
        if (lh_gl_transition_get_entity(lh_addr_of(lh_gl_view_get_transitions(self)[i])) == entity)
        {
            return lh_ptr_rcast(lh_gl_transition_t,
                                lh_addr_of(lh_gl_view_get_transitions(self)[i]));
        }
    }
    return lh_null;
}

lh_void
lh_gl_view_land(lh_gl_transition_t *slot)
{
    lh_gl_entity_t *entity = lh_gl_transition_get_entity(slot);
    const lh_gl_style_t *to = lh_gl_transition_get_target(slot);

    lh_gl_transition_init(slot);
    lh_gl_entity_set_style_at_once(entity, to);
}

lh_bool_t
lh_gl_view_start_transition(lh_gl_view_t *self, lh_gl_entity_t *entity, const lh_gl_style_t *to)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(entity);
    lh_assert_runtime_ref(to);
    const lh_gl_style_t *from = lh_gl_entity_get_style(entity);
    lh_return_if(lh_null_eq(from), lh_bool_false);
    lh_gl_transition_t *slot = lh_gl_view_slot_of(self, entity);
    if (lh_null_eq(slot))
    {
        /* A free slot is one with no entity in it. */
        slot = lh_gl_view_slot_of(self, lh_null);
    }
    lh_return_if(lh_null_eq(slot), lh_bool_false);
    /* From the look on screen, which may be this slot's own half-way look. */
    lh_gl_transition_start(slot, entity, from, to);
    lh_gl_entity_set_style_at_once(entity, lh_gl_transition_get_look(slot));
    lh_gl_entity_note(entity);
    return lh_bool_true;
}

const lh_gl_style_t *
lh_gl_view_get_transition_target(const lh_gl_view_t *self, const lh_gl_entity_t *entity)
{
    const lh_gl_transition_t *slot;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity), lh_null);
    slot = lh_gl_view_slot_of(self, entity);
    return lh_null_eq(slot) ? lh_null : lh_gl_transition_get_target(slot);
}

lh_void
lh_gl_view_drop_transition(lh_gl_view_t *self, const lh_gl_entity_t *entity)
{
    lh_gl_transition_t *slot;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(entity));
    slot = lh_gl_view_slot_of(self, entity);
    lh_return_if(lh_null_eq(slot));
    lh_gl_transition_init(slot);
}

lh_void
lh_gl_view_finish_transitions(lh_gl_view_t *self, const lh_gl_entity_t *subtree)
{
    lh_assert_runtime_ref(self);
    for (int i = 0; i < LH_GL_VIEW_TRANSITION_MAX; ++i)
    {
        lh_gl_transition_t *slot = lh_addr_of(lh_gl_view_get_transitions_mut(self)[i]);
        const lh_gl_entity_t *up = lh_gl_transition_get_entity(slot);

        if (lh_null_eq(up))
        {
            continue;
        }
        /* In the subtree when @p subtree is the entity or one of its ancestors. */
        while (lh_null_ne(subtree) && lh_null_ne(up) && up != subtree)
        {
            up = lh_gl_entity_get_parent(up);
        }
        if (lh_null_eq(subtree) || up == subtree)
        {
            lh_gl_view_land(slot);
        }
    }
}

lh_bool_t
lh_gl_view_advance(lh_gl_view_t *self, lh_u16_t ms)
{
    lh_bool_t moved = lh_bool_false;

    lh_assert_runtime_ref(self);
    lh_return_if(ms == 0, lh_bool_false);
    for (int i = 0; i < LH_GL_VIEW_TRANSITION_MAX; ++i)
    {
        lh_gl_transition_t *slot = lh_addr_of(lh_gl_view_get_transitions_mut(self)[i]);
        lh_gl_entity_t *entity = lh_gl_transition_get_entity(slot);

        if (lh_null_eq(entity))
        {
            continue;
        }
        moved = lh_bool_true;
        /* The entity already points at the slot's look, so a new look in the same
           place is a repaint and nothing else: both the old pixels and the new. */
        lh_gl_entity_note(entity);
        if (!lh_gl_transition_advance(slot, ms))
        {
            lh_gl_view_land(slot);
        }
        lh_gl_entity_note(entity);
    }
    return moved;
}

lh_bool_t
lh_gl_view_tick(lh_gl_view_t *self)
{
    /* Looks first, whether or not anything glides: a frame that only fades is a
       frame to paint. */
    lh_bool_t faded = lh_gl_view_advance(self, LH_GL_VIEW_FRAME_MS);
    lh_return_if(lh_null_eq(lh_gl_view_get_throwing(self)), faded);
    lh_gl_view_slow_down(lh_ptr_deref(lh_gl_view_get_velocity(self)),
                         lh_gl_view_get_velocity_mut(self));
    lh_bool_t moved = lh_gl_view_scroll_by(self, lh_gl_view_get_throwing(self),
                                           lh_gl_point_get_x(lh_gl_view_get_velocity_mut(self)),
                                           lh_gl_point_get_y(lh_gl_view_get_velocity_mut(self)));
    lh_return_if(
        moved && !lh_gl_view_is_slow(lh_ptr_deref(lh_gl_view_get_velocity(self)), lh_gl_scalar(1)),
        lh_bool_true);
    lh_gl_view_stop_throw(self);
    return moved ? lh_bool_true : faded;
}
