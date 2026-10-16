/**
 * @file socket.cpp
 * @brief Waiting on sockets: what a server loop calls instead of sleeping.
 */

#include <gtest/gtest.h>

#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/tick.h>

namespace
{

struct pair
{
    lh_os_net_socket_t listener;
    lh_os_net_socket_t client;
    lh_os_net_socket_t server;

    pair()
    {
        lh_net_ip4_t ip;
        lh_net_ip4_socket_addr_t addr;
        lh_net_ip4_socket_addr_t peer;

        EXPECT_TRUE(lh_os_net_init());
        lh_net_ip4_init(&ip, 127, 0, 0, 1);
        lh_net_ip4_socket_addr_init(&addr, &ip, 0);
        lh_os_net_socket_init(&listener);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_init(&server);
        EXPECT_TRUE(lh_os_net_socket_open(&listener, lh_os_system_net_socket_type_tcp));
        EXPECT_TRUE(lh_os_net_socket_bind(&listener, &addr));
        EXPECT_TRUE(lh_os_net_socket_listen(&listener, 4));
        EXPECT_TRUE(lh_os_net_socket_get_local_addr(&listener, &addr));
        EXPECT_TRUE(lh_os_net_socket_open(&client, lh_os_system_net_socket_type_tcp));
        EXPECT_TRUE(lh_os_net_socket_connect(&client, &addr));
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
    lh_os_net_socket_t late;
    lh_net_ip4_socket_addr_t addr;

    EXPECT_EQ(lh_os_net_socket_wait(read, 1u, lh_null, 0u, 0u), LH_OS_RESULT_PENDING);
    ASSERT_TRUE(lh_os_net_socket_get_local_addr(&p.listener, &addr));
    lh_os_net_socket_init(&late);
    ASSERT_TRUE(lh_os_net_socket_open(&late, lh_os_system_net_socket_type_tcp));
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
