/**
 * @file expect.cpp
 * @brief HTTP request expectation parsing tests.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/http/expect.h>

TEST(http_expect_get_continue, accepts_absence_and_case_insensitive_lists)
{
    EXPECT_FALSE(lh_http_expect_get_continue(nullptr, 0U));
    lh_http_header_t headers[2];
    lh_http_header_set(&headers[0], lh_str_view_lit("Expect"),
                       lh_str_view_lit(" 100-Continue, 100-continue "));
    lh_http_header_set(&headers[1], lh_str_view_lit("expect"), lh_str_view_lit("100-continue"));
    EXPECT_TRUE(lh_http_expect_get_continue(headers, 2U));
}

TEST(http_expect_try_get_continue, rejects_unsupported_members_without_changing_output)
{
    lh_http_header_t field;
    lh_http_header_set(&field, lh_str_view_lit("Expect"),
                       lh_str_view_lit("100-continue, surprise"));
    lh_bool_t value = lh_bool_true;
    EXPECT_FALSE(lh_http_expect_try_get_continue(&field, 1U, &value));
    EXPECT_TRUE(value);
    LH_EXPECT_CHECK_DEATH(lh_http_expect_get_continue(&field, 1U));
    lh_http_header_set(&field, lh_str_view_lit("Expect"), lh_str_view_lit(" , "));
    EXPECT_FALSE(lh_http_expect_try_get_continue(&field, 1U, &value));
}
