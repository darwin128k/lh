/**
 * @file deadline.c
 * @brief Deadline-bounded socket reader for the web host.
 */

#include <lh/ui/web/host/deadline.h>
#include <lh/assert.h>
#include <lh/os/tick.h>

lh_os_net_socket_t *
lh_ui_web_host_deadline_get_socket(const lh_ui_web_host_deadline_t *self)
{
    lh_assert_runtime_ref(self);
    return self->socket;
}

lh_void
lh_ui_web_host_deadline_set_socket(lh_ui_web_host_deadline_t *self, lh_os_net_socket_t *value)
{
    lh_assert_runtime_ref(self);
    self->socket = value;
}

lh_u64_t
lh_ui_web_host_deadline_get_start(const lh_ui_web_host_deadline_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_ui_web_host_deadline_set_start(lh_ui_web_host_deadline_t *self, lh_u64_t value)
{
    lh_assert_runtime_ref(self);
    self->start = value;
}

lh_u32_t
lh_ui_web_host_deadline_get_timeout(const lh_ui_web_host_deadline_t *self)
{
    lh_assert_runtime_ref(self);
    return self->timeout;
}

lh_void
lh_ui_web_host_deadline_set_timeout(lh_ui_web_host_deadline_t *self, lh_u32_t value)
{
    lh_assert_runtime_ref(self);
    self->timeout = value;
}

lh_void
lh_ui_web_host_deadline_init(lh_ui_web_host_deadline_t *self, lh_os_net_socket_t *socket,
                             lh_u64_t start, lh_u32_t timeout)
{
    lh_ui_web_host_deadline_set_socket(self, socket);
    lh_ui_web_host_deadline_set_start(self, start);
    lh_ui_web_host_deadline_set_timeout(self, timeout);
}

lh_ssize_t
lh_ui_web_host_deadline_read(lh_self_ptr context, lh_ptr out, lh_usize_t size)
{
    lh_ui_web_host_deadline_t *self = lh_ptr_cast(lh_ui_web_host_deadline_t, context);
    if (lh_math_is_zero(size))
    {
        return 0;
    }
    const lh_u64_t elapsed = lh_math_sub(lh_os_tick_ms(), lh_ui_web_host_deadline_get_start(self));
    const lh_u32_t timeout = lh_ui_web_host_deadline_get_timeout(self);
    if (lh_math_ge(elapsed, timeout))
    {
        return LH_IO_ERROR;
    }
    lh_os_net_socket_t *const watch[] = {lh_ui_web_host_deadline_get_socket(self)};
    if (lh_math_le(lh_os_net_socket_wait(watch, 1U, lh_null, 0U,
                                         lh_cast_static(lh_u32_t, lh_math_sub(timeout, elapsed))),
                   0))
    {
        return LH_IO_ERROR;
    }
    return lh_os_net_socket_recv(lh_ui_web_host_deadline_get_socket(self), out, size);
}

lh_void
lh_ui_web_host_deadline_get_reader(lh_ui_web_host_deadline_t *self, lh_io_reader_t *out)
{
    lh_io_reader_init(out, lh_ui_web_host_deadline_read, self);
}
