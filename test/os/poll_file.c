/**
 * @file poll_file.c
 * @brief Socket readiness, pollable notifications and explicit-offset file transfer.
 */
#define _GNU_SOURCE
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/system/net/socket.h>
#include <lh/os/system.h>
#include <lh/os/wake.h>
#include <lh/os/thread.h>
#include <lh/os/tick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#    include <io.h>
#    include <windows.h>
#else
#    include <unistd.h>
#    include <fcntl.h>
#    include <signal.h>
#    include <pthread.h>
#endif
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "poll_file:%d: %s\n", __LINE__, #value);                               \
            abort();                                                                               \
        }                                                                                          \
    } while (0)

/** @brief Verify that raw pair endpoints are configured before ownership is exposed. */
static lh_void
check_flags(lh_os_system_net_socket_handle_t handle)
{
#if defined(_WIN32)
    DWORD flags = 0;
    CHECK(GetHandleInformation(lh_cast_reinterpret(HANDLE, handle), &flags));
    CHECK(!(flags & HANDLE_FLAG_INHERIT));
#else
    const int native = lh_cast_static(int, handle);
    CHECK(fcntl(native, F_GETFD, 0) & FD_CLOEXEC);
    CHECK(fcntl(native, F_GETFL, 0) & O_NONBLOCK);
#endif
}

static lh_void
signal_later(lh_ptr user)
{
    CHECK(lh_os_thread_sleep(20));
    CHECK(lh_os_wake_signal(user));
}

/** @brief Timeout, output clearing, ignored entries, cross-thread wake and saturation. */
static lh_void
test_wake(void)
{
    lh_os_wake_t wake;
    lh_os_thread_t thread;
    lh_os_net_socket_poll_t watch[LH_OS_NET_SOCKET_POLL_MAX];
    lh_os_wake_init(&wake);
    lh_os_wake_close(&wake);
    CHECK(lh_os_wake_open(&wake, 1000));
    const lh_os_system_net_socket_handle_t original = lh_os_wake_get_handle(&wake);
    CHECK(!lh_os_wake_open(&wake, 1000));
    CHECK(lh_os_wake_get_handle(&wake) == original);
    check_flags(wake.reader);
    check_flags(wake.writer);
    CHECK(lh_os_net_socket_poll(lh_null, 0, 1000) == LH_OS_RESULT_PENDING);
    for (lh_usize_t i = 0; i < LH_OS_NET_SOCKET_POLL_MAX; ++i)
    {
        watch[i].handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
        watch[i].events = lh_os_net_socket_poll_read;
        watch[i].ready = 99;
    }
    CHECK(lh_os_net_socket_poll(watch, LH_OS_NET_SOCKET_POLL_MAX, 1000) == LH_OS_RESULT_PENDING);
    for (lh_usize_t i = 0; i < LH_OS_NET_SOCKET_POLL_MAX; ++i)
    {
        CHECK(watch[i].ready == 0);
    }
    watch[0].handle = wake.reader;
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    CHECK(lh_os_net_socket_poll(watch, 1, 10) == LH_OS_RESULT_PENDING);
    CHECK(lh_os_system_get_last_error_code() == 12345 && !watch[0].ready);
    watch[1].handle = wake.writer;
    watch[1].events = lh_os_net_socket_poll_write;
    CHECK(lh_os_net_socket_poll(watch, 2, 1000) == 1);
    CHECK(watch[0].ready == 0 && (watch[1].ready & lh_os_net_socket_poll_write));
    lh_os_thread_init(&thread);
    CHECK(lh_os_thread_start(&thread, signal_later, &wake, -1, 0));
    CHECK(lh_os_net_socket_poll(watch, 1, 5000) == 1);
    CHECK(watch[0].ready & lh_os_net_socket_poll_read);
    CHECK(lh_os_thread_join(&thread));
    CHECK(lh_os_wake_reset(&wake));
    CHECK(lh_os_net_socket_poll(watch, 1, 0) == LH_OS_RESULT_PENDING);
    CHECK(watch[0].ready == 0);
    for (lh_uint_t i = 0; i < 20000; ++i)
    {
        CHECK(lh_os_wake_signal(&wake));
    }
    CHECK(lh_os_net_socket_poll(watch, 1, 1000) == 1);
    CHECK(lh_os_wake_reset(&wake));
    lh_os_wake_close(&wake);
    CHECK(lh_os_wake_get_handle(&wake) == LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID);
    lh_os_wake_close(&wake);
    CHECK(lh_os_wake_open(&wake, 1000));
    CHECK(lh_os_wake_signal(&wake));
    watch[LH_OS_NET_SOCKET_POLL_MAX - 1].handle = lh_os_wake_get_handle(&wake);
    watch[0].handle = watch[1].handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    CHECK(lh_os_net_socket_poll(watch, LH_OS_NET_SOCKET_POLL_MAX, 1000) == 1);
    CHECK(watch[LH_OS_NET_SOCKET_POLL_MAX - 1].ready & lh_os_net_socket_poll_read);
    lh_os_wake_close(&wake);
}

/** @brief Exercise more than Winsock's default 64 active descriptors. */
static lh_void
test_large_batch(void)
{
    lh_os_wake_t wakes[70];
    lh_os_net_socket_poll_t watch[70];
    for (lh_usize_t i = 0; i < 70; ++i)
    {
        lh_os_wake_init(&wakes[i]);
        CHECK(lh_os_wake_open(&wakes[i], 1000));
        watch[i].handle = lh_os_wake_get_handle(&wakes[i]);
        watch[i].events = lh_os_net_socket_poll_read;
    }
    CHECK(lh_os_wake_signal(&wakes[69]));
    CHECK(lh_os_net_socket_poll(watch, 70, 1000) == 1);
    for (lh_usize_t i = 0; i < 69; ++i)
    {
        CHECK(watch[i].ready == 0);
    }
    CHECK(watch[69].ready & lh_os_net_socket_poll_read);
    for (lh_usize_t i = 0; i < 70; ++i)
    {
        lh_os_wake_close(&wakes[i]);
    }
}

/** @brief Read readiness must still report EOF after the peer closes. */
static lh_void
test_eof(void)
{
    lh_os_system_net_socket_handle_t pair[2];
    lh_char_t byte;
    CHECK(lh_os_system_net_socket_pair(pair, 1000));
    lh_os_system_net_socket_close(pair[1]);
    lh_os_net_socket_poll_t entry = {pair[0], lh_os_net_socket_poll_read, 0};
    CHECK(lh_os_net_socket_poll(&entry, 1, 1000) == 1);
    CHECK(entry.ready & (lh_os_net_socket_poll_read | lh_os_net_socket_poll_hangup));
    CHECK(lh_os_system_net_socket_recv(pair[0], &byte, 1) == 0);
    lh_os_system_net_socket_close(pair[0]);
}

static lh_void
read_exact(lh_os_system_net_socket_handle_t handle, lh_char_t *buffer, lh_usize_t size)
{
    lh_usize_t used = 0;
    while (used < size)
    {
        lh_os_net_socket_poll_t entry = {handle, lh_os_net_socket_poll_read, 0};
        CHECK(lh_os_net_socket_poll(&entry, 1, 5000) == 1);
        lh_ssize_t read_size = lh_os_system_net_socket_recv(handle, buffer + used, size - used);
        CHECK(read_size > 0);
        used += lh_cast_static(lh_usize_t, read_size);
    }
}

/** @brief Partial sends, EOF, errors, backpressure and offsets beyond 32 bits. */
static lh_void
test_file(void)
{
    lh_os_system_net_socket_handle_t pair[2];
    lh_char_t data[4096] = {0};
    lh_u64_t offset = 3;
    FILE *file = tmpfile();
    CHECK(file);
    CHECK(fwrite("0123456789abcdef", 1, 16, file) == 16);
    CHECK(fflush(file) == 0);
#if defined(_WIN32)
    const lh_os_system_fs_file_handle_t source =
        lh_cast_static(lh_os_system_fs_file_handle_t, _get_osfhandle(_fileno(file)));
#else
    const lh_os_system_fs_file_handle_t source = fileno(file);
#endif
    CHECK(lh_os_system_net_socket_pair(pair, 1000));
    check_flags(pair[0]);
    check_flags(pair[1]);
    while (offset < 8)
    {
        const lh_u64_t before = offset;
        const lh_ssize_t sent = lh_os_system_net_socket_send_file(
            pair[1], source, &offset, lh_cast_static(lh_usize_t, 8 - offset));
        CHECK(sent > 0 && offset == before + lh_cast_static(lh_u64_t, sent));
    }
    read_exact(pair[0], data, 5);
    CHECK(memcmp(data, "34567", 5) == 0);
    while (offset < 16)
    {
        const lh_ssize_t sent = lh_os_system_net_socket_send_file(pair[1], source, &offset, 64);
        CHECK(sent > 0);
    }
    read_exact(pair[0], data, 8);
    CHECK(memcmp(data, "89abcdef", 8) == 0);
    CHECK(lh_os_system_net_socket_send_file(pair[1], source, &offset, 64) == 0 && offset == 16);
    CHECK(lh_os_system_net_socket_send_file(pair[1], LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID, &offset,
                                            0) == 0 &&
          offset == 16);
    CHECK(lh_os_system_net_socket_send_file(pair[1], LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID, &offset,
                                            1) == LH_OS_RESULT_INVALID &&
          offset == 16);
    offset = ~lh_cast_static(lh_u64_t, 0);
    CHECK(lh_os_system_net_socket_send_file(pair[1], source, &offset, 1) == LH_OS_RESULT_INVALID);
    CHECK(offset == ~lh_cast_static(lh_u64_t, 0));
#if defined(_WIN32)
    offset = (lh_cast_static(lh_u64_t, 1) << 32) + 3;
    CHECK(lh_os_system_net_socket_send_file(pair[1], source, &offset, 1) == 0);
    CHECK(offset == (lh_cast_static(lh_u64_t, 1) << 32) + 3);
#else
    offset = (lh_cast_static(lh_u64_t, 1) << 32) + 123;
    CHECK(fseeko(file, lh_cast_static(off_t, offset), SEEK_SET) == 0);
    CHECK(fwrite("high", 1, 4, file) == 4 && fflush(file) == 0);
    CHECK(lh_os_system_net_socket_send_file(pair[1], source, &offset, 4) == 4);
    CHECK(offset == (lh_cast_static(lh_u64_t, 1) << 32) + 127);
    read_exact(pair[0], data, 4);
    CHECK(memcmp(data, "high", 4) == 0);
#endif
    /* Fill the stream without reading until a file send must retain its offset. */
    const lh_u64_t start = lh_os_tick_ms64();
    for (;;)
    {
        lh_ssize_t sent;
        do
        {
            sent = lh_os_system_net_socket_send(pair[1], data, sizeof data);
            CHECK(sent > 0 || sent == LH_OS_RESULT_PENDING);
            CHECK(lh_os_tick_ms64() - start < 10000);
        } while (sent > 0);
        offset = 2;
        sent = lh_os_system_net_socket_send_file(pair[1], source, &offset, 8);
        if (sent == LH_OS_RESULT_PENDING)
        {
            CHECK(offset == 2);
            break;
        }
        CHECK(sent > 0 && offset == 2 + lh_cast_static(lh_u64_t, sent));
    }
    lh_os_system_net_socket_close(pair[1]);
    lh_os_system_net_socket_close(pair[0]);
    CHECK(fclose(file) == 0);
}

#if !defined(_WIN32)
static volatile sig_atomic_t interrupted;
static pthread_t main_thread;
static void
receive_signal(int signal_number)
{
    (void)signal_number;
    ++interrupted;
}
static lh_void
interrupt_later(lh_ptr user)
{
    (lh_void) user;
    CHECK(lh_os_thread_sleep(20));
    CHECK(pthread_kill(main_thread, SIGUSR1) == 0);
}
/** @brief Interrupted waits leave both output readiness and captured errors clear. */
static lh_void
test_interruption(void)
{
    struct sigaction action = {0}, previous;
    lh_os_thread_t thread;
    lh_os_wake_t wake;
    action.sa_handler = receive_signal;
    sigemptyset(&action.sa_mask);
    CHECK(sigaction(SIGUSR1, &action, &previous) == 0);
    main_thread = pthread_self();
    lh_os_wake_init(&wake);
    CHECK(lh_os_wake_open(&wake, 1000));
    lh_os_net_socket_poll_t watch = {lh_os_wake_get_handle(&wake), lh_os_net_socket_poll_read, 99};
    lh_os_thread_init(&thread);
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    CHECK(lh_os_thread_start(&thread, interrupt_later, lh_null, -1, 0));
    CHECK(lh_os_net_socket_poll(&watch, 1, 5000) == LH_OS_RESULT_PENDING);
    CHECK(interrupted == 1 && watch.ready == 0 && lh_os_system_get_last_error_code() == 12345);
    CHECK(lh_os_thread_join(&thread));
    lh_os_wake_close(&wake);
    CHECK(sigaction(SIGUSR1, &previous, NULL) == 0);
}
#endif

int
main(void)
{
    CHECK(lh_os_net_init());
    test_wake();
    test_large_batch();
    test_eof();
#if !defined(_WIN32)
    struct sigaction ignore_pipe = {0}, previous_pipe;
    ignore_pipe.sa_handler = SIG_IGN;
    sigemptyset(&ignore_pipe.sa_mask);
    CHECK(sigaction(SIGPIPE, &ignore_pipe, &previous_pipe) == 0);
#endif
    test_file();
#if !defined(_WIN32)
    CHECK(sigaction(SIGPIPE, &previous_pipe, NULL) == 0);
    test_interruption();
#endif
    lh_os_net_deinit();
    return 0;
}
