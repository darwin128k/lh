/**
 * @file net_socket.c
 * @brief Non-blocking server sockets, inheritance and output-preserving failures.
 */
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/system.h>
#include <lh/os/thread.h>
#include <lh/os/tick.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#    include <windows.h>
#else
#    include <fcntl.h>
#endif

#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "net_socket:%d: %s\n", __LINE__, #value);                              \
            abort();                                                                               \
        }                                                                                          \
    } while (0)

/** @brief Check the native inheritance flag; POSIX also exposes non-blocking mode. */
static lh_void
check_flags(const lh_os_net_socket_t *self)
{
    const lh_os_system_net_socket_handle_t handle = lh_os_net_socket_get_handle(self);
#if defined(_WIN32)
    DWORD flags = 0;
    CHECK(GetHandleInformation(lh_cast_reinterpret(HANDLE, handle), &flags));
    CHECK(!(flags & HANDLE_FLAG_INHERIT));
#else
    const int native = lh_cast_static(int, handle);
    const int flags = fcntl(native, F_GETFD, 0);
    const int status = fcntl(native, F_GETFL, 0);
    CHECK(flags >= 0 && (flags & FD_CLOEXEC));
    CHECK(status >= 0 && (status & O_NONBLOCK));
#endif
}

/** @brief Wait for one byte or EOF without allowing a failure to masquerade as pending. */
static lh_ssize_t
receive_within(lh_os_net_socket_t *self, lh_char_t *byte)
{
    const lh_u64_t start = lh_os_tick_ms64();
    lh_ssize_t result;
    do
    {
        result = lh_os_net_socket_recv(self, byte, 1);
        CHECK(result != LH_OS_RESULT_INVALID);
        if (result != LH_OS_RESULT_PENDING)
        {
            return result;
        }
        CHECK(lh_os_tick_ms64() - start < 5000);
        CHECK(lh_os_thread_sleep(1));
    } while (lh_bool_true);
}

int
main(void)
{
    lh_os_net_socket_t listener, client, accepted, udp;
    lh_net_ip4_t ip, actual;
    lh_net_ip4_socket_addr_t address, peer;
    lh_char_t byte = 0;
    CHECK(lh_os_net_init());
    lh_os_net_socket_init(&listener);
    lh_os_net_socket_init(&client);
    lh_os_net_socket_init(&accepted);
    lh_os_net_socket_init(&udp);
    CHECK(lh_os_net_socket_open_nonblocking(&udp, lh_os_system_net_socket_type_udp));
    check_flags(&udp);
    lh_os_net_socket_close(&udp);
    CHECK(lh_os_net_socket_open_nonblocking(&listener, lh_os_system_net_socket_type_tcp));
    check_flags(&listener);
    lh_net_ip4_init(&ip, 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(&address, &ip, 0);
    CHECK(lh_os_net_socket_set_reuse_addr(&listener, lh_bool_true));
    CHECK(lh_os_net_socket_bind(&listener, &address));
    CHECK(lh_os_net_socket_listen(&listener, 4));
    CHECK(lh_os_net_socket_get_local_addr(&listener, &address));
    CHECK(lh_net_ip4_socket_addr_get_port(&address) != 0);
    lh_net_ip4_socket_addr_init(&peer, &ip, 77);
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    CHECK(!lh_os_net_socket_accept_nonblocking(&listener, &accepted, &peer));
    CHECK(lh_os_system_get_last_error_code() == 12345);
    CHECK(!lh_os_net_socket_is_valid(&accepted));
    CHECK(lh_net_ip4_socket_addr_get_port(&peer) == 77);
    lh_net_ip4_socket_addr_get_ip(&peer, &actual);
    CHECK(lh_net_ip4_equals(&actual, &ip));
    CHECK(lh_os_net_socket_open(&client, lh_os_system_net_socket_type_tcp));
    CHECK(lh_os_net_socket_connect(&client, &address));
    lh_os_net_socket_t *read[1] = {&listener};
    CHECK(lh_os_net_socket_wait(read, 1, lh_null, 0, 5000) > 0);
    CHECK(lh_os_net_socket_accept_nonblocking(&listener, &accepted, &peer));
    check_flags(&accepted);
    lh_net_ip4_socket_addr_get_ip(&peer, &actual);
    CHECK(lh_net_ip4_equals(&actual, &ip));
    CHECK(lh_os_net_socket_get_local_addr(&client, &address));
    CHECK(lh_net_ip4_socket_addr_get_port(&peer) == lh_net_ip4_socket_addr_get_port(&address));
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    CHECK(lh_os_net_socket_recv(&accepted, &byte, 1) == LH_OS_RESULT_PENDING);
    CHECK(lh_os_system_get_last_error_code() == 12345);
    CHECK(lh_os_net_socket_send(&client, "x", 1) == 1);
    CHECK(receive_within(&accepted, &byte) == 1 && byte == 'x');
    CHECK(lh_os_net_socket_send(&accepted, "y", 1) == 1);
    CHECK(receive_within(&client, &byte) == 1 && byte == 'y');
    lh_os_net_socket_close(&client);
    CHECK(receive_within(&accepted, &byte) == 0);
    lh_os_net_socket_close(&accepted);
    lh_os_net_socket_close(&listener);
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    CHECK(!lh_os_net_socket_accept_nonblocking(&listener, &accepted, &peer));
    CHECK(lh_os_system_get_last_error_code() != 12345);
    CHECK(!lh_os_net_socket_is_valid(&accepted));
    lh_os_net_deinit();
    return 0;
}
