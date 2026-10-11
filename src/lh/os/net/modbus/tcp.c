/**
 * @file tcp.c
 * @brief Modbus TCP over a socket that never waits.
 */

#include <lh/os/net/modbus/tcp.h>

#include <lh/assert/runtime.h>
#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/null.h>
#include <lh/os/result.h>
#include <lh/str/cstr.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── The four calls ───────────────────────────────────────────────────────────
   A link knows them only as function pointers over `self`, so these are the
   whole of what a Modbus TCP connection is to it. */

static lh_void
lh_os_net_modbus_tcp_close(lh_ptr self)
{
    lh_os_net_socket_close(lh_addr_of(lh_ptr_cast(lh_os_net_modbus_tcp_t, self)->socket));
}

static lh_bool_t
lh_os_net_modbus_tcp_open(lh_ptr self, const lh_char_t *target, lh_u16_t port)
{
    lh_os_net_modbus_tcp_t *tcp = lh_ptr_cast(lh_os_net_modbus_tcp_t, self);
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;
    lh_ssize_t connected;

    /* Opening again is reconnecting: whatever the old connection was in the middle
       of, the device will not finish it on the new one. */
    lh_os_net_modbus_tcp_close(self);
    lh_return_ifn(lh_null_ne(target), lh_bool_false);
    lh_return_ifn(lh_net_ip4_parse(target, lh_str_len(target), lh_addr_of(ip)), lh_bool_false);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), port);

    lh_return_ifn(lh_os_net_socket_open(lh_addr_of(tcp->socket), lh_os_system_net_socket_type_tcp),
                  lh_bool_false);
    if (!lh_os_net_socket_set_blocking(lh_addr_of(tcp->socket), lh_bool_false))
    {
        lh_os_net_modbus_tcp_close(self);
        return lh_bool_false;
    }
    /* Started, then waited for on our clock rather than the operating system's. A
       blocking connect to an address where nothing answers -- a device that is
       switched off, not one that refuses -- waits out the kernel's SYN retries,
       twenty seconds and more, and a configurator polling a panel spends that on
       every dead device in every pass. */
    connected = lh_os_net_socket_connect_start(lh_addr_of(tcp->socket), lh_addr_of(addr));
    if (connected == LH_OS_RESULT_PENDING)
    {
        connected = lh_os_net_socket_connect_wait(lh_addr_of(tcp->socket), tcp->connect_timeout_ms);
    }
    if (connected != 0)
    {
        lh_os_net_modbus_tcp_close(self);
        return lh_bool_false;
    }
    return lh_bool_true;
}

static lh_s32_t
lh_os_net_modbus_tcp_send(lh_ptr self, const lh_byte_t *bytes, lh_u16_t length)
{
    lh_os_net_modbus_tcp_t *tcp = lh_ptr_cast(lh_os_net_modbus_tcp_t, self);
    lh_ssize_t sent;

    lh_return_ifn(lh_os_net_socket_is_valid(lh_addr_of(tcp->socket)), (lh_s32_t)-1);
    sent = lh_os_net_socket_send(lh_addr_of(tcp->socket), (lh_ptr)bytes, (lh_usize_t)length);
    if (sent == LH_OS_RESULT_PENDING)
    {
        return 0; /* the send buffer is full: nothing went out, so nothing is broken */
    }
    if (sent < 0)
    {
        lh_os_net_modbus_tcp_close(self);
        return (lh_s32_t)-1;
    }
    if (sent < (lh_ssize_t)length)
    {
        /* Part of a request is on the wire and the rest is not, and the device will
           read whatever is sent next as the end of it. A frame cannot be taken
           back, so the connection is: the link opens a new one. */
        lh_os_net_modbus_tcp_close(self);
        return (lh_s32_t)-1;
    }
    return (lh_s32_t)sent;
}

static lh_s32_t
lh_os_net_modbus_tcp_recv(lh_ptr self, lh_byte_t *bytes, lh_u16_t cap)
{
    lh_os_net_modbus_tcp_t *tcp = lh_ptr_cast(lh_os_net_modbus_tcp_t, self);
    lh_ssize_t got;

    lh_return_ifn(lh_os_net_socket_is_valid(lh_addr_of(tcp->socket)), (lh_s32_t)-1);
    got = lh_os_net_socket_recv(lh_addr_of(tcp->socket), (lh_ptr)bytes, (lh_usize_t)cap);
    if (got == LH_OS_RESULT_PENDING)
    {
        return 0; /* nothing yet: the link's "not yet" */
    }
    if (got <= 0)
    {
        /* 0 from a socket is not "nothing yet", it is the peer closing. Passing it
           up as 0 would make a device that hung up look like one that is slow, and
           the link would wait out a timeout on every question for ever after. */
        lh_os_net_modbus_tcp_close(self);
        return (lh_s32_t)-1;
    }
    return (lh_s32_t)got;
}

/* ── Public ─────────────────────────────────────────────────────────────────── */

lh_void
lh_os_net_modbus_tcp_init(lh_os_net_modbus_tcp_t *self, lh_u32_t connect_timeout_ms)
{
    lh_assert_runtime_ref(self);
    lh_os_net_socket_init(lh_addr_of(self->socket));
    self->connect_timeout_ms = connect_timeout_ms;
}

lh_mb_transport_t
lh_os_net_modbus_tcp_get_transport(lh_os_net_modbus_tcp_t *self)
{
    lh_mb_transport_t transport;

    lh_assert_runtime_ref(self);
    transport.self = self;
    transport.open = lh_os_net_modbus_tcp_open;
    transport.close = lh_os_net_modbus_tcp_close;
    transport.send = lh_os_net_modbus_tcp_send;
    transport.recv = lh_os_net_modbus_tcp_recv;
    return transport;
}
