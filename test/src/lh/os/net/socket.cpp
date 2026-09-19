#include <gtest/gtest.h>

#include <string>
#include <thread>

#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>

namespace
{

class LoopbackEchoServer
{
public:
    LoopbackEchoServer()
    {
        lh_net_ip4_t loopback_ip = lh_net_ip4_make(127, 0, 0, 1);
        lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_make(&loopback_ip, 0);
        lh_net_ip4_socket_addr_t local{};

        lh_os_net_socket_init(&listen_);
        lh_os_net_socket_open(&listen_, lh_os_net_socket_type_tcp);
        lh_os_net_socket_bind(&listen_, &addr);
        lh_os_net_socket_get_local_addr(&listen_, &local);
        port_ = lh_net_ip4_socket_addr_get_port(&local);
        lh_os_net_socket_listen(&listen_, 1);

        thread_ = std::thread([this] { AcceptAndEchoOnce(); });
    }

    ~LoopbackEchoServer()
    {
        thread_.join();
        lh_os_net_socket_close(&listen_);
    }

    lh_net_port_t
    port() const
    {
        return port_;
    }

private:
    void
    AcceptAndEchoOnce()
    {
        lh_os_net_socket_t client;
        lh_net_ip4_socket_addr_t peer{};
        char buf[64];
        lh_ssize_t n;

        lh_os_net_socket_init(&client);
        if (lh_os_net_socket_accept(&listen_, &client, &peer) != lh_bool_true)
        {
            return;
        }
        {
            lh_io_stream_t stream = lh_os_net_socket_get_stream(&client);
            n = lh_io_stream_read(&stream, buf, sizeof(buf));
            if (n > 0)
            {
                lh_io_stream_write(&stream, buf, static_cast<lh_usize_t>(n));
            }
        }
        lh_os_net_socket_close(&client);
    }

    lh_os_net_socket_t listen_{};
    lh_net_port_t port_{};
    std::thread thread_;
};

class OsNetSocketTest : public ::testing::Test
{
protected:
    static void
    SetUpTestSuite()
    {
        ASSERT_EQ(lh_os_net_init(), lh_bool_true);
    }

    static void
    TearDownTestSuite()
    {
        lh_os_net_deinit();
    }
};

TEST_F(OsNetSocketTest, init_leaves_handle_invalid)
{
    lh_os_net_socket_t sock;
    lh_os_net_socket_init(&sock);
    EXPECT_EQ(lh_os_net_socket_is_valid(&sock), lh_bool_false);
    EXPECT_EQ(lh_os_net_socket_get_handle(&sock), LH_OS_NET_SOCKET_HANDLE_INVALID);
}

TEST_F(OsNetSocketTest, open_then_close_round_trips_validity)
{
    lh_os_net_socket_t sock;
    lh_os_net_socket_init(&sock);

    ASSERT_EQ(lh_os_net_socket_open(&sock, lh_os_net_socket_type_tcp), lh_bool_true);
    EXPECT_EQ(lh_os_net_socket_is_valid(&sock), lh_bool_true);

    lh_os_net_socket_close(&sock);
    EXPECT_EQ(lh_os_net_socket_is_valid(&sock), lh_bool_false);
}

TEST_F(OsNetSocketTest, open_udp_socket_succeeds)
{
    lh_os_net_socket_t sock;
    lh_os_net_socket_init(&sock);

    ASSERT_EQ(lh_os_net_socket_open(&sock, lh_os_net_socket_type_udp), lh_bool_true);
    EXPECT_EQ(lh_os_net_socket_is_valid(&sock), lh_bool_true);

    lh_os_net_socket_close(&sock);
}

TEST_F(OsNetSocketTest, bind_zero_port_reports_ephemeral_via_get_local_addr)
{
    lh_os_net_socket_t sock;
    lh_net_ip4_t loopback_ip = lh_net_ip4_make(127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_make(&loopback_ip, 0);
    lh_net_ip4_socket_addr_t local{};

    lh_os_net_socket_init(&sock);
    ASSERT_EQ(lh_os_net_socket_open(&sock, lh_os_net_socket_type_tcp), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_bind(&sock, &addr), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_get_local_addr(&sock, &local), lh_bool_true);

    EXPECT_NE(lh_net_ip4_socket_addr_get_port(&local), 0U);
    {
        lh_net_ip4_t local_ip = lh_net_ip4_socket_addr_get_ip(&local);
        EXPECT_TRUE(lh_net_ip4_is_loopback(&local_ip));
    }

    lh_os_net_socket_close(&sock);
}

TEST_F(OsNetSocketTest, set_reuse_addr_then_bind_succeeds)
{
    lh_os_net_socket_t sock;
    lh_net_ip4_t loopback_ip = lh_net_ip4_make(127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_make(&loopback_ip, 0);

    lh_os_net_socket_init(&sock);
    ASSERT_EQ(lh_os_net_socket_open(&sock, lh_os_net_socket_type_tcp), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_set_reuse_addr(&sock, lh_bool_true), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_bind(&sock, &addr), lh_bool_true);

    lh_os_net_socket_close(&sock);
}

TEST_F(OsNetSocketTest, udp_bind_zero_port_reports_ephemeral)
{
    lh_os_net_socket_t sock;
    lh_net_ip4_t loopback_ip = lh_net_ip4_make(127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_make(&loopback_ip, 0);
    lh_net_ip4_socket_addr_t local{};

    lh_os_net_socket_init(&sock);
    ASSERT_EQ(lh_os_net_socket_open(&sock, lh_os_net_socket_type_udp), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_bind(&sock, &addr), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_get_local_addr(&sock, &local), lh_bool_true);
    EXPECT_NE(lh_net_ip4_socket_addr_get_port(&local), 0U);

    lh_os_net_socket_close(&sock);
}

TEST_F(OsNetSocketTest, connect_send_recv_round_trip_over_real_loopback_socket)
{
    LoopbackEchoServer server;

    lh_os_net_socket_t sock;
    lh_os_net_socket_init(&sock);
    ASSERT_EQ(lh_os_net_socket_open(&sock, lh_os_net_socket_type_tcp), lh_bool_true);

    lh_net_ip4_t loopback_ip = lh_net_ip4_make(127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_make(&loopback_ip, server.port());

    ASSERT_EQ(lh_os_net_socket_connect(&sock, &addr), lh_bool_true);

    lh_io_stream_t stream = lh_os_net_socket_get_stream(&sock);

    lh_ssize_t written = lh_io_stream_write(&stream, "ping", 4);
    ASSERT_EQ(written, 4);

    char buf[8] = {};
    lh_ssize_t n = lh_io_stream_read(&stream, buf, sizeof(buf));
    ASSERT_EQ(n, 4);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "ping");

    lh_os_net_socket_close(&sock);
}

TEST_F(OsNetSocketTest, sendto_recvfrom_round_trip_over_real_loopback_udp)
{
    lh_os_net_socket_t server;
    lh_os_net_socket_t client;
    lh_net_ip4_t loopback_ip = lh_net_ip4_make(127, 0, 0, 1);
    lh_net_ip4_socket_addr_t bind_any = lh_net_ip4_socket_addr_make(&loopback_ip, 0);
    lh_net_ip4_socket_addr_t server_addr{};

    lh_os_net_socket_init(&server);
    lh_os_net_socket_init(&client);
    ASSERT_EQ(lh_os_net_socket_open(&server, lh_os_net_socket_type_udp), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_open(&client, lh_os_net_socket_type_udp), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_bind(&server, &bind_any), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_bind(&client, &bind_any), lh_bool_true);
    ASSERT_EQ(lh_os_net_socket_get_local_addr(&server, &server_addr), lh_bool_true);

    std::thread echo([&server] {
        lh_io_dgram_t dgram = lh_os_net_socket_get_dgram(&server);
        char buf[64] = {};
        lh_net_ip4_socket_addr_t peer{};
        lh_ssize_t n = lh_io_dgram_recv(&dgram, buf, sizeof(buf), &peer);
        if (n > 0)
        {
            lh_io_dgram_send(&dgram, buf, static_cast<lh_usize_t>(n), &peer);
        }
    });

    lh_io_dgram_t dgram = lh_os_net_socket_get_dgram(&client);
    ASSERT_EQ(lh_io_dgram_send(&dgram, "ping", 4, &server_addr), 4);

    char buf[8] = {};
    lh_net_ip4_socket_addr_t peer{};
    lh_ssize_t n = lh_io_dgram_recv(&dgram, buf, sizeof(buf), &peer);
    echo.join();

    ASSERT_EQ(n, 4);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "ping");
    EXPECT_TRUE(lh_net_ip4_socket_addr_equals(&peer, &server_addr));

    lh_os_net_socket_close(&client);
    lh_os_net_socket_close(&server);
}

} // namespace
