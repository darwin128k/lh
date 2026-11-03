/**
 * @file cursor.cpp
 * @brief HTTP cursor whitespace and token scanning tests.
 */

#include <gtest/gtest.h>

#include <lh/http/cursor.h>
#include <lh/expect/death.h>

TEST(http_cursor_skip_ows, stops_at_non_http_whitespace)
{
    lh_str_cursor_t cursor;
    lh_str_cursor_init(&cursor, lh_str_view_lit(" \t\n"));
    EXPECT_EQ(lh_http_cursor_skip_ows(&cursor), 2U);
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), '\n');
    EXPECT_EQ(lh_http_cursor_skip_ows(&cursor), 0U);
}

TEST(http_cursor_skip_token, stops_at_the_delimiter)
{
    lh_str_cursor_t cursor;
    lh_str_cursor_init(&cursor, lh_str_view_lit("key-1=value"));
    EXPECT_EQ(lh_http_cursor_skip_token(&cursor), 5U);
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), '=');
    EXPECT_EQ(lh_http_cursor_skip_token(&cursor), 0U);
}

TEST(http_cursor_try_skip_quoted, accepts_quoted_pairs_and_rejects_invalid_strings)
{
    lh_str_cursor_t cursor;
    lh_str_cursor_init(&cursor, lh_str_view_lit("\"a\\\"b\";"));
    EXPECT_TRUE(lh_http_cursor_try_skip_quoted(&cursor));
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), ';');
    lh_str_cursor_init(&cursor, lh_str_view_lit("\"unterminated"));
    EXPECT_FALSE(lh_http_cursor_try_skip_quoted(&cursor));
    lh_str_cursor_init(&cursor, lh_str_view_lit("plain"));
    EXPECT_FALSE(lh_http_cursor_try_skip_quoted(&cursor));
}

TEST(http_cursor_skip_quoted, checks_the_try_result)
{
    lh_str_cursor_t cursor;
    lh_str_cursor_init(&cursor, lh_str_view_lit("\"ok\""));
    lh_http_cursor_skip_quoted(&cursor);
    EXPECT_TRUE(lh_str_cursor_is_end(&cursor));
    lh_str_cursor_init(&cursor, lh_str_view_lit("\"bad"));
    LH_EXPECT_CHECK_DEATH(lh_http_cursor_skip_quoted(&cursor));
}
