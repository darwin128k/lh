/**
 * @file poll_file.cpp
 * @brief Socket readiness, pollable notifications and explicit-offset file transfer.
 */
#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/system/net/socket.h>
#include <lh/os/system.h>
#include <lh/os/wake.h>
#include <lh/os/thread.h>
#include <lh/os/tick.h>
#include <gtest/gtest.h>
#include <lh/test/os.h>
#include <memory>
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
namespace
{
using socket_poll = lh_test::network_test;
}

/** @brief Verify that raw pair endpoints are configured before ownership is exposed. */
static lh_void
check_flags(lh_os_system_net_socket_handle_t handle)
{
#if defined(_WIN32)
    DWORD flags = 0;
    ASSERT_TRUE(GetHandleInformation(lh_cast_reinterpret(HANDLE, handle), &flags));
    ASSERT_FALSE((flags & HANDLE_FLAG_INHERIT));
#else
    const int native = lh_cast_static(int, handle);
    const int flags = fcntl(native, F_GETFD, 0), status = fcntl(native, F_GETFL, 0);
    ASSERT_GE(flags, 0);
    ASSERT_GE(status, 0);
    EXPECT_NE(flags & FD_CLOEXEC, 0);
    EXPECT_NE(status & O_NONBLOCK, 0);
#endif
}

static lh_void
signal_later(lh_ptr user)
{
    ASSERT_TRUE(lh_os_thread_sleep(20));
    ASSERT_TRUE(lh_os_wake_signal(static_cast<lh_os_wake_t *>(user)));
}

/** @brief Timeout, output clearing, ignored entries, cross-thread wake and saturation. */
TEST_F(socket_poll, wakes_across_threads_and_coalesces_notifications)
{
    lh_test::wake_owner wake_owner;
    auto &wake = wake_owner.value;
    lh_test::thread_owner thread_owner;
    auto &thread = thread_owner.value;
    lh_os_net_socket_poll_t watch[LH_OS_NET_SOCKET_POLL_MAX];
    lh_os_wake_close(&wake);
    ASSERT_TRUE(lh_os_wake_open(&wake, 1000));
    const lh_os_system_net_socket_handle_t original = lh_os_wake_get_handle(&wake);
    ASSERT_FALSE(lh_os_wake_open(&wake, 1000));
    ASSERT_EQ(lh_os_wake_get_handle(&wake), original);
    ASSERT_NO_FATAL_FAILURE(check_flags(wake.reader));
    ASSERT_NO_FATAL_FAILURE(check_flags(wake.writer));
    ASSERT_EQ(lh_os_net_socket_poll(lh_null, 0, 1000), LH_OS_RESULT_PENDING);
    for (lh_usize_t i = 0; i < LH_OS_NET_SOCKET_POLL_MAX; ++i)
    {
        watch[i].handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
        watch[i].events = lh_os_net_socket_poll_read;
        watch[i].ready = 99;
    }
    ASSERT_EQ(lh_os_net_socket_poll(watch, LH_OS_NET_SOCKET_POLL_MAX, 1000), LH_OS_RESULT_PENDING);
    for (lh_usize_t i = 0; i < LH_OS_NET_SOCKET_POLL_MAX; ++i)
    {
        ASSERT_EQ(watch[i].ready, 0);
    }
    watch[0].handle = wake.reader;
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    ASSERT_EQ(lh_os_net_socket_poll(watch, 1, 10), LH_OS_RESULT_PENDING);
    ASSERT_TRUE(lh_os_system_get_last_error_code() == 12345 && !watch[0].ready);
    watch[1].handle = wake.writer;
    watch[1].events = lh_os_net_socket_poll_write;
    ASSERT_EQ(lh_os_net_socket_poll(watch, 2, 1000), 1);
    ASSERT_TRUE(watch[0].ready == 0 && (watch[1].ready & lh_os_net_socket_poll_write));
    ASSERT_TRUE(lh_os_thread_start(&thread, signal_later, &wake, -1, 0));
    ASSERT_EQ(lh_os_net_socket_poll(watch, 1, 5000), 1);
    ASSERT_TRUE(watch[0].ready & lh_os_net_socket_poll_read);
    ASSERT_TRUE(lh_os_thread_join(&thread));
    ASSERT_TRUE(lh_os_wake_reset(&wake));
    ASSERT_EQ(lh_os_net_socket_poll(watch, 1, 0), LH_OS_RESULT_PENDING);
    ASSERT_EQ(watch[0].ready, 0);
    for (lh_uint_t i = 0; i < 20000; ++i)
    {
        ASSERT_TRUE(lh_os_wake_signal(&wake));
    }
    ASSERT_EQ(lh_os_net_socket_poll(watch, 1, 1000), 1);
    ASSERT_TRUE(lh_os_wake_reset(&wake));
    lh_os_wake_close(&wake);
    ASSERT_EQ(lh_os_wake_get_handle(&wake), LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID);
    lh_os_wake_close(&wake);
    ASSERT_TRUE(lh_os_wake_open(&wake, 1000));
    ASSERT_TRUE(lh_os_wake_signal(&wake));
    watch[LH_OS_NET_SOCKET_POLL_MAX - 1].handle = lh_os_wake_get_handle(&wake);
    watch[0].handle = watch[1].handle = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
    ASSERT_EQ(lh_os_net_socket_poll(watch, LH_OS_NET_SOCKET_POLL_MAX, 1000), 1);
    ASSERT_TRUE(watch[LH_OS_NET_SOCKET_POLL_MAX - 1].ready & lh_os_net_socket_poll_read);
    lh_os_wake_close(&wake);
}

/** @brief Exercise more than Winsock's default 64 active descriptors. */
TEST_F(socket_poll, supports_more_than_64_active_descriptors)
{
    lh_test::wake_owner wakes[70];
    lh_os_net_socket_poll_t watch[70];
    for (lh_usize_t i = 0; i < 70; ++i)
    {
        ASSERT_TRUE(lh_os_wake_open(&wakes[i].value, 1000));
        watch[i].handle = lh_os_wake_get_handle(&wakes[i].value);
        watch[i].events = lh_os_net_socket_poll_read;
    }
    ASSERT_TRUE(lh_os_wake_signal(&wakes[69].value));
    ASSERT_EQ(lh_os_net_socket_poll(watch, 70, 1000), 1);
    for (lh_usize_t i = 0; i < 69; ++i)
    {
        ASSERT_EQ(watch[i].ready, 0);
    }
    ASSERT_TRUE(watch[69].ready & lh_os_net_socket_poll_read);
    for (lh_usize_t i = 0; i < 70; ++i)
    {
        lh_os_wake_close(&wakes[i].value);
    }
}

/** @brief Read readiness must still report EOF after the peer closes. */
TEST_F(socket_poll, reports_eof_as_readiness)
{
    lh_test::socket_pair pair_owner;
    auto *pair = pair_owner.handles;
    lh_char_t byte;
    ASSERT_TRUE(pair_owner.open());
    pair_owner.close(1);
    lh_os_net_socket_poll_t entry = {pair[0], lh_os_net_socket_poll_read, 0};
    ASSERT_EQ(lh_os_net_socket_poll(&entry, 1, 1000), 1);
    ASSERT_TRUE(entry.ready & (lh_os_net_socket_poll_read | lh_os_net_socket_poll_hangup));
    ASSERT_EQ(lh_os_system_net_socket_recv(pair[0], &byte, 1), 0);
    pair_owner.close(0);
}

static lh_void
read_exact(lh_os_system_net_socket_handle_t handle, lh_char_t *buffer, lh_usize_t size)
{
    lh_usize_t used = 0;
    while (used < size)
    {
        lh_os_net_socket_poll_t entry = {handle, lh_os_net_socket_poll_read, 0};
        ASSERT_EQ(lh_os_net_socket_poll(&entry, 1, 5000), 1);
        lh_ssize_t read_size = lh_os_system_net_socket_recv(handle, buffer + used, size - used);
        ASSERT_GT(read_size, 0);
        used += lh_cast_static(lh_usize_t, read_size);
    }
}

/** @brief Partial sends, EOF, errors, backpressure and offsets beyond 32 bits. */
TEST_F(socket_poll, transfers_file_with_explicit_offsets_and_preserves_offset_on_pending)
{
    lh_test::socket_pair pair_owner;
    auto *pair = pair_owner.handles;
#if !defined(_WIN32)
    struct sigaction ignore_pipe = {}, previous_pipe;
    ignore_pipe.sa_handler = SIG_IGN;
    sigemptyset(&ignore_pipe.sa_mask);
    ASSERT_EQ(sigaction(SIGPIPE, &ignore_pipe, &previous_pipe), 0);
    lh_test::scope_exit restore_pipe(
        [&] { EXPECT_EQ(sigaction(SIGPIPE, &previous_pipe, nullptr), 0); });
#endif
    lh_char_t data[4096] = {0};
    lh_u64_t offset = 3;
    std::unique_ptr<FILE, decltype(&fclose)> file_owner(tmpfile(), fclose);
    FILE *file = file_owner.get();
    ASSERT_TRUE(file);
    ASSERT_EQ(fwrite("0123456789abcdef", 1, 16, file), 16);
    ASSERT_EQ(fflush(file), 0);
#if defined(_WIN32)
    const lh_os_system_fs_file_handle_t source =
        lh_cast_static(lh_os_system_fs_file_handle_t, _get_osfhandle(_fileno(file)));
#else
    const lh_os_system_fs_file_handle_t source = fileno(file);
#endif
    ASSERT_TRUE(pair_owner.open());
    ASSERT_NO_FATAL_FAILURE(check_flags(pair[0]));
    ASSERT_NO_FATAL_FAILURE(check_flags(pair[1]));
    while (offset < 8)
    {
        const lh_u64_t before = offset;
        const lh_ssize_t sent = lh_os_system_net_socket_send_file(
            pair[1], source, &offset, lh_cast_static(lh_usize_t, 8 - offset));
        ASSERT_TRUE(sent > 0 && offset == before + lh_cast_static(lh_u64_t, sent));
    }
    ASSERT_NO_FATAL_FAILURE(read_exact(pair[0], data, 5));
    ASSERT_EQ(memcmp(data, "34567", 5), 0);
    while (offset < 16)
    {
        const lh_ssize_t sent = lh_os_system_net_socket_send_file(pair[1], source, &offset, 64);
        ASSERT_GT(sent, 0);
    }
    ASSERT_NO_FATAL_FAILURE(read_exact(pair[0], data, 8));
    ASSERT_EQ(memcmp(data, "89abcdef", 8), 0);
    ASSERT_TRUE(lh_os_system_net_socket_send_file(pair[1], source, &offset, 64) == 0 &&
                offset == 16);
    ASSERT_TRUE(lh_os_system_net_socket_send_file(pair[1], LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID,
                                                  &offset, 0) == 0 &&
                offset == 16);
    ASSERT_TRUE(lh_os_system_net_socket_send_file(pair[1], LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID,
                                                  &offset, 1) == LH_OS_RESULT_INVALID &&
                offset == 16);
    offset = ~lh_cast_static(lh_u64_t, 0);
    ASSERT_EQ(lh_os_system_net_socket_send_file(pair[1], source, &offset, 1), LH_OS_RESULT_INVALID);
    ASSERT_EQ(offset, ~lh_cast_static(lh_u64_t, 0));
#if defined(_WIN32)
    offset = (lh_cast_static(lh_u64_t, 1) << 32) + 3;
    ASSERT_EQ(lh_os_system_net_socket_send_file(pair[1], source, &offset, 1), 0);
    ASSERT_EQ(offset, (lh_cast_static(lh_u64_t, 1) << 32) + 3);
#else
    offset = (lh_cast_static(lh_u64_t, 1) << 32) + 123;
    ASSERT_EQ(fseeko(file, lh_cast_static(off_t, offset), SEEK_SET), 0);
    ASSERT_TRUE(fwrite("high", 1, 4, file) == 4 && fflush(file) == 0);
    ASSERT_EQ(lh_os_system_net_socket_send_file(pair[1], source, &offset, 4), 4);
    ASSERT_EQ(offset, (lh_cast_static(lh_u64_t, 1) << 32) + 127);
    ASSERT_NO_FATAL_FAILURE(read_exact(pair[0], data, 4));
    ASSERT_EQ(memcmp(data, "high", 4), 0);
#endif
    /* Fill the stream without reading until a file send must retain its offset. */
    const lh_u64_t start = lh_os_tick_ms64();
    for (;;)
    {
        lh_ssize_t sent;
        do
        {
            sent = lh_os_system_net_socket_send(pair[1], data, sizeof data);
            ASSERT_TRUE(sent > 0 || sent == LH_OS_RESULT_PENDING);
            ASSERT_LT(lh_os_tick_ms64() - start, 10000);
        } while (sent > 0);
        offset = 2;
        sent = lh_os_system_net_socket_send_file(pair[1], source, &offset, 8);
        if (sent == LH_OS_RESULT_PENDING)
        {
            ASSERT_EQ(offset, 2);
            break;
        }
        ASSERT_TRUE(sent > 0 && offset == 2 + lh_cast_static(lh_u64_t, sent));
    }
    pair_owner.close(1);
    pair_owner.close(0);
    EXPECT_EQ(fclose(file_owner.release()), 0);
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
    (void)user;
    ASSERT_TRUE(lh_os_thread_sleep(20));
    ASSERT_EQ(pthread_kill(main_thread, SIGUSR1), 0);
}
/** @brief Interrupted waits leave both output readiness and captured errors clear. */
TEST_F(socket_poll, interruption_preserves_captured_error_and_clears_readiness)
{
    struct sigaction action = {0}, previous;
    lh_test::wake_owner wake_owner;
    auto &wake = wake_owner.value;
    lh_test::thread_owner thread_owner;
    auto &thread = thread_owner.value;
    action.sa_handler = receive_signal;
    sigemptyset(&action.sa_mask);
    ASSERT_EQ(sigaction(SIGUSR1, &action, &previous), 0);
    lh_test::scope_exit restore_signal(
        [&]
        {
            EXPECT_TRUE(lh_os_thread_join(&thread));
            EXPECT_EQ(sigaction(SIGUSR1, &previous, nullptr), 0);
        });
    interrupted = 0;
    main_thread = pthread_self();
    ASSERT_TRUE(lh_os_wake_open(&wake, 1000));
    lh_os_net_socket_poll_t watch = {lh_os_wake_get_handle(&wake), lh_os_net_socket_poll_read, 99};
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    ASSERT_TRUE(lh_os_thread_start(&thread, interrupt_later, lh_null, -1, 0));
    ASSERT_EQ(lh_os_net_socket_poll(&watch, 1, 5000), LH_OS_RESULT_PENDING);
    ASSERT_TRUE(interrupted == 1 && watch.ready == 0 &&
                lh_os_system_get_last_error_code() == 12345);
    ASSERT_TRUE(lh_os_thread_join(&thread));
    lh_os_wake_close(&wake);
}
#endif
