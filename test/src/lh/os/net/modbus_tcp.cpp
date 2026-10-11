#include <gtest/gtest.h>

#include <lh/net/ip.h>
#include <lh/net/modbus/link.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/null.h>
#include <lh/os/net.h>
#include <lh/os/net/modbus/tcp.h>
#include <lh/os/net/socket.h>
#include <lh/os/tick.h>
#include <lh/util/addr.h>

#include <chrono>
#include <cstring>
#include <thread>

/* ── A device on this machine ───────────────────────────────────────────────────
   Everything in lh_mb_link was tested over a pair of arrays. These are the same
   questions over a real socket: a listener on 127.0.0.1 plays the device, with the
   ordinary blocking socket calls, and the link on the other end has to get its
   answers without ever waiting for one. */
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

struct device
{
    lh_os_net_socket_t listener;
    lh_u16_t port;

    device()
    {
        lh_net_ip4_t loopback;
        lh_net_ip4_socket_addr_t addr;

        lh_os_net_socket_init(&listener);
        lh_net_ip4_init(&loopback, 127, 0, 0, 1);
        lh_net_ip4_socket_addr_init(&addr, &loopback, 0);
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

    /* The connection the link opened; it is already in the backlog, so this does
       not wait. */
    void
    accept(lh_os_net_socket_t *client)
    {
        lh_net_ip4_socket_addr_t peer;

        lh_os_net_socket_init(client);
        ASSERT_TRUE(lh_os_net_socket_accept(&listener, client, &peer));
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
    lh_os_net_modbus_tcp_t tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;
    lh_os_net_socket_t client;
    lh_byte_t request[kRequestSize];
    const lh_u16_t *values;
    lh_u16_t count = 0;

    lh_os_net_modbus_tcp_init(&tcp, 1000);
    transport = lh_os_net_modbus_tcp_get_transport(&tcp);
    lh_mb_link_init(&link, &transport, lh_mb_framing_mbap, 1, tick_us, lh_null);
    ASSERT_TRUE(lh_mb_link_open(&link, "127.0.0.1", dev.port));
    dev.accept(&client);

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
    transport.close(transport.self);
}

TEST_F(os_net_modbus_tcp, a_poll_with_nothing_on_the_wire_does_not_wait)
{
    device dev;
    lh_os_net_modbus_tcp_t tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;
    lh_os_net_socket_t client;

    /* The device has the question and has not answered. A blocking recv would sit
       here until it did -- and with eight devices in a pass, one quiet device would
       hold up the other seven. */
    lh_os_net_modbus_tcp_init(&tcp, 1000);
    transport = lh_os_net_modbus_tcp_get_transport(&tcp);
    lh_mb_link_init(&link, &transport, lh_mb_framing_mbap, 1, tick_us, lh_null);
    ASSERT_TRUE(lh_mb_link_open(&link, "127.0.0.1", dev.port));
    dev.accept(&client);
    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));

    const lh_u64_t before = lh_os_tick_us();
    for (int i = 0; i < 100; ++i)
    {
        ASSERT_EQ(lh_mb_link_poll(&link, 1000000), lh_mb_link_waiting) << "nothing came, nothing broke";
    }
    EXPECT_LT(lh_os_tick_us() - before, 500000u) << "a hundred polls of an idle socket take no time";
    EXPECT_EQ(link.counters.failed, 0u);

    lh_os_net_socket_close(&client);
    transport.close(transport.self);
}

TEST_F(os_net_modbus_tcp, a_device_that_hangs_up_is_a_failure_and_a_reopen_brings_it_back)
{
    device dev;
    lh_os_net_modbus_tcp_t tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;
    lh_os_net_socket_t client;
    lh_byte_t request[kRequestSize];

    lh_os_net_modbus_tcp_init(&tcp, 1000);
    transport = lh_os_net_modbus_tcp_get_transport(&tcp);
    lh_mb_link_init(&link, &transport, lh_mb_framing_mbap, 1, tick_us, lh_null);
    ASSERT_TRUE(lh_mb_link_open(&link, "127.0.0.1", dev.port));
    dev.accept(&client);
    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));
    ASSERT_TRUE(read_request(&client, request));

    /* The device closes without answering. A recv of 0 is the peer hanging up, and
       a transport that passed it on as "nothing yet" would leave the link waiting
       out its timeout on every question from now on. */
    lh_os_net_socket_close(&client);
    EXPECT_EQ(poll_until_done(&link), lh_mb_link_failed);
    EXPECT_EQ(link.counters.failed, 1u);
    EXPECT_FALSE(lh_mb_link_ask(&link, 10, 3)) << "nothing to ask over until it is opened again";

    ASSERT_TRUE(lh_mb_link_open(&link, "127.0.0.1", dev.port)) << "the device is still there";
    dev.accept(&client);
    ASSERT_TRUE(lh_mb_link_ask(&link, 10, 3));
    ASSERT_TRUE(read_request(&client, request));
    answer(&client, request);
    EXPECT_EQ(poll_until_done(&link), lh_mb_link_answered);

    lh_os_net_socket_close(&client);
    transport.close(transport.self);
}

TEST_F(os_net_modbus_tcp, a_port_nobody_listens_on_is_not_opened_and_costs_no_more_than_the_timeout)
{
    lh_u16_t port;
    lh_os_net_modbus_tcp_t tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;

    {
        device gone; /* a port that was just in use and is not any more */
        port = gone.port;
    }
    /* Linux refuses it at once. Windows does not: it retries a connect that was
       reset and says "refused" only after about two seconds, even on loopback --
       so on Windows it is the timeout that answers, and the timeout is the bound. */
    lh_os_net_modbus_tcp_init(&tcp, 300);
    transport = lh_os_net_modbus_tcp_get_transport(&tcp);
    lh_mb_link_init(&link, &transport, lh_mb_framing_mbap, 1, tick_us, lh_null);

    const lh_u64_t before = lh_os_tick_us();
    EXPECT_FALSE(lh_mb_link_open(&link, "127.0.0.1", port));
    EXPECT_LT(lh_os_tick_us() - before, 300000u + 500000u) << "never much longer than the timeout";
    EXPECT_EQ(link.counters.failed, 1u);
}

TEST_F(os_net_modbus_tcp, an_address_that_never_answers_costs_the_timeout_and_no_more)
{
    lh_os_net_modbus_tcp_t tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;

    /* 10.255.255.1 is private and routed nowhere: a connect to it is not refused,
       it is simply never answered, which is what a switched-off device looks like.
       Left to the operating system that is tens of seconds; here it is 200 ms. A
       machine with no route at all refuses it at once, which is as good. */
    lh_os_net_modbus_tcp_init(&tcp, 200);
    transport = lh_os_net_modbus_tcp_get_transport(&tcp);
    lh_mb_link_init(&link, &transport, lh_mb_framing_mbap, 1, tick_us, lh_null);

    const lh_u64_t before = lh_os_tick_us();
    EXPECT_FALSE(lh_mb_link_open(&link, "10.255.255.1", LH_OS_NET_MODBUS_TCP_PORT));
    EXPECT_LT(lh_os_tick_us() - before, 1500000u);
}

TEST_F(os_net_modbus_tcp, a_target_that_is_not_an_address_is_not_opened)
{
    lh_os_net_modbus_tcp_t tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;

    lh_os_net_modbus_tcp_init(&tcp, 200);
    transport = lh_os_net_modbus_tcp_get_transport(&tcp);
    lh_mb_link_init(&link, &transport, lh_mb_framing_mbap, 1, tick_us, lh_null);
    EXPECT_FALSE(lh_mb_link_open(&link, "plc.local", LH_OS_NET_MODBUS_TCP_PORT)) << "no name lookup";
    EXPECT_FALSE(lh_mb_link_open(&link, "", LH_OS_NET_MODBUS_TCP_PORT));
}

TEST_F(os_net_modbus_tcp, eight_devices_answer_in_any_order_on_eight_connections)
{
    enum { k_devices = 8 };
    device dev;
    lh_os_net_modbus_tcp_t tcp[k_devices];
    lh_mb_transport_t transport[k_devices];
    lh_mb_link_t link[k_devices];
    lh_os_net_socket_t client[k_devices];
    lh_byte_t request[k_devices][kRequestSize];
    int answered = 0;

    /* Every link has a question out at once, and the devices answer last first.
       Nobody waits for anybody: the first answer to arrive is the first one read. */
    for (int i = 0; i < k_devices; ++i)
    {
        lh_os_net_modbus_tcp_init(&tcp[i], 1000);
        transport[i] = lh_os_net_modbus_tcp_get_transport(&tcp[i]);
        lh_mb_link_init(&link[i], &transport[i], lh_mb_framing_mbap, 1, tick_us, lh_null);
        ASSERT_TRUE(lh_mb_link_open(&link[i], "127.0.0.1", dev.port));
        dev.accept(&client[i]);
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
        transport[i].close(transport[i].self);
    }
}
