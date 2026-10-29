/**
 * @file transition.c
 * @brief Implementation of `lh/ui/transition.h`.
 */
#include <lh/assert.h>

#include <lh/ui/transition.h>
#include <lh/util/return.h>

lh_void
lh_ui_transition_init(lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_transition_set_entity(self, lh_null);
    lh_ui_transition_set_to(self, lh_null);
    lh_ui_style_init(lh_ui_transition_get_from_mut(self));
    lh_ui_style_init(lh_ui_transition_get_look_mut(self));
    lh_ui_transition_set_elapsed(self, 0);
    lh_ui_transition_set_total(self, 0);
}

struct lh_ui_entity *
lh_ui_transition_get_entity(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->entity;
}

const lh_ui_style_t *
lh_ui_transition_get_target(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->to;
}

const lh_ui_style_t *
lh_ui_transition_get_look(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->look);
}

lh_void
lh_ui_transition_start(lh_ui_transition_t *self, struct lh_ui_entity *entity,
                       const lh_ui_style_t *from, const lh_ui_style_t *to)
{
    lh_ui_style_t start;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    /* Through a copy: @p from may be this slot's own look, and the look is about to
       be written. */
    start = lh_ptr_deref(from);
    lh_ui_transition_set_from(self, start);
    lh_ui_transition_set_entity(self, entity);
    lh_ui_transition_set_to(self, to);
    lh_ui_transition_set_elapsed(self, 0);
    lh_ui_transition_set_total(self, lh_ui_style_get_transition(to));
    lh_ui_style_init_blend(lh_ui_transition_get_look_mut(self), lh_ui_transition_get_from_mut(self), to, 0U);
}

lh_bool_t
lh_ui_transition_advance(lh_ui_transition_t *self, lh_u16_t ms)
{
    lh_u32_t spent;
    lh_u32_t time;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(lh_ui_transition_get_entity(self)) || lh_null_eq(lh_ui_transition_get_to(self)), lh_bool_false);
    spent = lh_cast_static(lh_u32_t, lh_ui_transition_get_elapsed(self)) + lh_cast_static(lh_u32_t, ms);
    if (spent >= lh_cast_static(lh_u32_t, lh_ui_transition_get_total(self)))
    {
        lh_ui_transition_set_elapsed(self, lh_ui_transition_get_total(self));
        lh_ui_transition_set_look(self, lh_ptr_deref(lh_ui_transition_get_to(self)));
        return lh_bool_false;
    }
    lh_ui_transition_set_elapsed(self, lh_cast_static(lh_u16_t, spent));
    time = spent * 255U / lh_cast_static(lh_u32_t, lh_ui_transition_get_total(self));
    lh_ui_style_init_blend(lh_ui_transition_get_look_mut(self), lh_ui_transition_get_from_mut(self), lh_ui_transition_get_to(self),
                           lh_ui_easing_apply(lh_ui_style_get_easing(lh_ui_transition_get_to(self)), time));
    return lh_bool_true;
}

lh_void
lh_ui_transition_set_entity(lh_ui_transition_t *self, struct lh_ui_entity * entity)
{
    lh_assert_runtime_ref(self);
    self->entity = entity;
}

const lh_ui_style_t *
lh_ui_transition_get_to(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->to;
}

lh_void
lh_ui_transition_set_to(lh_ui_transition_t *self, const lh_ui_style_t * to)
{
    lh_assert_runtime_ref(self);
    self->to = to;
}

const lh_ui_style_t *
lh_ui_transition_get_from(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

lh_ui_style_t *
lh_ui_transition_get_from_mut(lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

lh_void
lh_ui_transition_set_from(lh_ui_transition_t *self, lh_ui_style_t from)
{
    lh_assert_runtime_ref(self);
    self->from = from;
}

lh_ui_style_t *
lh_ui_transition_get_look_mut(lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->look);
}

lh_void
lh_ui_transition_set_look(lh_ui_transition_t *self, lh_ui_style_t look)
{
    lh_assert_runtime_ref(self);
    self->look = look;
}

lh_u16_t
lh_ui_transition_get_elapsed(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->elapsed;
}

lh_void
lh_ui_transition_set_elapsed(lh_ui_transition_t *self, lh_u16_t elapsed)
{
    lh_assert_runtime_ref(self);
    self->elapsed = elapsed;
}

lh_u16_t
lh_ui_transition_get_total(const lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->total;
}

lh_void
lh_ui_transition_set_total(lh_ui_transition_t *self, lh_u16_t total)
{
    lh_assert_runtime_ref(self);
    self->total = total;
}
