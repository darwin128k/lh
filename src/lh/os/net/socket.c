#include <lh/os/net/socket.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/os/system/net/socket.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

void
lh_os_net_socket_init(lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
}

lh_bool_t
lh_os_net_socket_open(lh_os_net_socket_t *self, lh_os_system_net_socket_type_t type)
{
    lh_os_system_net_socket_handle_t handle;

    lh_assert_runtime_ref(self);
    handle = lh_os_system_net_socket_open(type);
    if (lh_math_eq(handle, LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    self->handle = handle;
    return lh_bool_true;
}

void
lh_os_net_socket_close(lh_os_net_socket_t *self)
{
    if (lh_os_net_socket_is_valid(self))
    {
        lh_os_system_net_socket_close(lh_os_net_socket_get_handle(self));
        self->handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    }
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

lh_bool_t
lh_os_net_socket_connect(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_connect(lh_os_net_socket_get_handle(self), addr);
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
    lh_os_system_net_socket_handle_t handle;

    lh_assert_runtime_if(lh_os_net_socket_is_valid(client), lh_runtime_error_code_invalid_argument);

    handle = lh_os_system_net_socket_accept(lh_os_net_socket_get_handle(self), peer);
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

lh_ssize_t
lh_os_net_socket_send(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_send(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, context)), buf,
                                        size);
}

lh_ssize_t
lh_os_net_socket_recv(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_recv(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, context)), buf,
                                        size);
}

lh_ssize_t
lh_os_net_socket_sendto(lh_ptr context, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_sendto(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, context)),
                                          buf, size, addr);
}

lh_ssize_t
lh_os_net_socket_recvfrom(lh_ptr context, lh_ptr buf, lh_usize_t size,
                          lh_net_ip4_socket_addr_t *addr)
{
    return lh_os_system_net_socket_recvfrom(lh_os_net_socket_get_handle(lh_ptr_cast(lh_os_net_socket_t, context)),
                                            buf, size, addr);
}

lh_io_reader_t
lh_os_net_socket_get_reader(lh_os_net_socket_t *self)
{
    lh_io_reader_t reader;

    lh_assert_runtime_ref(self);
    lh_io_reader_init(lh_addr_of(reader), lh_os_net_socket_recv, self);
    return reader;
}

lh_io_writer_t
lh_os_net_socket_get_writer(lh_os_net_socket_t *self)
{
    lh_io_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_io_writer_init(lh_addr_of(writer), lh_os_net_socket_send, self);
    return writer;
}

lh_io_stream_t
lh_os_net_socket_get_stream(lh_os_net_socket_t *self)
{
    lh_io_reader_t reader;
    lh_io_writer_t writer;

    reader = lh_os_net_socket_get_reader(self);
    writer = lh_os_net_socket_get_writer(self);
    return lh_io_stream_make(lh_addr_of(reader), lh_addr_of(writer));
}

lh_io_dgram_reader_t
lh_os_net_socket_get_dgram_reader(lh_os_net_socket_t *self)
{
    lh_io_dgram_reader_t reader;

    lh_assert_runtime_ref(self);
    lh_io_dgram_reader_init(lh_addr_of(reader), lh_os_net_socket_recvfrom, self);
    return reader;
}

lh_io_dgram_writer_t
lh_os_net_socket_get_dgram_writer(lh_os_net_socket_t *self)
{
    lh_io_dgram_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_io_dgram_writer_init(lh_addr_of(writer), lh_os_net_socket_sendto, self);
    return writer;
}

lh_io_dgram_t
lh_os_net_socket_get_dgram(lh_os_net_socket_t *self)
{
    lh_io_dgram_reader_t reader;
    lh_io_dgram_writer_t writer;

    reader = lh_os_net_socket_get_dgram_reader(self);
    writer = lh_os_net_socket_get_dgram_writer(self);
    return lh_io_dgram_make(lh_addr_of(reader), lh_addr_of(writer));
}
