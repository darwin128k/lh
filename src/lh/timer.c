/**
 * @file timer.c
 * @brief Implementation of `lh/timer.h` and `lh/timer/group.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/runtime/error/code.h>
#include <lh/timer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

/**
 * @brief Higher priority first; equals keep relative order.
 */
LH_ATTRIBUTE_STATIC
lh_int_t
lh_timer_cmp_priority(const lh_list_node_t *a, const lh_list_node_t *b, lh_ptr context)
{
    const lh_timer_t *left;
    const lh_timer_t *right;

    (void)context;
    left = lh_timer_get_by_node_as_const(a);
    right = lh_timer_get_by_node_as_const(b);
    if (left->priority > right->priority)
    {
        return -1;
    }
    if (left->priority < right->priority)
    {
        return 1;
    }
    return 0;
}

lh_list_t *
lh_timer_group_get_timers(lh_timer_group_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

const lh_list_t *
lh_timer_group_get_timers_as_const(const lh_timer_group_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

lh_void
lh_timer_group_init(lh_timer_group_t *self)
{
    lh_list_init(lh_timer_group_get_timers(self));
}

lh_void
lh_timer_group_deinit(lh_timer_group_t *self)
{
    lh_list_t *timers;

    timers = lh_timer_group_get_timers(self);
    while (!lh_list_is_empty(timers))
    {
        lh_list_node_unlink(lh_list_get_first(timers));
    }
}

lh_void
lh_timer_group_handler(lh_timer_group_t *self, lh_tick_t now)
{
    lh_list_t *timers;
    lh_list_node_t *node;
    lh_list_node_t *next;
    lh_timer_t *timer;

    timers = lh_timer_group_get_timers(self);
    for (node = lh_list_get_first(timers); lh_null_ne(node); node = next)
    {
        next = lh_list_get_next(timers, node);
        timer = lh_timer_get_by_node(node);
        if (timer->paused)
        {
            continue;
        }
        if ((now - timer->last) < timer->period)
        {
            continue;
        }
        timer->last = now;
        lh_assert_runtime_ref(timer->cb);
        timer->cb(timer, timer->context);
        if (!timer->repeat && lh_timer_is_running(timer))
        {
            lh_timer_stop(timer);
        }
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

    timers = lh_timer_group_get_timers_as_const(self);
    best = LH_TICK_T_MAX;
    for (node = lh_list_get_first(timers); lh_null_ne(node); node = lh_list_get_next(timers, node))
    {
        timer = lh_timer_get_by_node_as_const(node);
        if (timer->paused)
        {
            continue;
        }
        remain = timer->period - (now - timer->last);
        if ((now - timer->last) >= timer->period)
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

lh_list_node_t *
lh_timer_get_node(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_timer_get_node_as_const(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_timer_t *
lh_timer_get_by_node(lh_list_node_t *node)
{
    return lh_list_entry(lh_timer_t, link, node);
}

const lh_timer_t *
lh_timer_get_by_node_as_const(const lh_list_node_t *node)
{
    return lh_list_entry(const lh_timer_t, link, node);
}

lh_void
lh_timer_init(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_list_node_init(lh_timer_get_node(self));
    self->period = 0;
    self->last = 0;
    self->priority = 0;
    self->repeat = lh_bool_false;
    self->paused = lh_bool_false;
    self->cb = lh_null;
    self->context = lh_null;
}

lh_void
lh_timer_deinit(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_timer_stop(self);
    self->cb = lh_null;
    self->context = lh_null;
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
    self->period = period;
    self->last = now;
    self->priority = priority;
    self->repeat = repeat;
    self->paused = lh_bool_false;
    self->cb = cb;
    self->context = context;
    lh_list_insert_sorted(lh_timer_group_get_timers(group), lh_timer_get_node(self),
                          lh_timer_cmp_priority, lh_null);
}

lh_void
lh_timer_stop(lh_timer_t *self)
{
    lh_list_node_t *node;

    lh_assert_runtime_ref(self);
    node = lh_timer_get_node(self);
    if (lh_list_node_is_linked(node))
    {
        lh_list_node_unlink(node);
    }
    self->paused = lh_bool_false;
}

lh_bool_t
lh_timer_is_running(const lh_timer_t *self)
{
    return lh_cast_static(lh_bool_t, lh_list_node_is_linked(lh_timer_get_node_as_const(self)));
}

lh_u8_t
lh_timer_get_priority(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->priority;
}

lh_void
lh_timer_set_priority(lh_timer_group_t *group, lh_timer_t *self, lh_u8_t priority)
{
    lh_list_t *timers;
    lh_list_node_t *node;
    lh_bool_t running;

    lh_assert_runtime_ref(group);
    lh_assert_runtime_ref(self);
    if (self->priority == priority)
    {
        return;
    }
    timers = lh_timer_group_get_timers(group);
    node = lh_timer_get_node(self);
    running = lh_timer_is_running(self);
    if (running)
    {
        lh_assert_runtime_if(!lh_list_contains(timers, node),
                             lh_runtime_error_code_invalid_argument);
        lh_list_node_unlink(node);
    }
    self->priority = priority;
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
        self->paused = lh_bool_true;
    }
}

lh_void
lh_timer_resume(lh_timer_t *self, lh_tick_t now)
{
    lh_assert_runtime_ref(self);
    if (lh_timer_is_running(self))
    {
        self->paused = lh_bool_false;
        self->last = now;
    }
}
