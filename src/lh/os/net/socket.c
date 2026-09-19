#include <lh/os/net/socket.h>
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/memory.h>
#include <lh/net/ip.h>
#include <lh/numeric/types.h>
#include <lh/util/addr.h>
#include <lh/util/bit/endian.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <winsock2.h>
typedef SOCKET lh_os_net_native_handle_t;
typedef int lh_os_net_native_addr_len_t;
#else
#    include <netinet/in.h>
#    include <sys/socket.h>
#    include <unistd.h>
typedef int lh_os_net_native_handle_t;
typedef socklen_t lh_os_net_native_addr_len_t;
#endif

static lh_os_net_native_handle_t
lh_os_net_socket_native_handle(const lh_os_net_socket_t *self)
{
    return lh_cast_static(lh_os_net_native_handle_t, self->handle);
}

static void
lh_os_net_socket_native_addr_from_ip4(struct sockaddr_in *native_addr,
                                      const lh_net_ip4_socket_addr_t *addr)
{
    lh_net_ip4_t ip;
    lh_net_port_t port;
    lh_uchar_t *addr_bytes;

    ip = lh_net_ip4_socket_addr_get_ip(addr);
    port = lh_net_ip4_socket_addr_get_port(addr);

    lh_memory_set(lh_addr_of(*native_addr), sizeof(*native_addr), 0);
    native_addr->sin_family = AF_INET;

    /* sin_addr is always network byte order in memory, byte for byte the same
     * order as the dotted-quad octets — writing the raw bytes avoids
     * depending on htonl. sin_port is also network byte order, but that one
     * genuinely is "pack a u16 as big-endian bytes", so it goes through
     * lh_bit_pack_be16 instead of a second hand-rolled shift/mask. */
    addr_bytes = lh_ptr_rcast(lh_uchar_t, lh_addr_of(native_addr->sin_addr));
    addr_bytes[0] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_0);
    addr_bytes[1] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_1);
    addr_bytes[2] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_2);
    addr_bytes[3] = lh_net_ip4_get_octet(lh_addr_of(ip), LH_NET_IP4_OCTET_INDEX_3);

    lh_bit_pack_be16(port, lh_ptr_rcast(lh_uchar_t, lh_addr_of(native_addr->sin_port)));
}

static void
lh_os_net_socket_ip4_from_native_addr(lh_net_ip4_socket_addr_t *addr,
                                      const struct sockaddr_in *native_addr)
{
    const lh_uchar_t *addr_bytes;
    lh_net_ip4_t ip;
    lh_net_port_t port;

    addr_bytes = lh_ptr_ccast(lh_uchar_t, lh_addr_of(native_addr->sin_addr));
    ip = lh_net_ip4_make(addr_bytes[0], addr_bytes[1], addr_bytes[2], addr_bytes[3]);
    port = lh_bit_unpack_be16(lh_ptr_ccast(lh_uchar_t, lh_addr_of(native_addr->sin_port)));
    *addr = lh_net_ip4_socket_addr_make(lh_addr_of(ip), port);
}

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

    handle = lh_cast_static(lh_os_net_socket_handle_t, socket(AF_INET, native_type, native_protocol));
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
        closesocket(lh_cast_static(lh_os_net_native_handle_t, self->handle));
#else
        close(lh_cast_static(lh_os_net_native_handle_t, self->handle));
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
    return lh_cast_static(lh_bool_t, self->handle != LH_OS_NET_SOCKET_HANDLE_INVALID);
}

lh_bool_t
lh_os_net_socket_connect(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;
    lh_int_t result;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(addr);

    lh_os_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    result = connect(lh_os_net_socket_native_handle(self),
                     lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), sizeof(native_addr));
    return lh_cast_static(lh_bool_t, result == 0);
}

lh_bool_t
lh_os_net_socket_bind(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;
    lh_int_t result;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(addr);

    lh_os_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    result = bind(lh_os_net_socket_native_handle(self),
                  lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), sizeof(native_addr));
    return lh_cast_static(lh_bool_t, result == 0);
}

lh_bool_t
lh_os_net_socket_listen(lh_os_net_socket_t *self, lh_int_t backlog)
{
    lh_int_t result;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(backlog < 0,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    result = listen(lh_os_net_socket_native_handle(self), backlog);
    return lh_cast_static(lh_bool_t, result == 0);
}

lh_bool_t
lh_os_net_socket_accept(lh_os_net_socket_t *self, lh_os_net_socket_t *client,
                        lh_net_ip4_socket_addr_t *peer)
{
    struct sockaddr_in native_addr;
    lh_os_net_native_addr_len_t addr_len;
    lh_os_net_socket_handle_t handle;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(client);
    lh_assert_runtime_ref(peer);
    lh_assert_runtime_if(client->handle != LH_OS_NET_SOCKET_HANDLE_INVALID,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    addr_len = sizeof(native_addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    handle = lh_cast_static(lh_os_net_socket_handle_t,
                            accept(lh_os_net_socket_native_handle(self),
                                   lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                   lh_addr_of(addr_len)));
    if (handle == LH_OS_NET_SOCKET_HANDLE_INVALID)
    {
        return lh_bool_false;
    }
    client->handle = handle;
    lh_os_net_socket_ip4_from_native_addr(peer, lh_addr_of(native_addr));
    return lh_bool_true;
}

lh_bool_t
lh_os_net_socket_get_local_addr(const lh_os_net_socket_t *self, lh_net_ip4_socket_addr_t *out)
{
    struct sockaddr_in native_addr;
    lh_os_net_native_addr_len_t addr_len;
    lh_int_t result;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);

    addr_len = sizeof(native_addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    result = getsockname(lh_os_net_socket_native_handle(self),
                         lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                         lh_addr_of(addr_len));
    if (result != 0)
    {
        return lh_bool_false;
    }
    lh_os_net_socket_ip4_from_native_addr(out, lh_addr_of(native_addr));
    return lh_bool_true;
}

lh_bool_t
lh_os_net_socket_set_reuse_addr(lh_os_net_socket_t *self, lh_bool_t enabled)
{
    lh_int_t value;
    lh_int_t result;

    lh_assert_runtime_ref(self);

    value = enabled ? 1 : 0;
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    result = setsockopt(lh_os_net_socket_native_handle(self), SOL_SOCKET, SO_REUSEADDR,
                        lh_ptr_ccast(char, lh_addr_of(value)),
                        lh_cast_static(lh_int_t, sizeof(value)));
#else
    result = setsockopt(lh_os_net_socket_native_handle(self), SOL_SOCKET, SO_REUSEADDR,
                        lh_addr_of(value), sizeof(value));
#endif
    return lh_cast_static(lh_bool_t, result == 0);
}

lh_ssize_t
lh_os_net_socket_send(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    lh_os_net_socket_t *self;

    lh_assert_runtime_ref(context);
    self = lh_ptr_cast(lh_os_net_socket_t, context);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_int_t result =
            send(lh_cast_static(lh_os_net_native_handle_t, self->handle), lh_ptr_ccast(char, buf),
                 lh_cast_static(lh_int_t, size), 0);
        return result == SOCKET_ERROR ? lh_cast_static(lh_ssize_t, -1)
                                      : lh_cast_static(lh_ssize_t, result);
    }
#else
    return lh_cast_static(lh_ssize_t,
                          send(lh_cast_static(lh_os_net_native_handle_t, self->handle), buf,
                               lh_cast_static(size_t, size), 0));
#endif
}

lh_ssize_t
lh_os_net_socket_recv(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    lh_os_net_socket_t *self;

    lh_assert_runtime_ref(context);
    self = lh_ptr_cast(lh_os_net_socket_t, context);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_int_t result =
            recv(lh_cast_static(lh_os_net_native_handle_t, self->handle), lh_ptr_cast(char, buf),
                 lh_cast_static(lh_int_t, size), 0);
        return result == SOCKET_ERROR ? lh_cast_static(lh_ssize_t, -1)
                                      : lh_cast_static(lh_ssize_t, result);
    }
#else
    return lh_cast_static(lh_ssize_t,
                          recv(lh_cast_static(lh_os_net_native_handle_t, self->handle), buf,
                               lh_cast_static(size_t, size), 0));
#endif
}

lh_ssize_t
lh_os_net_socket_sendto(lh_ptr context, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr)
{
    lh_os_net_socket_t *self;
    struct sockaddr_in native_addr;

    lh_assert_runtime_ref(context);
    lh_assert_runtime_ref(addr);
    self = lh_ptr_cast(lh_os_net_socket_t, context);

    lh_os_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_int_t result = sendto(lh_os_net_socket_native_handle(self), lh_ptr_ccast(char, buf),
                                 lh_cast_static(lh_int_t, size), 0,
                                 lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                 sizeof(native_addr));
        return result == SOCKET_ERROR ? lh_cast_static(lh_ssize_t, -1)
                                      : lh_cast_static(lh_ssize_t, result);
    }
#else
    return lh_cast_static(
        lh_ssize_t, sendto(lh_os_net_socket_native_handle(self), buf, lh_cast_static(size_t, size),
                           0, lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                           sizeof(native_addr)));
#endif
}

lh_ssize_t
lh_os_net_socket_recvfrom(lh_ptr context, lh_ptr buf, lh_usize_t size,
                          lh_net_ip4_socket_addr_t *addr)
{
    lh_os_net_socket_t *self;
    struct sockaddr_in native_addr;
    lh_os_net_native_addr_len_t addr_len;

    lh_assert_runtime_ref(context);
    lh_assert_runtime_ref(addr);
    self = lh_ptr_cast(lh_os_net_socket_t, context);

    addr_len = sizeof(native_addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        lh_int_t result = recvfrom(lh_os_net_socket_native_handle(self), lh_ptr_cast(char, buf),
                                   lh_cast_static(lh_int_t, size), 0,
                                   lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                   lh_addr_of(addr_len));
        if (result == SOCKET_ERROR)
        {
            return lh_cast_static(lh_ssize_t, -1);
        }
        lh_os_net_socket_ip4_from_native_addr(addr, lh_addr_of(native_addr));
        return lh_cast_static(lh_ssize_t, result);
    }
#else
    {
        lh_ssize_t result = lh_cast_static(
            lh_ssize_t, recvfrom(lh_os_net_socket_native_handle(self), buf,
                                 lh_cast_static(size_t, size), 0,
                                 lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                 lh_addr_of(addr_len)));
        if (result < 0)
        {
            return result;
        }
        lh_os_net_socket_ip4_from_native_addr(addr, lh_addr_of(native_addr));
        return result;
    }
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

    lh_assert_runtime_ref(self);
    reader = lh_os_net_socket_get_dgram_reader(self);
    writer = lh_os_net_socket_get_dgram_writer(self);
    return lh_io_dgram_make(lh_addr_of(reader), lh_addr_of(writer));
}
