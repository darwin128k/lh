/**
 * @file timer.c
 * @brief Implementation of `lh/timer.h` and `lh/timer/group.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/timer.h>

lh_list_t *
lh_timer_group_get_timers_mut(lh_timer_group_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

const lh_list_t *
lh_timer_group_get_timers(const lh_timer_group_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

lh_list_node_t *
lh_timer_get_node_mut(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_timer_get_node(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_timer_t *
lh_timer_get_by_node_mut(lh_list_node_t *node)
{
    return lh_list_entry(lh_timer_t, link, node);
}

const lh_timer_t *
lh_timer_get_by_node(const lh_list_node_t *node)
{
    return lh_list_entry(const lh_timer_t, link, node);
}

lh_void
lh_timer_set_priority(lh_timer_t *self, lh_u8_t priority)
{
    lh_assert_runtime_ref(self);
    self->priority = priority;
}

lh_u8_t
lh_timer_get_priority(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->priority;
}

lh_tick_t
lh_timer_get_period(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->period;
}

lh_void
lh_timer_set_period(lh_timer_t *self, lh_tick_t period)
{
    lh_assert_runtime_ref(self);
    self->period = period;
}

lh_tick_t
lh_timer_get_last(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last;
}

lh_void
lh_timer_set_last(lh_timer_t *self, lh_tick_t last)
{
    lh_assert_runtime_ref(self);
    self->last = last;
}

lh_bool_t
lh_timer_get_repeat(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->repeat;
}

lh_void
lh_timer_set_repeat(lh_timer_t *self, lh_bool_t repeat)
{
    lh_assert_runtime_ref(self);
    self->repeat = repeat;
}

lh_bool_t
lh_timer_get_paused(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paused;
}

lh_void
lh_timer_set_paused(lh_timer_t *self, lh_bool_t paused)
{
    lh_assert_runtime_ref(self);
    self->paused = paused;
}

lh_timer_cb
lh_timer_get_cb(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cb;
}

lh_void
lh_timer_set_cb(lh_timer_t *self, lh_timer_cb cb)
{
    lh_assert_runtime_ref(self);
    self->cb = cb;
}

lh_ptr
lh_timer_get_context(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_timer_set_context(lh_timer_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_void
lh_timer_group_init(lh_timer_group_t *self)
{
    lh_list_init(lh_timer_group_get_timers_mut(self));
}

lh_void
lh_timer_group_deinit(lh_timer_group_t *self)
{
    lh_list_t *timers;

    timers = lh_timer_group_get_timers_mut(self);
    while (!lh_list_is_empty(timers))
    {
        lh_list_node_unlink(lh_list_get_first(timers));
    }
}

lh_tick_t
lh_timer_group_until_next(const lh_timer_group_t *self, lh_tick_t now)
{
    const lh_list_t *timers;
    lh_list_node_t *node;
    const lh_timer_t *timer;
    lh_tick_t best;
    lh_tick_t remain;

    timers = lh_timer_group_get_timers(self);
    best = LH_TICK_T_MAX;
    for (node = lh_list_get_first(timers); lh_null_ne(node); node = lh_list_get_next(timers, node))
    {
        timer = lh_timer_get_by_node(node);
        if (lh_timer_get_paused(timer))
        {
            continue;
        }
        remain = lh_timer_get_period(timer) - (now - lh_timer_get_last(timer));
        if ((now - lh_timer_get_last(timer)) >= lh_timer_get_period(timer))
        {
            return 0;
        }
        if (remain < best)
        {
            best = remain;
        }
    }
    return best;
}

lh_void
lh_timer_stop(lh_timer_t *self)
{
    lh_list_node_t *node;

    lh_assert_runtime_ref(self);
    node = lh_timer_get_node_mut(self);
    if (lh_list_node_is_linked(node))
    {
        lh_list_node_unlink(node);
    }
    lh_timer_set_paused(self, lh_bool_false);
}

lh_bool_t
lh_timer_is_running(const lh_timer_t *self)
{
    return lh_cast_static(lh_bool_t, lh_list_node_is_linked(lh_timer_get_node(self)));
}

lh_bool_t
lh_timer_priority_equals(const lh_timer_t *self, const lh_timer_t *other)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(lh_timer_get_priority(self), lh_timer_get_priority(other)));
}

lh_bool_t
lh_timer_priority_is_at_least(const lh_timer_t *self, const lh_timer_t *minimum)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ge(lh_timer_get_priority(self), lh_timer_get_priority(minimum)));
}

lh_void
lh_timer_init(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_list_node_init(lh_timer_get_node_mut(self));
    lh_timer_set_period(self, 0);
    lh_timer_set_last(self, 0);
    lh_timer_set_priority(self, 0);
    lh_timer_set_repeat(self, lh_bool_false);
    lh_timer_set_paused(self, lh_bool_false);
    lh_timer_set_cb(self, lh_null);
    lh_timer_set_context(self, lh_null);
}

lh_void
lh_timer_deinit(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_timer_stop(self);
    lh_timer_set_cb(self, lh_null);
    lh_timer_set_context(self, lh_null);
}

lh_void
lh_timer_group_handler(lh_timer_group_t *self, lh_tick_t now)
{
    lh_list_t *timers;
    lh_list_node_t *node;
    lh_list_node_t *next;
    lh_timer_t *timer;

    timers = lh_timer_group_get_timers_mut(self);
    for (node = lh_list_get_first(timers); lh_null_ne(node); node = next)
    {
        next = lh_list_get_next(timers, node);
        timer = lh_timer_get_by_node_mut(node);
        if (lh_timer_get_paused(timer))
        {
            continue;
        }
        if ((now - lh_timer_get_last(timer)) < lh_timer_get_period(timer))
        {
            continue;
        }
        lh_timer_set_last(timer, now);
        lh_assert_runtime_ref(lh_timer_get_cb(timer));
        lh_timer_get_cb(timer)(timer, lh_timer_get_context(timer));
        if (!lh_timer_get_repeat(timer) && lh_timer_is_running(timer))
        {
            lh_timer_stop(timer);
        }
    }
}

lh_bool_t
lh_timer_priority_is_less(const lh_timer_t *self, const lh_timer_t *other)
{
    return lh_cast_static(lh_bool_t, !lh_timer_priority_is_at_least(self, other));
}

lh_void
lh_timer_group_set_priority(lh_timer_group_t *group, lh_timer_t *self, lh_u8_t priority)
{
    lh_list_t *timers;
    lh_list_node_t *node;
    lh_bool_t running;

    lh_assert_runtime_ref(group);
    lh_assert_runtime_ref(self);
    if (lh_timer_get_priority(self) == priority)
    {
        return;
    }
    timers = lh_timer_group_get_timers_mut(group);
    node = lh_timer_get_node_mut(self);
    running = lh_timer_is_running(self);
    if (running)
    {
        lh_assert_runtime_if(!lh_list_contains(timers, node),
                             lh_runtime_error_code_invalid_argument);
        lh_list_node_unlink(node);
    }
    lh_timer_set_priority(self, priority);
    if (running)
    {
        lh_list_insert_sorted(timers, node, lh_timer_cmp_priority, lh_null);
    }
}

lh_void
lh_timer_pause(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_timer_is_running(self))
    {
        lh_timer_set_paused(self, lh_bool_true);
    }
}

lh_void
lh_timer_resume(lh_timer_t *self, lh_tick_t now)
{
    lh_assert_runtime_ref(self);
    if (lh_timer_is_running(self))
    {
        lh_timer_set_paused(self, lh_bool_false);
        lh_timer_set_last(self, now);
    }
}

lh_void
lh_timer_start(lh_timer_group_t *group, lh_timer_t *self, lh_tick_t period, lh_bool_t repeat,
               lh_u8_t priority, lh_timer_cb cb, lh_ptr context, lh_tick_t now)
{
    lh_assert_runtime_ref(group);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(cb);
    lh_assert_runtime_if(period == 0, lh_runtime_error_code_invalid_argument);
    lh_timer_stop(self);
    lh_timer_set_period(self, period);
    lh_timer_set_last(self, now);
    lh_timer_set_priority(self, priority);
    lh_timer_set_repeat(self, repeat);
    lh_timer_set_paused(self, lh_bool_false);
    lh_timer_set_cb(self, cb);
    lh_timer_set_context(self, context);
    lh_list_insert_sorted(lh_timer_group_get_timers_mut(group), lh_timer_get_node_mut(self),
                          lh_timer_cmp_priority, lh_null);
}

lh_bool_t
lh_timer_priority_is_greater(const lh_timer_t *self, const lh_timer_t *other)
{
    return lh_timer_priority_is_less(other, self);
}

lh_int_t
lh_timer_cmp_priority(const lh_list_node_t *a, const lh_list_node_t *b, lh_ptr context)
{
    const lh_timer_t *left;
    const lh_timer_t *right;

    (void)context;
    left = lh_timer_get_by_node(a);
    right = lh_timer_get_by_node(b);
    if (lh_timer_priority_is_greater(left, right))
    {
        return -1;
    }
    if (lh_timer_priority_is_less(left, right))
    {
        return 1;
    }
    return 0;
}
