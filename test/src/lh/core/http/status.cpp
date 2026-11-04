/**
 * @file status.cpp
 * @brief HTTP status code and reason tests.
 */

#include <gtest/gtest.h>

#include <lh/core/http/status.h>

#include <string>

TEST(http_status_is_valid, accepts_the_http_status_range)
{
    EXPECT_FALSE(lh_http_status_is_valid(99));
    EXPECT_TRUE(lh_http_status_is_valid(100));
    EXPECT_TRUE(lh_http_status_is_valid(599));
    EXPECT_FALSE(lh_http_status_is_valid(600));
}

TEST(http_status_get_reason, returns_known_phrases_and_empty_for_unknown_codes)
{
    const lh_str_view_t ok = lh_http_status_get_reason(lh_http_status_ok);
    const lh_str_view_t error = lh_http_status_get_reason(lh_http_status_internal_server_error);
    const lh_str_view_t unknown = lh_http_status_get_reason(418);
    EXPECT_EQ(
        std::string(lh_str_view_get_data(lh_addr_of(ok)), lh_str_view_get_size(lh_addr_of(ok))),
        "OK");
    EXPECT_EQ(std::string(lh_str_view_get_data(lh_addr_of(error)),
                          lh_str_view_get_size(lh_addr_of(error))),
              "Internal Server Error");
    EXPECT_TRUE(lh_str_view_is_empty(lh_addr_of(unknown)));
}

TEST(http_status_is_content_forbidden, covers_status_only_restrictions)
{
    EXPECT_FALSE(lh_http_status_is_content_forbidden(99));
    EXPECT_TRUE(lh_http_status_is_content_forbidden(100));
    EXPECT_TRUE(lh_http_status_is_content_forbidden(199));
    EXPECT_FALSE(lh_http_status_is_content_forbidden(lh_http_status_ok));
    EXPECT_TRUE(lh_http_status_is_content_forbidden(lh_http_status_no_content));
    EXPECT_TRUE(lh_http_status_is_content_forbidden(lh_http_status_reset_content));
    EXPECT_TRUE(lh_http_status_is_content_forbidden(lh_http_status_not_modified));
    EXPECT_FALSE(lh_http_status_is_content_forbidden(lh_http_status_not_found));
}

TEST(http_status_is_success, accepts_only_the_2xx_class)
{
    EXPECT_FALSE(lh_http_status_is_success(199));
    EXPECT_TRUE(lh_http_status_is_success(200));
    EXPECT_TRUE(lh_http_status_is_success(299));
    EXPECT_FALSE(lh_http_status_is_success(300));
}
