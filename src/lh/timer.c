/**
 * @file timer.c
 * @brief Implementation of `lh/timer.h` and `lh/timer/group.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/timer.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_timer_group_init(lh_timer_group_t *self)
{
    lh_assert_runtime_ref(self);
    lh_list_init(lh_addr_of(self->timers));
}

lh_void
lh_timer_group_deinit(lh_timer_group_t *self)
{
    lh_list_node_t *node;

    lh_assert_runtime_ref(self);
    while (!lh_list_is_empty(lh_addr_of(self->timers)))
    {
        node = lh_list_get_first(lh_addr_of(self->timers));
        lh_list_node_unlink(node);
    }
}

lh_void
lh_timer_group_handler(lh_timer_group_t *self, lh_tick_t now)
{
    lh_list_node_t *node;
    lh_list_node_t *next;
    lh_timer_t *timer;

    lh_assert_runtime_ref(self);
    for (node = lh_list_get_first(lh_addr_of(self->timers)); lh_null_ne(node); node = next)
    {
        next = lh_list_get_next(lh_addr_of(self->timers), node);
        timer = lh_list_entry(lh_timer_t, link, node);
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
    lh_list_node_t *node;
    lh_timer_t *timer;
    lh_tick_t best;
    lh_tick_t remain;

    lh_assert_runtime_ref(self);
    best = LH_TICK_T_MAX;
    for (node = lh_list_get_first(lh_addr_of(self->timers)); lh_null_ne(node);
         node = lh_list_get_next(lh_addr_of(self->timers), node))
    {
        timer = lh_list_entry(lh_timer_t, link, node);
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

lh_void
lh_timer_init(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_list_node_init(lh_addr_of(self->link));
    self->period = 0;
    self->last = 0;
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
               lh_timer_cb cb, lh_ptr context, lh_tick_t now)
{
    lh_assert_runtime_ref(group);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(cb);
    lh_assert_runtime_if(period == 0, lh_runtime_error_code_invalid_argument);
    lh_timer_stop(self);
    self->period = period;
    self->last = now;
    self->repeat = repeat;
    self->paused = lh_bool_false;
    self->cb = cb;
    self->context = context;
    lh_list_push_back(lh_addr_of(group->timers), lh_addr_of(self->link));
}

lh_void
lh_timer_stop(lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    if (lh_list_node_is_linked(lh_addr_of(self->link)))
    {
        lh_list_node_unlink(lh_addr_of(self->link));
    }
    self->paused = lh_bool_false;
}

lh_bool_t
lh_timer_is_running(const lh_timer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_bool_t, lh_list_node_is_linked(lh_addr_of(self->link)));
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
