#include <lh/os.h>
#include <lh/os/thread.h>
#include <lh/os/mutex.h>
#include <lh/os/condition.h>
#include <lh/os/system/tick.h>
#include <lh/os/error/code.h>
static lh_bool_t
lh_os_concurrency_invalid(void)
{
    lh_os_set_last_error_lit(lh_os_error_code_invalid_argument,
                             "invalid concurrency object or state");
    return lh_bool_false;
}

lh_void
lh_os_thread_init(lh_os_thread_t *self)
{
    if (self)
    {
        self->handle = lh_null;
    }
}

lh_void
lh_os_mutex_init(lh_os_mutex_t *self)
{
    if (self)
    {
        self->handle = lh_null;
    }
}

lh_bool_t
lh_os_mutex_open(lh_os_mutex_t *self)
{
    if (!self || self->handle)
    {
        return lh_os_concurrency_invalid();
    }
    self->handle = lh_os_system_mutex_open();
    return self->handle != lh_null;
}
lh_bool_t
lh_os_mutex_close(lh_os_mutex_t *self)
{
    if (!self)
    {
        return lh_os_concurrency_invalid();
    }
    if (!self->handle)
    {
        return lh_bool_true;
    }
    if (!lh_os_system_mutex_close(self->handle))
    {
        return lh_bool_false;
    }
    self->handle = lh_null;
    return lh_bool_true;
}

lh_void
lh_os_condition_init(lh_os_condition_t *self)
{
    if (self)
    {
        self->handle = lh_null;
    }
}

lh_bool_t
lh_os_condition_open(lh_os_condition_t *self)
{
    if (!self || self->handle)
    {
        return lh_os_concurrency_invalid();
    }
    self->handle = lh_os_system_condition_open();
    return self->handle != lh_null;
}
lh_bool_t
lh_os_condition_close(lh_os_condition_t *self)
{
    if (!self)
    {
        return lh_os_concurrency_invalid();
    }
    if (!self->handle)
    {
        return lh_bool_true;
    }
    if (!lh_os_system_condition_close(self->handle))
    {
        return lh_bool_false;
    }
    self->handle = lh_null;
    return lh_bool_true;
}

lh_bool_t
lh_os_thread_start(lh_os_thread_t *self, lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu,
                   lh_u32_t flags)
{
    if (!self || self->handle)
    {
        return lh_os_concurrency_invalid();
    }
    self->handle = lh_os_system_thread_start(run, user, cpu, flags);
    return self->handle != lh_null;
}
lh_bool_t
lh_os_thread_join(lh_os_thread_t *self)
{
    if (!self)
    {
        return lh_os_concurrency_invalid();
    }
    if (!self->handle)
    {
        return lh_bool_true;
    }
    if (!lh_os_system_thread_join(self->handle))
    {
        return lh_bool_false;
    }
    self->handle = lh_null;
    return lh_bool_true;
}
lh_bool_t
lh_os_thread_sleep(lh_u32_t milliseconds)
{
    return lh_os_system_sleep_ms(milliseconds);
}
lh_bool_t
lh_os_mutex_lock(lh_os_mutex_t *self)
{
    return self && self->handle ? lh_os_system_mutex_lock(self->handle)
                                : lh_os_concurrency_invalid();
}
lh_bool_t
lh_os_mutex_unlock(lh_os_mutex_t *self)
{
    return self && self->handle ? lh_os_system_mutex_unlock(self->handle)
                                : lh_os_concurrency_invalid();
}
lh_bool_t
lh_os_condition_wait(lh_os_condition_t *self, lh_os_mutex_t *mutex)
{
    return self && self->handle && mutex && mutex->handle
               ? lh_os_system_condition_wait(self->handle, mutex->handle)
               : lh_os_concurrency_invalid();
}
lh_bool_t
lh_os_condition_signal(lh_os_condition_t *self)
{
    return self && self->handle ? lh_os_system_condition_signal(self->handle)
                                : lh_os_concurrency_invalid();
}
lh_bool_t
lh_os_condition_broadcast(lh_os_condition_t *self)
{
    return self && self->handle ? lh_os_system_condition_broadcast(self->handle)
                                : lh_os_concurrency_invalid();
}
