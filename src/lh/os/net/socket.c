#include <lh/os/net/socket.h>
#include <lh/assert.h>
#include <lh/memory.h>
#include <lh/numeric/types.h>
#include <lh/util/addr.h>
#include <lh/util/bit/endian.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <winsock2.h>
typedef SOCKET lh_os_net_native_handle_t;
#else
#    include <netinet/in.h>
#    include <sys/socket.h>
#    include <unistd.h>
typedef int lh_os_net_native_handle_t;
#endif

void
lh_os_net_socket_init(lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_NET_SOCKET_HANDLE_INVALID;
}

lh_bool_t
lh_os_net_socket_open(lh_os_net_socket_t *self, lh_os_net_socket_type_t type)
{
    lh_os_net_socket_handle_t handle;
    lh_int_t native_type;
    lh_int_t native_protocol;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(type != lh_os_net_socket_type_tcp && type != lh_os_net_socket_type_udp,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    if (type == lh_os_net_socket_type_tcp)
    {
        native_type = SOCK_STREAM;
        native_protocol = IPPROTO_TCP;
    }
    else
    {
        native_type = SOCK_DGRAM;
        native_protocol = IPPROTO_UDP;
    }

    handle = (lh_os_net_socket_handle_t)socket(AF_INET, native_type, native_protocol);
    if (handle == LH_OS_NET_SOCKET_HANDLE_INVALID)
    {
        return lh_bool_false;
    }
    self->handle = handle;
    return lh_bool_true;
}

void
lh_os_net_socket_close(lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);

    if (self->handle != LH_OS_NET_SOCKET_HANDLE_INVALID)
    {
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
        closesocket((lh_os_net_native_handle_t)self->handle);
#else
        close((lh_os_net_native_handle_t)self->handle);
#endif
        self->handle = LH_OS_NET_SOCKET_HANDLE_INVALID;
    }
}

lh_os_net_socket_handle_t
lh_os_net_socket_get_handle(const lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_bool_t
lh_os_net_socket_is_valid(const lh_os_net_socket_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle != LH_OS_NET_SOCKET_HANDLE_INVALID ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_os_net_socket_connect(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;
    lh_net_ip4_t ip;
    lh_net_port_t port;
    lh_uchar_t *addr_bytes;
    lh_int_t result;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(addr);

    ip = lh_net_ip4_socket_addr_get_ip(addr);
    port = lh_net_ip4_socket_addr_get_port(addr);

    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    native_addr.sin_family = AF_INET;

    /* sin_addr is always network byte order in memory, byte for byte the same
     * order as the dotted-quad octets — writing the raw bytes avoids
     * depending on htonl. sin_port is also network byte order, but that one
     * genuinely is "pack a u16 as big-endian bytes", so it goes through
     * lh_bit_pack_be16 instead of a second hand-rolled shift/mask. */
    addr_bytes = (lh_uchar_t *)lh_addr_of(native_addr.sin_addr);
    addr_bytes[0] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_0);
    addr_bytes[1] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_1);
    addr_bytes[2] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_2);
    addr_bytes[3] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_3);

    lh_bit_pack_be16(port, (lh_uchar_t *)lh_addr_of(native_addr.sin_port));

    result = connect((lh_os_net_native_handle_t)self->handle, (struct sockaddr *)lh_addr_of(native_addr),
                     sizeof(native_addr));
    return result == 0 ? lh_bool_true : lh_bool_false;
}

lh_ssize_t
lh_os_net_socket_send(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    lh_os_net_socket_t *self;

    lh_assert_runtime_ref(context);
    self = (lh_os_net_socket_t *)context;

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_int_t result = send((lh_os_net_native_handle_t)self->handle, (const char *)buf,
                               (lh_int_t)size, 0);
        return result == SOCKET_ERROR ? (lh_ssize_t)-1 : (lh_ssize_t)result;
    }
#else
    return (lh_ssize_t)send((lh_os_net_native_handle_t)self->handle, buf, (size_t)size, 0);
#endif
}

lh_ssize_t
lh_os_net_socket_recv(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    lh_os_net_socket_t *self;

    lh_assert_runtime_ref(context);
    self = (lh_os_net_socket_t *)context;

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_int_t result = recv((lh_os_net_native_handle_t)self->handle, (char *)buf, (lh_int_t)size, 0);
        return result == SOCKET_ERROR ? (lh_ssize_t)-1 : (lh_ssize_t)result;
    }
#else
    return (lh_ssize_t)recv((lh_os_net_native_handle_t)self->handle, buf, (size_t)size, 0);
#endif
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

    lh_assert_runtime_ref(self);
    reader = lh_os_net_socket_get_reader(self);
    writer = lh_os_net_socket_get_writer(self);
    return lh_io_stream_make(lh_addr_of(reader), lh_addr_of(writer));
}
