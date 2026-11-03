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

#    include <lh/attribute/unused.h>
#    include <lh/expect/death.h>
#    include <lh/http/method.h>
#    include <lh/http/status.h>
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
    char buffer;
    lh_usize_t length = 17;
    lh_http_request_t request;
    lh_str_view_t request_body = lh_str_view_initializer_lit("unchanged");

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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
    lh_os_net_socket_t reader;
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(lh_addr_of(reader));
    lh_os_net_socket_init(lh_addr_of(writer));
    lh_os_net_socket_set_handle(lh_addr_of(reader), handles[0]);
    lh_os_net_socket_set_handle(lh_addr_of(writer), handles[1]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(lh_addr_of(reader), lh_bool_true));
    const char text[] = "GET /ui HTTP/1.1\r\nX-Example: value\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(lh_addr_of(writer), text, sizeof(text) - 1));
    char buffer[128];
    lh_usize_t length = 0;
    lh_http_request_t request;
    lh_str_view_t request_body;
    lh_http_header_t headers[2];
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
    lh_os_net_socket_t reader;
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(lh_addr_of(reader));
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
    char buffer[128];
    lh_usize_t length = 0;
    lh_http_request_t request;
    lh_str_view_t request_body;
    lh_http_header_t headers[2];
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    char buffer[256];
    lh_usize_t length = 0U;
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[4];
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    char buffer[128];
    lh_usize_t length = 17U;
    lh_str_view_t body = lh_str_view_lit("unchanged");
    lh_http_request_t request;
    lh_http_header_t headers[4];
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
    lh_os_net_socket_t reader;
    lh_os_net_socket_init(&reader);
    lh_os_net_socket_set_handle(&reader, handles[0]);
    ASSERT_TRUE(lh_os_net_socket_set_blocking(&reader, lh_bool_true));
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(&writer);
    lh_os_net_socket_set_handle(&writer, handles[1]);
    const char input[] = "POST / HTTP/1.0\r\nTransfer-Encoding: chunked\r\n\r\n0\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&writer, input, sizeof(input) - 1U));
    char buffer[128];
    lh_usize_t length = 17U;
    lh_str_view_t body = lh_str_view_lit("unchanged");
    lh_http_request_t request;
    lh_http_header_t headers[2];
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
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
    lh_os_net_socket_t server;
    lh_os_net_socket_t client;
    lh_os_net_socket_init(lh_addr_of(server));
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
        if (size == 0)
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
    lh_os_net_socket_t server;
    lh_os_net_socket_t client;
    lh_os_net_socket_init(&server);
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
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    if (lh_io_reader_read(body, &byte, 1U) != 1)
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
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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

TEST(ui_web_host_handle_request_is_upgraded, rejects_oversized_stream_before_continue)
{
    ASSERT_TRUE(lh_os_net_init());
    lh_os_system_net_socket_handle_t handles[2];
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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

TEST(ui_web_host_query, searches_only_the_parsed_query_and_exact_keys)
{
    lh_http_request_t request;
    const lh_str_view_t target = lh_str_view_initializer_lit("/ui?dx=7&x=12&x=99&flag=0");
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
    lh_http_request_t request;
    const lh_str_view_t get = lh_str_view_initializer_lit("GET");
    const lh_str_view_t post = lh_str_view_initializer_lit("POST");
    const lh_str_view_t put = lh_str_view_initializer_lit("PUT");
    const lh_str_view_t path = lh_str_view_initializer_lit("/ui/key?x=1");
    const lh_str_view_t longer = lh_str_view_initializer_lit("/ui/keyboard");

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
    lh_os_net_socket_t probe;
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;
    lh_net_port_t port = 0u;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), 0);
    lh_os_net_socket_init(lh_addr_of(probe));
    if (lh_os_net_socket_open(lh_addr_of(probe), lh_os_system_net_socket_type_tcp) &&
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
    lh_net_ip4_socket_addr_t addr;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), s_world.port);
    lh_os_net_socket_init(sock);
    if (!lh_os_net_socket_open(sock, lh_os_system_net_socket_type_tcp))
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
    lh_os_net_socket_t sock;
    const std::string req = "GET " + path + " HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
    std::string head;

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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    char buffer[256];
    lh_usize_t length = 0U;
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[4];
    bool ok = false;
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    char buffer[256];
    lh_usize_t length = 0U;
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[4];
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
    lh_os_net_socket_t reader;
    lh_os_net_socket_t writer;
    lh_os_net_socket_init(lh_addr_of(reader));
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
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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
    lh_http_request_t request;
    const lh_str_view_t method = lh_str_view_initializer_lit("GET");
    lh_http_request_set_method(lh_addr_of(request), method);
    for (lh_uint_t status : statuses)
    {
        lh_os_system_net_socket_handle_t handles[2];
        ASSERT_TRUE(lh_os_system_net_socket_pair(handles, 1000));
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

#endif /* LH_LIBRARY_OPTION_OS */
