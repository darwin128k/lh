/**
 * @file transition.c
 * @brief Implementation of `lh/ui/transition.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/easing.h>
#include <lh/ui/transition.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_transition_init(lh_ui_transition_t *self)
{
    lh_assert_runtime_ref(self);
    self->entity = lh_null;
    self->to = lh_null;
    lh_ui_style_init(lh_addr_of(self->from));
    lh_ui_style_init(lh_addr_of(self->look));
    self->elapsed = 0;
    self->total = 0;
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
lh_ui_transition_start(lh_ui_transition_t *self, struct lh_ui_entity *entity, const lh_ui_style_t *from,
                       const lh_ui_style_t *to)
{
    lh_ui_style_t start;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    /* Through a copy: @p from may be this slot's own look, and the look is about to
       be written. */
    start = *from;
    self->from = start;
    self->entity = entity;
    self->to = to;
    self->elapsed = 0;
    self->total = lh_ui_style_get_transition(to);
    lh_ui_style_init_blend(lh_addr_of(self->look), lh_addr_of(self->from), to, 0U);
}

lh_bool_t
lh_ui_transition_advance(lh_ui_transition_t *self, lh_u16_t ms)
{
    lh_u32_t spent;
    lh_u32_t time;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->entity) || lh_null_eq(self->to), lh_bool_false);
    spent = (lh_u32_t)self->elapsed + (lh_u32_t)ms;
    if (spent >= (lh_u32_t)self->total)
    {
        self->elapsed = self->total;
        self->look = *self->to;
        return lh_bool_false;
    }
    self->elapsed = (lh_u16_t)spent;
    time = spent * 255U / (lh_u32_t)self->total;
    lh_ui_style_init_blend(lh_addr_of(self->look), lh_addr_of(self->from), self->to,
                           lh_ui_easing_apply(lh_ui_style_get_easing(self->to), time));
    return lh_bool_true;
}
