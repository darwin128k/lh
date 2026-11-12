/**
 * @file route.cpp
 * @brief The route's setters: each stores its one field and leaves the others
 *        alone, and a route matches by the method and path it was given.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/core/str/lit.h>
#    include <lh/ui/web/app/route.h>

namespace
{

/* A route for @p method and @p path, with no handler. */
lh_void
start(lh_ui_web_route_t *route, const lh_str_ptr method, lh_usize_t method_len,
      const lh_str_ptr path, lh_usize_t path_len)
{
    lh_ui_web_route_set_method(route, method);
    lh_ui_web_route_set_method_len(route, method_len);
    lh_ui_web_route_set_path(route, path);
    lh_ui_web_route_set_path_len(route, path_len);
    lh_ui_web_route_set_handler(route, lh_null);
}

} // namespace

TEST(ui_web_route, set_method_and_set_method_len_store_one_field_each)
{
    const lh_str_ptr get = "GET";
    const lh_str_ptr post = "POST";
    lh_ui_web_route_t route;

    start(lh_addr_of(route), get, lh_str_lit_size("GET"), "/", lh_str_lit_size("/"));
    lh_ui_web_route_set_method(lh_addr_of(route), post);

    EXPECT_EQ(lh_ui_web_route_get_method(&route), post);
    EXPECT_EQ(lh_ui_web_route_get_method_len(&route), 3u);

    lh_ui_web_route_set_method_len(lh_addr_of(route), lh_str_lit_size("POST"));

    EXPECT_EQ(lh_ui_web_route_get_method(&route), post);
    EXPECT_EQ(lh_ui_web_route_get_method_len(&route), 4u);
}

TEST(ui_web_route, set_path_and_set_path_len_store_one_field_each)
{
    const lh_str_ptr root = "/";
    const lh_str_ptr state = "/state";
    lh_ui_web_route_t route;

    start(lh_addr_of(route), "GET", lh_str_lit_size("GET"), root, lh_str_lit_size("/"));
    lh_ui_web_route_set_path(lh_addr_of(route), state);

    EXPECT_EQ(lh_ui_web_route_get_path(&route), state);
    EXPECT_EQ(lh_ui_web_route_get_path_len(&route), 1u);

    lh_ui_web_route_set_path_len(lh_addr_of(route), lh_str_lit_size("/state"));

    EXPECT_EQ(lh_ui_web_route_get_path(&route), state);
    EXPECT_EQ(lh_ui_web_route_get_path_len(&route), 6u);
}

TEST(ui_web_route, is_match_uses_the_parsed_method_and_path)
{
    const lh_str_view_t get = lh_str_view_initializer_lit("GET");
    const lh_str_view_t post = lh_str_view_initializer_lit("POST");
    const lh_str_view_t plain = lh_str_view_initializer_lit("/state");
    const lh_str_view_t query = lh_str_view_initializer_lit("/state?x=1");
    const lh_str_view_t longer = lh_str_view_initializer_lit("/states");

    lh_ui_web_route_t route;
    start(lh_addr_of(route), "GET", lh_str_lit_size("GET"), "/state", lh_str_lit_size("/state"));
    lh_http_request_t request;
    lh_http_request_set_method(lh_addr_of(request), get);
    lh_http_request_set_target(lh_addr_of(request), plain);
    EXPECT_TRUE(lh_ui_web_route_is_match(lh_addr_of(route), lh_addr_of(request)));
    lh_http_request_set_target(lh_addr_of(request), query);
    EXPECT_TRUE(lh_ui_web_route_is_match(lh_addr_of(route), lh_addr_of(request)));
    lh_http_request_set_method(lh_addr_of(request), post);
    EXPECT_FALSE(lh_ui_web_route_is_match(lh_addr_of(route), lh_addr_of(request)));
    lh_http_request_set_method(lh_addr_of(request), get);
    lh_http_request_set_target(lh_addr_of(request), longer);
    EXPECT_FALSE(lh_ui_web_route_is_match(lh_addr_of(route), lh_addr_of(request)));
    lh_ui_web_route_set_method_len(lh_addr_of(route), 0);
    lh_http_request_set_method(lh_addr_of(request), post);
    lh_http_request_set_target(lh_addr_of(request), plain);
    EXPECT_TRUE(lh_ui_web_route_is_match(lh_addr_of(route), lh_addr_of(request)));
}

#endif /* LH_LIBRARY_OPTION_OS */
