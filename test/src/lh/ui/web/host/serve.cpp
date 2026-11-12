/**
 * @file serve.cpp
 * @brief The web host as a server: on its own loop thread, many connections at
 *        once, a slow client that holds nobody else, a streamed body in pieces,
 *        and a stop that aborts what is unfinished.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_UI && LH_LIBRARY_OPTION_OS

#    include <lh/core/attribute/unused.h>
#    include <lh/os/net.h>
#    include <lh/os/net/socket.h>
#    include <lh/os/system/net/socket.h>
#    include <lh/os/thread.h>
#    include <lh/os/tick.h>
#    include <lh/ui/web/host.h>
#    include <lh/ui/web/host/gate.h>
#    include <lh/ui/web/host/tap.h>

#    include <atomic>
#    include <cstring>
#    include <mutex>
#    include <string>

namespace
{

lh_net_port_t
free_port()
{
    lh_net_port_t port = 0u;
    lh_net_ip4_t ip;
    lh_net_ip4_init(&ip, 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(&addr, &ip, 0);
    lh_os_net_socket_t probe;
    lh_os_net_socket_init(&probe);
    if (lh_os_net_socket_open(&probe, lh_net_socket_type_tcp) &&
        lh_os_net_socket_bind(&probe, &addr) && lh_os_net_socket_get_local_addr(&probe, &addr))
    {
        port = lh_net_ip4_socket_addr_get_port(&addr);
    }
    lh_os_net_socket_close(&probe);
    return port;
}

bool
connect_to(lh_os_net_socket_t *sock, lh_net_port_t port)
{
    lh_net_ip4_t ip;
    lh_net_ip4_init(&ip, 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(&addr, &ip, port);
    lh_os_net_socket_init(sock);
    if (!lh_os_net_socket_open(sock, lh_net_socket_type_tcp))
    {
        return false;
    }
    if (!lh_os_net_socket_connect(sock, &addr))
    {
        lh_os_net_socket_close(sock);
        return false;
    }
    return true;
}

/* Everything the peer sends until it closes, or until @p wait_ms pass. */
std::string
read_all(lh_os_net_socket_t *sock, lh_u32_t wait_ms)
{
    std::string out;
    lh_os_net_socket_t *watch[] = {sock};
    const lh_u64_t t0 = lh_os_tick_ms64();
    while (lh_os_tick_ms64() - t0 < wait_ms)
    {
        if (lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 50U) <= 0)
        {
            continue;
        }
        char buf[512];
        const lh_ssize_t got = lh_os_net_socket_recv(sock, buf, sizeof(buf));
        if (got <= 0)
        {
            break;
        }
        out.append(buf, static_cast<std::size_t>(got));
    }
    return out;
}

/* A buffered route answering "pong". */
lh_bool_t
ping_route(lh_ptr, const lh_http_request_t *request, const lh_http_header_t *,
           const lh_str_view_t *, lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(client, request, 200U, "pong", 4U));
    return lh_bool_true;
}

/* A streaming route that counts its calls and keeps what it read. */
struct staged
{
    std::mutex lock;
    std::string body;
    std::atomic<int> calls{0};
    std::atomic<int> commits{0};
    std::atomic<int> aborts{0};
};

lh_bool_t
stage(lh_ptr ctx, const lh_http_request_t *, const lh_http_header_t *, lh_io_reader_t *body)
{
    auto *s = static_cast<staged *>(ctx);
    s->calls.fetch_add(1);
    char buf[64];
    for (;;)
    {
        const lh_ssize_t got = lh_io_reader_read(body, buf, sizeof(buf));
        if (got == 0 || got == LH_IO_PENDING)
        {
            return lh_bool_true;
        }
        if (got < 0)
        {
            return lh_bool_false;
        }
        std::lock_guard<std::mutex> guard(s->lock);
        s->body.append(buf, static_cast<std::size_t>(got));
    }
}

void
commit(lh_ptr ctx, const lh_http_request_t *request, const lh_http_header_t *,
       lh_os_net_socket_t *client)
{
    static_cast<staged *>(ctx)->commits.fetch_add(1);
    LH_ATTRIBUTE_UNUSED(lh_ui_web_host_try_reply_text(client, request, 200U, "stored", 6U));
}

void
abort_fn(lh_ptr ctx, const lh_http_request_t *, const lh_http_header_t *, lh_os_net_socket_t *)
{
    static_cast<staged *>(ctx)->aborts.fetch_add(1);
}

/* One host with a /ping route and a streaming /data route, on its own thread. */
struct served
{
    lh_ui_web_route_t routes[1] = {};
    lh_ui_web_route_t match = {};
    lh_ui_web_stream_route_t stream = {};
    lh_ui_web_app_t app = {};
    staged state;
    lh_net_port_t port = 0u;
    lh_ui_web_host_t *host = nullptr;

    bool
    start()
    {
        lh_ui_web_route_set_method(&routes[0], "GET");
        lh_ui_web_route_set_method_len(&routes[0], 3U);
        lh_ui_web_route_set_path(&routes[0], "/ping");
        lh_ui_web_route_set_path_len(&routes[0], 5U);
        lh_ui_web_route_set_handler(&routes[0], ping_route);
        lh_ui_web_route_set_method(&match, "POST");
        lh_ui_web_route_set_method_len(&match, 4U);
        lh_ui_web_route_set_path(&match, "/data");
        lh_ui_web_route_set_path_len(&match, 5U);
        lh_ui_web_stream_route_init(&stream, &match, stage, commit, abort_fn, 1U << 20, 1024U,
                                    10000U);
        lh_ui_web_app_init(&app, routes, 1U);
        lh_ui_web_app_set_stream_routes(&app, &stream);
        lh_ui_web_app_set_nstream_routes(&app, 1U);
        lh_ui_web_app_set_ctx(&app, &state);
        port = free_port();
        host = lh_ui_web_host_try_create(&app, port);
        return host != nullptr && lh_ui_web_host_try_start(host);
    }

    ~served()
    {
        lh_ui_web_host_destroy(host);
    }
};

const char ping[] = "GET /ping HTTP/1.1\r\nHost: x\r\n\r\n";

} // namespace

TEST(ui_web_host_serve, answers_on_its_own_thread_and_stops)
{
    ASSERT_TRUE(lh_os_net_init());
    {
        served s;
        ASSERT_TRUE(s.start());
        lh_os_net_socket_t client;
        ASSERT_TRUE(connect_to(&client, s.port));
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, ping, sizeof(ping) - 1U));
        const std::string reply = read_all(&client, 3000U);
        lh_os_net_socket_close(&client);
        EXPECT_EQ(reply.rfind("HTTP/1.1 200", 0), 0U) << reply;
        EXPECT_NE(reply.find("pong"), std::string::npos);
        EXPECT_TRUE(lh_ui_web_host_try_stop(s.host, 5000U));
    }
    lh_os_net_deinit();
}

/* One client sends half a head and stalls; another is answered meanwhile. */
TEST(ui_web_host_serve, a_slow_client_holds_nobody_else)
{
    ASSERT_TRUE(lh_os_net_init());
    {
        served s;
        ASSERT_TRUE(s.start());
        lh_os_net_socket_t slow;
        ASSERT_TRUE(connect_to(&slow, s.port));
        ASSERT_TRUE(lh_os_net_socket_send_all(&slow, "GET /pi", 7U));
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(50U));
        lh_os_net_socket_t fast;
        ASSERT_TRUE(connect_to(&fast, s.port));
        const lh_u64_t t0 = lh_os_tick_ms64();
        ASSERT_TRUE(lh_os_net_socket_send_all(&fast, ping, sizeof(ping) - 1U));
        const std::string reply = read_all(&fast, 1500U);
        const lh_u64_t took = lh_os_tick_ms64() - t0;
        lh_os_net_socket_close(&fast);
        EXPECT_NE(reply.find("pong"), std::string::npos) << reply;
        /* The old host read one request at a time: it would wait out the slow one. */
        EXPECT_LT(took, 1500U);
        /* The slow one finishes its request afterwards and is answered too. */
        const char rest[] = "ng HTTP/1.1\r\nHost: x\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&slow, rest, sizeof(rest) - 1U));
        EXPECT_NE(read_all(&slow, 1500U).find("pong"), std::string::npos);
        lh_os_net_socket_close(&slow);
    }
    lh_os_net_deinit();
}

/* A streamed body sent in pieces reaches the handler over several calls,
   and the route commits once, after the end. */
TEST(ui_web_host_serve, a_streamed_body_arrives_in_pieces)
{
    ASSERT_TRUE(lh_os_net_init());
    {
        served s;
        ASSERT_TRUE(s.start());
        lh_os_net_socket_t client;
        ASSERT_TRUE(connect_to(&client, s.port));
        const char head[] = "POST /data HTTP/1.1\r\nHost: x\r\nTransfer-Encoding: chunked\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
        std::string expected;
        for (int i = 0; i < 5; ++i)
        {
            const std::string piece(100U, static_cast<char>('a' + i));
            expected += piece;
            const std::string chunk = "64\r\n" + piece + "\r\n";
            ASSERT_TRUE(lh_os_net_socket_send_all(&client, chunk.data(), chunk.size()));
            LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(40U));
        }
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, "0\r\n\r\n", 5U));
        const std::string reply = read_all(&client, 3000U);
        lh_os_net_socket_close(&client);
        EXPECT_NE(reply.find("stored"), std::string::npos) << reply;
        std::lock_guard<std::mutex> guard(s.state.lock);
        EXPECT_EQ(s.state.body, expected);
        EXPECT_GT(s.state.calls.load(), 1);
        EXPECT_EQ(s.state.commits.load(), 1);
        EXPECT_EQ(s.state.aborts.load(), 0);
    }
    lh_os_net_deinit();
}

/* Destroying the host while a body is still arriving aborts that route. */
TEST(ui_web_host_serve, destroy_aborts_an_unfinished_stream)
{
    ASSERT_TRUE(lh_os_net_init());
    staged *state = nullptr;
    int aborts = 0;
    int commits = 0;
    {
        served s;
        state = &s.state;
        ASSERT_TRUE(s.start());
        lh_os_net_socket_t client;
        ASSERT_TRUE(connect_to(&client, s.port));
        const char head[] = "POST /data HTTP/1.1\r\nHost: x\r\nContent-Length: 1000\r\n\r\n12345";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
        for (int i = 0; i < 200 && state->calls.load() == 0; ++i)
        {
            LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(5U));
        }
        lh_ui_web_host_destroy(s.host);
        s.host = nullptr;
        aborts = state->aborts.load();
        commits = state->commits.load();
        lh_os_net_socket_close(&client);
    }
    EXPECT_EQ(aborts, 1);
    EXPECT_EQ(commits, 0);
    lh_os_net_deinit();
}

TEST(ui_web_host_serve, refuses_a_null_application)
{
    EXPECT_EQ(lh_ui_web_host_try_create(nullptr, 1U), nullptr);
}

/* The gate lets one receive through per readiness. */
TEST(ui_web_host_gate, receives_once_per_readiness)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t a;
    lh_os_net_socket_init(&a);
    lh_os_net_socket_t b;
    lh_os_net_socket_init(&b);
    lh_os_net_socket_set_handle(&a, handles[0]);
    lh_os_net_socket_set_handle(&b, handles[1]);
    ASSERT_TRUE(lh_os_net_socket_send_all(&b, "abcd", 4U));
    lh_ui_web_host_gate_t gate;
    lh_ui_web_host_gate_init(&gate, &a);
    lh_io_reader_t reader;
    lh_ui_web_host_gate_get_reader(&gate, &reader);
    char buf[8];
    EXPECT_EQ(lh_io_reader_read(&reader, buf, sizeof(buf)), LH_IO_PENDING);
    /* As the host does: the gate opens only once a wait found the socket readable. */
    lh_os_net_socket_t *watch[] = {&a};
    ASSERT_GT(lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 2000U), 0);
    lh_ui_web_host_gate_set_ready(&gate, lh_bool_true);
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 2U), 2);
    EXPECT_FALSE(lh_ui_web_host_gate_is_ready(&gate));
    EXPECT_EQ(lh_io_reader_read(&reader, buf, sizeof(buf)), LH_IO_PENDING);
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 0U), 0);
    EXPECT_EQ(lh_ui_web_host_gate_get_socket(&gate), &a);
    lh_os_net_socket_close(&a);
    lh_os_net_socket_close(&b);
    lh_os_net_deinit();
}

/* The tap remembers the last result, so the host knows why a handler stopped. */
namespace
{

struct script
{
    lh_ssize_t results[4];
    int at = 0;
};

lh_ssize_t
scripted_read(lh_self_ptr context, lh_ptr, lh_usize_t)
{
    auto *s = static_cast<script *>(context);
    return s->results[s->at++];
}

} // namespace

TEST(ui_web_host_tap, remembers_the_last_result)
{
    script s{{3, LH_IO_PENDING, 0, LH_IO_ERROR}};
    lh_io_reader_t source;
    lh_io_reader_init(&source, scripted_read, &s);
    lh_ui_web_host_tap_t tap;
    lh_ui_web_host_tap_init(&tap, &source);
    EXPECT_EQ(lh_ui_web_host_tap_get_last(&tap), LH_UI_WEB_HOST_TAP_NOTHING_YET);
    EXPECT_FALSE(lh_ui_web_host_tap_is_at_end(&tap));
    lh_io_reader_t reader;
    lh_ui_web_host_tap_get_reader(&tap, &reader);
    char buf[4];
    EXPECT_EQ(lh_io_reader_read(&reader, buf, sizeof(buf)), 3);
    EXPECT_FALSE(lh_ui_web_host_tap_is_pending(&tap));
    EXPECT_EQ(lh_io_reader_read(&reader, buf, sizeof(buf)), LH_IO_PENDING);
    EXPECT_TRUE(lh_ui_web_host_tap_is_pending(&tap));
    EXPECT_FALSE(lh_ui_web_host_tap_is_failed(&tap));
    EXPECT_EQ(lh_io_reader_read(&reader, buf, 0U), 0); /* passes nothing, remembers nothing */
    EXPECT_TRUE(lh_ui_web_host_tap_is_pending(&tap));
    EXPECT_EQ(lh_io_reader_read(&reader, buf, sizeof(buf)), 0);
    EXPECT_TRUE(lh_ui_web_host_tap_is_at_end(&tap));
    EXPECT_EQ(lh_io_reader_read(&reader, buf, sizeof(buf)), LH_IO_ERROR);
    EXPECT_TRUE(lh_ui_web_host_tap_is_failed(&tap));
}

#endif
