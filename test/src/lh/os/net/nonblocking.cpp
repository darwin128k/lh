#include <gtest/gtest.h>
#include <lh/test/os.h>
#include <lh/os/system.h>
#if defined(_WIN32)
#    include <windows.h>
#else
#    include <fcntl.h>
#endif

namespace
{
void
check_flags(const lh_os_net_socket_t *self)
{
    const auto handle = lh_os_net_socket_get_handle(self);
#if defined(_WIN32)
    DWORD flags = 0;
    ASSERT_TRUE(GetHandleInformation(reinterpret_cast<HANDLE>(handle), &flags));
    EXPECT_EQ(flags & HANDLE_FLAG_INHERIT, 0u);
#else
    const int flags = fcntl(static_cast<int>(handle), F_GETFD, 0);
    const int status = fcntl(static_cast<int>(handle), F_GETFL, 0);
    ASSERT_GE(flags, 0);
    ASSERT_GE(status, 0);
    EXPECT_NE(flags & FD_CLOEXEC, 0);
    EXPECT_NE(status & O_NONBLOCK, 0);
#endif
}

lh_ssize_t
receive_within(lh_os_net_socket_t *self, lh_char_t *byte)
{
    lh_ssize_t result = LH_OS_RESULT_PENDING;
    if (!lh_test::wait_until(
            [&]
            {
                result = lh_os_net_socket_recv(self, byte, 1);
                return result != LH_OS_RESULT_PENDING;
            }))
    {
        ADD_FAILURE() << "receive timed out";
        return LH_OS_RESULT_INVALID;
    }
    return result;
}
class nonblocking_socket : public lh_test::network_test
{
  protected:
    lh_test::socket_owner listener, client, accepted;
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t address, peer;
    void
    SetUp() override
    {
        ASSERT_NO_FATAL_FAILURE(lh_test::network_test::SetUp());
        lh_net_ip4_init(&ip, 127, 0, 0, 1);
        lh_net_ip4_socket_addr_init(&address, &ip, 0);
        lh_net_ip4_socket_addr_init(&peer, &ip, 77);
        ASSERT_TRUE(
            lh_os_net_socket_open_nonblocking(&listener.value, lh_os_system_net_socket_type_tcp));
        ASSERT_NO_FATAL_FAILURE(check_flags(&listener.value));
        ASSERT_TRUE(lh_os_net_socket_set_reuse_addr(&listener.value, lh_bool_true));
        ASSERT_TRUE(lh_os_net_socket_bind(&listener.value, &address));
        ASSERT_TRUE(lh_os_net_socket_listen(&listener.value, 4));
        ASSERT_TRUE(lh_os_net_socket_get_local_addr(&listener.value, &address));
        ASSERT_NE(lh_net_ip4_socket_addr_get_port(&address), 0u);
    }
    void
    TearDown() override
    {
        lh_os_net_socket_close(&accepted.value);
        lh_os_net_socket_close(&client.value);
        lh_os_net_socket_close(&listener.value);
        lh_test::network_test::TearDown();
    }
};

using nonblocking_udp = lh_test::network_test;
} // namespace
TEST_F(nonblocking_udp, opens_with_noninheritable_handle)
{
    lh_test::socket_owner socket;
    ASSERT_TRUE(lh_os_net_socket_open_nonblocking(&socket.value, lh_os_system_net_socket_type_udp));
    check_flags(&socket.value);
}

TEST_F(nonblocking_socket, empty_accept_preserves_destination_peer_and_error)
{
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    EXPECT_FALSE(lh_os_net_socket_accept_nonblocking(&listener.value, &accepted.value, &peer));
    EXPECT_EQ(lh_os_system_get_last_error_code(), 12345u);
    EXPECT_FALSE(lh_os_net_socket_is_valid(&accepted.value));
    EXPECT_EQ(lh_net_ip4_socket_addr_get_port(&peer), 77u);
    lh_net_ip4_t actual;
    lh_net_ip4_socket_addr_get_ip(&peer, &actual);
    EXPECT_TRUE(lh_net_ip4_equals(&actual, &ip));
}

TEST_F(nonblocking_socket, accepts_peer_with_nonblocking_io_and_bidirectional_data)
{
    ASSERT_TRUE(lh_os_net_socket_open(&client.value, lh_os_system_net_socket_type_tcp));
    ASSERT_TRUE(lh_os_net_socket_connect(&client.value, &address));
    lh_os_net_socket_t *read[] = {&listener.value};
    ASSERT_GT(lh_os_net_socket_wait(read, 1, nullptr, 0, 5000), 0);
    ASSERT_TRUE(lh_os_net_socket_accept_nonblocking(&listener.value, &accepted.value, &peer));
    ASSERT_NO_FATAL_FAILURE(check_flags(&accepted.value));
    lh_net_ip4_t actual;
    lh_net_ip4_socket_addr_get_ip(&peer, &actual);
    EXPECT_TRUE(lh_net_ip4_equals(&actual, &ip));
    ASSERT_TRUE(lh_os_net_socket_get_local_addr(&client.value, &address));
    EXPECT_EQ(lh_net_ip4_socket_addr_get_port(&peer), lh_net_ip4_socket_addr_get_port(&address));
    lh_char_t byte = 0;
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    EXPECT_EQ(lh_os_net_socket_recv(&accepted.value, &byte, 1), LH_OS_RESULT_PENDING);
    EXPECT_EQ(lh_os_system_get_last_error_code(), 12345u);
    ASSERT_EQ(lh_os_net_socket_send(&client.value, "x", 1), 1);
    ASSERT_EQ(receive_within(&accepted.value, &byte), 1);
    EXPECT_EQ(byte, 'x');
    ASSERT_EQ(lh_os_net_socket_send(&accepted.value, "y", 1), 1);
    ASSERT_EQ(receive_within(&client.value, &byte), 1);
    EXPECT_EQ(byte, 'y');
    lh_os_net_socket_close(&client.value);
    EXPECT_EQ(receive_within(&accepted.value, &byte), 0);
}

TEST_F(nonblocking_socket, closed_listener_captures_failure_without_creating_destination)
{
    lh_os_net_socket_close(&listener.value);
    lh_os_system_error_set_code(lh_os_system_last_error(), 12345);
    EXPECT_FALSE(lh_os_net_socket_accept_nonblocking(&listener.value, &accepted.value, &peer));
    EXPECT_NE(lh_os_system_get_last_error_code(), 12345u);
    EXPECT_FALSE(lh_os_net_socket_is_valid(&accepted.value));
}
