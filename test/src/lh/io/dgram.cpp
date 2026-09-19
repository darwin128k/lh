#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <lh/io/dgram.h>
#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>

namespace
{

struct PacketBox
{
    char data[64];
    lh_usize_t size;
    lh_net_ip4_socket_addr_t peer;
};

lh_ssize_t
PacketBoxRecv(lh_ptr context, lh_ptr buf, lh_usize_t size, lh_net_ip4_socket_addr_t *addr)
{
    PacketBox *box = static_cast<PacketBox *>(context);
    lh_usize_t to_copy = size < box->size ? size : box->size;
    std::memcpy(buf, box->data, to_copy);
    *addr = box->peer;
    return static_cast<lh_ssize_t>(to_copy);
}

lh_ssize_t
PacketBoxSend(lh_ptr context, const lh_ptr buf, lh_usize_t size, const lh_net_ip4_socket_addr_t *addr)
{
    PacketBox *box = static_cast<PacketBox *>(context);
    std::memcpy(box->data, buf, size);
    box->size = size;
    box->peer = *addr;
    return static_cast<lh_ssize_t>(size);
}

lh_net_ip4_socket_addr_t
LoopbackAddr(lh_net_port_t port)
{
    lh_net_ip4_t ip = lh_net_ip4_make(127, 0, 0, 1);
    return lh_net_ip4_socket_addr_make(&ip, port);
}

TEST(io_dgram_make, roundtrip_via_getters)
{
    PacketBox box{};
    lh_io_dgram_reader_t reader{};
    lh_io_dgram_writer_t writer{};
    lh_io_dgram_reader_init(&reader, &PacketBoxRecv, &box);
    lh_io_dgram_writer_init(&writer, &PacketBoxSend, &box);

    lh_io_dgram_t dgram = lh_io_dgram_make(&reader, &writer);

    lh_io_dgram_reader_t got_reader = lh_io_dgram_get_reader(&dgram);
    lh_io_dgram_writer_t got_writer = lh_io_dgram_get_writer(&dgram);
    EXPECT_EQ(lh_io_dgram_reader_get_recv_cb(&got_reader), &PacketBoxRecv);
    EXPECT_EQ(lh_io_dgram_writer_get_send_cb(&got_writer), &PacketBoxSend);
}

TEST(io_dgram_send_recv, roundtrips_packet_and_address)
{
    PacketBox box{};
    lh_io_dgram_reader_t reader{};
    lh_io_dgram_writer_t writer{};
    lh_io_dgram_reader_init(&reader, &PacketBoxRecv, &box);
    lh_io_dgram_writer_init(&writer, &PacketBoxSend, &box);

    lh_io_dgram_t dgram = lh_io_dgram_make(&reader, &writer);
    lh_net_ip4_socket_addr_t dest = LoopbackAddr(27015);

    lh_ssize_t written = lh_io_dgram_send(&dgram, "ping", 4, &dest);
    ASSERT_EQ(written, 4);

    char buf[8] = {};
    lh_net_ip4_socket_addr_t peer{};
    lh_ssize_t n = lh_io_dgram_recv(&dgram, buf, sizeof(buf), &peer);
    ASSERT_EQ(n, 4);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "ping");
    EXPECT_TRUE(lh_net_ip4_socket_addr_equals(&peer, &dest));
}

TEST(io_dgram_reader_recv, copies_one_datagram)
{
    PacketBox box{};
    box.size = 4;
    std::memcpy(box.data, "ping", 4);
    box.peer = LoopbackAddr(80);

    lh_io_dgram_reader_t reader{};
    lh_io_dgram_reader_init(&reader, &PacketBoxRecv, &box);

    char buf[8] = {};
    lh_net_ip4_socket_addr_t peer{};
    lh_ssize_t n = lh_io_dgram_reader_recv(&reader, buf, sizeof(buf), &peer);
    ASSERT_EQ(n, 4);
    EXPECT_EQ(std::string(buf, static_cast<size_t>(n)), "ping");
    EXPECT_EQ(lh_net_ip4_socket_addr_get_port(&peer), 80U);
}

TEST(io_dgram_writer_send, stores_one_datagram)
{
    PacketBox box{};
    lh_io_dgram_writer_t writer{};
    lh_io_dgram_writer_init(&writer, &PacketBoxSend, &box);

    lh_net_ip4_socket_addr_t dest = LoopbackAddr(443);
    ASSERT_EQ(lh_io_dgram_writer_send(&writer, "pong", 4, &dest), 4);
    EXPECT_EQ(box.size, 4U);
    EXPECT_EQ(std::string(box.data, 4), "pong");
    EXPECT_TRUE(lh_net_ip4_socket_addr_equals(&box.peer, &dest));
}

} // namespace
