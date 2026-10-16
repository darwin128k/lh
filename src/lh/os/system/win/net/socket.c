#include <lh/os/system/net/socket.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/memory.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/net/socket/addr.h>
#include <lh/os/system/win/ws2_32.h>
#include <lh/util/return.h>

LH_ATTRIBUTE_STATIC
lh_os_system_win_socket_t
lh_os_system_net_socket_native(lh_os_system_net_socket_handle_t handle)
{
    return lh_cast_static(lh_os_system_win_socket_t, handle);
}

LH_ATTRIBUTE_STATIC
int
lh_os_system_net_socket_size(lh_usize_t size)
{
    return lh_cast_static(int, lh_math_min(size, lh_cast_static(lh_usize_t, LH_INT_T_MAX)));
}

/* A failed call on a non-blocking socket that only means "not yet" is not
   captured: it is what every empty poll says, and capturing formats a message. */
LH_ATTRIBUTE_STATIC
lh_ssize_t
lh_os_system_net_socket_result(int result)
{
    if (lh_math_eq(result, LH_OS_SYSTEM_WIN_SOCKET_ERROR))
    {
        if (lh_math_eq(lh_os_system_error_get_native_code(),
                       lh_cast_static(lh_os_system_error_code_t, LH_OS_SYSTEM_WIN_WSAEWOULDBLOCK)))
        {
            return LH_OS_RESULT_PENDING;
        }
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_cast_static(lh_ssize_t, result);
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

    const lh_os_system_win_socket_t native =
        lh_math_eq(type, lh_os_system_net_socket_type_tcp)
            ? socket(LH_OS_SYSTEM_WIN_AF_INET, LH_OS_SYSTEM_WIN_SOCK_STREAM,
                     LH_OS_SYSTEM_WIN_IPPROTO_TCP)
            : socket(LH_OS_SYSTEM_WIN_AF_INET, LH_OS_SYSTEM_WIN_SOCK_DGRAM,
                     LH_OS_SYSTEM_WIN_IPPROTO_UDP);
    if (lh_math_eq(native, LH_OS_SYSTEM_WIN_INVALID_SOCKET))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    }
    return lh_cast_static(lh_os_system_net_socket_handle_t, native);
}

lh_void
lh_os_system_net_socket_close(lh_os_system_net_socket_handle_t handle)
{
    (void)closesocket(lh_os_system_net_socket_native(handle));
}

lh_bool_t
lh_os_system_net_socket_connect(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr)
{
    lh_os_system_win_sockaddr_in_t native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    return lh_os_system_net_socket_ok(connect(lh_os_system_net_socket_native(handle),
                                              lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)),
                                              sizeof(native_addr)));
}

lh_bool_t
lh_os_system_net_socket_set_blocking(lh_os_system_net_socket_handle_t handle, lh_bool_t blocking)
{
    lh_ulong_t non_blocking = blocking ? 0UL : 1UL;

    return lh_os_system_net_socket_ok(
        ioctlsocket(lh_os_system_net_socket_native(handle), LH_OS_SYSTEM_WIN_FIONBIO, lh_addr_of(non_blocking)));
}

lh_ssize_t
lh_os_system_net_socket_connect_start(lh_os_system_net_socket_handle_t handle,
                                      const lh_net_ip4_socket_addr_t *addr)
{
    lh_os_system_win_sockaddr_in_t native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    /* Winsock says WSAEWOULDBLOCK for a connect under way, which the result
       helper already turns into PENDING. */
    return lh_os_system_net_socket_result(connect(lh_os_system_net_socket_native(handle),
                                                  lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)),
                                                  sizeof(native_addr)));
}

lh_ssize_t
lh_os_system_net_socket_connect_wait(lh_os_system_net_socket_handle_t handle, lh_u32_t timeout_ms)
{
    const lh_os_system_win_socket_t native = lh_os_system_net_socket_native(handle);
    lh_os_system_win_fd_set_t writable;
    lh_os_system_win_fd_set_t failed;
    lh_os_system_win_timeval_t timeout;
    lh_int_t error = 0;
    lh_int_t error_size = lh_cast_static(lh_int_t, sizeof(error));

    /* Winsock reports a connect that succeeded as writable and one that failed in
       the except set -- not as writable, which is where POSIX puts both. */
    writable.fd_count = 1;
    writable.fd_array[0] = native;
    failed.fd_count = 1;
    failed.fd_array[0] = native;
    timeout.tv_sec = lh_cast_static(lh_long_t, timeout_ms / 1000U);
    timeout.tv_usec = lh_cast_static(lh_long_t, (timeout_ms % 1000U) * 1000U);

    const lh_int_t ready = select(0, lh_null, lh_addr_of(writable), lh_addr_of(failed), lh_addr_of(timeout));
    if (lh_math_eq(ready, LH_OS_SYSTEM_WIN_SOCKET_ERROR))
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    if (lh_math_is_zero(ready))
    {
        return LH_OS_RESULT_PENDING;
    }
    if (!lh_os_system_net_socket_ok(getsockopt(native, LH_OS_SYSTEM_WIN_SOL_SOCKET, LH_OS_SYSTEM_WIN_SO_ERROR,
                                               lh_ptr_cast(char, lh_addr_of(error)), lh_addr_of(error_size))))
    {
        return LH_OS_RESULT_INVALID;
    }
    if (lh_math_ne(error, 0))
    {
        /* The reason is the socket's, not the last call's: put it where capture
           reads from. */
        WSASetLastError(error);
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return 0;
}

lh_bool_t
lh_os_system_net_socket_bind(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr)
{
    lh_os_system_win_sockaddr_in_t native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    return lh_os_system_net_socket_ok(bind(lh_os_system_net_socket_native(handle),
                                           lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)),
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
    lh_os_system_win_sockaddr_in_t native_addr;
    int addr_len = sizeof(native_addr);

    lh_assert_runtime_ref(peer);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    const lh_os_system_win_socket_t native = accept(
        lh_os_system_net_socket_native(handle),
        lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)), lh_addr_of(addr_len));
    if (lh_math_eq(native, LH_OS_SYSTEM_WIN_INVALID_SOCKET))
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
    lh_os_system_win_sockaddr_in_t native_addr;
    int addr_len = sizeof(native_addr);

    lh_assert_runtime_ref(out);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    if (!lh_os_system_net_socket_ok(getsockname(lh_os_system_net_socket_native(handle),
                                                lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)),
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

    return lh_os_system_net_socket_ok(setsockopt(lh_os_system_net_socket_native(handle),
                                                 LH_OS_SYSTEM_WIN_SOL_SOCKET,
                                                 LH_OS_SYSTEM_WIN_SO_REUSEADDR,
                                                 lh_ptr_ccast(char, lh_addr_of(value)),
                                                 lh_cast_static(int, sizeof(value))));
}

lh_bool_t
lh_os_system_net_socket_set_no_delay(lh_os_system_net_socket_handle_t handle, lh_bool_t enabled)
{
    const int value = enabled ? 1 : 0;

    return lh_os_system_net_socket_ok(setsockopt(lh_os_system_net_socket_native(handle),
                                                 LH_OS_SYSTEM_WIN_IPPROTO_TCP, LH_OS_SYSTEM_WIN_TCP_NODELAY,
                                                 lh_ptr_ccast(char, lh_addr_of(value)),
                                                 lh_cast_static(int, sizeof(value))));
}

LH_ATTRIBUTE_STATIC
lh_void
lh_os_system_net_socket_fill_set(lh_os_system_win_fd_set_t *set, const lh_os_system_net_socket_handle_t *handles,
                                 lh_usize_t count)
{
    lh_usize_t i;

    lh_assert_runtime_if(count > LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX, lh_runtime_error_code_invalid_argument);
    set->fd_count = lh_cast_static(lh_uint_t, count);
    for (i = 0; i < count; ++i)
    {
        set->fd_array[i] = lh_os_system_net_socket_native(handles[i]);
    }
}

lh_ssize_t
lh_os_system_net_socket_wait(const lh_os_system_net_socket_handle_t *read, lh_usize_t read_count,
                             const lh_os_system_net_socket_handle_t *write, lh_usize_t write_count,
                             lh_u32_t timeout_ms)
{
    lh_os_system_win_fd_set_t readable;
    lh_os_system_win_fd_set_t writable;
    lh_os_system_win_timeval_t timeout;

    /* Winsock refuses a select with every set empty (WSAEINVAL) where POSIX
       would sleep; neither is what a caller with nothing to watch wants. */
    lh_return_if(lh_math_is_zero(read_count + write_count), LH_OS_RESULT_PENDING);
    lh_os_system_net_socket_fill_set(lh_addr_of(readable), read, read_count);
    lh_os_system_net_socket_fill_set(lh_addr_of(writable), write, write_count);
    timeout.tv_sec = lh_cast_static(lh_long_t, timeout_ms / 1000U);
    timeout.tv_usec = lh_cast_static(lh_long_t, (timeout_ms % 1000U) * 1000U);

    const lh_int_t ready = select(0, lh_math_is_zero(read_count) ? lh_null : lh_addr_of(readable),
                                  lh_math_is_zero(write_count) ? lh_null : lh_addr_of(writable), lh_null,
                                  lh_addr_of(timeout));
    if (lh_math_eq(ready, LH_OS_SYSTEM_WIN_SOCKET_ERROR))
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_math_is_zero(ready) ? LH_OS_RESULT_PENDING : lh_cast_static(lh_ssize_t, ready);
}

lh_ssize_t
lh_os_system_net_socket_send(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_result(send(lh_os_system_net_socket_native(handle), lh_ptr_ccast(char, buf),
                                               lh_os_system_net_socket_size(size), 0));
}

lh_ssize_t
lh_os_system_net_socket_recv(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_result(recv(lh_os_system_net_socket_native(handle), lh_ptr_cast(char, buf),
                                               lh_os_system_net_socket_size(size), 0));
}

lh_ssize_t
lh_os_system_net_socket_pending(lh_os_system_net_socket_handle_t handle)
{
    lh_ulong_t pending = 0UL;

    if (!lh_os_system_net_socket_ok(
            ioctlsocket(lh_os_system_net_socket_native(handle), LH_OS_SYSTEM_WIN_FIONREAD, lh_addr_of(pending))))
    {
        return LH_OS_RESULT_INVALID;
    }
    return lh_cast_static(lh_ssize_t, pending);
}

lh_ssize_t
lh_os_system_net_socket_sendto(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size,
                               const lh_net_ip4_socket_addr_t *addr)
{
    lh_os_system_win_sockaddr_in_t native_addr;

    lh_assert_runtime_ref(addr);
    lh_os_system_net_socket_native_addr_from_ip4(lh_addr_of(native_addr), addr);
    return lh_os_system_net_socket_result(sendto(lh_os_system_net_socket_native(handle), lh_ptr_ccast(char, buf),
                                                 lh_os_system_net_socket_size(size), 0,
                                                 lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)),
                                                 sizeof(native_addr)));
}

lh_ssize_t
lh_os_system_net_socket_recvfrom(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size,
                                 lh_net_ip4_socket_addr_t *addr)
{
    lh_os_system_win_sockaddr_in_t native_addr;
    int addr_len = sizeof(native_addr);

    lh_assert_runtime_ref(addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    const lh_ssize_t result = lh_os_system_net_socket_result(recvfrom(
        lh_os_system_net_socket_native(handle), lh_ptr_cast(char, buf),
        lh_os_system_net_socket_size(size), 0,
        lh_ptr_rcast(lh_os_system_win_sockaddr_t, lh_addr_of(native_addr)), lh_addr_of(addr_len)));
    if (!lh_math_is_negative(result))
    {
        lh_os_system_net_socket_ip4_from_native_addr(addr, lh_addr_of(native_addr));
    }
    return result;
}
