/**
 * @file encoding.cpp
 * @brief Tests for HTTP request content coding selection.
 */

#include <gtest/gtest.h>
#include <lh/core/http/content/encoding.h>
#include <lh/expect/death.h>

TEST(http_content_encoding, accepts_gzip_identity_and_absence)
{
    lh_bool_t gzip = lh_bool_true;
    EXPECT_TRUE(lh_http_content_encoding_try_is_gzip(nullptr, 0U, &gzip));
    EXPECT_FALSE(gzip);
    lh_http_header_t field;
    lh_http_header_set(&field, lh_str_view_lit("content-encoding"), lh_str_view_lit("GZip"));
    EXPECT_TRUE(lh_http_content_encoding_is_gzip(&field, 1U));
    lh_http_header_set(&field, lh_str_view_lit("Content-Encoding"), lh_str_view_lit("identity"));
    EXPECT_FALSE(lh_http_content_encoding_is_gzip(&field, 1U));
}

TEST(http_content_encoding, rejects_unknown_lists_and_repeated_fields)
{
    lh_http_header_t fields[2];
    lh_http_header_set(&fields[0], lh_str_view_lit("Content-Encoding"),
                       lh_str_view_lit("gzip, gzip"));
    lh_bool_t gzip = lh_bool_true;
    EXPECT_FALSE(lh_http_content_encoding_try_is_gzip(fields, 1U, &gzip));
    EXPECT_TRUE(gzip);
    LH_EXPECT_CHECK_DEATH(lh_http_content_encoding_is_gzip(fields, 1U));
    lh_http_header_set(&fields[0], lh_str_view_lit("Content-Encoding"), lh_str_view_lit("br"));
    EXPECT_FALSE(lh_http_content_encoding_try_is_gzip(fields, 1U, &gzip));
    lh_http_header_set(&fields[0], lh_str_view_lit("Content-Encoding"), lh_str_view_lit("gzip"));
    lh_http_header_set(&fields[1], lh_str_view_lit("content-encoding"), lh_str_view_lit("gzip"));
    EXPECT_FALSE(lh_http_content_encoding_try_is_gzip(fields, 2U, &gzip));
}
