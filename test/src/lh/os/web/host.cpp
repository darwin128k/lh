/**
 * @file host.cpp
 * @brief The web host on its own: reading a request, the replies, the heads,
 *        the gzip bodies, the streamed routes and the websocket frame.
 *
 * No application draws here. A request goes in over a pair of loopback
 * sockets and the reply is read back; the frame test hands the host a slot
 * that only writes down what it was asked. The host serving the gl player is
 * tested in `test/src/lh/gl/web/player/host.cpp`.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/core/attribute/unused.h>
#    include <lh/expect/death.h>
#    include <lh/core/http/method.h>
#    include <lh/core/http/status.h>
#    include <lh/core/str/lit.h>
#    include <lh/os/net.h>
#    include <lh/os/net/socket.h>
#    include <lh/os/system/net/socket.h>
#    include <lh/os/tick.h>
#    include <lh/os/web/host.h>
#    include <lh/os/web/host/gzip/workspace.h>
#    include <lh/os/web/host/deadline.h>
#    include <lh/core/web/ws.h>
#    include <lh/test/os/net/read_head.h>

#    include <chrono>
#    include <cstring>
#    include <string>
#    include <thread>

using lh_test::read_head;

TEST(ui_web_host_try_read_request, leaves_length_unchanged_when_storage_is_empty)
{
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    lh_usize_t length = 17;
    lh_str_view_t request_body = lh_str_view_initializer_lit("unchanged");

    char buffer;
    lh_http_request_t request;
    EXPECT_FALSE(lh_os_web_host_try_read_request(lh_addr_of(socket), lh_addr_of(buffer), 0, 10,
                                                 lh_addr_of(length), lh_addr_of(request_body),
                                                 lh_addr_of(request), nullptr, 0));
    EXPECT_EQ(length, 17u);
    EXPECT_EQ(lh_str_view_get_size(lh_addr_of(request_body)), 9u);
    LH_EXPECT_CHECK_DEATH(lh_os_web_host_read_request(lh_addr_of(socket), lh_addr_of(buffer), 0, 10,
                                                      lh_addr_of(length), lh_addr_of(request_body),
                                                      lh_addr_of(request), nullptr, 0));
}

TEST(ui_web_host_handle_request_is_upgraded, rejects_a_socket_that_cannot_be_configured)
{
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0);

    EXPECT_FALSE(lh_os_web_host_handle_request_is_upgraded(&app, &socket));
}

TEST(ui_web_host_try_read_request, returns_parsed_request_and_borrowed_headers)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(lh_addr_of(reader));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(lh_addr_of(writer));
    lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
    lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
    const char text[] = "GET /ui HTTP/1.1\r\nX-Example: value\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(lh_addr_of(writer), text, sizeof(text) - 1));
    lh_usize_t length = 0;
    lh_http_request_t request;
    lh_str_view_t request_body;
    lh_http_header_t headers[2];
    char buffer[128];
    ASSERT_TRUE(lh_os_web_host_try_read_request(lh_addr_of(reader), buffer, sizeof(buffer), 1000,
                                                lh_addr_of(length), lh_addr_of(request_body),
                                                lh_addr_of(request), headers, 2));
    const lh_str_view_t get = lh_str_view_initializer_lit("GET");
    const lh_str_view_t target = lh_str_view_initializer_lit("/ui");
    const lh_str_view_t name = lh_str_view_initializer_lit("x-example");
    const lh_str_view_t value = lh_str_view_initializer_lit("value");
    EXPECT_EQ(length, sizeof(text) - 1);
    EXPECT_EQ(lh_str_view_get_size(lh_addr_of(request_body)), 0u);
    EXPECT_TRUE(lh_str_view_equals(lh_http_request_get_method(lh_addr_of(request)), lh_addr_of(get),
                                   lh_bool_false));
    EXPECT_TRUE(lh_str_view_equals(lh_http_request_get_target(lh_addr_of(request)),
                                   lh_addr_of(target), lh_bool_false));
    ASSERT_EQ(lh_http_request_get_header_count(lh_addr_of(request)), 1u);
    const lh_http_header_t *header = lh_http_header_find(headers, 1, lh_addr_of(name));
    ASSERT_NE(header, nullptr);
    EXPECT_TRUE(
        lh_str_view_equals(lh_http_header_get_value(header), lh_addr_of(value), lh_bool_false));
    lh_os_net_socket_close(lh_addr_of(reader));
    lh_os_net_socket_close(lh_addr_of(writer));
}

TEST(ui_web_host_try_read_request, waits_for_the_complete_body)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(lh_addr_of(reader));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(lh_addr_of(writer));
    lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
    lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
    const char first[] = "POST / HTTP/1.1\r\nContent-Length: 4\r\n\r\nab";
    ASSERT_TRUE(lh_os_net_socket_send_all(lh_addr_of(writer), first, sizeof(first) - 1));
    std::thread sender(
        [&writer]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            LH_ATTRIBUTE_UNUSED(lh_os_net_socket_send_all(lh_addr_of(writer), "cd", 2));
        });
    lh_usize_t length = 0;
    lh_http_request_t request;
    lh_str_view_t request_body;
    lh_http_header_t headers[2];
    char buffer[128];
    const bool received = lh_os_web_host_try_read_request(
        lh_addr_of(reader), buffer, sizeof(buffer), 1000, lh_addr_of(length),
        lh_addr_of(request_body), lh_addr_of(request), headers, 2);
    sender.join();
    ASSERT_TRUE(received);
    EXPECT_EQ(std::string(buffer, length), std::string(first, sizeof(first) - 1) + "cd");
    EXPECT_EQ(std::string(lh_str_view_get_data(lh_addr_of(request_body)),
                          lh_str_view_get_size(lh_addr_of(request_body))),
              "abcd");
    lh_os_net_socket_close(lh_addr_of(reader));
    lh_os_net_socket_close(lh_addr_of(writer));
}

TEST(ui_web_host_try_read_request, decodes_fragmented_chunked_binary_body)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(&reader);
    lh_os_net_socket_set_handle(&reader, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&reader, lh_bool_true));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(&writer);
    lh_os_net_socket_set_handle(&writer, handles[1]);
    const char first[] =
        "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\nX-Name: kept\r\n\r\n2\r\na";
    ASSERT_TRUE(lh_os_net_socket_send_all(&writer, first, sizeof(first) - 1U));
    std::thread sender(
        [&writer]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            const char rest[] = "\0\r\n2;foo=bar\r\nbc\r\n0\r\nX-Trailer: discarded\r\n\r\n";
            LH_ATTRIBUTE_UNUSED(lh_os_net_socket_send_all(&writer, rest, sizeof(rest) - 1U));
        });
    lh_usize_t length = 0U;
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[4];
    char buffer[256];
    const bool ok = lh_os_web_host_try_read_request(&reader, buffer, sizeof(buffer), 1000U, &length,
                                                    &body, &request, headers, 4U);
    sender.join();
    ASSERT_TRUE(ok);
    EXPECT_EQ(std::string(lh_str_view_get_data(&body), lh_str_view_get_size(&body)),
              std::string("a\0bc", 4U));
    const lh_str_view_t name = lh_str_view_lit("X-Name");
    const lh_http_header_t *field = lh_http_header_find(headers, 2U, &name);
    ASSERT_NE(field, nullptr);
    const lh_str_view_t expected = lh_str_view_lit("kept");
    EXPECT_TRUE(lh_str_view_equals(lh_http_header_get_value(field), &expected, lh_bool_false));
    EXPECT_GT(length, lh_str_view_get_size(&body));
    lh_os_net_socket_close(&reader);
    lh_os_net_socket_close(&writer);
}

TEST(ui_web_host_try_read_request, rejects_ambiguous_chunked_framing)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(&reader);
    lh_os_net_socket_set_handle(&reader, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&reader, lh_bool_true));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(&writer);
    lh_os_net_socket_set_handle(&writer, handles[1]);
    const char input[] =
        "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Length: 0\r\n\r\n0\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&writer, input, sizeof(input) - 1U));
    lh_usize_t length = 17U;
    lh_str_view_t body = lh_str_view_lit("unchanged");
    lh_http_request_t request;
    lh_http_header_t headers[4];
    char buffer[128];
    EXPECT_FALSE(lh_os_web_host_try_read_request(&reader, buffer, sizeof(buffer), 1000U, &length,
                                                 &body, &request, headers, 4U));
    EXPECT_EQ(length, 17U);
    EXPECT_EQ(lh_str_view_get_size(&body), 9U);
    lh_os_net_socket_close(&reader);
    lh_os_net_socket_close(&writer);
}

TEST(ui_web_host_try_read_request, rejects_chunked_http_1_0)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(&reader);
    lh_os_net_socket_set_handle(&reader, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&reader, lh_bool_true));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(&writer);
    lh_os_net_socket_set_handle(&writer, handles[1]);
    const char input[] = "POST / HTTP/1.0\r\nTransfer-Encoding: chunked\r\n\r\n0\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&writer, input, sizeof(input) - 1U));
    lh_usize_t length = 17U;
    lh_str_view_t body = lh_str_view_lit("unchanged");
    lh_http_request_t request;
    lh_http_header_t headers[2];
    char buffer[128];
    EXPECT_FALSE(lh_os_web_host_try_read_request(&reader, buffer, sizeof(buffer), 1000U, &length,
                                                 &body, &request, headers, 2U));
    EXPECT_EQ(length, 17U);
    EXPECT_EQ(lh_str_view_get_size(&body), 9U);
    lh_os_net_socket_close(&reader);
    lh_os_net_socket_close(&writer);
}

TEST(ui_web_host_try_read_request, rejects_oversized_and_incomplete_bodies)
{
    ASSERT_TRUE(lh_os_net_init());
    const char *requests[] = {"POST / HTTP/1.1\r\nContent-Length: 1000\r\n\r\n",
                              "POST / HTTP/1.1\r\nContent-Length: 4\r\n\r\nab"};
    for (const char *text : requests)
    {
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t reader;
        lh_os_net_socket_t writer;
        lh_os_net_socket_init(lh_addr_of(reader));
        lh_os_net_socket_init(lh_addr_of(writer));
        lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
        lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
        ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
        ASSERT_TRUE(lh_os_net_socket_send_all(lh_addr_of(writer), text, std::strlen(text)));
        char buffer[128];
        lh_usize_t length = 17;
        lh_str_view_t body = lh_str_view_initializer_lit("unchanged");
        lh_http_request_t request;
        lh_http_header_t headers[2];
        EXPECT_FALSE(lh_os_web_host_try_read_request(lh_addr_of(reader), buffer, sizeof(buffer), 40,
                                                     lh_addr_of(length), lh_addr_of(body),
                                                     lh_addr_of(request), headers, 2));
        EXPECT_EQ(length, 17u);
        EXPECT_EQ(lh_str_view_get_size(lh_addr_of(body)), 9u);
        lh_os_net_socket_close(lh_addr_of(reader));
        lh_os_net_socket_close(lh_addr_of(writer));
    }
}

lh_bool_t
ui_web_host_test_capture_body(lh_ptr ctx, const lh_http_request_t *request,
                              const lh_http_header_t *headers, const lh_str_view_t *body,
                              lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(request);
    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(client);
    std::string *captured = static_cast<std::string *>(ctx);
    captured->assign(lh_str_view_get_data(body), lh_str_view_get_size(body));
    return lh_bool_true;
}

TEST(ui_web_host_handle_request_is_upgraded, dispatches_complete_binary_body)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(lh_addr_of(server));
    lh_os_net_socket_t client;
    lh_os_net_socket_init(lh_addr_of(client));
    lh_os_net_socket_set_handle(lh_addr_of(server), handles[0]);
    lh_os_net_socket_set_handle(lh_addr_of(client), handles[1]);
    lh_os_web_app_route_t route;
    lh_os_web_app_route_set_method(lh_addr_of(route), "POST");
    lh_os_web_app_route_set_method_len(lh_addr_of(route), 4);
    lh_os_web_app_route_set_path(lh_addr_of(route), "/data");
    lh_os_web_app_route_set_path_len(lh_addr_of(route), 5);
    lh_os_web_app_route_set_handler(lh_addr_of(route), ui_web_host_test_capture_body);
    lh_os_web_app_t app;
    lh_os_web_app_init(lh_addr_of(app), lh_addr_of(route), 1);
    std::string captured;
    lh_os_web_app_set_ctx(lh_addr_of(app), lh_addr_of(captured));
    const char head[] = "POST /data HTTP/1.1\r\nContent-Length: 3\r\n\r\na";
    ASSERT_TRUE(lh_os_net_socket_send_all(lh_addr_of(client), head, sizeof(head) - 1));
    std::thread host(
        [&app, &server]
        {
            LH_ATTRIBUTE_UNUSED(
                lh_os_web_host_handle_request_is_upgraded(lh_addr_of(app), lh_addr_of(server)));
        });
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    const char tail[] = {0, 'b'};
    const bool sent = lh_os_net_socket_send_all(lh_addr_of(client), tail, sizeof(tail));
    host.join();
    EXPECT_TRUE(sent);
    EXPECT_EQ(captured, std::string("a\0b", 3));
    lh_os_net_socket_close(lh_addr_of(server));
    lh_os_net_socket_close(lh_addr_of(client));
}

TEST(ui_web_host_handle_request_is_upgraded, dispatches_decoded_chunked_body)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_set_handle(&server, handles[0]);
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&client, handles[1]);
    lh_os_web_app_route_t route;
    lh_os_web_app_route_set_method(&route, "POST");
    lh_os_web_app_route_set_method_len(&route, 4U);
    lh_os_web_app_route_set_path(&route, "/data");
    lh_os_web_app_route_set_path_len(&route, 5U);
    lh_os_web_app_route_set_handler(&route, ui_web_host_test_capture_body);
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, &route, 1U);
    std::string captured;
    lh_os_web_app_set_ctx(&app, &captured);
    const char head[] = "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n";
    const char body[] = "2\r\na\0\r\n2\r\nbc\r\n0\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, body, sizeof(body) - 1U));
    EXPECT_FALSE(lh_os_web_host_handle_request_is_upgraded(&app, &server));
    EXPECT_EQ(captured, std::string("a\0bc", 4U));
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

lh_bool_t
ui_web_host_test_stream_body(lh_ptr ctx, const lh_http_request_t *request,
                             const lh_http_header_t *headers, lh_io_reader_t *body)
{
    LH_ATTRIBUTE_UNUSED(request);
    LH_ATTRIBUTE_UNUSED(headers);
    std::string *captured = static_cast<std::string *>(ctx);
    char buffer[113];
    for (;;)
    {
        const lh_ssize_t size = lh_io_reader_read(body, buffer, sizeof(buffer));
        /* Nothing more yet: the host calls again when the body has more. */
        if (size == 0 || size == LH_IO_PENDING)
        {
            return lh_bool_true;
        }
        if (size < 0)
        {
            return lh_bool_false;
        }
        captured->append(buffer, static_cast<std::size_t>(size));
    }
}

void
ui_web_host_test_stream_commit(lh_ptr, const lh_http_request_t *, const lh_http_header_t *,
                               lh_os_net_socket_t *)
{
}

void
ui_web_host_test_stream_abort(lh_ptr ctx, const lh_http_request_t *, const lh_http_header_t *,
                              lh_os_net_socket_t *)
{
    static_cast<std::string *>(ctx)->clear();
}

TEST(ui_web_host_serve_stream, checks_initial_scratch_capacity)
{
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0U);
    lh_os_web_app_stream_route_t route = {};
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    lh_http_request_t request = {};
    const std::string bytes(LH_OS_WEB_HOST_REQUEST_CAPACITY + 1U, 'x');
    const lh_str_view_t initial = lh_str_view_initializer(bytes.data(), bytes.size());
    EXPECT_FALSE(
        lh_os_web_host_try_serve_stream(&app, &route, &socket, &request, nullptr, initial, 0U));
    LH_EXPECT_CHECK_DEATH(
        lh_os_web_host_serve_stream(&app, &route, &socket, &request, nullptr, initial, 0U));
}

TEST(ui_web_host_deadline, reads_available_data_and_obeys_expired_deadline)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&server, handles[0]);
    lh_os_net_socket_set_handle(&client, handles[1]);
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, "a", 1U));
    lh_os_web_host_deadline_t deadline;
    lh_os_web_host_deadline_init(&deadline, &server, lh_os_tick_ms(), 1000U);
    lh_io_reader_t reader;
    lh_os_web_host_deadline_get_reader(&deadline, &reader);
    char out;
    EXPECT_EQ(lh_io_reader_read(&reader, &out, 1U), 1);
    EXPECT_EQ(out, 'a');
    lh_os_web_host_deadline_init(&deadline, &server, lh_os_tick_ms(), 0U);
    EXPECT_EQ(lh_io_reader_read(&reader, &out, 1U), LH_IO_ERROR);
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

TEST(ui_web_host_handle_request_is_upgraded, streams_large_bodies_with_continue)
{
    for (bool chunked : {false, true})
    {
        ASSERT_TRUE(lh_os_net_init());
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t server;
        lh_os_net_socket_t client;
        lh_os_net_socket_init(&server);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_set_handle(&server, handles[0]);
        lh_os_net_socket_set_handle(&client, handles[1]);
        lh_os_web_app_route_t match = {};
        lh_os_web_app_route_set_method(&match, "POST");
        lh_os_web_app_route_set_method_len(&match, 4U);
        lh_os_web_app_route_set_path(&match, "/data");
        lh_os_web_app_route_set_path_len(&match, 5U);
        lh_os_web_app_stream_route_t route;
        lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                        ui_web_host_test_stream_commit,
                                        ui_web_host_test_stream_abort, 20000U, 1024U, 5000U);
        lh_os_web_app_t app;
        lh_os_web_app_init(&app, nullptr, 0U);
        lh_os_web_app_set_stream_routes(&app, &route);
        lh_os_web_app_set_nstream_routes(&app, 1U);
        std::string captured;
        lh_os_web_app_set_ctx(&app, &captured);
        const std::string head =
            chunked
                ? "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\nExpect: "
                  "100-continue\r\n\r\n"
                : "POST /data HTTP/1.1\r\nContent-Length: 16384\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
        std::thread worker(
            [&] { LH_ATTRIBUTE_UNUSED(lh_os_web_host_handle_request_is_upgraded(&app, &server)); });
        lh_os_net_socket_t *watch[] = {&client};
        const bool ready = lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 1000U) > 0;
        const std::string interim = ready ? read_head(&client) : "";
        std::string expected(16384U, 'x');
        expected[42] = '\0';
        std::string wire =
            chunked ? std::string("4000\r\n") + expected + "\r\n0\r\nX-End: yes\r\n\r\n" : expected;
        const bool sent = lh_os_net_socket_send_all(&client, wire.data(), wire.size());
        worker.join();
        EXPECT_TRUE(ready);
        EXPECT_EQ(interim, "HTTP/1.1 100 Continue\r\n\r\n");
        EXPECT_TRUE(sent);
        EXPECT_EQ(captured, expected);
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
    }
}

lh_bool_t
ui_web_host_test_stream_stage_one(lh_ptr ctx, const lh_http_request_t *, const lh_http_header_t *,
                                  lh_io_reader_t *body)
{
    char byte;
    const lh_ssize_t got = lh_io_reader_read(body, &byte, 1U);
    if (got == LH_IO_PENDING)
    {
        return lh_bool_true; /* called again when the first byte is there */
    }
    if (got != 1)
    {
        return lh_bool_false;
    }
    static_cast<std::string *>(ctx)->append(1U, byte);
    return lh_bool_true;
}

void
ui_web_host_test_stream_mark_commit(lh_ptr ctx, const lh_http_request_t *, const lh_http_header_t *,
                                    lh_os_net_socket_t *)
{
    static_cast<std::string *>(ctx)->append(" COMMIT");
}

void
ui_web_host_test_stream_mark_abort(lh_ptr ctx, const lh_http_request_t *, const lh_http_header_t *,
                                   lh_os_net_socket_t *)
{
    *static_cast<std::string *>(ctx) = "ABORT";
}

TEST(ui_web_host_handle_request_is_upgraded,
     finishes_staged_data_only_after_remaining_chunked_body_is_validated)
{
    for (int scenario : {0, 1, 2})
    {
        const bool oversize = scenario == 1;
        ASSERT_TRUE(lh_os_net_init());
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t server;
        lh_os_net_socket_t client;
        lh_os_net_socket_init(&server);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_set_handle(&server, handles[0]);
        lh_os_net_socket_set_handle(&client, handles[1]);
        lh_os_web_app_route_t match = {};
        lh_os_web_app_route_set_method(&match, "POST");
        lh_os_web_app_route_set_method_len(&match, 4U);
        lh_os_web_app_route_set_path(&match, "/data");
        lh_os_web_app_route_set_path_len(&match, 5U);
        lh_os_web_app_stream_route_t route;
        lh_os_web_app_stream_route_init(
            &route, &match, ui_web_host_test_stream_stage_one, ui_web_host_test_stream_mark_commit,
            ui_web_host_test_stream_mark_abort, oversize ? 2U : 100U, 1024U, 5000U);
        lh_os_web_app_t app;
        lh_os_web_app_init(&app, nullptr, 0U);
        lh_os_web_app_set_stream_routes(&app, &route);
        lh_os_web_app_set_nstream_routes(&app, 1U);
        std::string staged;
        lh_os_web_app_set_ctx(&app, &staged);
        const char head[] =
            "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
        std::thread worker(
            [&] { LH_ATTRIBUTE_UNUSED(lh_os_web_host_handle_request_is_upgraded(&app, &server)); });
        lh_os_net_socket_t *watch[] = {&client};
        const bool ready = lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 1000U) > 0;
        const std::string interim = ready ? read_head(&client) : "";
        const std::string wire =
            oversize ? "1\r\na\r\n3\r\nbcd\r\n0\r\n\r\n"
                     : (scenario == 2 ? "1\r\na\r\n1\r\nb\r\n0\r\n\r\n" : "1\r\na\r\nZZ\r\n");
        const bool sent = lh_os_net_socket_send_all(&client, wire.data(), wire.size());
        worker.join();
        EXPECT_TRUE(ready);
        EXPECT_EQ(interim, "HTTP/1.1 100 Continue\r\n\r\n");
        EXPECT_TRUE(sent);
        EXPECT_EQ(staged, scenario == 2 ? "a COMMIT" : "ABORT");
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
    }
}

const lh_byte_t ui_web_host_gzip_sample[] = {
    31,  139, 8,   0,  0,   0,  0,   0,  2,   255, 237, 193, 49, 1,  0, 0, 0, 194,
    160, 108, 235, 95, 202, 24, 62,  64, 1,   0,   0,   0,   0,  0,  0, 0, 0, 0,
    0,   0,   0,   0,  0,   0,  111, 3,  172, 195, 74,  150, 0,  64, 0, 0};

TEST(ui_web_host_handle_request_is_upgraded, streams_gzip_and_aborts_on_corruption_or_limits)
{
    for (int scenario = 0; scenario < 11; ++scenario)
    {
        ASSERT_TRUE(lh_os_net_init());
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t server;
        lh_os_net_socket_t client;
        lh_os_net_socket_init(&server);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_set_handle(&server, handles[0]);
        lh_os_net_socket_set_handle(&client, handles[1]);
        lh_os_web_app_route_t match = {};
        lh_os_web_app_route_set_method(&match, "POST");
        lh_os_web_app_route_set_method_len(&match, 4U);
        lh_os_web_app_route_set_path(&match, "/data");
        lh_os_web_app_route_set_path_len(&match, 5U);
        lh_os_web_app_stream_route_t route;
        lh_os_web_app_stream_route_init(
            &route, &match,
            scenario >= 6 ? ui_web_host_test_stream_stage_one : ui_web_host_test_stream_body,
            ui_web_host_test_stream_mark_commit, ui_web_host_test_stream_mark_abort,
            scenario == 3 || scenario == 10 ? 100U : 16384U, scenario == 9 ? 17U : 1024U, 5000U);
        lh_os_web_app_stream_route_set_wire_limit(
            &route, scenario == 4 || scenario == 8 ? sizeof(ui_web_host_gzip_sample) - 1U : 1000U);
        lh_os_web_app_t app;
        lh_os_web_app_init(&app, nullptr, 0U);
        lh_os_web_app_set_stream_routes(&app, &route);
        lh_os_web_app_set_nstream_routes(&app, 1U);
        std::string staged;
        lh_os_web_app_set_ctx(&app, &staged);
        const bool chunked = scenario == 1 || scenario == 5 || scenario == 8;
        const std::string framing =
            chunked ? (scenario == 5 ? "Transfer-Encoding: gzip, chunked\r\n"
                                     : "Transfer-Encoding: chunked\r\n")
                    : "Content-Length: " + std::to_string(sizeof(ui_web_host_gzip_sample)) + "\r\n";
        const std::string head = "POST /data HTTP/1.1\r\n" + framing +
                                 (scenario == 5 ? "" : "Content-Encoding: gzip\r\n") +
                                 "Expect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
        std::thread worker(
            [&] { LH_ATTRIBUTE_UNUSED(lh_os_web_host_handle_request_is_upgraded(&app, &server)); });
        lh_os_net_socket_t *watch[] = {&client};
        const bool ready = lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 1000U) > 0;
        const std::string response = ready ? read_head(&client) : "";
        if (scenario == 4)
        {
            worker.join();
            EXPECT_NE(response.find("HTTP/1.1 413 Content Too Large\r\n"), std::string::npos);
            EXPECT_TRUE(staged.empty());
        }
        else
        {
            std::string body(reinterpret_cast<const char *>(ui_web_host_gzip_sample),
                             sizeof(ui_web_host_gzip_sample));
            if (scenario == 2 || scenario == 6)
            {
                body[body.size() - 8U] ^= 1;
            }
            if (chunked)
            {
                body = "34\r\n" + body + "\r\n0\r\n\r\n";
            }
            const bool sent = lh_os_net_socket_send_all(&client, body.data(), body.size());
            worker.join();
            EXPECT_TRUE(sent);
            EXPECT_EQ(response, "HTTP/1.1 100 Continue\r\n\r\n");
            if (scenario == 2 || scenario == 3 || scenario == 6 || scenario == 8 || scenario == 9 ||
                scenario == 10)
            {
                EXPECT_EQ(staged, "ABORT");
            }
            else if (scenario == 7)
            {
                EXPECT_EQ(staged, "A COMMIT");
            }
            else
            {
                EXPECT_EQ(staged, std::string(16384U, 'A') + " COMMIT");
            }
        }
        EXPECT_TRUE(ready);
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
    }
}

const lh_byte_t ui_web_host_gzip_binary[] = {
    31,  139, 8,   0,   0,   0,   0,   0,   2,   255, 1,   0,   8,   255, 247, 165, 163, 196, 152,
    136, 77,  29,  41,  167, 17,  248, 248, 160, 21,  198, 105, 146, 157, 201, 148, 191, 62,  12,
    33,  214, 81,  104, 249, 132, 123, 250, 172, 71,  89,  172, 7,   172, 154, 98,  14,  238, 210,
    41,  13,  245, 20,  190, 25,  93,  192, 165, 0,   205, 239, 4,   8,   14,  202, 95,  236, 184,
    121, 152, 135, 23,  185, 252, 101, 19,  115, 211, 99,  36,  130, 130, 92,  196, 9,   10,  61,
    204, 45,  92,  195, 23,  121, 64,  122, 14,  239, 45,  112, 54,  221, 102, 186, 173, 24,  242,
    8,   165, 7,   130, 21,  96,  71,  43,  62,  8,   87,  121, 27,  255, 183, 170, 165, 170, 89,
    73,  181, 234, 225, 172, 99,  171, 225, 213, 30,  65,  119, 78,  191, 120, 20,  19,  194, 54,
    249, 152, 174, 40,  192, 245, 35,  118, 127, 23,  93,  28,  174, 171, 195, 190, 110, 13,  69,
    216, 3,   231, 86,  250, 43,  88,  98,  124, 187, 241, 49,  41,  196, 76,  62,  186, 147, 76,
    64,  212, 17,  88,  182, 190, 115, 76,  85,  45,  111, 99,  107, 19,  133, 232, 142, 241, 173,
    232, 172, 101, 249, 217, 16,  156, 210, 247, 156, 38,  195, 167, 14,  228, 89,  187, 70,  98,
    73,  120, 2,   253, 172, 160, 46,  190, 229, 54,  173, 220, 14,  66,  22,  206, 109, 7,   103,
    97,  123, 234, 183, 246, 102, 183, 33,  82,  122, 111, 186, 100, 25,  232, 224, 19,  63,  193,
    43,  20,  100, 211, 233, 228, 176, 115, 232, 170, 86,  139, 112, 1,   143, 79,  249, 96,  22,
    101, 249, 104, 186, 236, 84,  121, 153, 161, 229, 196, 113, 80,  115, 223, 60,  236, 222, 158,
    231, 169, 49,  35,  70,  129, 127, 43,  151, 100, 84,  65,  173, 37,  182, 144, 51,  136, 4,
    137, 50,  54,  77,  122, 171, 104, 171, 157, 33,  197, 157, 31,  219, 177, 230, 254, 120, 146,
    201, 146, 36,  110, 214, 133, 132, 83,  119, 95,  215, 166, 23,  2,   199, 119, 18,  162, 54,
    212, 190, 196, 12,  189, 155, 46,  87,  202, 52,  147, 205, 73,  152, 231, 90,  226, 33,  221,
    11,  151, 183, 207, 77,  120, 16,  133, 237, 144, 23,  214, 154, 164, 251, 169, 87,  61,  66,
    80,  197, 113, 191, 52,  129, 79,  155, 129, 200, 11,  6,   170, 66,  226, 137, 4,   216, 30,
    119, 164, 54,  17,  48,  215, 180, 135, 78,  161, 162, 70,  243, 159, 50,  238, 127, 96,  115,
    100, 241, 228, 52,  227, 96,  241, 61,  186, 208, 164, 229, 211, 220, 38,  135, 242, 184, 77,
    73,  113, 201, 169, 208, 133, 139, 238, 8,   28,  42,  120, 108, 224, 8,   121, 134, 33,  2,
    37,  135, 6,   231, 59,  139, 103, 173, 202, 175, 16,  243, 47,  40,  11,  102, 44,  11,  202,
    175, 77,  66,  252, 204, 95,  224, 222, 77,  117, 90,  148, 111, 225, 230, 136, 198, 171, 192,
    193, 78,  166, 184, 181, 190, 146, 55,  131, 87,  8,   115, 253, 22,  214, 210, 214, 241, 180,
    194, 40,  116, 174, 35,  19,  251, 208, 123, 222, 245, 65,  68,  219, 241, 98,  149, 204, 68,
    129, 176, 82,  192, 161, 116, 76,  146, 111, 16,  8,   99,  218, 159, 29,  21,  75,  195, 89,
    91,  217, 142, 171, 23,  221, 156, 203, 254, 48,  121, 37,  142, 67,  138, 102, 166, 71,  190,
    183, 39,  237, 207, 69,  143, 35,  165, 29,  84,  100, 92,  124, 159, 167, 67,  35,  239, 97,
    239, 76,  18,  246, 201, 76,  25,  41,  252, 139, 35,  69,  26,  27,  160, 238, 68,  72,  148,
    11,  246, 14,  203, 246, 35,  29,  251, 129, 34,  69,  115, 15,  28,  64,  105, 93,  99,  172,
    167, 213, 41,  74,  200, 26,  159, 49,  160, 254, 85,  10,  138, 137, 193, 27,  77,  138, 84,
    91,  51,  133, 5,   177, 201, 134, 124, 98,  145, 65,  74,  165, 47,  91,  29,  43,  101, 62,
    88,  181, 23,  77,  81,  89,  159, 162, 116, 123, 23,  248, 22,  171, 177, 5,   141, 116, 77,
    221, 73,  101, 248, 2,   142, 185, 196, 87,  90,  23,  71,  41,  192, 204, 105, 65,  213, 141,
    6,   231, 44,  38,  4,   91,  216, 136, 186, 79,  30,  45,  96,  14,  212, 14,  97,  211, 205,
    135, 177, 215, 175, 117, 55,  51,  140, 171, 75,  87,  193, 255, 180, 157, 50,  156, 87,  198,
    134, 247, 111, 52,  117, 102, 29,  227, 116, 168, 97,  216, 4,   223, 40,  41,  53,  131, 195,
    255, 187, 99,  42,  87,  85,  139, 216, 57,  234, 29,  185, 18,  85,  167, 47,  40,  55,  169,
    128, 149, 63,  239, 16,  186, 1,   175, 212, 121, 1,   82,  254, 162, 21,  41,  163, 77,  181,
    204, 204, 84,  151, 14,  195, 229, 165, 153, 137, 13,  106, 43,  104, 199, 154, 82,  33,  231,
    90,  254, 72,  250, 20,  62,  192, 157, 31,  159, 89,  241, 112, 215, 93,  214, 149, 142, 71,
    138, 206, 21,  107, 163, 120, 84,  149, 82,  3,   62,  15,  84,  22,  4,   36,  183, 166, 167,
    245, 73,  170, 32,  94,  0,   146, 10,  216, 173, 103, 157, 117, 234, 103, 49,  160, 159, 41,
    217, 83,  4,   91,  20,  176, 137, 226, 176, 247, 173, 147, 110, 164, 73,  17,  230, 81,  228,
    254, 131, 238, 138, 48,  57,  183, 46,  14,  240, 132, 31,  234, 217, 22,  185, 153, 221, 13,
    5,   154, 241, 64,  180, 128, 250, 173, 139, 111, 200, 137, 202, 168, 42,  33,  95,  26,  140,
    103, 236, 74,  120, 13,  67,  57,  152, 169, 111, 162, 41,  198, 204, 180, 2,   227, 122, 63,
    241, 76,  11,  89,  1,   48,  8,   228, 89,  64,  225, 208, 5,   116, 231, 189, 187, 198, 231,
    62,  32,  43,  175, 165, 243, 232, 134, 200, 91,  167, 247, 65,  247, 50,  192, 207, 119, 142,
    75,  161, 40,  9,   253, 229, 83,  197, 21,  136, 142, 238, 197, 68,  75,  154, 212, 90,  91,
    123, 113, 252, 53,  173, 222, 157, 150, 236, 118, 186, 250, 174, 182, 60,  51,  142, 48,  7,
    21,  211, 183, 118, 22,  224, 124, 125, 194, 255, 158, 230, 156, 217, 224, 69,  62,  248, 95,
    81,  179, 34,  140, 100, 218, 124, 238, 70,  166, 194, 147, 103, 84,  88,  125, 17,  51,  164,
    94,  156, 130, 4,   129, 251, 115, 97,  84,  191, 153, 228, 139, 121, 19,  16,  186, 135, 191,
    68,  47,  200, 242, 24,  30,  232, 240, 40,  62,  136, 247, 136, 10,  32,  70,  27,  78,  139,
    57,  136, 172, 143, 112, 151, 249, 202, 109, 128, 103, 76,  204, 44,  19,  160, 205, 21,  35,
    187, 139, 0,   24,  206, 73,  208, 78,  161, 17,  165, 244, 133, 245, 67,  179, 225, 236, 249,
    166, 84,  54,  43,  236, 124, 98,  75,  131, 176, 112, 4,   123, 91,  36,  91,  64,  46,  158,
    190, 33,  163, 199, 192, 129, 242, 16,  36,  199, 88,  126, 22,  135, 17,  228, 84,  48,  167,
    71,  180, 215, 225, 46,  153, 212, 219, 106, 73,  43,  15,  157, 205, 187, 118, 180, 170, 68,
    177, 64,  205, 239, 156, 200, 24,  144, 192, 144, 100, 8,   137, 197, 17,  227, 237, 83,  239,
    174, 144, 180, 131, 75,  221, 117, 117, 192, 150, 97,  246, 247, 253, 92,  249, 123, 168, 250,
    0,   4,   125, 190, 185, 192, 166, 231, 233, 150, 150, 183, 126, 195, 59,  47,  14,  49,  245,
    211, 206, 155, 85,  92,  17,  217, 34,  70,  141, 116, 125, 252, 102, 114, 147, 223, 144, 178,
    82,  197, 174, 225, 189, 173, 149, 136, 244, 100, 31,  233, 220, 125, 190, 138, 156, 100, 101,
    36,  244, 106, 82,  123, 6,   117, 57,  120, 215, 55,  55,  101, 44,  201, 216, 233, 174, 73,
    20,  180, 211, 96,  30,  171, 18,  203, 96,  156, 246, 173, 80,  3,   145, 111, 181, 41,  84,
    163, 193, 178, 159, 132, 96,  205, 164, 17,  154, 80,  44,  186, 201, 72,  73,  217, 24,  231,
    194, 210, 113, 16,  0,   185, 68,  234, 181, 200, 86,  215, 146, 107, 51,  63,  74,  164, 19,
    63,  191, 212, 155, 216, 248, 56,  68,  187, 137, 57,  246, 206, 161, 199, 234, 45,  70,  26,
    101, 50,  22,  125, 208, 153, 205, 73,  139, 59,  141, 169, 16,  190, 35,  30,  144, 208, 46,
    126, 14,  46,  237, 161, 183, 169, 51,  144, 130, 119, 127, 139, 14,  229, 102, 115, 207, 2,
    255, 155, 203, 52,  236, 226, 219, 70,  12,  190, 29,  224, 56,  180, 198, 10,  67,  15,  238,
    139, 124, 80,  225, 181, 121, 191, 215, 50,  134, 42,  202, 203, 16,  33,  152, 82,  226, 118,
    83,  165, 103, 21,  140, 70,  121, 194, 147, 218, 99,  187, 164, 14,  185, 16,  37,  107, 0,
    204, 83,  249, 241, 33,  158, 64,  167, 151, 136, 198, 148, 197, 84,  166, 183, 228, 218, 114,
    168, 135, 175, 159, 220, 47,  8,   56,  225, 94,  152, 47,  152, 61,  12,  7,   80,  162, 223,
    56,  135, 191, 142, 61,  181, 223, 93,  197, 68,  226, 137, 31,  136, 33,  32,  26,  248, 55,
    96,  210, 29,  185, 111, 48,  189, 21,  156, 13,  67,  192, 97,  194, 35,  104, 51,  70,  225,
    68,  98,  176, 11,  102, 38,  191, 232, 9,   114, 96,  153, 200, 170, 163, 207, 235, 15,  233,
    120, 85,  216, 174, 231, 90,  233, 97,  208, 37,  183, 16,  231, 246, 124, 120, 201, 31,  170,
    208, 248, 38,  218, 245, 216, 130, 140, 95,  206, 35,  76,  189, 11,  76,  239, 240, 143, 231,
    212, 229, 32,  168, 183, 238, 45,  52,  121, 62,  91,  115, 195, 70,  22,  68,  245, 18,  1,
    44,  79,  134, 196, 104, 172, 140, 148, 89,  250, 113, 7,   165, 99,  27,  132, 143, 141, 240,
    230, 210, 27,  254, 210, 124, 34,  83,  108, 230, 114, 184, 74,  151, 128, 217, 127, 5,   165,
    215, 26,  106, 51,  76,  216, 230, 205, 116, 157, 39,  0,   229, 124, 100, 109, 37,  110, 5,
    10,  248, 115, 13,  207, 175, 230, 10,  217, 114, 8,   52,  173, 79,  49,  22,  198, 224, 207,
    176, 2,   243, 8,   123, 214, 175, 33,  42,  195, 153, 6,   86,  215, 172, 138, 169, 14,  51,
    165, 201, 133, 81,  39,  133, 40,  191, 28,  250, 131, 128, 176, 46,  18,  231, 31,  253, 116,
    22,  127, 12,  130, 109, 49,  204, 46,  79,  176, 131, 111, 105, 110, 158, 113, 63,  246, 66,
    253, 44,  130, 8,   13,  187, 220, 194, 228, 126, 51,  190, 160, 98,  133, 179, 255, 223, 177,
    54,  202, 149, 224, 145, 154, 69,  75,  249, 177, 82,  239, 151, 110, 169, 224, 12,  224, 153,
    108, 246, 219, 201, 71,  80,  219, 163, 129, 202, 30,  5,   250, 179, 46,  160, 76,  134, 224,
    241, 88,  19,  44,  19,  25,  139, 66,  71,  135, 69,  54,  86,  47,  209, 16,  242, 69,  61,
    78,  115, 90,  113, 212, 159, 215, 32,  237, 128, 192, 122, 247, 35,  150, 23,  150, 189, 14,
    181, 177, 44,  22,  145, 25,  4,   82,  245, 230, 125, 226, 219, 3,   34,  73,  182, 58,  191,
    129, 243, 121, 207, 169, 129, 23,  194, 40,  89,  229, 93,  130, 178, 26,  218, 195, 88,  153,
    201, 116, 202, 51,  151, 47,  101, 145, 247, 224, 115, 112, 1,   185, 16,  68,  224, 138, 4,
    74,  134, 94,  39,  176, 222, 28,  195, 102, 187, 222, 134, 74,  4,   131, 34,  205, 24,  21,
    47,  109, 38,  136, 212, 14,  107, 96,  97,  236, 118, 122, 185, 37,  141, 83,  194, 79,  192,
    143, 44,  81,  183, 97,  132, 101, 22,  32,  112, 126, 161, 60,  11,  111, 148, 125, 174, 123,
    74,  198, 40,  3,   105, 242, 153, 32,  127, 122, 222, 121, 36,  16,  113, 205, 88,  12,  231,
    126, 5,   233, 134, 164, 153, 210, 182, 180, 17,  181, 211, 95,  108, 94,  202, 225, 190, 94,
    113, 95,  84,  24,  47,  80,  35,  57,  50,  124, 21,  151, 215, 179, 17,  156, 68,  66,  71,
    27,  115, 34,  15,  77,  178, 169, 53,  234, 116, 86,  175, 111, 66,  44,  0,   8,   0,   0};

TEST(ui_web_host_handle_request_is_upgraded, commits_only_after_every_gzip_member_validates)
{
    for (bool corrupt : {false, true})
    {
        ASSERT_TRUE(lh_os_net_init());
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t server, client;
        lh_os_net_socket_init(&server);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_set_handle(&server, handles[0]);
        lh_os_net_socket_set_handle(&client, handles[1]);
        lh_os_web_app_route_t match = {};
        lh_os_web_app_route_set_method(&match, "POST");
        lh_os_web_app_route_set_method_len(&match, 4U);
        lh_os_web_app_route_set_path(&match, "/data");
        lh_os_web_app_route_set_path_len(&match, 5U);
        lh_os_web_app_stream_route_t route;
        lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_stage_one,
                                        ui_web_host_test_stream_mark_commit,
                                        ui_web_host_test_stream_mark_abort, 32768U, 36U, 5000U);
        lh_os_web_app_stream_route_set_wire_limit(&route, 2U * sizeof(ui_web_host_gzip_sample));
        lh_os_web_app_t app;
        lh_os_web_app_init(&app, nullptr, 0U);
        lh_os_web_app_set_stream_routes(&app, &route);
        lh_os_web_app_set_nstream_routes(&app, 1U);
        std::string staged;
        lh_os_web_app_set_ctx(&app, &staged);
        std::string body(reinterpret_cast<const char *>(ui_web_host_gzip_sample),
                         sizeof(ui_web_host_gzip_sample));
        body += body;
        if (corrupt)
        {
            body[body.size() - 8U] ^= 1;
        }
        const std::string head =
            "POST /data HTTP/1.1\r\nContent-Encoding: gzip\r\nContent-Length: " +
            std::to_string(body.size()) + "\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
        std::thread worker(
            [&] { LH_ATTRIBUTE_UNUSED(lh_os_web_host_handle_request_is_upgraded(&app, &server)); });
        lh_os_net_socket_t *watch[] = {&client};
        const bool ready = lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 1000U) > 0;
        const std::string response = ready ? read_head(&client) : "";
        const bool sent = lh_os_net_socket_send_all(&client, body.data(), body.size());
        worker.join();
        EXPECT_TRUE(ready);
        EXPECT_TRUE(sent);
        EXPECT_EQ(response, "HTTP/1.1 100 Continue\r\n\r\n");
        EXPECT_EQ(staged, corrupt ? "ABORT" : "A COMMIT");
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
    }
}

TEST(ui_web_host_handle_request_is_upgraded, accepts_empty_gzip_with_zero_decoded_limit)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&server, handles[0]);
    lh_os_net_socket_set_handle(&client, handles[1]);
    lh_os_web_app_route_t match = {};
    lh_os_web_app_route_set_method(&match, "POST");
    lh_os_web_app_route_set_method_len(&match, 4U);
    lh_os_web_app_route_set_path(&match, "/data");
    lh_os_web_app_route_set_path_len(&match, 5U);
    lh_os_web_app_stream_route_t route;
    lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                    ui_web_host_test_stream_mark_commit,
                                    ui_web_host_test_stream_mark_abort, 0U, 18U, 1000U);
    EXPECT_EQ(lh_os_web_app_stream_route_get_wire_limit(&route), LH_USIZE_T_MAX);
    lh_os_web_app_stream_route_set_wire_limit(&route, 20U);
    EXPECT_EQ(lh_os_web_app_stream_route_get_wire_limit(&route), 20U);
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0U);
    lh_os_web_app_set_stream_routes(&app, &route);
    lh_os_web_app_set_nstream_routes(&app, 1U);
    std::string staged;
    lh_os_web_app_set_ctx(&app, &staged);
    const lh_byte_t body[] = {31, 139, 8, 0, 0, 0, 0, 0, 2, 255, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const std::string head =
        "POST /data HTTP/1.1\r\nContent-Length: 20\r\nContent-Encoding: gzip\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, body, sizeof(body)));
    EXPECT_FALSE(lh_os_web_host_handle_request_is_upgraded(&app, &server));
    EXPECT_EQ(staged, " COMMIT");
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

TEST(ui_web_host_handle_request_is_upgraded, rejects_unsupported_coding_before_continue)
{
    for (int scenario : {0, 1, 2})
    {
        ASSERT_TRUE(lh_os_net_init());
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t server;
        lh_os_net_socket_t client;
        lh_os_net_socket_init(&server);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_set_handle(&server, handles[0]);
        lh_os_net_socket_set_handle(&client, handles[1]);
        lh_os_web_app_route_t match = {};
        lh_os_web_app_route_set_method(&match, "POST");
        lh_os_web_app_route_set_method_len(&match, 4U);
        lh_os_web_app_route_set_path(&match, "/data");
        lh_os_web_app_route_set_path_len(&match, 5U);
        lh_os_web_app_route_set_handler(&match, ui_web_host_test_capture_body);
        lh_os_web_app_stream_route_t route;
        lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                        ui_web_host_test_stream_mark_commit,
                                        ui_web_host_test_stream_mark_abort, 100U, 100U, 1000U);
        lh_os_web_app_t app;
        lh_os_web_app_init(&app, scenario == 2 ? &match : nullptr, scenario == 2 ? 1U : 0U);
        if (scenario != 2)
        {
            lh_os_web_app_set_stream_routes(&app, &route);
            lh_os_web_app_set_nstream_routes(&app, 1U);
        }
        std::string staged;
        lh_os_web_app_set_ctx(&app, &staged);
        const std::string head =
            scenario == 1 ? "POST /data HTTP/1.1\r\nTransfer-Encoding: gzip, "
                            "chunked\r\nContent-Encoding: gzip\r\nExpect: 100-continue\r\n\r\n"
                          : "POST /data HTTP/1.1\r\nContent-Length: 10\r\nContent-Encoding: "
                            "br\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
        EXPECT_FALSE(lh_os_web_host_handle_request_is_upgraded(&app, &server));
        EXPECT_NE(read_head(&client).find("HTTP/1.1 415 Unsupported Media Type\r\n"),
                  std::string::npos);
        EXPECT_TRUE(staged.empty());
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
    }
}

TEST(ui_web_host_handle_request_is_upgraded, streams_fragmented_large_gzip_chunks)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&server, handles[0]);
    lh_os_net_socket_set_handle(&client, handles[1]);
    lh_os_web_app_route_t match = {};
    lh_os_web_app_route_set_method(&match, "POST");
    lh_os_web_app_route_set_method_len(&match, 4U);
    lh_os_web_app_route_set_path(&match, "/data");
    lh_os_web_app_route_set_path_len(&match, 5U);
    lh_os_web_app_stream_route_t route;
    lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                    ui_web_host_test_stream_mark_commit,
                                    ui_web_host_test_stream_mark_abort, 2048U, 2048U, 10000U);
    lh_os_web_app_stream_route_set_wire_limit(&route, sizeof(ui_web_host_gzip_binary));
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0U);
    lh_os_web_app_set_stream_routes(&app, &route);
    lh_os_web_app_set_nstream_routes(&app, 1U);
    std::string captured;
    lh_os_web_app_set_ctx(&app, &captured);
    const char head[] =
        "POST /data HTTP/1.1\r\nTransfer-Encoding: gzip, chunked\r\nExpect: 100-continue\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    std::thread worker(
        [&] { LH_ATTRIBUTE_UNUSED(lh_os_web_host_handle_request_is_upgraded(&app, &server)); });
    lh_os_net_socket_t *watch[] = {&client};
    const bool ready = lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 1000U) > 0;
    const std::string response = ready ? read_head(&client) : "";
    const char hex[] = "0123456789abcdef";
    std::string encoded;
    for (size_t pos = 0; pos < sizeof(ui_web_host_gzip_binary);)
    {
        const size_t remaining = sizeof(ui_web_host_gzip_binary) - pos;
        const size_t count = remaining < 64U ? remaining : 64U;
        if (count >= 16U)
        {
            encoded.push_back(hex[count / 16U]);
        }
        encoded.push_back(hex[count % 16U]);
        encoded += "\r\n";
        encoded.append(reinterpret_cast<const char *>(ui_web_host_gzip_binary + pos), count);
        encoded += "\r\n";
        pos += count;
    }
    encoded += "0\r\nX-Final: yes\r\n\r\n";
    bool sent = true;
    for (size_t pos = 0; pos < encoded.size() && sent;)
    {
        const size_t count = encoded.size() - pos < 31U ? encoded.size() - pos : 31U;
        sent = lh_os_net_socket_send_all(&client, encoded.data() + pos, count);
        pos += count;
    }
    worker.join();
    EXPECT_TRUE(ready);
    EXPECT_TRUE(sent);
    EXPECT_EQ(response, "HTTP/1.1 100 Continue\r\n\r\n");
    std::string expected;
    unsigned x = 0x12345678U;
    for (int i = 0; i < 2048; ++i)
    {
        x ^= x << 13U;
        x ^= x >> 17U;
        x ^= x << 5U;
        expected.push_back(static_cast<char>(x & 255U));
    }
    EXPECT_EQ(captured, expected + " COMMIT");
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

TEST(ui_web_host_handle_request_is_upgraded, rejects_oversized_stream_before_continue)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&server, handles[0]);
    lh_os_net_socket_set_handle(&client, handles[1]);
    lh_os_web_app_route_t match = {};
    lh_os_web_app_route_set_method(&match, "POST");
    lh_os_web_app_route_set_method_len(&match, 4U);
    lh_os_web_app_route_set_path(&match, "/data");
    lh_os_web_app_route_set_path_len(&match, 5U);
    lh_os_web_app_stream_route_t route;
    lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                    ui_web_host_test_stream_commit, ui_web_host_test_stream_abort,
                                    8U, 1024U, 1000U);
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0U);
    lh_os_web_app_set_stream_routes(&app, &route);
    lh_os_web_app_set_nstream_routes(&app, 1U);
    std::string captured;
    lh_os_web_app_set_ctx(&app, &captured);
    const char head[] = "POST /data HTTP/1.1\r\nContent-Length: 9\r\nExpect: 100-continue\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    EXPECT_FALSE(lh_os_web_host_handle_request_is_upgraded(&app, &server));
    EXPECT_NE(read_head(&client).find("HTTP/1.1 413 Content Too Large\r\n"), std::string::npos);
    EXPECT_TRUE(captured.empty());
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

TEST(ui_web_host_handle_request_is_upgraded, validates_and_decodes_buffered_gzip)
{
    for (int scenario : {0, 1, 2, 3})
    {
        ASSERT_TRUE(lh_os_net_init());
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t server;
        lh_os_net_socket_t client;
        lh_os_net_socket_init(&server);
        lh_os_net_socket_init(&client);
        lh_os_net_socket_set_handle(&server, handles[0]);
        lh_os_net_socket_set_handle(&client, handles[1]);
        lh_os_web_app_route_t route;
        lh_os_web_app_route_set_method(&route, "POST");
        lh_os_web_app_route_set_method_len(&route, 4U);
        lh_os_web_app_route_set_path(&route, "/data");
        lh_os_web_app_route_set_path_len(&route, 5U);
        lh_os_web_app_route_set_handler(&route, ui_web_host_test_capture_body);
        lh_os_web_app_t app;
        lh_os_web_app_init(&app, &route, 1U);
        std::string captured;
        lh_os_web_app_set_ctx(&app, &captured);
        const lh_byte_t encoded[] = {31, 139, 8, 0, 0,   0,   0,   0,  2, 255, 75, 100,
                                     72, 74,  6, 0, 189, 118, 174, 33, 4, 0,   0,  0};
        std::string body(reinterpret_cast<const char *>(encoded), sizeof(encoded));
        const bool corrupt = scenario == 1 || scenario == 3;
        const bool transfer = scenario >= 2;
        if (corrupt)
        {
            body[body.size() - 8U] ^= 1;
        }
        const std::string wire =
            transfer ? "POST /data HTTP/1.1\r\nTransfer-Encoding: gzip, chunked\r\n\r\n18\r\n" +
                           body + "\r\n0\r\n\r\n"
                     : "POST /data HTTP/1.1\r\nContent-Encoding: gzip\r\nContent-Length: " +
                           std::to_string(body.size()) + "\r\n\r\n" + body;
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, wire.data(), wire.size()));
        EXPECT_FALSE(lh_os_web_host_handle_request_is_upgraded(&app, &server));
        if (corrupt)
        {
            EXPECT_TRUE(captured.empty());
            if (!transfer)
            {
                EXPECT_NE(read_head(&client).find("HTTP/1.1 400 Bad Request\r\n"),
                          std::string::npos);
            }
        }
        else
        {
            EXPECT_EQ(captured, std::string("a\0bc", 4));
        }
        lh_os_net_socket_close(&server);
        lh_os_net_socket_close(&client);
    }
}

TEST(ui_web_host_query, searches_only_the_parsed_query_and_exact_keys)
{
    const lh_str_view_t target = lh_str_view_initializer_lit("/ui?dx=7&x=12&x=99&flag=0");
    lh_http_request_t request;
    lh_http_request_set_target(lh_addr_of(request), target);
    lh_str_cptr value = nullptr;
    EXPECT_EQ(lh_os_web_host_get_query_value_size(lh_addr_of(request), lh_str_lit("x="),
                                                  lh_addr_of(value)),
              2u);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(std::string(value, 2), "12");
    EXPECT_EQ(lh_os_web_host_get_query_int(lh_addr_of(request), lh_str_lit("dx=")), 7);
    EXPECT_FALSE(
        lh_os_web_host_get_query_flag(lh_addr_of(request), lh_str_lit("flag="), lh_bool_true));
    EXPECT_TRUE(
        lh_os_web_host_get_query_flag(lh_addr_of(request), lh_str_lit("missing="), lh_bool_true));
    const lh_str_view_t empty_value = lh_str_view_initializer_lit("/?x=");
    lh_http_request_set_target(lh_addr_of(request), empty_value);
    value = nullptr;
    EXPECT_EQ(lh_os_web_host_get_query_value_size(lh_addr_of(request), lh_str_lit("x="),
                                                  lh_addr_of(value)),
              0u);
    EXPECT_EQ(value, lh_str_view_get_end(lh_addr_of(empty_value)));
    const lh_str_view_t no_query = lh_str_view_initializer_lit("/x=12");
    lh_http_request_set_target(lh_addr_of(request), no_query);
    value = nullptr;
    EXPECT_EQ(lh_os_web_host_get_query_value_size(lh_addr_of(request), lh_str_lit("x="),
                                                  lh_addr_of(value)),
              0u);
    EXPECT_EQ(value, nullptr);
    EXPECT_EQ(lh_os_web_host_get_query_value_size(lh_addr_of(request), "", 0, lh_addr_of(value)),
              0u);
    EXPECT_EQ(value, nullptr);
}

TEST(ui_web_host_is_route, matches_only_complete_paths_for_get_and_post)
{
    const lh_str_view_t get = lh_str_view_initializer_lit("GET");
    const lh_str_view_t post = lh_str_view_initializer_lit("POST");
    const lh_str_view_t put = lh_str_view_initializer_lit("PUT");
    const lh_str_view_t path = lh_str_view_initializer_lit("/ui/key?x=1");
    const lh_str_view_t longer = lh_str_view_initializer_lit("/ui/keyboard");

    lh_http_request_t request;
    lh_http_request_set_method(lh_addr_of(request), get);
    lh_http_request_set_target(lh_addr_of(request), path);
    EXPECT_TRUE(lh_os_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
    lh_http_request_set_method(lh_addr_of(request), post);
    EXPECT_TRUE(lh_os_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
    lh_http_request_set_method(lh_addr_of(request), put);
    EXPECT_FALSE(lh_os_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
    lh_http_request_set_method(lh_addr_of(request), get);
    lh_http_request_set_target(lh_addr_of(request), longer);
    EXPECT_FALSE(lh_os_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
}

TEST(ui_web_host_try_read_request, sends_continue_before_waiting_for_content)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_set_handle(&server, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&server, lh_bool_true));
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&client, handles[1]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&client, lh_bool_true));
    const char head[] = "POST / HTTP/1.1\r\nContent-Length: 4\r\nExpect: 100-continue\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    lh_usize_t length = 0U;
    bool ok = false;
    char buffer[256];
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[4];
    std::thread worker(
        [&]
        {
            ok = lh_os_web_host_try_read_request(&server, buffer, sizeof(buffer), 1000U, &length,
                                                 &body, &request, headers, 4U);
        });
    lh_os_net_socket_t *watch[] = {&client};
    const bool ready = lh_os_net_socket_wait(watch, 1U, nullptr, 0U, 500U) > 0;
    const std::string interim = ready ? read_head(&client) : "";
    const bool sent = lh_os_net_socket_send_all(&client, "body", 4U);
    worker.join();
    EXPECT_TRUE(ready);
    EXPECT_EQ(interim, "HTTP/1.1 100 Continue\r\n\r\n");
    EXPECT_TRUE(sent);
    ASSERT_TRUE(ok);
    EXPECT_EQ(std::string(lh_str_view_get_data(&body), lh_str_view_get_size(&body)), "body");
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

TEST(ui_web_host_try_read_request, rejects_unsupported_expectation_with_417)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t server;
    lh_os_net_socket_init(&server);
    lh_os_net_socket_set_handle(&server, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&server, lh_bool_true));
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    lh_os_net_socket_set_handle(&client, handles[1]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&client, lh_bool_true));
    const char head[] = "POST / HTTP/1.1\r\nContent-Length: 4\r\nExpect: unknown\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    lh_usize_t length = 0U;
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[4];
    char buffer[256];
    EXPECT_FALSE(lh_os_web_host_try_read_request(&server, buffer, sizeof(buffer), 1000U, &length,
                                                 &body, &request, headers, 4U));
    EXPECT_NE(read_head(&client).find("HTTP/1.1 417 Expectation Failed\r\n"), std::string::npos);
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

TEST(ui_web_host_send_continue, reports_and_checks_socket_failure)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(lh_os_web_host_try_send_continue(&socket));
    LH_EXPECT_CHECK_DEATH(lh_os_web_host_send_continue(&socket));
}

TEST(ui_web_host_try_send_response, sends_caller_fields_and_body)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(&reader);
    lh_os_net_socket_set_handle(&reader, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&reader, lh_bool_true));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(&writer);
    lh_os_net_socket_set_handle(&writer, handles[1]);
    lh_http_response_t response;
    lh_http_response_set_version_minor(&response, 1U);
    lh_http_response_set_status(&response, 201U);
    lh_http_response_set_reason(&response, lh_str_view_lit("Created"));
    lh_http_response_set_header_count(&response, 2U);
    lh_http_header_t headers[2];
    lh_http_header_set(&headers[0], lh_str_view_lit("X-Custom"), lh_str_view_lit("yes"));
    lh_http_header_set(&headers[1], lh_str_view_lit("Content-Length"), lh_str_view_lit("4"));
    char scratch[128];
    ASSERT_TRUE(lh_os_web_host_try_send_response(&writer, &response, headers, scratch,
                                                 sizeof(scratch), "body", 4U));
    EXPECT_EQ(read_head(&reader),
              "HTTP/1.1 201 Created\r\nX-Custom: yes\r\nContent-Length: 4\r\n\r\n");
    char body[4];
    ASSERT_EQ(lh_os_net_socket_recv(&reader, body, sizeof(body)), 4);
    EXPECT_EQ(std::string(body, sizeof(body)), "body");
    lh_os_net_socket_close(&reader);
    lh_os_net_socket_close(&writer);
}

TEST(ui_web_host_send_response, checks_format_failure)
{
    lh_http_response_t response;
    lh_http_response_set_version_minor(&response, 1U);
    lh_http_response_set_status(&response, 200U);
    lh_http_response_set_reason(&response, lh_str_view_lit("OK"));
    lh_http_response_set_header_count(&response, 0U);
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    char scratch[1];
    EXPECT_FALSE(lh_os_web_host_try_send_response(&socket, &response, nullptr, scratch,
                                                  sizeof(scratch), nullptr, 0U));
    LH_EXPECT_CHECK_DEATH(lh_os_web_host_send_response(&socket, &response, nullptr, scratch,
                                                       sizeof(scratch), nullptr, 0U));
}

TEST(ui_web_host_reply, reports_and_checks_invalid_status)
{
    lh_http_request_t request;
    lh_http_request_set_method(&request, lh_http_method_get_name(lh_http_method_get));
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(lh_os_web_host_try_reply(&socket, &request, 99U, "text/plain", "x", 1U));
    LH_EXPECT_CHECK_DEATH(lh_os_web_host_reply(&socket, &request, 99U, "text/plain", "x", 1U));
}

TEST(ui_web_host_try_reply, returns_false_when_the_client_is_disconnected)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_http_request_t request;
    lh_http_request_set_method(&request, lh_http_method_get_name(lh_http_method_get));
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(
        lh_os_web_host_try_reply(&socket, &request, lh_http_status_ok, "text/plain", "x", 1U));
}

TEST(ui_web_host_reply_text, reports_and_checks_invalid_status)
{
    lh_http_request_t request;
    lh_http_request_set_method(&request, lh_http_method_get_name(lh_http_method_get));
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(lh_os_web_host_try_reply_text(&socket, &request, 99U, "x", 1U));
    LH_EXPECT_CHECK_DEATH(lh_os_web_host_reply_text(&socket, &request, 99U, "x", 1U));
}

TEST(web_host, head_preserves_a_64_bit_representation_length)
{
    if (sizeof(lh_usize_t) < sizeof(lh_u64_t))
    {
        GTEST_SKIP() << "size type cannot represent this length";
    }
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(lh_addr_of(reader));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(lh_addr_of(writer));
    lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
    lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
    lh_http_request_t request;
    lh_http_request_set_method(lh_addr_of(request), lh_http_method_get_name(lh_http_method_head));
    lh_os_web_host_reply(lh_addr_of(writer), lh_addr_of(request), lh_http_status_ok, "text/plain",
                         nullptr, static_cast<lh_usize_t>(4294967297ULL));
    const std::string head = read_head(lh_addr_of(reader));
    EXPECT_NE(head.find("Content-Length: 4294967297\r\n"), std::string::npos);
    EXPECT_EQ(lh_os_net_socket_pending(lh_addr_of(reader)), 0);
    lh_os_net_socket_close(lh_addr_of(reader));
    lh_os_net_socket_close(lh_addr_of(writer));
}

TEST(web_host, head_and_connect_responses_do_not_send_content)
{
    ASSERT_TRUE(lh_os_net_init());
    const lh_http_method_kind_t methods[] = {lh_http_method_head, lh_http_method_connect};
    for (lh_http_method_kind_t method : methods)
    {
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t reader;
        lh_os_net_socket_t writer;
        lh_os_net_socket_init(lh_addr_of(reader));
        lh_os_net_socket_init(lh_addr_of(writer));
        lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
        lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
        ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
        lh_http_request_t request;
        lh_http_request_set_method(lh_addr_of(request), lh_http_method_get_name(method));
        lh_os_web_host_reply(lh_addr_of(writer), lh_addr_of(request), lh_http_status_ok,
                             "text/plain", "body", 4);
        const std::string head = read_head(lh_addr_of(reader));
        if (method == lh_http_method_head)
        {
            EXPECT_NE(head.find("Content-Length: 4\r\n"), std::string::npos);
        }
        else
        {
            EXPECT_EQ(head.find("Content-Length:"), std::string::npos);
        }
        EXPECT_EQ(lh_os_net_socket_pending(lh_addr_of(reader)), 0);
        lh_os_net_socket_close(lh_addr_of(reader));
        lh_os_net_socket_close(lh_addr_of(writer));
    }
}

TEST(web_host, content_forbidden_statuses_send_no_body_or_content_length)
{
    ASSERT_TRUE(lh_os_net_init());
    const lh_uint_t statuses[] = {100, lh_http_status_no_content, lh_http_status_reset_content,
                                  lh_http_status_not_modified};
    const lh_str_view_t method = lh_str_view_initializer_lit("GET");
    lh_http_request_t request;
    lh_http_request_set_method(lh_addr_of(request), method);
    for (lh_uint_t status : statuses)
    {
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, LH_OS_SYSTEM_NET_SOCKET_PAIR_TIMEOUT_MS));
        lh_os_net_socket_t reader;
        lh_os_net_socket_t writer;
        lh_os_net_socket_init(lh_addr_of(reader));
        lh_os_net_socket_init(lh_addr_of(writer));
        lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
        lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
        ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
        lh_os_web_host_reply(lh_addr_of(writer), lh_addr_of(request), status, "text/plain", "body",
                             4);
        const std::string head = read_head(lh_addr_of(reader));
        EXPECT_EQ(head.find("Content-Length:"), std::string::npos) << status;
        EXPECT_EQ(lh_os_net_socket_pending(lh_addr_of(reader)), 0) << status;
        lh_os_net_socket_close(lh_addr_of(reader));
        lh_os_net_socket_close(lh_addr_of(writer));
    }
}

TEST(ui_web_host_decode_gzip_in_place, reports_and_checks_invalid_input)
{
    char data[1] = {0};
    lh_usize_t result = 17U;
    EXPECT_FALSE(lh_os_web_host_try_decode_gzip_in_place(data, 1U, 0U, &result));
    EXPECT_EQ(result, 17U);
    LH_EXPECT_CHECK_DEATH(lh_os_web_host_decode_gzip_in_place(data, 1U, 0U, &result));
}

lh_ssize_t
ui_web_host_test_error_reader(lh_self_ptr, lh_ptr, lh_usize_t)
{
    return LH_IO_ERROR;
}

TEST(ui_web_host_gzip_workspace, provides_distinct_owned_states_and_buffers)
{
    lh_os_web_host_gzip_workspace_t workspace;
    EXPECT_NE(lh_os_web_host_gzip_workspace_get_scratch(&workspace), nullptr);
    EXPECT_NE(lh_os_web_host_gzip_workspace_get_history(&workspace), nullptr);
    EXPECT_NE(lh_os_web_host_gzip_workspace_get_scratch(&workspace),
              lh_os_web_host_gzip_workspace_get_history(&workspace));
    EXPECT_NE(lh_os_web_host_gzip_workspace_get_header(&workspace), nullptr);
    EXPECT_NE(lh_os_web_host_gzip_workspace_get_deflate(&workspace), nullptr);
    EXPECT_NE(lh_os_web_host_gzip_workspace_get_reader(&workspace), nullptr);
}

TEST(ui_web_host_dispatch_stream, aborts_and_checked_form_reports_read_failure)
{
    lh_os_web_app_route_t match = {};
    lh_os_web_app_stream_route_t route;
    lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_stage_one,
                                    ui_web_host_test_stream_mark_commit,
                                    ui_web_host_test_stream_mark_abort, 100U, 100U, 1000U);
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0U);
    std::string staged;
    lh_os_web_app_set_ctx(&app, &staged);
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, ui_web_host_test_error_reader, nullptr);
    lh_http_request_t request = {};
    lh_http_header_t headers[1] = {};
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    EXPECT_FALSE(
        lh_os_web_host_try_dispatch_stream(&app, &route, &request, headers, &reader, &client));
    EXPECT_EQ(staged, "ABORT");
    LH_EXPECT_CHECK_DEATH(
        lh_os_web_host_dispatch_stream(&app, &route, &request, headers, &reader, &client));
}

TEST(ui_web_host_dispatch_gzip, aborts_and_checked_form_reports_invalid_source)
{
    lh_os_web_app_route_t match = {};
    lh_os_web_app_stream_route_t route;
    lh_os_web_app_stream_route_init(&route, &match, ui_web_host_test_stream_stage_one,
                                    ui_web_host_test_stream_mark_commit,
                                    ui_web_host_test_stream_mark_abort, 100U, 100U, 1000U);
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, nullptr, 0U);
    std::string staged;
    lh_os_web_app_set_ctx(&app, &staged);
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, ui_web_host_test_error_reader, nullptr);
    lh_http_request_t request = {};
    lh_http_header_t headers[1] = {};
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    EXPECT_FALSE(
        lh_os_web_host_try_dispatch_gzip(&app, &route, &request, headers, &reader, &client));
    EXPECT_EQ(staged, "ABORT");
    LH_EXPECT_CHECK_DEATH(
        lh_os_web_host_dispatch_gzip(&app, &route, &request, headers, &reader, &client));
}

namespace
{

/* The frame slot of a test: it writes down the restart it was given and makes
   no frame, so nothing is sent. */
lh_void
remember_restart(lh_ptr ctx, lh_bool_t restart, const lh_str_ptr *bytes, lh_usize_t *len)
{
    LH_ATTRIBUTE_UNUSED(bytes);
    LH_ATTRIBUTE_UNUSED(len);
    *static_cast<int *>(ctx) = restart ? 1 : 0;
}

} // namespace

TEST(web_host_ws_frame, a_client_out_of_sync_asks_for_a_frame_that_stands_on_its_own)
{
    int heard = -1;

    lh_os_web_host_client_t clients[LH_OS_WEB_HOST_WS_MAX];
    std::memset(clients, 0, sizeof(clients));
    lh_os_web_host_stream_t stream;
    std::memset(&stream, 0, sizeof(stream));
    lh_os_web_app_t app;
    lh_os_web_app_init(&app, lh_null, 0u);
    lh_os_web_app_set_ctx(&app, &heard);
    lh_os_web_app_set_frame_fn(&app, remember_restart);
    lh_os_web_app_set_generation(&app, 1u);

    lh_os_web_host_ws_frame(clients, &stream, &app);
    EXPECT_EQ(heard, -1) << "no client wants a frame: none is drawn";

    lh_os_web_host_client_set_used(&clients[0], lh_bool_true);
    lh_os_web_host_ws_frame(clients, &stream, &app);
    EXPECT_EQ(heard, 1) << "a new client holds nothing";

    lh_os_web_host_client_set_synced(&clients[0], lh_bool_true);
    lh_os_web_host_ws_frame(clients, &stream, &app);
    EXPECT_EQ(heard, 0) << "a synced client holds what the earlier frames sent";
}

#endif /* LH_LIBRARY_OPTION_OS */
