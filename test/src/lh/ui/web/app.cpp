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
