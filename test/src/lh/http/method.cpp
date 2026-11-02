/**
 * @file method.cpp
 * @brief Known HTTP method names and request matching.
 */

#include <gtest/gtest.h>

#include <lh/http/method.h>
#include <lh/http/request.h>

#include <string>

TEST(http_method_get_name, returns_known_wire_tokens)
{
    const lh_http_method_kind_t methods[] = {
        lh_http_method_get,     lh_http_method_head,   lh_http_method_post,
        lh_http_method_put,     lh_http_method_delete, lh_http_method_connect,
        lh_http_method_options, lh_http_method_trace,  lh_http_method_patch};
    const char *names[] = {"GET",     "HEAD",    "POST",  "PUT",  "DELETE",
                           "CONNECT", "OPTIONS", "TRACE", "PATCH"};
    for (lh_usize_t i = 0; i < 9; i = lh_math_add_one(i))
    {
        const lh_str_view_t name = lh_http_method_get_name(methods[i]);
        EXPECT_EQ(std::string(lh_str_view_get_data(lh_addr_of(name)),
                              lh_str_view_get_size(lh_addr_of(name))),
                  names[i]);
    }
    const lh_str_view_t unknown = lh_http_method_get_name(static_cast<lh_http_method_kind_t>(99));
    EXPECT_TRUE(lh_str_view_is_empty(lh_addr_of(unknown)));
}

TEST(http_request_is_method, compares_case_sensitively_and_rejects_unknown_kinds)
{
    lh_http_request_t request;
    const lh_str_view_t upper = lh_str_view_initializer_lit("HEAD");
    const lh_str_view_t lower = lh_str_view_initializer_lit("head");
    lh_http_request_set_method(lh_addr_of(request), upper);
    EXPECT_TRUE(lh_http_request_is_method(lh_addr_of(request), lh_http_method_head));
    EXPECT_FALSE(lh_http_request_is_method(lh_addr_of(request), lh_http_method_get));
    EXPECT_FALSE(
        lh_http_request_is_method(lh_addr_of(request), static_cast<lh_http_method_kind_t>(99)));
    lh_http_request_set_method(lh_addr_of(request), lower);
    EXPECT_FALSE(lh_http_request_is_method(lh_addr_of(request), lh_http_method_head));
}
