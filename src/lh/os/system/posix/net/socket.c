#include <lh/os/system/net/socket.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/os/system/error/capture.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../net/socket/addr.h"

LH_ATTRIBUTE_STATIC
int
lh_os_system_net_socket_native(lh_os_system_net_socket_handle_t handle)
{
    return lh_cast_static(int, handle);
}

LH_ATTRIBUTE_STATIC
lh_ssize_t
lh_os_system_net_socket_result(lh_ssize_t result)
{
    if (lh_math_is_negative(result))
    {
        lh_os_system_error_capture();
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
    int native;

    lh_assert_runtime_if(lh_math_ne(type, lh_os_system_net_socket_type_tcp) &&
                             lh_math_ne(type, lh_os_system_net_socket_type_udp),
                         lh_runtime_error_code_invalid_argument);

    native = lh_math_eq(type, lh_os_system_net_socket_type_tcp) ? socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
                                                                : socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (lh_math_is_negative(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    }
    return lh_cast_static(lh_os_system_net_socket_handle_t, native);
}

void
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
    socklen_t addr_len;
    int native;

    lh_assert_runtime_ref(peer);
    addr_len = sizeof(native_addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    native = accept(lh_os_system_net_socket_native(handle), lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)),
                    lh_addr_of(addr_len));
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
    socklen_t addr_len;

    lh_assert_runtime_ref(out);
    addr_len = sizeof(native_addr);
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
    int value;

    value = enabled ? 1 : 0;
    return lh_os_system_net_socket_ok(
        setsockopt(lh_os_system_net_socket_native(handle), SOL_SOCKET, SO_REUSEADDR, lh_addr_of(value), sizeof(value)));
}

lh_ssize_t
lh_os_system_net_socket_send(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_result(
        lh_cast_static(lh_ssize_t, send(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size), 0)));
}

lh_ssize_t
lh_os_system_net_socket_recv(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_net_socket_result(
        lh_cast_static(lh_ssize_t, recv(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size), 0)));
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
    socklen_t addr_len;
    lh_ssize_t result;

    lh_assert_runtime_ref(addr);
    addr_len = sizeof(native_addr);
    lh_memory_set(lh_addr_of(native_addr), sizeof(native_addr), 0);
    result = lh_os_system_net_socket_result(lh_cast_static(
        lh_ssize_t, recvfrom(lh_os_system_net_socket_native(handle), buf, lh_cast_static(size_t, size), 0,
                             lh_ptr_rcast(struct sockaddr, lh_addr_of(native_addr)), lh_addr_of(addr_len))));
    if (!lh_math_is_negative(result))
    {
        lh_os_system_net_socket_ip4_from_native_addr(addr, lh_addr_of(native_addr));
    }
    return result;
}
