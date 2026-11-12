/**
 * @file host.cpp
 * @brief The web host's input lines: keys down and up, the old key line, pointer moves,
 *        the wheel.
 *
 * The host is a loop that does not return, so one runs on a thread of its own for the whole
 * test program and the tests talk to it the way the player does: WebSocket lines, and the
 * HTTP fallback. What it did is read back through its hooks -- `input` logs every key and
 * hands every event on to the view, `controller` counts what reaches it after `input` and
 * how often the host updates it, and `tick`, which runs on the host's thread once a pass,
 * copies the scroll the view is at.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/core/attribute/unused.h>
#    include <lh/expect/death.h>
#    include <lh/core/http/method.h>
#    include <lh/core/http/status.h>
#    include <lh/test/ui/draw_log.h>

#    include <lh/os/controller.h>
#    include <lh/os/input.h>
#    include <lh/os/net.h>
#    include <lh/os/net/socket.h>
#    include <lh/os/system/net/socket.h>
#    include <lh/os/tick.h>
#    include <lh/ui/container.h>
#    include <lh/ui/scrollbar.h>
#    include <lh/ui/view.h>
#    include <lh/ui/web/host.h>
#    include <lh/ui/web/host/gzip/workspace.h>
#    include <lh/ui/web/host/deadline.h>
#    include <lh/ui/web/player.h>
#    include <lh/ui/web/ws.h>

#    include <atomic>
#    include <chrono>
#    include <cstring>
#    include <string>
#    include <thread>

TEST(ui_web_host_try_read_request, leaves_length_unchanged_when_storage_is_empty)
{
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    lh_usize_t length = 17;
    lh_str_view_t request_body = lh_str_view_initializer_lit("unchanged");

    char buffer;
    lh_http_request_t request;
    EXPECT_FALSE(lh_ui_web_host_try_read_request(lh_addr_of(socket), lh_addr_of(buffer), 0, 10,
                                                 lh_addr_of(length), lh_addr_of(request_body),
                                                 lh_addr_of(request), nullptr, 0));
    EXPECT_EQ(length, 17u);
    EXPECT_EQ(lh_str_view_get_size(lh_addr_of(request_body)), 9u);
    LH_EXPECT_CHECK_DEATH(lh_ui_web_host_read_request(lh_addr_of(socket), lh_addr_of(buffer), 0, 10,
                                                      lh_addr_of(length), lh_addr_of(request_body),
                                                      lh_addr_of(request), nullptr, 0));
}

TEST(ui_web_host_handle_request_is_upgraded, rejects_a_socket_that_cannot_be_configured)
{
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0);

    EXPECT_FALSE(lh_ui_web_host_handle_request_is_upgraded(&app, &socket));
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
    ASSERT_TRUE(lh_ui_web_host_try_read_request(lh_addr_of(reader), buffer, sizeof(buffer), 1000,
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
    const bool received = lh_ui_web_host_try_read_request(
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
    const bool ok = lh_ui_web_host_try_read_request(&reader, buffer, sizeof(buffer), 1000U, &length,
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
    EXPECT_FALSE(lh_ui_web_host_try_read_request(&reader, buffer, sizeof(buffer), 1000U, &length,
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
    EXPECT_FALSE(lh_ui_web_host_try_read_request(&reader, buffer, sizeof(buffer), 1000U, &length,
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
        EXPECT_FALSE(lh_ui_web_host_try_read_request(lh_addr_of(reader), buffer, sizeof(buffer), 40,
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
    lh_ui_web_route_t route;
    lh_ui_web_route_set_method(lh_addr_of(route), "POST");
    lh_ui_web_route_set_method_len(lh_addr_of(route), 4);
    lh_ui_web_route_set_path(lh_addr_of(route), "/data");
    lh_ui_web_route_set_path_len(lh_addr_of(route), 5);
    lh_ui_web_route_set_handler(lh_addr_of(route), ui_web_host_test_capture_body);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(lh_addr_of(app), lh_addr_of(route), 1);
    std::string captured;
    lh_ui_web_app_set_ctx(lh_addr_of(app), lh_addr_of(captured));
    const char head[] = "POST /data HTTP/1.1\r\nContent-Length: 3\r\n\r\na";
    ASSERT_TRUE(lh_os_net_socket_send_all(lh_addr_of(client), head, sizeof(head) - 1));
    std::thread host(
        [&app, &server]
        {
            LH_ATTRIBUTE_UNUSED(
                lh_ui_web_host_handle_request_is_upgraded(lh_addr_of(app), lh_addr_of(server)));
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
    lh_ui_web_route_t route;
    lh_ui_web_route_set_method(&route, "POST");
    lh_ui_web_route_set_method_len(&route, 4U);
    lh_ui_web_route_set_path(&route, "/data");
    lh_ui_web_route_set_path_len(&route, 5U);
    lh_ui_web_route_set_handler(&route, ui_web_host_test_capture_body);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, &route, 1U);
    std::string captured;
    lh_ui_web_app_set_ctx(&app, &captured);
    const char head[] = "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n";
    const char body[] = "2\r\na\0\r\n2\r\nbc\r\n0\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, body, sizeof(body) - 1U));
    EXPECT_FALSE(lh_ui_web_host_handle_request_is_upgraded(&app, &server));
    EXPECT_EQ(captured, std::string("a\0bc", 4U));
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
}

namespace
{
std::string
read_head(lh_os_net_socket_t *sock);
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
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0U);
    lh_ui_web_stream_route_t route = {};
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    lh_http_request_t request = {};
    const std::string bytes(LH_UI_WEB_HOST_REQUEST_CAPACITY + 1U, 'x');
    const lh_str_view_t initial = lh_str_view_initializer(bytes.data(), bytes.size());
    EXPECT_FALSE(
        lh_ui_web_host_try_serve_stream(&app, &route, &socket, &request, nullptr, initial, 0U));
    LH_EXPECT_CHECK_DEATH(
        lh_ui_web_host_serve_stream(&app, &route, &socket, &request, nullptr, initial, 0U));
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
    lh_ui_web_host_deadline_t deadline;
    lh_ui_web_host_deadline_init(&deadline, &server, lh_os_tick_ms(), 1000U);
    lh_io_reader_t reader;
    lh_ui_web_host_deadline_get_reader(&deadline, &reader);
    char out;
    EXPECT_EQ(lh_io_reader_read(&reader, &out, 1U), 1);
    EXPECT_EQ(out, 'a');
    lh_ui_web_host_deadline_init(&deadline, &server, lh_os_tick_ms(), 0U);
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
        lh_ui_web_route_t match = {};
        lh_ui_web_route_set_method(&match, "POST");
        lh_ui_web_route_set_method_len(&match, 4U);
        lh_ui_web_route_set_path(&match, "/data");
        lh_ui_web_route_set_path_len(&match, 5U);
        lh_ui_web_stream_route_t route;
        lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                    ui_web_host_test_stream_commit, ui_web_host_test_stream_abort,
                                    20000U, 1024U, 5000U);
        lh_ui_web_app_t app;
        lh_ui_web_app_init(&app, nullptr, 0U);
        lh_ui_web_app_set_stream_routes(&app, &route);
        lh_ui_web_app_set_nstream_routes(&app, 1U);
        std::string captured;
        lh_ui_web_app_set_ctx(&app, &captured);
        const std::string head =
            chunked
                ? "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\nExpect: "
                  "100-continue\r\n\r\n"
                : "POST /data HTTP/1.1\r\nContent-Length: 16384\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
        std::thread worker(
            [&] { LH_ATTRIBUTE_UNUSED(lh_ui_web_host_handle_request_is_upgraded(&app, &server)); });
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
        lh_ui_web_route_t match = {};
        lh_ui_web_route_set_method(&match, "POST");
        lh_ui_web_route_set_method_len(&match, 4U);
        lh_ui_web_route_set_path(&match, "/data");
        lh_ui_web_route_set_path_len(&match, 5U);
        lh_ui_web_stream_route_t route;
        lh_ui_web_stream_route_init(
            &route, &match, ui_web_host_test_stream_stage_one, ui_web_host_test_stream_mark_commit,
            ui_web_host_test_stream_mark_abort, oversize ? 2U : 100U, 1024U, 5000U);
        lh_ui_web_app_t app;
        lh_ui_web_app_init(&app, nullptr, 0U);
        lh_ui_web_app_set_stream_routes(&app, &route);
        lh_ui_web_app_set_nstream_routes(&app, 1U);
        std::string staged;
        lh_ui_web_app_set_ctx(&app, &staged);
        const char head[] =
            "POST /data HTTP/1.1\r\nTransfer-Encoding: chunked\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
        std::thread worker(
            [&] { LH_ATTRIBUTE_UNUSED(lh_ui_web_host_handle_request_is_upgraded(&app, &server)); });
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
        lh_ui_web_route_t match = {};
        lh_ui_web_route_set_method(&match, "POST");
        lh_ui_web_route_set_method_len(&match, 4U);
        lh_ui_web_route_set_path(&match, "/data");
        lh_ui_web_route_set_path_len(&match, 5U);
        lh_ui_web_stream_route_t route;
        lh_ui_web_stream_route_init(
            &route, &match,
            scenario >= 6 ? ui_web_host_test_stream_stage_one : ui_web_host_test_stream_body,
            ui_web_host_test_stream_mark_commit, ui_web_host_test_stream_mark_abort,
            scenario == 3 || scenario == 10 ? 100U : 16384U, scenario == 9 ? 17U : 1024U, 5000U);
        lh_ui_web_stream_route_set_wire_limit(
            &route, scenario == 4 || scenario == 8 ? sizeof(ui_web_host_gzip_sample) - 1U : 1000U);
        lh_ui_web_app_t app;
        lh_ui_web_app_init(&app, nullptr, 0U);
        lh_ui_web_app_set_stream_routes(&app, &route);
        lh_ui_web_app_set_nstream_routes(&app, 1U);
        std::string staged;
        lh_ui_web_app_set_ctx(&app, &staged);
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
            [&] { LH_ATTRIBUTE_UNUSED(lh_ui_web_host_handle_request_is_upgraded(&app, &server)); });
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
        lh_ui_web_route_t match = {};
        lh_ui_web_route_set_method(&match, "POST");
        lh_ui_web_route_set_method_len(&match, 4U);
        lh_ui_web_route_set_path(&match, "/data");
        lh_ui_web_route_set_path_len(&match, 5U);
        lh_ui_web_stream_route_t route;
        lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_stage_one,
                                    ui_web_host_test_stream_mark_commit,
                                    ui_web_host_test_stream_mark_abort, 32768U, 36U, 5000U);
        lh_ui_web_stream_route_set_wire_limit(&route, 2U * sizeof(ui_web_host_gzip_sample));
        lh_ui_web_app_t app;
        lh_ui_web_app_init(&app, nullptr, 0U);
        lh_ui_web_app_set_stream_routes(&app, &route);
        lh_ui_web_app_set_nstream_routes(&app, 1U);
        std::string staged;
        lh_ui_web_app_set_ctx(&app, &staged);
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
            [&] { LH_ATTRIBUTE_UNUSED(lh_ui_web_host_handle_request_is_upgraded(&app, &server)); });
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
    lh_ui_web_route_t match = {};
    lh_ui_web_route_set_method(&match, "POST");
    lh_ui_web_route_set_method_len(&match, 4U);
    lh_ui_web_route_set_path(&match, "/data");
    lh_ui_web_route_set_path_len(&match, 5U);
    lh_ui_web_stream_route_t route;
    lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                ui_web_host_test_stream_mark_commit,
                                ui_web_host_test_stream_mark_abort, 0U, 18U, 1000U);
    EXPECT_EQ(lh_ui_web_stream_route_get_wire_limit(&route), LH_USIZE_T_MAX);
    lh_ui_web_stream_route_set_wire_limit(&route, 20U);
    EXPECT_EQ(lh_ui_web_stream_route_get_wire_limit(&route), 20U);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0U);
    lh_ui_web_app_set_stream_routes(&app, &route);
    lh_ui_web_app_set_nstream_routes(&app, 1U);
    std::string staged;
    lh_ui_web_app_set_ctx(&app, &staged);
    const lh_byte_t body[] = {31, 139, 8, 0, 0, 0, 0, 0, 2, 255, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const std::string head =
        "POST /data HTTP/1.1\r\nContent-Length: 20\r\nContent-Encoding: gzip\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, body, sizeof(body)));
    EXPECT_FALSE(lh_ui_web_host_handle_request_is_upgraded(&app, &server));
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
        lh_ui_web_route_t match = {};
        lh_ui_web_route_set_method(&match, "POST");
        lh_ui_web_route_set_method_len(&match, 4U);
        lh_ui_web_route_set_path(&match, "/data");
        lh_ui_web_route_set_path_len(&match, 5U);
        lh_ui_web_route_set_handler(&match, ui_web_host_test_capture_body);
        lh_ui_web_stream_route_t route;
        lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                    ui_web_host_test_stream_mark_commit,
                                    ui_web_host_test_stream_mark_abort, 100U, 100U, 1000U);
        lh_ui_web_app_t app;
        lh_ui_web_app_init(&app, scenario == 2 ? &match : nullptr, scenario == 2 ? 1U : 0U);
        if (scenario != 2)
        {
            lh_ui_web_app_set_stream_routes(&app, &route);
            lh_ui_web_app_set_nstream_routes(&app, 1U);
        }
        std::string staged;
        lh_ui_web_app_set_ctx(&app, &staged);
        const std::string head =
            scenario == 1 ? "POST /data HTTP/1.1\r\nTransfer-Encoding: gzip, "
                            "chunked\r\nContent-Encoding: gzip\r\nExpect: 100-continue\r\n\r\n"
                          : "POST /data HTTP/1.1\r\nContent-Length: 10\r\nContent-Encoding: "
                            "br\r\nExpect: 100-continue\r\n\r\n";
        ASSERT_TRUE(lh_os_net_socket_send_all(&client, head.data(), head.size()));
        EXPECT_FALSE(lh_ui_web_host_handle_request_is_upgraded(&app, &server));
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
    lh_ui_web_route_t match = {};
    lh_ui_web_route_set_method(&match, "POST");
    lh_ui_web_route_set_method_len(&match, 4U);
    lh_ui_web_route_set_path(&match, "/data");
    lh_ui_web_route_set_path_len(&match, 5U);
    lh_ui_web_stream_route_t route;
    lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                ui_web_host_test_stream_mark_commit,
                                ui_web_host_test_stream_mark_abort, 2048U, 2048U, 10000U);
    lh_ui_web_stream_route_set_wire_limit(&route, sizeof(ui_web_host_gzip_binary));
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0U);
    lh_ui_web_app_set_stream_routes(&app, &route);
    lh_ui_web_app_set_nstream_routes(&app, 1U);
    std::string captured;
    lh_ui_web_app_set_ctx(&app, &captured);
    const char head[] =
        "POST /data HTTP/1.1\r\nTransfer-Encoding: gzip, chunked\r\nExpect: 100-continue\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    std::thread worker(
        [&] { LH_ATTRIBUTE_UNUSED(lh_ui_web_host_handle_request_is_upgraded(&app, &server)); });
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
    lh_ui_web_route_t match = {};
    lh_ui_web_route_set_method(&match, "POST");
    lh_ui_web_route_set_method_len(&match, 4U);
    lh_ui_web_route_set_path(&match, "/data");
    lh_ui_web_route_set_path_len(&match, 5U);
    lh_ui_web_stream_route_t route;
    lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_body,
                                ui_web_host_test_stream_commit, ui_web_host_test_stream_abort, 8U,
                                1024U, 1000U);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0U);
    lh_ui_web_app_set_stream_routes(&app, &route);
    lh_ui_web_app_set_nstream_routes(&app, 1U);
    std::string captured;
    lh_ui_web_app_set_ctx(&app, &captured);
    const char head[] = "POST /data HTTP/1.1\r\nContent-Length: 9\r\nExpect: 100-continue\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&client, head, sizeof(head) - 1U));
    EXPECT_FALSE(lh_ui_web_host_handle_request_is_upgraded(&app, &server));
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
        lh_ui_web_route_t route;
        lh_ui_web_route_set_method(&route, "POST");
        lh_ui_web_route_set_method_len(&route, 4U);
        lh_ui_web_route_set_path(&route, "/data");
        lh_ui_web_route_set_path_len(&route, 5U);
        lh_ui_web_route_set_handler(&route, ui_web_host_test_capture_body);
        lh_ui_web_app_t app;
        lh_ui_web_app_init(&app, &route, 1U);
        std::string captured;
        lh_ui_web_app_set_ctx(&app, &captured);
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
        EXPECT_FALSE(lh_ui_web_host_handle_request_is_upgraded(&app, &server));
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
    EXPECT_EQ(lh_ui_web_host_get_query_value_size(lh_addr_of(request), lh_str_lit("x="),
                                                  lh_addr_of(value)),
              2u);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(std::string(value, 2), "12");
    EXPECT_EQ(lh_ui_web_host_get_query_int(lh_addr_of(request), lh_str_lit("dx=")), 7);
    EXPECT_FALSE(
        lh_ui_web_host_get_query_flag(lh_addr_of(request), lh_str_lit("flag="), lh_bool_true));
    EXPECT_TRUE(
        lh_ui_web_host_get_query_flag(lh_addr_of(request), lh_str_lit("missing="), lh_bool_true));
    const lh_str_view_t empty_value = lh_str_view_initializer_lit("/?x=");
    lh_http_request_set_target(lh_addr_of(request), empty_value);
    value = nullptr;
    EXPECT_EQ(lh_ui_web_host_get_query_value_size(lh_addr_of(request), lh_str_lit("x="),
                                                  lh_addr_of(value)),
              0u);
    EXPECT_EQ(value, lh_str_view_get_end(lh_addr_of(empty_value)));
    const lh_str_view_t no_query = lh_str_view_initializer_lit("/x=12");
    lh_http_request_set_target(lh_addr_of(request), no_query);
    value = nullptr;
    EXPECT_EQ(lh_ui_web_host_get_query_value_size(lh_addr_of(request), lh_str_lit("x="),
                                                  lh_addr_of(value)),
              0u);
    EXPECT_EQ(value, nullptr);
    EXPECT_EQ(lh_ui_web_host_get_query_value_size(lh_addr_of(request), "", 0, lh_addr_of(value)),
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
    EXPECT_TRUE(lh_ui_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
    lh_http_request_set_method(lh_addr_of(request), post);
    EXPECT_TRUE(lh_ui_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
    lh_http_request_set_method(lh_addr_of(request), put);
    EXPECT_FALSE(lh_ui_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
    lh_http_request_set_method(lh_addr_of(request), get);
    lh_http_request_set_target(lh_addr_of(request), longer);
    EXPECT_FALSE(lh_ui_web_host_is_route(lh_addr_of(request), "/ui/key", 7));
}

namespace
{

constexpr int key_log_max = 32;
constexpr lh_u64_t wait_ms = 5000u;

/* root 200x200 > box (0,0) 100x100 over content 100x400, and its vertical bar
   (100,0) 10x200: thumb 50 of a 200 track, scroll 0..300. */
struct host_world
{
    lh_ui_entity_t root;
    lh_ui_container_t box;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t down;
    lh_ui_view_t view;
    lh_ui_web_host_hooks_t hooks;
    lh_ui_web_player_t player;
    char json[256u * 1024u];
    char page[LH_UI_WEB_PLAYER_PAGE_CAP];
    lh_net_port_t port;
    std::atomic<int> started;
    std::atomic<int> failed;
    std::atomic<int> keys;
    int key[key_log_max];
    int pressed[key_log_max];
    std::atomic<int> ticks;
    std::atomic<int> scroll_y;
    lh_os_controller_t controller;
    std::atomic<int> controlled_presses;
    std::atomic<int> controlled_clicks;
    std::atomic<int> controller_updates;
};

host_world s_world;

lh_void
world_tick(lh_ptr ctx)
{
    auto *world = static_cast<host_world *>(ctx);
    lh_ui_point_t scroll;

    lh_ui_container_get_clamped_scroll(lh_addr_of(world->box), lh_addr_of(scroll));
    world->scroll_y.store(static_cast<int>(lh_ui_point_get_y(lh_addr_of(scroll))));
    world->ticks.fetch_add(1);
}

/* Every line is one event: a key is logged, and everything goes on to the view, as an
   application's own sink would pass it. */
lh_void
world_input(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    auto *world = static_cast<host_world *>(self);
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);

    if (kind == lh_os_input_kind_key_down || kind == lh_os_input_kind_key_up)
    {
        const int at = world->keys.load();

        if (at < key_log_max)
        {
            world->key[at] = static_cast<int>(lh_os_input_event_get_key(event));
            world->pressed[at] = kind == lh_os_input_kind_key_down ? 1 : 0;
        }
        world->keys.store(at + 1);
    }
    (void)lh_ui_view_input(lh_addr_of(world->view), event);
}

/* The controller's handler: presses and the recognizer's clicks, counted. */
lh_bool_t
world_control(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    auto *world = static_cast<host_world *>(self);
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);

    if (kind == lh_os_input_kind_pointer_down)
    {
        world->controlled_presses.fetch_add(1);
    }
    if (kind == lh_os_input_kind_click)
    {
        world->controlled_clicks.fetch_add(1);
    }
    return lh_bool_false;
}

lh_void
world_update(lh_self_ptr self, lh_tick_t /*now*/)
{
    static_cast<host_world *>(self)->controller_updates.fetch_add(1);
}

/* A port nothing listens on right now: bind to 0, read what was given, let it go. */
lh_net_port_t
free_port()
{
    lh_net_port_t port = 0u;

    lh_net_ip4_t ip;
    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), 0);
    lh_os_net_socket_t probe;
    lh_os_net_socket_init(lh_addr_of(probe));
    if (lh_os_net_socket_open(lh_addr_of(probe), lh_net_socket_type_tcp) &&
        lh_os_net_socket_bind(lh_addr_of(probe), lh_addr_of(addr)) &&
        lh_os_net_socket_get_local_addr(lh_addr_of(probe), lh_addr_of(addr)))
    {
        port = lh_net_ip4_socket_addr_get_port(lh_addr_of(addr));
    }
    lh_os_net_socket_close(lh_addr_of(probe));
    return port;
}

bool
connect_to_host(lh_os_net_socket_t *sock)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), s_world.port);
    lh_os_net_socket_init(sock);
    if (!lh_os_net_socket_open(sock, lh_net_socket_type_tcp))
    {
        return false;
    }
    if (!lh_os_net_socket_connect(sock, lh_addr_of(addr)))
    {
        lh_os_net_socket_close(sock);
        return false;
    }
    return true;
}

/* The host's own thread: it returns only when the host would not listen. */
void
host_thread()
{
    (void)lh_ui_web_host_run(lh_ui_web_player_get_app(lh_addr_of(s_world.player)), s_world.port);
    s_world.failed.store(1);
}

/* Start the one host of this program, once; false when it would not listen. */
bool
host_up()
{
    static bool tried = false;
    static bool up = false;

    if (tried)
    {
        return up;
    }
    tried = true;
    if (!lh_os_net_init())
    {
        return false;
    }
    lh_ui_entity_init(lh_addr_of(s_world.root), lh_test::rect_of(0, 0, 200, 200));
    lh_ui_container_init(lh_addr_of(s_world.box), lh_test::rect_of(0, 0, 100, 100));
    lh_ui_entity_init(lh_addr_of(s_world.content), lh_test::rect_of(0, 0, 100, 400));
    lh_ui_scrollbar_init(lh_addr_of(s_world.down), lh_test::rect_of(100, 0, 10, 200),
                         lh_ui_axis_vertical, lh_addr_of(s_world.box));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(s_world.box)),
                           lh_addr_of(s_world.content));
    lh_ui_entity_add_child(lh_addr_of(s_world.root),
                           lh_ui_container_as_entity(lh_addr_of(s_world.box)));
    lh_ui_entity_add_child(lh_addr_of(s_world.root),
                           lh_ui_scrollbar_as_entity(lh_addr_of(s_world.down)));
    lh_ui_view_init(lh_addr_of(s_world.view));
    lh_ui_view_set_root_v(lh_addr_of(s_world.view), lh_addr_of(s_world.root));
    std::memset(lh_addr_of(s_world.hooks), 0, sizeof(s_world.hooks));
    lh_ui_web_host_hooks_set_ctx(lh_addr_of(s_world.hooks), lh_addr_of(s_world));
    lh_ui_web_host_hooks_set_tick_fn(lh_addr_of(s_world.hooks), world_tick);
    lh_ui_web_host_hooks_set_input_fn(lh_addr_of(s_world.hooks), world_input);
    lh_os_controller_init(lh_addr_of(s_world.controller), world_control, lh_addr_of(s_world),
                          lh_null);
    lh_os_controller_set_update_cb(lh_addr_of(s_world.controller), world_update);
    lh_ui_web_host_hooks_set_controller(lh_addr_of(s_world.hooks), lh_addr_of(s_world.controller));
    if (!lh_ui_web_player_init(lh_addr_of(s_world.player), lh_addr_of(s_world.view), 200u, 200u,
                               s_world.json, sizeof(s_world.json),
                               lh_ptr_rcast(lh_byte_t, s_world.page), sizeof(s_world.page),
                               lh_addr_of(s_world.hooks)))
    {
        return false;
    }
    s_world.port = free_port();
    if (s_world.port == 0u)
    {
        return false;
    }
    /* Never joined: the host returns only when it could not listen, and the process
       ending is what stops it otherwise. */
    std::thread(host_thread).detach();
    /* Up once it answers a connection and has run a pass. */
    const lh_u64_t t0 = lh_os_tick_ms();
    while (lh_os_tick_ms() - t0 < wait_ms && s_world.failed.load() == 0)
    {
        lh_os_net_socket_t probe;

        if (s_world.ticks.load() > 0 && connect_to_host(lh_addr_of(probe)))
        {
            lh_os_net_socket_close(lh_addr_of(probe));
            up = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return up;
}

#    define LH_TEST_NEEDS_HOST()                                                                   \
        if (!host_up())                                                                            \
        GTEST_SKIP() << "the web host would not listen on loopback"

bool
send_all(lh_os_net_socket_t *sock, const void *bytes, lh_usize_t len)
{
    const char *p = static_cast<const char *>(bytes);

    while (len > 0u)
    {
        const lh_ssize_t n = lh_os_net_socket_send(sock, const_cast<char *>(p), len);

        if (n <= 0)
        {
            return false;
        }
        p += n;
        len -= static_cast<lh_usize_t>(n);
    }
    return true;
}

/* Everything up to the blank line that ends an HTTP head, or up to the peer closing. */
std::string
read_head(lh_os_net_socket_t *sock)
{
    std::string head;
    char byte;

    while (head.find("\r\n\r\n") == std::string::npos &&
           lh_os_net_socket_recv(sock, lh_addr_of(byte), 1u) == 1)
    {
        head += byte;
    }
    return head;
}

/* One HTTP fallback request; its status line. The host answers and closes. */
std::string
http_get(const std::string &path)
{
    const std::string req = "GET " + path + " HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
    std::string head;

    lh_os_net_socket_t sock;
    if (!connect_to_host(lh_addr_of(sock)))
    {
        return head;
    }
    if (send_all(lh_addr_of(sock), req.data(), req.size()))
    {
        head = read_head(lh_addr_of(sock));
    }
    lh_os_net_socket_close(lh_addr_of(sock));
    return head.substr(0, head.find("\r\n"));
}

/* A WebSocket the way the player opens one. Frames the host pushes back are not read. */
struct ws_client
{
    lh_os_net_socket_t sock;
    bool open;

    ws_client() : open(false)
    {
        static const char upgrade[] = "GET /ws HTTP/1.1\r\nHost: 127.0.0.1\r\n"
                                      "Upgrade: websocket\r\nConnection: Upgrade\r\n"
                                      "sEc-WeBsOcKeT-kEy: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                                      "Sec-WebSocket-Version: 13\r\n\r\n";

        if (!connect_to_host(lh_addr_of(sock)))
        {
            return;
        }
        open = send_all(lh_addr_of(sock), upgrade, sizeof(upgrade) - 1u) &&
               read_head(lh_addr_of(sock)).find(" 101 ") != std::string::npos;
    }

    ~ws_client()
    {
        lh_os_net_socket_close(lh_addr_of(sock));
    }

    bool
    line(const char *text)
    {
        unsigned char frame[64];
        const lh_usize_t n = lh_ui_web_ws_write(frame, sizeof(frame), LH_UI_WEB_WS_TEXT,
                                                lh_bool_true, text, std::strlen(text));

        return n != 0u && send_all(lh_addr_of(sock), frame, n);
    }
};

/* Wait until @p done holds or the time is up; whether it held. */
template <typename F>
bool
wait_for(F done)
{
    const lh_u64_t t0 = lh_os_tick_ms();

    while (!done())
    {
        if (lh_os_tick_ms() - t0 >= wait_ms)
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return true;
}

/* Wait for @p count more keys than @p base; whether they came. */
bool
wait_keys(int base, int count)
{
    return wait_for([&] { return s_world.keys.load() >= base + count; });
}

/* Wait for two passes of the host past now, so its tick has seen every input before. */
void
wait_passes()
{
    const int t = s_world.ticks.load();

    (void)wait_for([&] { return s_world.ticks.load() >= t + 2; });
}

} // namespace

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
            ok = lh_ui_web_host_try_read_request(&server, buffer, sizeof(buffer), 1000U, &length,
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
    EXPECT_FALSE(lh_ui_web_host_try_read_request(&server, buffer, sizeof(buffer), 1000U, &length,
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
    EXPECT_FALSE(lh_ui_web_host_try_send_continue(&socket));
    LH_EXPECT_CHECK_DEATH(lh_ui_web_host_send_continue(&socket));
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
    ASSERT_TRUE(lh_ui_web_host_try_send_response(&writer, &response, headers, scratch,
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
    EXPECT_FALSE(lh_ui_web_host_try_send_response(&socket, &response, nullptr, scratch,
                                                  sizeof(scratch), nullptr, 0U));
    LH_EXPECT_CHECK_DEATH(lh_ui_web_host_send_response(&socket, &response, nullptr, scratch,
                                                       sizeof(scratch), nullptr, 0U));
}

TEST(ui_web_host_reply, reports_and_checks_invalid_status)
{
    lh_http_request_t request;
    lh_http_request_set_method(&request, lh_http_method_get_name(lh_http_method_get));
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(lh_ui_web_host_try_reply(&socket, &request, 99U, "text/plain", "x", 1U));
    LH_EXPECT_CHECK_DEATH(lh_ui_web_host_reply(&socket, &request, 99U, "text/plain", "x", 1U));
}

TEST(ui_web_host_try_reply, returns_false_when_the_client_is_disconnected)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_http_request_t request;
    lh_http_request_set_method(&request, lh_http_method_get_name(lh_http_method_get));
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(
        lh_ui_web_host_try_reply(&socket, &request, lh_http_status_ok, "text/plain", "x", 1U));
}

TEST(ui_web_host_reply_text, reports_and_checks_invalid_status)
{
    lh_http_request_t request;
    lh_http_request_set_method(&request, lh_http_method_get_name(lh_http_method_get));
    lh_os_net_socket_t socket;
    lh_os_net_socket_init(&socket);
    EXPECT_FALSE(lh_ui_web_host_try_reply_text(&socket, &request, 99U, "x", 1U));
    LH_EXPECT_CHECK_DEATH(lh_ui_web_host_reply_text(&socket, &request, 99U, "x", 1U));
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
    lh_ui_web_host_reply(lh_addr_of(writer), lh_addr_of(request), lh_http_status_ok, "text/plain",
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
        lh_ui_web_host_reply(lh_addr_of(writer), lh_addr_of(request), lh_http_status_ok,
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
        lh_ui_web_host_reply(lh_addr_of(writer), lh_addr_of(request), status, "text/plain", "body",
                             4);
        const std::string head = read_head(lh_addr_of(reader));
        EXPECT_EQ(head.find("Content-Length:"), std::string::npos) << status;
        EXPECT_EQ(lh_os_net_socket_pending(lh_addr_of(reader)), 0) << status;
        lh_os_net_socket_close(lh_addr_of(reader));
        lh_os_net_socket_close(lh_addr_of(writer));
    }
}

TEST(web_host, serializes_http_and_websocket_response_headers)
{
    LH_TEST_NEEDS_HOST();
    lh_os_net_socket_t http;
    ASSERT_TRUE(connect_to_host(lh_addr_of(http)));
    const char get[] = "GET /ui.rev HTTP/1.1\r\nHost: localhost\r\n\r\n";
    ASSERT_TRUE(send_all(lh_addr_of(http), get, sizeof(get) - 1));
    const std::string reply = read_head(lh_addr_of(http));
    lh_os_net_socket_close(lh_addr_of(http));
    EXPECT_NE(reply.find("HTTP/1.1 200 OK\r\n"), std::string::npos);
    EXPECT_NE(reply.find("Content-Type: text/plain; charset=utf-8\r\n"), std::string::npos);
    EXPECT_NE(reply.find("Content-Length: "), std::string::npos);

    lh_os_net_socket_t websocket;
    ASSERT_TRUE(connect_to_host(lh_addr_of(websocket)));
    const char upgrade[] = "GET /ws HTTP/1.1\r\nHost: localhost\r\n"
                           "Upgrade: websocket\r\nConnection: Upgrade\r\n"
                           "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                           "Sec-WebSocket-Version: 13\r\n\r\n";
    ASSERT_TRUE(send_all(lh_addr_of(websocket), upgrade, sizeof(upgrade) - 1));
    const std::string handshake = read_head(lh_addr_of(websocket));
    EXPECT_NE(handshake.find("HTTP/1.1 101 Switching Protocols\r\n"), std::string::npos);
    EXPECT_NE(handshake.find("Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n"),
              std::string::npos);
    lh_os_net_socket_close(lh_addr_of(websocket));
}

TEST(web_host, receives_a_request_head_in_separate_fragments)
{
    LH_TEST_NEEDS_HOST();
    lh_os_net_socket_t sock;
    ASSERT_TRUE(connect_to_host(&sock));
    const char first[] = "GET /ui.json HTTP/1.1\r\nHost: ";
    const char last[] = "127.0.0.1\r\n\r\n";

    ASSERT_TRUE(send_all(&sock, first, sizeof(first) - 1));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_TRUE(send_all(&sock, last, sizeof(last) - 1));
    EXPECT_NE(read_head(&sock).find(" 200 "), std::string::npos);
    lh_os_net_socket_close(&sock);
}

TEST(web_host, ws_key_lines_carry_the_direction_and_the_old_line_is_a_key_down)
{
    LH_TEST_NEEDS_HOST();
    ws_client ws;
    ASSERT_TRUE(ws.open);
    const int base = s_world.keys.load();

    ASSERT_TRUE(ws.line("k 6 1"));
    ASSERT_TRUE(ws.line("k 6 0"));
    ASSERT_TRUE(ws.line("k 3"));
    ASSERT_TRUE(wait_keys(base, 3));
    ASSERT_LE(base + 3, key_log_max);
    EXPECT_EQ(s_world.key[base], static_cast<int>(lh_os_key_space));
    EXPECT_EQ(s_world.pressed[base], 1);
    EXPECT_EQ(s_world.key[base + 1], static_cast<int>(lh_os_key_space));
    EXPECT_EQ(s_world.pressed[base + 1], 0);
    EXPECT_EQ(s_world.key[base + 2], static_cast<int>(lh_os_key_escape));
    EXPECT_EQ(s_world.pressed[base + 2], 1);
}

TEST(web_host, http_key_takes_p_and_without_it_is_a_key_down)
{
    LH_TEST_NEEDS_HOST();
    const int base = s_world.keys.load();

    EXPECT_NE(http_get("/ui/key?k=5&p=0").find(" 200 "), std::string::npos);
    ASSERT_TRUE(wait_keys(base, 1));
    EXPECT_NE(http_get("/ui/key?k=5").find(" 200 "), std::string::npos);
    ASSERT_TRUE(wait_keys(base, 2));
    ASSERT_LE(base + 2, key_log_max);
    EXPECT_EQ(s_world.key[base], static_cast<int>(lh_os_key_delete));
    EXPECT_EQ(s_world.pressed[base], 0);
    EXPECT_EQ(s_world.key[base + 1], static_cast<int>(lh_os_key_delete));
    EXPECT_EQ(s_world.pressed[base + 1], 1);
}

TEST(web_host, query_parameters_are_not_read_from_headers)
{
    LH_TEST_NEEDS_HOST();
    const int base = s_world.keys.load();
    lh_os_net_socket_t socket;
    ASSERT_TRUE(connect_to_host(lh_addr_of(socket)));
    const char request[] = "GET /ui/key HTTP/1.1\r\nHost: localhost\r\nX-Noise: ?k=5\r\n\r\n";
    ASSERT_TRUE(send_all(lh_addr_of(socket), request, sizeof(request) - 1));
    EXPECT_NE(read_head(lh_addr_of(socket)).find(" 200 "), std::string::npos);
    lh_os_net_socket_close(lh_addr_of(socket));
    ASSERT_TRUE(wait_keys(base, 1));
    ASSERT_LT(base, key_log_max);
    EXPECT_EQ(s_world.key[base], static_cast<int>(lh_os_key_other));
}

/* Thumb 50 of a 200 track over scroll 0..300: dragging its top from 0 to 75 is scroll 150,
   and back to 0 is scroll 0 -- once as WebSocket lines, once as HTTP requests. */
TEST(web_host, a_move_while_held_drags_the_thumb_and_one_not_held_does_nothing)
{
    LH_TEST_NEEDS_HOST();
    ws_client ws;
    ASSERT_TRUE(ws.open);
    const int base = s_world.keys.load();

    wait_passes();
    ASSERT_EQ(s_world.scroll_y.load(), 0);
    ASSERT_TRUE(ws.line("p 105 10"));
    ASSERT_TRUE(ws.line("m 105 85"));
    ASSERT_TRUE(ws.line("r 105 85"));
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == 150; }));

    /* Nothing held: a move is no drag. The key after it is the mark that it was read. */
    ASSERT_TRUE(ws.line("m 105 10"));
    ASSERT_TRUE(ws.line("k 0 1"));
    ASSERT_TRUE(wait_keys(base, 1));
    wait_passes();
    EXPECT_EQ(s_world.scroll_y.load(), 150);

    EXPECT_NE(http_get("/ui/press?x=105&y=85").find(" 200 "), std::string::npos);
    EXPECT_NE(http_get("/ui/move?x=105&y=10").find(" 200 "), std::string::npos);
    EXPECT_NE(http_get("/ui/release?x=105&y=10").find(" 200 "), std::string::npos);
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == 0; }));
}

/* A `w` line is a notch, positive up: one down scrolls the view one step, one up brings it
   back -- the same direction a desktop wheel scrolls. The step is the host machine's
   (lh_os_input_get_wheel_step): the view asks the OS layer, not the browser. */
TEST(web_host, a_wheel_line_scrolls_one_step_per_notch)
{
    LH_TEST_NEEDS_HOST();
    ws_client ws;
    ASSERT_TRUE(ws.open);

    wait_passes();
    ASSERT_EQ(s_world.scroll_y.load(), 0);
    ASSERT_TRUE(ws.line("w 50 50 0 -1"));
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == lh_os_input_get_wheel_step(); }));
    ASSERT_TRUE(ws.line("w 50 50 0 1"));
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == 0; }));
}

/* The hooks' controller takes every event after `input` and is updated at every pass:
   a click over HTTP is a press to it, and the recognizer's click after the release. */
TEST(web_host, the_controller_gets_the_events_after_input_and_is_updated_every_pass)
{
    LH_TEST_NEEDS_HOST();
    const int presses = s_world.controlled_presses.load();
    const int clicks = s_world.controlled_clicks.load();
    const int updates = s_world.controller_updates.load();

    EXPECT_NE(http_get("/ui/click?x=150&y=150").find(" 200 "), std::string::npos);
    EXPECT_TRUE(wait_for([&] { return s_world.controlled_clicks.load() == clicks + 1; }));
    EXPECT_EQ(s_world.controlled_presses.load(), presses + 1);
    wait_passes();
    EXPECT_GE(s_world.controller_updates.load(), updates + 2);
}

TEST(ui_web_host_decode_gzip_in_place, reports_and_checks_invalid_input)
{
    char data[1] = {0};
    lh_usize_t result = 17U;
    EXPECT_FALSE(lh_ui_web_host_try_decode_gzip_in_place(data, 1U, 0U, &result));
    EXPECT_EQ(result, 17U);
    LH_EXPECT_CHECK_DEATH(lh_ui_web_host_decode_gzip_in_place(data, 1U, 0U, &result));
}

lh_ssize_t
ui_web_host_test_error_reader(lh_self_ptr, lh_ptr, lh_usize_t)
{
    return LH_IO_ERROR;
}

TEST(ui_web_host_gzip_workspace, provides_distinct_owned_states_and_buffers)
{
    lh_ui_web_host_gzip_workspace_t workspace;
    EXPECT_NE(lh_ui_web_host_gzip_workspace_get_scratch(&workspace), nullptr);
    EXPECT_NE(lh_ui_web_host_gzip_workspace_get_history(&workspace), nullptr);
    EXPECT_NE(lh_ui_web_host_gzip_workspace_get_scratch(&workspace),
              lh_ui_web_host_gzip_workspace_get_history(&workspace));
    EXPECT_NE(lh_ui_web_host_gzip_workspace_get_header(&workspace), nullptr);
    EXPECT_NE(lh_ui_web_host_gzip_workspace_get_deflate(&workspace), nullptr);
    EXPECT_NE(lh_ui_web_host_gzip_workspace_get_reader(&workspace), nullptr);
}

TEST(ui_web_host_dispatch_stream, aborts_and_checked_form_reports_read_failure)
{
    lh_ui_web_route_t match = {};
    lh_ui_web_stream_route_t route;
    lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_stage_one,
                                ui_web_host_test_stream_mark_commit,
                                ui_web_host_test_stream_mark_abort, 100U, 100U, 1000U);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0U);
    std::string staged;
    lh_ui_web_app_set_ctx(&app, &staged);
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, ui_web_host_test_error_reader, nullptr);
    lh_http_request_t request = {};
    lh_http_header_t headers[1] = {};
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    EXPECT_FALSE(
        lh_ui_web_host_try_dispatch_stream(&app, &route, &request, headers, &reader, &client));
    EXPECT_EQ(staged, "ABORT");
    LH_EXPECT_CHECK_DEATH(
        lh_ui_web_host_dispatch_stream(&app, &route, &request, headers, &reader, &client));
}

TEST(ui_web_host_dispatch_gzip, aborts_and_checked_form_reports_invalid_source)
{
    lh_ui_web_route_t match = {};
    lh_ui_web_stream_route_t route;
    lh_ui_web_stream_route_init(&route, &match, ui_web_host_test_stream_stage_one,
                                ui_web_host_test_stream_mark_commit,
                                ui_web_host_test_stream_mark_abort, 100U, 100U, 1000U);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(&app, nullptr, 0U);
    std::string staged;
    lh_ui_web_app_set_ctx(&app, &staged);
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, ui_web_host_test_error_reader, nullptr);
    lh_http_request_t request = {};
    lh_http_header_t headers[1] = {};
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&client);
    EXPECT_FALSE(
        lh_ui_web_host_try_dispatch_gzip(&app, &route, &request, headers, &reader, &client));
    EXPECT_EQ(staged, "ABORT");
    LH_EXPECT_CHECK_DEATH(
        lh_ui_web_host_dispatch_gzip(&app, &route, &request, headers, &reader, &client));
}

#endif /* LH_LIBRARY_OPTION_OS */
