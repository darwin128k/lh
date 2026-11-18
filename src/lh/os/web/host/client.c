/**
 * @file client.c
 * @brief Level-1 accessors of ::lh_os_web_host_client_t.
 */

#include <lh/os/web/host/client.h>
#include <lh/core/assert.h>

const lh_os_net_socket_t *
lh_os_web_host_client_get_sock(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sock);
}

lh_os_net_socket_t *
lh_os_web_host_client_get_sock_mut(lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sock);
}

lh_void
lh_os_web_host_client_set_sock(lh_os_web_host_client_t *self, lh_os_net_socket_t sock)
{
    lh_assert_runtime_ref(self);
    self->sock = sock;
}

lh_byte_ptr
lh_os_web_host_client_get_rx(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rx;
}

lh_void
lh_os_web_host_client_set_rx(lh_os_web_host_client_t *self, lh_byte_ptr rx)
{
    lh_assert_runtime_ref(self);
    self->rx = rx;
}

lh_uint_t
lh_os_web_host_client_get_rx_len(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rx_len;
}

lh_void
lh_os_web_host_client_set_rx_len(lh_os_web_host_client_t *self, lh_uint_t rx_len)
{
    lh_assert_runtime_ref(self);
    self->rx_len = rx_len;
}

lh_byte_ptr
lh_os_web_host_client_get_msg(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->msg;
}

lh_void
lh_os_web_host_client_set_msg(lh_os_web_host_client_t *self, lh_byte_ptr msg)
{
    lh_assert_runtime_ref(self);
    self->msg = msg;
}

lh_uint_t
lh_os_web_host_client_get_msg_len(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->msg_len;
}

lh_void
lh_os_web_host_client_set_msg_len(lh_os_web_host_client_t *self, lh_uint_t msg_len)
{
    lh_assert_runtime_ref(self);
    self->msg_len = msg_len;
}

lh_uint_t
lh_os_web_host_client_get_msg_cap(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->msg_cap;
}

lh_void
lh_os_web_host_client_set_msg_cap(lh_os_web_host_client_t *self, lh_uint_t msg_cap)
{
    lh_assert_runtime_ref(self);
    self->msg_cap = msg_cap;
}

lh_uint_t
lh_os_web_host_client_get_msg_op(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->msg_op;
}

lh_void
lh_os_web_host_client_set_msg_op(lh_os_web_host_client_t *self, lh_uint_t msg_op)
{
    lh_assert_runtime_ref(self);
    self->msg_op = msg_op;
}

lh_bool_t
lh_os_web_host_client_is_msg_on(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->msg_on;
}

lh_void
lh_os_web_host_client_set_msg_on(lh_os_web_host_client_t *self, lh_bool_t msg_on)
{
    lh_assert_runtime_ref(self);
    self->msg_on = msg_on;
}

lh_os_web_host_frame_t *
lh_os_web_host_client_get_qhead(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->qhead;
}

lh_void
lh_os_web_host_client_set_qhead(lh_os_web_host_client_t *self, lh_os_web_host_frame_t *qhead)
{
    lh_assert_runtime_ref(self);
    self->qhead = qhead;
}

lh_os_web_host_frame_t *
lh_os_web_host_client_get_qtail(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->qtail;
}

lh_void
lh_os_web_host_client_set_qtail(lh_os_web_host_client_t *self, lh_os_web_host_frame_t *qtail)
{
    lh_assert_runtime_ref(self);
    self->qtail = qtail;
}

lh_uint_t
lh_os_web_host_client_get_queued(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->queued;
}

lh_void
lh_os_web_host_client_set_queued(lh_os_web_host_client_t *self, lh_uint_t queued)
{
    lh_assert_runtime_ref(self);
    self->queued = queued;
}

lh_uint_t
lh_os_web_host_client_get_seen(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->seen;
}

lh_void
lh_os_web_host_client_set_seen(lh_os_web_host_client_t *self, lh_uint_t seen)
{
    lh_assert_runtime_ref(self);
    self->seen = seen;
}

lh_tick_t
lh_os_web_host_client_get_last_rx_ms(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_rx_ms;
}

lh_void
lh_os_web_host_client_set_last_rx_ms(lh_os_web_host_client_t *self, lh_tick_t last_rx_ms)
{
    lh_assert_runtime_ref(self);
    self->last_rx_ms = last_rx_ms;
}

lh_tick_t
lh_os_web_host_client_get_last_ping_ms(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_ping_ms;
}

lh_void
lh_os_web_host_client_set_last_ping_ms(lh_os_web_host_client_t *self, lh_tick_t last_ping_ms)
{
    lh_assert_runtime_ref(self);
    self->last_ping_ms = last_ping_ms;
}

lh_bool_t
lh_os_web_host_client_is_used(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->used;
}

lh_void
lh_os_web_host_client_set_used(lh_os_web_host_client_t *self, lh_bool_t used)
{
    lh_assert_runtime_ref(self);
    self->used = used;
}

lh_bool_t
lh_os_web_host_client_is_closing(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->closing;
}

lh_void
lh_os_web_host_client_set_closing(lh_os_web_host_client_t *self, lh_bool_t closing)
{
    lh_assert_runtime_ref(self);
    self->closing = closing;
}

lh_bool_t
lh_os_web_host_client_is_synced(const lh_os_web_host_client_t *self)
{
    lh_assert_runtime_ref(self);
    return self->synced;
}

lh_void
lh_os_web_host_client_set_synced(lh_os_web_host_client_t *self, lh_bool_t synced)
{
    lh_assert_runtime_ref(self);
    self->synced = synced;
}
