#include <gtest/gtest.h>

#include <string>
#include <thread>

#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>

#if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    include <winsock2.h>
using native_socket_t = SOCKET;
using socklen_compat_t = int;
#else
#    include <arpa/inet.h>
#    include <netinet/in.h>
#    include <sys/socket.h>
#    include <unistd.h>
using native_socket_t = int;
using socklen_compat_t = socklen_t;
#endif

namespace
{

// A tiny loopback echo server built directly on native sockets — not on
// lh_os_net_socket_t, which is deliberately client-only for now (see
// os/net/socket.h). This lets the client-side test below prove a real
// connect/send/recv round trip without lh needing bind/listen/accept yet.
class LoopbackEchoServer
{
public:
    LoopbackEchoServer()
    {
        sockaddr_in addr{};
        socklen_compat_t addr_len = sizeof(addr);

        listen_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = 0; /* ask the OS for an ephemeral port */
        bind(listen_socket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

        getsockname(listen_socket_, reinterpret_cast<sockaddr *>(&addr), &addr_len);
        port_ = static_cast<lh_net_port_t>(ntohs(addr.sin_port));

        listen(listen_socket_, 1);

        thread_ = std::thread([this] { AcceptAndEchoOnce(); });
    }

    ~LoopbackEchoServer()
    {
        thread_.join();
#if defined(_WIN32)
        closesocket(listen_socket_);
#else
        close(listen_socket_);
#endif
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
        native_socket_t client = accept(listen_socket_, nullptr, nullptr);
        char buf[64];
        int n = static_cast<int>(recv(client, buf, sizeof(buf), 0));
        if (n > 0)
        {
            send(client, buf, n, 0);
        }
#if defined(_WIN32)
        closesocket(client);
#else
        close(client);
#endif
    }

    native_socket_t listen_socket_{};
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

} // namespace
