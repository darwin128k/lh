/**
 * @file app.cpp
 * @brief The application's setters: each stores its one field and leaves the
 *        others alone, and init starts every field from nothing.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/str/lit.h>
#    include <lh/ui/web/app.h>
#    include <lh/http/parser.h>
#    include <lh/attribute/unused.h>
#    include <string>

lh_bool_t
ui_web_test_handle(lh_ptr ctx, const lh_http_request_t *request, const lh_http_header_t *headers,
                   const lh_str_view_t *body, lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(client);
    bool *called = static_cast<bool *>(ctx);
    *called = true;
    const lh_str_view_t method = lh_str_view_initializer_lit("GET");
    const lh_str_view_t name = lh_str_view_initializer_lit("x-test");
    EXPECT_TRUE(
        lh_str_view_equals(lh_http_request_get_method(request), lh_addr_of(method), lh_bool_false));
    EXPECT_NE(
        lh_http_header_find(headers, lh_http_request_get_header_count(request), lh_addr_of(name)),
        nullptr);
    EXPECT_EQ(std::string(lh_str_view_get_data(body), lh_str_view_get_size(body)),
              std::string("a\0b", 3));
    return lh_bool_true;
}

TEST(ui_web_app, handler_receives_parsed_request_and_headers)
{
    const char text[] = "GET /state HTTP/1.1\r\nX-Test: yes\r\n\r\n";
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_header_t headers[2];
    lh_http_parser_init(lh_addr_of(parser));
    const lh_str_view_t input = lh_str_view_initializer(text, sizeof(text) - 1);
    ASSERT_EQ(lh_http_parser_request(lh_addr_of(parser), input, lh_addr_of(request), headers, 2),
              lh_http_parse_complete);
    lh_ui_web_route_t route;
    lh_ui_web_route_set_method(lh_addr_of(route), "GET");
    lh_ui_web_route_set_method_len(lh_addr_of(route), 3);
    lh_ui_web_route_set_path(lh_addr_of(route), "/state");
    lh_ui_web_route_set_path_len(lh_addr_of(route), 6);
    lh_ui_web_route_set_handler(lh_addr_of(route), ui_web_test_handle);
    lh_ui_web_app_t app;
    lh_ui_web_app_init(lh_addr_of(app), lh_addr_of(route), 1);
    bool called = false;
    lh_ui_web_app_set_ctx(lh_addr_of(app), lh_addr_of(called));
    const lh_str_view_t body = lh_str_view_initializer("a\0b", 3);
    EXPECT_TRUE(lh_ui_web_app_handle_request_is_handled(lh_addr_of(app), lh_addr_of(request),
                                                        headers, lh_addr_of(body), nullptr));
    EXPECT_TRUE(called);
}

TEST(ui_web_app, init_starts_with_the_routes_and_nothing_else)
{
    lh_ui_web_route_t routes[2] = {};
    lh_ui_web_app_t app;

    lh_ui_web_app_init(lh_addr_of(app), routes, 2u);

    EXPECT_EQ(lh_ui_web_app_get_routes(&app), routes);
    EXPECT_EQ(lh_ui_web_app_get_nroutes(&app), 2u);
    EXPECT_EQ(lh_ui_web_app_get_ws_path(&app), lh_null);
    EXPECT_EQ(lh_ui_web_app_get_ws_path_len(&app), 0u);
    EXPECT_EQ(lh_ui_web_app_get_frame_fn(&app), lh_null);
    EXPECT_EQ(lh_ui_web_app_get_message_fn(&app), lh_null);
    EXPECT_EQ(lh_ui_web_app_get_tick_fn(&app), lh_null);
    EXPECT_EQ(lh_ui_web_app_get_ctx(&app), lh_null);
    EXPECT_EQ(lh_ui_web_app_get_generation(&app), 0u);
}

TEST(ui_web_app, set_routes_and_set_nroutes_store_one_field_each)
{
    lh_ui_web_route_t first[2] = {};
    lh_ui_web_route_t second[1] = {};
    lh_ui_web_app_t app;

    lh_ui_web_app_init(lh_addr_of(app), first, 2u);
    lh_ui_web_app_set_routes(lh_addr_of(app), second);

    EXPECT_EQ(lh_ui_web_app_get_routes(&app), second);
    EXPECT_EQ(lh_ui_web_app_get_nroutes(&app), 2u);

    lh_ui_web_app_set_nroutes(lh_addr_of(app), 1u);

    EXPECT_EQ(lh_ui_web_app_get_routes(&app), second);
    EXPECT_EQ(lh_ui_web_app_get_nroutes(&app), 1u);
}

TEST(ui_web_app, set_ws_path_and_set_ws_path_len_store_one_field_each)
{
    const lh_str_ptr path = "/ws";
    lh_ui_web_app_t app;

    lh_ui_web_app_init(lh_addr_of(app), lh_null, 0u);
    lh_ui_web_app_set_ws_path(lh_addr_of(app), path);

    EXPECT_EQ(lh_ui_web_app_get_ws_path(&app), path);
    EXPECT_EQ(lh_ui_web_app_get_ws_path_len(&app), 0u);

    lh_ui_web_app_set_ws_path_len(lh_addr_of(app), lh_str_lit_size("/ws"));

    EXPECT_EQ(lh_ui_web_app_get_ws_path(&app), path);
    EXPECT_EQ(lh_ui_web_app_get_ws_path_len(&app), 3u);
}

TEST(ui_web_app, is_websocket_matches_the_parsed_target_path)
{
    lh_ui_web_app_t app;
    lh_http_request_t request;
    const lh_str_view_t path = lh_str_view_initializer_lit("/ws");
    const lh_str_view_t query = lh_str_view_initializer_lit("/ws?x=1");
    const lh_str_view_t longer = lh_str_view_initializer_lit("/wsx");

    lh_ui_web_app_init(lh_addr_of(app), lh_null, 0u);
    lh_ui_web_app_set_ws_path(lh_addr_of(app), "/ws");
    lh_http_request_set_target(lh_addr_of(request), path);
    EXPECT_FALSE(lh_ui_web_app_is_websocket(lh_addr_of(app), lh_addr_of(request)));
    lh_ui_web_app_set_ws_path_len(lh_addr_of(app), lh_str_lit_size("/ws"));
    EXPECT_TRUE(lh_ui_web_app_is_websocket(lh_addr_of(app), lh_addr_of(request)));
    lh_http_request_set_target(lh_addr_of(request), query);
    EXPECT_TRUE(lh_ui_web_app_is_websocket(lh_addr_of(app), lh_addr_of(request)));
    lh_http_request_set_target(lh_addr_of(request), longer);
    EXPECT_FALSE(lh_ui_web_app_is_websocket(lh_addr_of(app), lh_addr_of(request)));
}

#endif /* LH_LIBRARY_OPTION_OS */
