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

TEST(ui_web_app, is_websocket_reads_the_path_up_to_its_length)
{
    const lh_str_ptr upgrade = "GET /ws HTTP/1.1";
    const lh_str_ptr query = "GET /ws?x=1 HTTP/1.1";
    const lh_str_ptr longer = "GET /wsx HTTP/1.1";
    lh_ui_web_app_t app;

    lh_ui_web_app_init(lh_addr_of(app), lh_null, 0u);
    lh_ui_web_app_set_ws_path(lh_addr_of(app), "/ws");

    /* A path with no length is no websocket. */
    EXPECT_FALSE(lh_ui_web_app_is_websocket(&app, upgrade, lh_str_lit_size("GET /ws HTTP/1.1")));

    lh_ui_web_app_set_ws_path_len(lh_addr_of(app), lh_str_lit_size("/ws"));

    EXPECT_TRUE(lh_ui_web_app_is_websocket(&app, upgrade, lh_str_lit_size("GET /ws HTTP/1.1")));
    EXPECT_TRUE(lh_ui_web_app_is_websocket(&app, query, lh_str_lit_size("GET /ws?x=1 HTTP/1.1")));
    EXPECT_FALSE(lh_ui_web_app_is_websocket(&app, longer, lh_str_lit_size("GET /wsx HTTP/1.1")));
}

#endif /* LH_LIBRARY_OPTION_OS */
