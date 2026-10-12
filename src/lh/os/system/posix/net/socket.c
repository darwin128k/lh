#include <lh/os/system/net/socket.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/os/system/error/capture.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

/* A send to a peer that has gone raises SIGPIPE, whose default is to end the
   process. A client that keeps connections to many devices cannot have one device
   pulling its cable kill it: the send fails with EPIPE instead. Linux has the flag;
   where it does not exist (macOS has SO_NOSIGPIPE instead) the send is unchanged. */
#if defined(MSG_NOSIGNAL)
#    define LH_OS_SYSTEM_POSIX_SEND_FLAGS MSG_NOSIGNAL
#else
#    define LH_OS_SYSTEM_POSIX_SEND_FLAGS 0
#endif

#include <lh/os/system/net/socket/addr.h>

LH_ATTRIBUTE_STATIC
int
lh_os_system_net_socket_native(lh_os_system_net_socket_handle_t handle)
{
    return lh_cast_static(int, handle);
}

/* A failed call on a non-blocking socket that only means "not yet" is not
   captured: it is what every empty poll says, and capturing formats a message. */
LH_ATTRIBUTE_STATIC
lh_ssize_t
lh_os_system_net_socket_result(lh_ssize_t result)
{
    if (lh_math_is_negative(result))
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return LH_OS_RESULT_PENDING;
        }
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return result;
}

LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_net_socket_ok(int result)
{
    if (lh_math_ne(result, 0))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_os_system_net_socket_handle_t
lh_os_system_net_socket_open(lh_os_system_net_socket_type_t type)
{
    lh_assert_runtime_if(lh_math_ne(type, lh_os_system_net_socket_type_tcp) &&
                             lh_math_ne(type, lh_os_system_net_socket_type_udp),
                         lh_runtime_error_code_invalid_argument);

    const int native = lh_math_eq(type, lh_os_system_net_socket_type_tcp)
                           ? socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
                           : socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (lh_math_is_negative(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    }
    return lh_cast_static(lh_os_system_net_socket_handle_t, native);
}

lh_void
lh_os_system_net_socket_close(lh_os_system_net_socket_handle_t handle)
{
    (void)close(lh_os_system_net_socket_native(handle));
}

lh_bool_t
lh_os_system_net_socket_connect(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    return lh_os_system_net_socket_ok(connect(lh_os_system_net_socket_native(handle),
                                              lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                              sizeof(native_addr)));
}

lh_bool_t
lh_os_system_net_socket_set_blocking(lh_os_system_net_socket_handle_t handle, lh_bool_t blocking)
{
    const int native = lh_os_system_net_socket_native(handle);
    const int flags = fcntl(native, F_GETFL, 0);

    if (lh_math_is_negative(flags))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_os_system_net_socket_ok(fcntl(native, F_SETFL, blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK)));
}

lh_ssize_t
lh_os_system_net_socket_connect_start(lh_os_system_net_socket_handle_t handle,
                                      const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    if (lh_math_is_zero(connect(lh_os_system_net_socket_native(handle),
                                lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), sizeof(native_addr))))
    {
        return 0;
    }
    /* EINTR on a non-blocking connect does not stop it: it carries on in the
       background, exactly as EINPROGRESS does. */
    if (errno == EINPROGRESS || errno == EINTR)
    {
        return LH_OS_RESULT_PENDING;
    }
    lh_os_system_error_capture();
    return LH_OS_RESULT_INVALID;
}

lh_ssize_t
lh_os_system_net_socket_connect_wait(lh_os_system_net_socket_handle_t handle, lh_u32_t timeout_ms)
{
    struct pollfd watch;
    int error = 0;
    socklen_t error_size = sizeof(error);

    /* poll, not select: select's fd_set is a bit set of FD_SETSIZE (1024) bits,
       and a descriptor numbered past it -- which a client holding many
       connections gets -- writes outside the set. */
    watch.fd = lh_os_system_net_socket_native(handle);
    watch.events = POLLOUT;
    watch.revents = 0;
    const int ready = poll(lh_addr_of(watch), 1, timeout_ms > lh_cast_static(lh_u32_t, INT_MAX)? INT_MAX : lh_cast_static(int, timeout_ms));
    if (lh_math_is_negative(ready))
    {
        if (errno == EINTR)
        {
            return LH_OS_RESULT_PENDING;
        }
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    if (lh_math_is_zero(ready))
    {
        return LH_OS_RESULT_PENDING;
    }
    if (!lh_os_system_net_socket_ok(
            getsockopt(watch.fd, SOL_SOCKET, SO_ERROR, lh_addr_of(error), lh_addr_of(error_size))))
    {
        return LH_OS_RESULT_INVALID;
    }
    if (lh_math_ne(error, 0))
    {
        /* The reason is the socket's, not the last call's: put it where capture
           reads from. */
        errno = error;
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return 0;
}

lh_bool_t
lh_os_system_net_socket_bind(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    return lh_os_system_net_socket_ok(bind(lh_os_system_net_socket_native(handle),
                                           lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                           sizeof(native_addr)));
}

lh_bool_t
lh_os_system_net_socket_listen(lh_os_system_net_socket_handle_t handle, lh_int_t backlog)
{
    lh_assert_runtime_if(lh_math_is_negative(backlog), lh_runtime_error_code_invalid_argument);
    return lh_os_system_net_socket_ok(listen(lh_os_system_net_socket_native(handle), backlog));
}

lh_os_system_net_socket_handle_t
lh_os_system_net_socket_accept(lh_os_system_net_socket_handle_t handle, lh_net_ip4_socket_addr_t *peer)
{
    struct sockaddr_in native_addr;
    socklen_t addr_len = sizeof(native_addr);

    lh_assert_runtime_ref(peer);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    const int native =
        accept(lh_os_system_net_socket_native(handle),
               lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), lh_addr_of(addr_len));
    if (lh_math_is_negative(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    }
    lh_os_system_net_socket_ip4_from_native_addr(peer, lh_addr_of(native_addr));
    return lh_cast_static(lh_os_system_net_socket_handle_t, native);
}

lh_bool_t
lh_os_system_net_socket_get_local_addr(lh_os_system_net_socket_handle_t handle, lh_net_ip4_socket_addr_t *out)
{
    struct sockaddr_in native_addr;
    socklen_t addr_len = sizeof(native_addr);

    lh_assert_runtime_ref(out);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    if (!lh_os_system_net_socket_ok(getsockname(lh_os_system_net_socket_native(handle),
                                                lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                                                lh_addr_of(addr_len))))
    {
        return lh_bool_false;
    }
    lh_os_system_net_socket_ip4_from_native_addr(out, lh_addr_of(native_addr));
    return lh_bool_true;
}

lh_bool_t
lh_os_system_net_socket_set_reuse_addr(lh_os_system_net_socket_handle_t handle, lh_bool_t enabled)
{
    const int value = enabled ? 1 : 0;

    return lh_os_system_net_socket_ok(
        setsockopt(lh_os_system_net_socket_native(handle), SOL_SOCKET, SO_REUSEADDR, lh_addr_of(value), sizeof(value)));
}

lh_bool_t
lh_os_system_net_socket_set_no_delay(lh_os_system_net_socket_handle_t handle, lh_bool_t enabled)
{
    const int value = enabled ? 1 : 0;

    return lh_os_system_net_socket_ok(
        setsockopt(lh_os_system_net_socket_native(handle), IPPROTO_TCP, TCP_NODELAY, lh_addr_of(value), sizeof(value)));
}

LH_ATTRIBUTE_STATIC
lh_usize_t
lh_os_system_net_socket_fill_watch(struct pollfd *watch, const lh_os_system_net_socket_handle_t *handles,
                                   lh_usize_t count, short events)
{
    lh_usize_t i;

    lh_assert_runtime_if(count > LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX, lh_runtime_error_code_invalid_argument);
    for (i = 0; i < count; ++i)
    {
        watch[i].fd = lh_os_system_net_socket_native(handles[i]);
        watch[i].events = events;
        watch[i].revents = 0;
    }
    return count;
}

lh_ssize_t
lh_os_system_net_socket_wait(const lh_os_system_net_socket_handle_t *read, lh_usize_t read_count,
                             const lh_os_system_net_socket_handle_t *write, lh_usize_t write_count,
                             lh_u32_t timeout_ms)
{
    struct pollfd watch[2U * LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX];
    lh_usize_t count;

    lh_return_if(lh_math_is_zero(read_count + write_count), LH_OS_RESULT_PENDING);
    count = lh_os_system_net_socket_fill_watch(watch, read, read_count, POLLIN);
    count += lh_os_system_net_socket_fill_watch(watch + count, write, write_count, POLLOUT);

    const int ready = poll(watch, (nfds_t)count, timeout_ms > lh_cast_static(lh_u32_t, INT_MAX)? INT_MAX : lh_cast_static(int, timeout_ms));
    if (lh_math_is_negative(ready))
    {
        lh_return_if(errno == EINTR, LH_OS_RESULT_PENDING);
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_math_is_zero(ready) ? LH_OS_RESULT_PENDING : lh_cast_static(lh_ssize_t, ready);
}

lh_ssize_t
lh_os_system_net_socket_send(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_result(
        lh_cast_static(lh_ssize_t, send(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size),
                                    LH_OS_SYSTEM_POSIX_SEND_FLAGS)));
}

lh_ssize_t
lh_os_system_net_socket_recv(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_result(
        lh_cast_static(lh_ssize_t, recv(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size), 0)));
}

lh_ssize_t
lh_os_system_net_socket_pending(lh_os_system_net_socket_handle_t handle)
{
    int pending = 0;

    if (lh_math_is_negative(ioctl(lh_os_system_net_socket_native(handle), FIONREAD, lh_addr_of(pending))))
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_cast_static(lh_ssize_t, pending);
}

lh_ssize_t
lh_os_system_net_socket_sendto(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size,
                               const lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    return lh_os_system_net_socket_result(lh_cast_static(
        lh_ssize_t, sendto(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size), 0,
                           lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), sizeof(native_addr))));
}

lh_ssize_t
lh_os_system_net_socket_recvfrom(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size,
                                 lh_net_ip4_socket_addr_t *addr)
{
    struct sockaddr_in native_addr;
    socklen_t addr_len = sizeof(native_addr);

    lh_assert_runtime_ref(addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    const lh_ssize_t result = lh_os_system_net_socket_result(lh_cast_static(
        lh_ssize_t,
        recvfrom(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size), 0,
                 lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), lh_addr_of(addr_len))));
    if (!lh_math_is_negative(result))
    {
        lh_os_system_net_socket_ip4_from_native_addr(addr, lh_addr_of(native_addr));
    }
    return result;
}
