/**
 * @file framing.cpp
 * @brief HTTP request body framing tests.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/core/http/framing.h>

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
    const lh_str_view_t name = lh_str_view_initializer_lit("Content-Length");
    lh_http_header_t fields[2];
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

TEST(http_framing_is_chunked, checks_coding_and_ambiguous_lengths)
{
    lh_bool_t chunked = lh_bool_true;
    EXPECT_TRUE(lh_http_framing_try_is_chunked(nullptr, 0U, &chunked));
    EXPECT_FALSE(chunked);
    lh_http_header_t fields[2];
    lh_http_header_set(&fields[0], lh_str_view_lit("Transfer-Encoding"),
                       lh_str_view_lit("ChUnKeD"));
    EXPECT_TRUE(lh_http_framing_is_chunked(fields, 1U));
    lh_http_header_set(&fields[1], lh_str_view_lit("Content-Length"), lh_str_view_lit("0"));
    chunked = lh_bool_false;
    EXPECT_FALSE(lh_http_framing_try_is_chunked(fields, 2U, &chunked));
    EXPECT_FALSE(chunked);
    LH_EXPECT_CHECK_DEATH(lh_http_framing_is_chunked(fields, 2U));
    lh_http_header_set(&fields[1], lh_str_view_lit("Transfer-Encoding"),
                       lh_str_view_lit("chunked"));
    EXPECT_FALSE(lh_http_framing_try_is_chunked(fields, 2U, &chunked));
    lh_http_header_set(&fields[0], lh_str_view_lit("Transfer-Encoding"),
                       lh_str_view_lit("gzip, chunked"));
    EXPECT_TRUE(lh_http_framing_try_is_chunked(fields, 1U, &chunked));
    EXPECT_TRUE(chunked);
}

TEST(http_framing_get_transfer, validates_gzip_chunked_and_rejects_ambiguous_chains)
{
    lh_bool_t chunked = lh_bool_false;
    lh_bool_t gzip = lh_bool_false;
    EXPECT_TRUE(lh_http_framing_try_get_transfer(nullptr, 0U, &chunked, &gzip));
    EXPECT_FALSE(chunked);
    EXPECT_FALSE(gzip);
    lh_http_header_t fields[2];
    lh_http_header_set(&fields[0], lh_str_view_lit("Transfer-Encoding"),
                       lh_str_view_lit(" GzIp , ChUnKeD "));
    lh_http_framing_get_transfer(fields, 1U, &chunked, &gzip);
    EXPECT_TRUE(chunked);
    EXPECT_TRUE(gzip);
    for (const char *value :
         {"gzip", "chunked, gzip", "gzip, chunked,", "gzip,,chunked", "gzip, gzip, chunked"})
    {
        lh_str_view_t view;
        lh_str_view_init(&view, value);
        lh_http_header_set(&fields[0], lh_str_view_lit("Transfer-Encoding"), view);
        chunked = lh_bool_true;
        gzip = lh_bool_false;
        EXPECT_FALSE(lh_http_framing_try_get_transfer(fields, 1U, &chunked, &gzip)) << value;
        EXPECT_TRUE(chunked);
        EXPECT_FALSE(gzip);
    }
    lh_http_header_set(&fields[0], lh_str_view_lit("Transfer-Encoding"),
                       lh_str_view_lit("gzip, chunked"));
    lh_http_header_set(&fields[1], lh_str_view_lit("Content-Length"), lh_str_view_lit("4"));
    EXPECT_FALSE(lh_http_framing_try_get_transfer(fields, 2U, &chunked, &gzip));
    LH_EXPECT_CHECK_DEATH(lh_http_framing_get_transfer(fields, 2U, &chunked, &gzip));
}
