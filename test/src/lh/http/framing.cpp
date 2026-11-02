/**
 * @file framing.cpp
 * @brief HTTP request body framing tests.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/http/framing.h>

TEST(http_framing, accepts_one_decimal_length_and_missing_length)
{
    lh_usize_t length = 9;
    EXPECT_TRUE(lh_http_framing_try_get_content_length(nullptr, 0, lh_addr_of(length)));
    EXPECT_EQ(length, 0u);
    lh_http_header_t field;
    lh_http_header_set(lh_addr_of(field), lh_str_view_initializer_lit("content-length"),
                       lh_str_view_initializer_lit("0042"));
    EXPECT_EQ(lh_http_framing_get_content_length(lh_addr_of(field), 1), 42u);
}

TEST(http_framing, rejects_ambiguous_unsupported_and_invalid_lengths)
{
    lh_http_header_t fields[2];
    const lh_str_view_t name = lh_str_view_initializer_lit("Content-Length");
    lh_http_header_set(lh_addr_of(fields[0]), name, lh_str_view_initializer_lit("1"));
    lh_http_header_set(lh_addr_of(fields[1]), name, lh_str_view_initializer_lit("1"));
    lh_usize_t length = 17;
    EXPECT_FALSE(lh_http_framing_try_get_content_length(fields, 2, lh_addr_of(length)));
    EXPECT_EQ(length, 17u);
    LH_EXPECT_CHECK_DEATH(lh_http_framing_get_content_length(fields, 2));
    lh_http_header_set(lh_addr_of(fields[1]), lh_str_view_initializer_lit("Transfer-Encoding"),
                       lh_str_view_initializer_lit("chunked"));
    EXPECT_FALSE(lh_http_framing_try_get_content_length(fields, 2, lh_addr_of(length)));
    lh_http_header_set(lh_addr_of(fields[0]), name, lh_str_view_initializer_lit("1, 1"));
    EXPECT_FALSE(lh_http_framing_try_get_content_length(fields, 1, lh_addr_of(length)));
    lh_http_header_set(lh_addr_of(fields[0]), name,
                       lh_str_view_initializer_lit("999999999999999999999999999999999999"));
    EXPECT_FALSE(lh_http_framing_try_get_content_length(fields, 1, lh_addr_of(length)));
    EXPECT_EQ(length, 17u);
}
