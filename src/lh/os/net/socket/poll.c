/**
 * @file poll.c
 * @brief Implementation of `lh/os/net/socket/poll.h`.
 */

#include <lh/os/net/socket/poll.h>
#include <lh/core/assert/runtime.h>

lh_os_system_net_socket_handle_t
lh_os_net_socket_poll_get_handle(const lh_os_net_socket_poll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_net_socket_poll_set_handle(lh_os_net_socket_poll_t *self,
                                 lh_os_system_net_socket_handle_t value)
{
    lh_assert_runtime_ref(self);
    self->handle = value;
}

lh_uint_t
lh_os_net_socket_poll_get_events(const lh_os_net_socket_poll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->events;
}

lh_void
lh_os_net_socket_poll_set_events(lh_os_net_socket_poll_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
    self->events = value;
}

lh_uint_t
lh_os_net_socket_poll_get_ready(const lh_os_net_socket_poll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ready;
}

lh_void
lh_os_net_socket_poll_set_ready(lh_os_net_socket_poll_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
    self->ready = value;
}

lh_void
lh_os_net_socket_poll_init(lh_os_net_socket_poll_t *self, lh_os_system_net_socket_handle_t handle,
                           lh_uint_t events)
{
    lh_os_net_socket_poll_set_handle(self, handle);
    lh_os_net_socket_poll_set_events(self, events);
    lh_os_net_socket_poll_set_ready(self, 0U);
}

lh_void
lh_os_net_socket_poll_add_ready(lh_os_net_socket_poll_t *self, lh_uint_t bits)
{
    lh_os_net_socket_poll_set_ready(self, lh_bit_or(lh_os_net_socket_poll_get_ready(self), bits));
}

lh_bool_t
lh_os_net_socket_poll_is_readable(const lh_os_net_socket_poll_t *self)
{
    return lh_math_ne(lh_bit_and(lh_os_net_socket_poll_get_ready(self), lh_os_net_socket_poll_read),
                      0U);
}
