#include <gtest/gtest.h>

#include <lh/net/modbus/link.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/tick.h>
#include <lh/test/out.h>

#include <chrono>
#include <cstring>
#include <thread>

/* -- A device on this machine ----------------------------------------------
   Everything in lh_mb_link was tested over a pair of arrays. These are the same
   questions over a real socket: a listener on 127.0.0.1 plays the device, with the
   ordinary blocking socket calls, and the link on the other end reads the socket's
   own stream (::lh_os_net_socket_get_stream) without ever waiting for an answer.
   There is no Modbus transport in between any more: a non-blocking socket already
   says "nothing yet" and "closed" the way a link needs them said. */
namespace
{

/** The captured answer to a read of three registers at 10 (see modbus_link.cpp);
    its transaction id is overwritten with the one the request carried. */
const lh_byte_t kRead3Holding[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x03,
                                   0x06, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};

/** A read request over MBAP is twelve bytes. */
const lh_u16_t kRequestSize = 12;

lh_u64_t
tick_us(lh_ptr self)
{
    (void)self;
    return lh_os_tick_us();
}

lh_net_ip4_socket_addr_t
loopback(lh_u16_t port)
{
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;

    lh_net_ip4_init(&ip, 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(&addr, &ip, port);
    return addr;
}

struct device
{
    lh_os_net_socket_t listener;
    lh_u16_t port;

    device()
    {
        lh_net_ip4_socket_addr_t addr = loopback(0);

        lh_os_net_socket_init(&listener);
        EXPECT_TRUE(lh_os_net_socket_open(&listener, lh_os_system_net_socket_type_tcp));
        EXPECT_TRUE(lh_os_net_socket_bind(&listener, &addr));
        EXPECT_TRUE(lh_os_net_socket_listen(&listener, 16));
        EXPECT_TRUE(lh_os_net_socket_get_local_addr(&listener, &addr));
        port = lh_net_ip4_socket_addr_get_port(&addr);
    }

    ~device()
    {
        lh_os_net_socket_close(&listener);
    }

    /* The connection the client made; it is already in the backlog, so this does
       not wait. */
    void
    accept(lh_os_net_socket_t *client)
    {
        lh_net_ip4_socket_addr_t peer;

        lh_os_net_socket_init(client);
        ASSERT_TRUE(lh_os_net_socket_accept(&listener, client, &peer));
    }
};

/* The client's side: a socket, connected without waiting longer than asked, and
   the stream a link reads. */
struct connection
{
    lh_os_net_socket_t socket;
    lh_io_stream_t stream;

    connection()
    {
        lh_os_net_socket_init(&socket);
    }

    ~connection()
    {
        lh_os_net_socket_close(&socket);
    }

    bool
    open(const lh_net_ip4_socket_addr_t &addr, lh_u32_t timeout_ms)
    {
        lh_os_net_socket_close(&socket);
        if (!lh_os_net_socket_open(&socket, lh_os_system_net_socket_type_tcp) ||
            !lh_os_net_socket_connect_within(&socket, &addr, timeout_ms))
        {
            lh_os_net_socket_close(&socket);
            return false;
        }
        stream = lh_test::out(lh_os_net_socket_get_stream, &socket);
        return true;
    }
};

/* Read one whole request off a blocking socket. */
bool
read_request(lh_os_net_socket_t *client, lh_byte_t *request)
{
    lh_u16_t have = 0;

    while (have < kRequestSize)
    {
        const lh_ssize_t got = lh_os_net_socket_recv(client, request + have, kRequestSize - have);
        if (got <= 0)
        {
            return false;
        }
        have = (lh_u16_t)(have + got);
    }
    return true;
}

/* Answer @p request with the captured frame, under the request's own transaction. */
void
answer(lh_os_net_socket_t *client, const lh_byte_t *request)
{
    lh_byte_t frame[sizeof(kRead3Holding)];

    memcpy(frame, kRead3Holding, sizeof(frame));
    frame[0] = request[0];
    frame[1] = request[1];
    ASSERT_EQ(lh_os_net_socket_send(client, frame, sizeof(frame)), (lh_ssize_t)sizeof(frame));
}

/* Poll until the link leaves `waiting`, a millisecond apart, for at most two
   seconds of wall time. */
lh_mb_link_state_t
poll_until_done(lh_mb_link_t *link)
{
    lh_mb_link_state_t state = lh_mb_link_waiting;

    for (int i = 0; i < 2000 && state == lh_mb_link_waiting; ++i)
    {
        state = lh_mb_link_poll(link, 1000000);
        if (state == lh_mb_link_waiting)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    return state;
}

class os_net_modbus_tcp : public ::testing::Test
{
  protected:
    void
    SetUp() override
    {
        ASSERT_TRUE(lh_os_net_init());
    }

    void
    TearDown() override
    {
        lh_os_net_deinit();
    }
};

} /* namespace */

TEST_F(os_net_modbus_tcp, a_question_is_answered_over_a_real_socket)
{
    device dev;
    connection to;
    lh_mb_link_t link;
    lh_os_net_socket_t client;
    lh_byte_t request[kRequestSize];
    const lh_u16_t *values;
    lh_u16_t count = 0;

    ASSERT_TRUE(to.open(loopback(dev.port), 1000));
    dev.accept(&client);
    lh_mb_link_init(&link, &to.stream, lh_mb_framing_mbap, 1, tick_us, lh_null);

    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));
    ASSERT_TRUE(read_request(&client, request));
    EXPECT_EQ(request[7], LH_MB_FC_READ_HOLDING);
    answer(&client, request);

    ASSERT_EQ(poll_until_done(&link), lh_mb_link_answered);
    values = lh_mb_link_take(&link, &count);
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(count, 3);
    EXPECT_EQ(values[0], 1);

    lh_os_net_socket_close(&client);
}

TEST_F(os_net_modbus_tcp, a_poll_with_nothing_on_the_wire_does_not_wait)
{
    device dev;
    connection to;
    lh_mb_link_t link;
    lh_os_net_socket_t client;

    /* The device has the question and has not answered. A blocking read would sit
       here until it did -- and with eight devices in a pass, one quiet device would
       hold up the other seven. */
    ASSERT_TRUE(to.open(loopback(dev.port), 1000));
    dev.accept(&client);
    lh_mb_link_init(&link, &to.stream, lh_mb_framing_mbap, 1, tick_us, lh_null);
    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));

    const lh_u64_t before = lh_os_tick_us();
    for (int i = 0; i < 100; ++i)
    {
        ASSERT_EQ(lh_mb_link_poll(&link, 1000000), lh_mb_link_waiting) << "nothing came, nothing broke";
    }
    EXPECT_LT(lh_os_tick_us() - before, 500000u) << "a hundred polls of an idle socket take no time";
    EXPECT_EQ(link.counters.failed, 0u);

    lh_os_net_socket_close(&client);
}

TEST_F(os_net_modbus_tcp, a_device_that_hangs_up_is_a_lost_stream_and_a_new_one_brings_it_back)
{
    device dev;
    connection to;
    lh_mb_link_t link;
    lh_os_net_socket_t client;
    lh_byte_t request[kRequestSize];

    ASSERT_TRUE(to.open(loopback(dev.port), 1000));
    dev.accept(&client);
    lh_mb_link_init(&link, &to.stream, lh_mb_framing_mbap, 1, tick_us, lh_null);
    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));
    ASSERT_TRUE(read_request(&client, request));

    /* The device closes without answering. A read of 0 is the peer hanging up, and
       a link that took it for "nothing yet" would wait out its timeout on every
       question from now on. */
    lh_os_net_socket_close(&client);
    EXPECT_EQ(poll_until_done(&link), lh_mb_link_failed);
    EXPECT_EQ(link.reason, lh_mb_status_lost) << "said as a lost stream, so the app knows to reconnect";
    EXPECT_EQ(link.counters.failed, 1u);

    ASSERT_TRUE(to.open(loopback(dev.port), 1000)) << "the device is still there";
    dev.accept(&client);
    lh_mb_link_set_stream(&link, &to.stream);
    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));
    ASSERT_TRUE(read_request(&client, request));
    answer(&client, request);
    EXPECT_EQ(poll_until_done(&link), lh_mb_link_answered);

    lh_os_net_socket_close(&client);
}

TEST_F(os_net_modbus_tcp, a_port_nobody_listens_on_is_not_connected_and_costs_no_more_than_the_timeout)
{
    lh_u16_t port;
    connection to;

    {
        device gone; /* a port that was just in use and is not any more */
        port = gone.port;
    }
    /* Linux refuses it at once. Windows does not: it retries a connect that was
       reset and says "refused" only after about two seconds, even on loopback --
       so on Windows it is the timeout that answers, and the timeout is the bound. */
    const lh_u64_t before = lh_os_tick_us();
    EXPECT_FALSE(to.open(loopback(port), 300));
    EXPECT_LT(lh_os_tick_us() - before, 300000u + 500000u) << "never much longer than the timeout";
}

TEST_F(os_net_modbus_tcp, an_address_that_never_answers_costs_the_timeout_and_no_more)
{
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t nowhere;
    connection to;

    /* 10.255.255.1 is private and routed nowhere: a connect to it is not refused,
       it is simply never answered, which is what a switched-off device looks like.
       Left to the operating system that is tens of seconds; here it is 200 ms. A
       machine with no route at all refuses it at once, which is as good. */
    lh_net_ip4_init(&ip, 10, 255, 255, 1);
    lh_net_ip4_socket_addr_init(&nowhere, &ip, 502);
    const lh_u64_t before = lh_os_tick_us();
    EXPECT_FALSE(to.open(nowhere, 200));
    EXPECT_LT(lh_os_tick_us() - before, 1500000u);
}

TEST_F(os_net_modbus_tcp, eight_devices_answer_in_any_order_on_eight_connections)
{
    enum { k_devices = 8 };
    device dev;
    connection to[k_devices];
    lh_mb_link_t link[k_devices];
    lh_os_net_socket_t client[k_devices];
    lh_byte_t request[k_devices][kRequestSize];
    int answered = 0;

    /* Every link has a question out at once, and the devices answer last first.
       Nobody waits for anybody: the first answer to arrive is the first one read. */
    for (int i = 0; i < k_devices; ++i)
    {
        ASSERT_TRUE(to[i].open(loopback(dev.port), 1000));
        dev.accept(&client[i]);
        lh_mb_link_init(&link[i], &to[i].stream, lh_mb_framing_mbap, 1, tick_us, lh_null);
        ASSERT_TRUE(lh_mb_link_ask(&link[i], 10, 3));
    }
    for (int i = 0; i < k_devices; ++i)
    {
        ASSERT_TRUE(read_request(&client[i], request[i]));
    }
    for (int i = k_devices - 1; i >= 0; --i)
    {
        answer(&client[i], request[i]);
        /* Only device i has answered so far; the others are still waiting, and
           polling them must not hold this one up. */
        for (int j = 0; j < k_devices; ++j)
        {
            if (j != i && link[j].state == lh_mb_link_waiting)
            {
                EXPECT_EQ(lh_mb_link_poll(&link[j], 1000000), lh_mb_link_waiting);
            }
        }
        if (poll_until_done(&link[i]) == lh_mb_link_answered)
        {
            ++answered;
        }
    }
    EXPECT_EQ(answered, k_devices);

    for (int i = 0; i < k_devices; ++i)
    {
        lh_os_net_socket_close(&client[i]);
    }
}
