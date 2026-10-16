#include <lh/os/net/socket.h>
#include <lh/assert.h>
#include <lh/os/system/net/socket.h>
#include <lh/math.h>
#include <lh/util/return.h>

lh_void
lh_os_net_socket_init(lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
}

lh_bool_t
lh_os_net_socket_open(lh_os_net_socket_t *self, lh_os_system_net_socket_type_t type)
{
    lh_assert_runtime_ref(self);
    const lh_os_system_net_socket_handle_t handle = lh_os_system_net_socket_open(type);
    if (lh_math_eq(handle, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    self->handle = handle;
    return lh_bool_true;
}

lh_os_system_net_socket_handle_t
lh_os_net_socket_get_handle(const lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_bool_t
lh_os_net_socket_is_valid(const lh_os_net_socket_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ne(lh_os_net_socket_get_handle(self), LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID));
}

lh_void
lh_os_net_socket_close(lh_os_net_socket_t *self)
{
    if (lh_os_net_socket_is_valid(self))
    {
        lh_os_system_net_socket_close(lh_os_net_socket_get_handle(self));
        self->handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    }
}

lh_bool_t
lh_os_net_socket_connect(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_connect(lh_os_net_socket_get_handle(self), addr);
}

lh_bool_t
lh_os_net_socket_set_blocking(lh_os_net_socket_t *self, lh_bool_t blocking)
{
    return lh_os_system_net_socket_set_blocking(lh_os_net_socket_get_handle(self), blocking);
}

lh_ssize_t
lh_os_net_socket_connect_start(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_connect_start(lh_os_net_socket_get_handle(self), addr);
}

lh_ssize_t
lh_os_net_socket_connect_wait(lh_os_net_socket_t *self, lh_u32_t timeout_ms)
{
    return lh_os_system_net_socket_connect_wait(lh_os_net_socket_get_handle(self), timeout_ms);
}

lh_bool_t
lh_os_net_socket_connect_within(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr,
                                lh_u32_t timeout_ms)
{
    lh_ssize_t connected;

    lh_return_ifn(lh_os_net_socket_set_blocking(self, lh_bool_false), lh_bool_false);
    connected = lh_os_net_socket_connect_start(self, addr);
    if (connected == LH_OS_RESULT_PENDING)
    {
        connected = lh_os_net_socket_connect_wait(self, timeout_ms);
    }
    return connected == 0 ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_os_net_socket_bind(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_bind(lh_os_net_socket_get_handle(self), addr);
}

lh_bool_t
lh_os_net_socket_listen(lh_os_net_socket_t *self, lh_int_t backlog)
{
    return lh_os_system_net_socket_listen(lh_os_net_socket_get_handle(self), backlog);
}

lh_bool_t
lh_os_net_socket_accept(lh_os_net_socket_t *self, lh_os_net_socket_t *client,
                        lh_net_ip4_socket_addr_t *peer)
{
    lh_assert_runtime_if(lh_os_net_socket_is_valid(client), lh_runtime_error_code_invalid_argument);

    const lh_os_system_net_socket_handle_t handle =
        lh_os_system_net_socket_accept(lh_os_net_socket_get_handle(self), peer);
    if (lh_math_eq(handle, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    client->handle = handle;
    return lh_bool_true;
}

lh_bool_t
lh_os_net_socket_get_local_addr(const lh_os_net_socket_t *self, lh_net_ip4_socket_addr_t *out)
{
    return lh_os_system_net_socket_get_local_addr(lh_os_net_socket_get_handle(self), out);
}

lh_bool_t
lh_os_net_socket_set_reuse_addr(lh_os_net_socket_t *self, lh_bool_t enabled)
{
    return lh_os_system_net_socket_set_reuse_addr(lh_os_net_socket_get_handle(self), enabled);
}

lh_bool_t
lh_os_net_socket_set_no_delay(lh_os_net_socket_t *self, lh_bool_t enabled)
{
    return lh_os_system_net_socket_set_no_delay(lh_os_net_socket_get_handle(self), enabled);
}

lh_usize_t
lh_os_net_socket_get_handles(lh_os_system_net_socket_handle_t *out, lh_os_net_socket_t *const *sockets,
                             lh_usize_t count)
{
    lh_usize_t i;

    lh_assert_runtime_if(count > LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX, lh_runtime_error_code_invalid_argument);
    for (i = 0; i < count; ++i)
    {
        out[i] = lh_os_net_socket_get_handle(sockets[i]);
    }
    return count;
}

lh_ssize_t
lh_os_net_socket_wait(lh_os_net_socket_t *const *read, lh_usize_t read_count, lh_os_net_socket_t *const *write,
                      lh_usize_t write_count, lh_u32_t timeout_ms)
{
    lh_os_system_net_socket_handle_t readable[LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX];
    lh_os_system_net_socket_handle_t writable[LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX];

    return lh_os_system_net_socket_wait(readable, lh_os_net_socket_get_handles(readable, read, read_count), writable,
                                        lh_os_net_socket_get_handles(writable, write, write_count), timeout_ms);
}

lh_ssize_t
lh_os_net_socket_send(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_send(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, self)), buf,
                                        size);
}

lh_ssize_t
lh_os_net_socket_recv(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_recv(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, self)), buf,
                                        size);
}

lh_ssize_t
lh_os_net_socket_pending(const lh_os_net_socket_t *self)
{
    return lh_os_system_net_socket_pending(lh_os_net_socket_get_handle(self));
}

lh_ssize_t
lh_os_net_socket_sendto(lh_self_ptr self, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_sendto(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, self)),
                                          buf, size, addr);
}

lh_ssize_t
lh_os_net_socket_recvfrom(lh_self_ptr self, lh_ptr buf, lh_usize_t size,
                          lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_recvfrom(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, self)),
                                            buf, size, addr);
}

lh_void
lh_os_net_socket_get_reader(lh_os_net_socket_t *self, lh_io_reader_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_reader_t reader;

    lh_assert_runtime_ref(self);
    lh_io_reader_init(lh_addr_of(reader), lh_os_net_socket_recv, self);
    lh_ptr_deref(out) = reader;
}

lh_void
lh_os_net_socket_get_writer(lh_os_net_socket_t *self, lh_io_writer_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_io_writer_init(lh_addr_of(writer), lh_os_net_socket_send, self);
    lh_ptr_deref(out) = writer;
}

lh_void
lh_os_net_socket_get_stream(lh_os_net_socket_t *self, lh_io_stream_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_reader_t reader;
    lh_os_net_socket_get_reader(self, lh_addr_of(reader));
    lh_io_writer_t writer;
    lh_os_net_socket_get_writer(self, lh_addr_of(writer));

    lh_io_stream_t stream;
    lh_io_stream_init(lh_addr_of(stream), lh_addr_of(reader), lh_addr_of(writer));
    lh_ptr_deref(out) = stream;
}

lh_void
lh_os_net_socket_get_dgram_reader(lh_os_net_socket_t *self, lh_io_dgram_reader_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_dgram_reader_t reader;

    lh_assert_runtime_ref(self);
    lh_io_dgram_reader_init(lh_addr_of(reader), lh_os_net_socket_recvfrom, self);
    lh_ptr_deref(out) = reader;
}

lh_void
lh_os_net_socket_get_dgram_writer(lh_os_net_socket_t *self, lh_io_dgram_writer_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_dgram_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_io_dgram_writer_init(lh_addr_of(writer), lh_os_net_socket_sendto, self);
    lh_ptr_deref(out) = writer;
}

lh_void
lh_os_net_socket_get_dgram(lh_os_net_socket_t *self, lh_io_dgram_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_dgram_reader_t reader;
    lh_os_net_socket_get_dgram_reader(self, lh_addr_of(reader));
    lh_io_dgram_writer_t writer;
    lh_os_net_socket_get_dgram_writer(self, lh_addr_of(writer));

    lh_io_dgram_t dgram;
    lh_io_dgram_init(lh_addr_of(dgram), lh_addr_of(reader), lh_addr_of(writer));
    lh_ptr_deref(out) = dgram;
}
