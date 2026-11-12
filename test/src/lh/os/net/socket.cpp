/**
 * @file socket.cpp
 * @brief Waiting on sockets: what a server loop calls instead of sleeping.
 */

#include <gtest/gtest.h>

#include <lh/os/net.h>
#include <lh/os/fs/file.h>
#include <lh/os/net/socket.h>
#include <lh/os/system/net/socket/handle.h>
#include <lh/os/tick.h>

#include <cstdio>
#include <cstring>
#include <memory>

namespace
{

struct pair
{
    lh_os_net_socket_t listener;
    lh_os_net_socket_t client;
    lh_os_net_socket_t server;

    pair()
    {
        EXPECT_TRUE(lh_os_net_init());
        lh_net_ip4_t ip;
        lh_net_ip4_init(&ip, 127, 0, 0, 1);
        lh_net_ip4_socket_addr_t addr;
        lh_net_ip4_socket_addr_init(&addr, &ip, 0);
        lh_os_net_socket_init(&listener);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_init(&server);
        EXPECT_TRUE(lh_os_net_socket_open(&listener, lh_net_socket_type_tcp));
        EXPECT_TRUE(lh_os_net_socket_bind(&listener, &addr));
        EXPECT_TRUE(lh_os_net_socket_listen(&listener, 4));
        EXPECT_TRUE(lh_os_net_socket_get_local_addr(&listener, &addr));
        EXPECT_TRUE(lh_os_net_socket_open(&client, lh_net_socket_type_tcp));
        EXPECT_TRUE(lh_os_net_socket_connect(&client, &addr));
        lh_net_ip4_socket_addr_t peer;
        EXPECT_TRUE(lh_os_net_socket_accept(&listener, &server, &peer));
    }

    ~pair()
    {
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
        lh_os_net_socket_close(&listener);
    }
};

} // namespace

TEST(os_net_socket, wait_with_nothing_to_watch_answers_at_once)
{
    const lh_u64_t t0 = lh_os_tick_ms();

    EXPECT_EQ(lh_os_net_socket_wait(lh_null, 0u, lh_null, 0u, 500u), LH_OS_RESULT_PENDING);
    EXPECT_LT(lh_os_tick_ms() - t0, 400u);
}

TEST(os_net_socket, wait_times_out_on_a_quiet_socket_and_wakes_on_data)
{
    pair p;
    lh_os_net_socket_t *const read[1] = {&p.server};
    const char byte = 'x';

    EXPECT_EQ(lh_os_net_socket_wait(read, 1u, lh_null, 0u, 20u), LH_OS_RESULT_PENDING);
    ASSERT_EQ(lh_os_net_socket_send(&p.client, (lh_ptr)&byte, 1u), 1);
    EXPECT_EQ(lh_os_net_socket_wait(read, 1u, lh_null, 0u, 2000u), 1);
}

TEST(os_net_socket, a_listener_is_readable_when_a_connection_waits)
{
    pair p;
    lh_os_net_socket_t *const read[1] = {&p.listener};

    EXPECT_EQ(lh_os_net_socket_wait(read, 1u, lh_null, 0u, 0u), LH_OS_RESULT_PENDING);
    lh_net_ip4_socket_addr_t addr;
    ASSERT_TRUE(lh_os_net_socket_get_local_addr(&p.listener, &addr));
    lh_os_net_socket_t late;
    lh_os_net_socket_init(&late);
    ASSERT_TRUE(lh_os_net_socket_open(&late, lh_net_socket_type_tcp));
    ASSERT_TRUE(lh_os_net_socket_connect(&late, &addr));
    EXPECT_EQ(lh_os_net_socket_wait(read, 1u, lh_null, 0u, 2000u), 1);
    lh_os_net_socket_close(&late);
}

TEST(os_net_socket, a_connected_socket_is_writable_and_takes_no_delay)
{
    pair p;
    lh_os_net_socket_t *const write[1] = {&p.server};

    EXPECT_TRUE(lh_os_net_socket_set_no_delay(&p.server, lh_bool_true));
    EXPECT_TRUE(lh_os_net_socket_set_no_delay(&p.server, lh_bool_false));
    EXPECT_EQ(lh_os_net_socket_wait(lh_null, 0u, write, 1u, 2000u), 1);
}

TEST(os_net_socket, get_poll_watches_the_socket_for_the_events)
{
    const char byte = 'x';

    lh_os_net_socket_poll_t entry;
    pair p;
    lh_os_net_socket_get_poll(&p.server, lh_os_net_socket_poll_read, &entry);
    EXPECT_EQ(lh_os_net_socket_poll_get_handle(&entry), lh_os_net_socket_get_handle(&p.server));
    EXPECT_EQ(lh_os_net_socket_poll_get_events(&entry), lh_os_net_socket_poll_read);
    EXPECT_EQ(lh_os_net_socket_poll_get_ready(&entry), 0u);
    EXPECT_EQ(lh_os_net_socket_poll(&entry, 1u, 0u), LH_OS_RESULT_PENDING);
    ASSERT_EQ(lh_os_net_socket_send(&p.client, (lh_ptr)&byte, 1u), 1);
    EXPECT_EQ(lh_os_net_socket_poll(&entry, 1u, 2000u), 1);
    EXPECT_TRUE(lh_os_net_socket_poll_is_readable(&entry));
}

TEST(os_net_socket, an_empty_poll_entry_is_skipped)
{
    const char byte = 'x';

    lh_os_net_socket_poll_t entries[2];
    lh_os_net_socket_poll_init_empty(&entries[0]);
    EXPECT_EQ(lh_os_net_socket_poll_get_handle(&entries[0]),
              LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID);
    EXPECT_EQ(lh_os_net_socket_poll_get_events(&entries[0]), 0u);
    EXPECT_EQ(lh_os_net_socket_poll_get_ready(&entries[0]), 0u);
    pair p;
    lh_os_net_socket_get_poll(&p.server, lh_os_net_socket_poll_read, &entries[1]);
    ASSERT_EQ(lh_os_net_socket_send(&p.client, (lh_ptr)&byte, 1u), 1);
    EXPECT_EQ(lh_os_net_socket_poll(entries, 2u, 2000u), 1);
    EXPECT_EQ(lh_os_net_socket_poll_get_ready(&entries[0]), 0u);
    EXPECT_TRUE(lh_os_net_socket_poll_is_readable(&entries[1]));
}

TEST(os_net_socket, send_file_sends_from_the_offset_and_advances_it)
{
    const char *name = "lh-test-socket-send-file.tmp";
    std::unique_ptr<std::FILE, decltype(&std::fclose)> file(std::fopen(name, "wb"), std::fclose);
    ASSERT_TRUE(file);
    ASSERT_EQ(std::fwrite("0123456789", 1, 10, file.get()), 10u);
    ASSERT_EQ(std::fflush(file.get()), 0);
    file.reset();
    lh_os_fs_file_t source;
    lh_os_fs_file_init(&source);
    ASSERT_TRUE(lh_os_fs_file_open_cstr(&source, name, lh_fs_file_mode_read));
    lh_u64_t offset = 4;
    char got[8] = {0};
    lh_usize_t received = 0;

    pair p;
    while (offset < 10)
    {
        const lh_u64_t before = offset;
        const lh_ssize_t sent = lh_os_net_socket_send_file(&p.server, &source, &offset, 64u);
        ASSERT_GT(sent, 0);
        ASSERT_EQ(offset, before + static_cast<lh_u64_t>(sent));
    }
    EXPECT_EQ(lh_os_net_socket_send_file(&p.server, &source, &offset, 64u), 0);
    EXPECT_EQ(offset, 10u);
    while (received < 6u)
    {
        const lh_ssize_t n = lh_os_net_socket_recv(&p.client, got + received, 6u - received);
        ASSERT_GT(n, 0);
        received += static_cast<lh_usize_t>(n);
    }
    EXPECT_EQ(std::memcmp(got, "456789", 6), 0);
    lh_os_fs_file_deinit(&source);
    EXPECT_EQ(std::remove(name), 0);
}
