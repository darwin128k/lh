/**
 * @file cursor.cpp
 * @brief ::lh_str_cursor_t: position, peeking, moving and matching in a borrowed text.
 */

#include <gtest/gtest.h>

#include <lh/str/cursor.h>

#include <string>

namespace
{

std::string
text_of(lh_str_view_t view)
{
    return std::string(lh_str_view_get_data(&view), lh_str_view_get_size(&view));
}

} // namespace

TEST(str_cursor, init_starts_at_the_first_character)
{
    lh_str_cursor_t cursor;

    lh_str_cursor_init(&cursor, lh_str_view_lit("ab"));
    EXPECT_EQ(lh_str_cursor_get_pos(&cursor), 0U);
    EXPECT_EQ(lh_str_cursor_get_left(&cursor), 2U);
    EXPECT_FALSE(lh_str_cursor_is_end(&cursor));
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), 'a');
    EXPECT_EQ(lh_str_cursor_peek(&cursor, 1U), 'b');
    EXPECT_EQ(lh_str_cursor_peek(&cursor, 2U), '\0');
}

TEST(str_cursor, advance_stops_at_the_end)
{
    lh_str_cursor_t cursor;

    lh_str_cursor_init(&cursor, lh_str_view_lit("abc"));
    lh_str_cursor_advance(&cursor, 2U);
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), 'c');
    lh_str_cursor_advance(&cursor, 10U);
    EXPECT_EQ(lh_str_cursor_get_pos(&cursor), 3U);
    EXPECT_TRUE(lh_str_cursor_is_end(&cursor));
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), '\0');
}

TEST(str_cursor, rest_and_since_are_views_of_the_text)
{
    lh_str_cursor_t cursor;
    lh_str_view_t view;

    lh_str_cursor_init(&cursor, lh_str_view_lit("key=value"));
    lh_str_cursor_advance(&cursor, 4U);
    lh_str_cursor_get_rest(&cursor, &view);
    EXPECT_EQ(text_of(view), "value");
    lh_str_cursor_get_since(&cursor, 0U, &view);
    EXPECT_EQ(text_of(view), "key=");
    lh_str_cursor_get_since(&cursor, 4U, &view);
    EXPECT_EQ(text_of(view), "");
}

TEST(str_cursor, skip_char_moves_only_on_a_match)
{
    lh_str_cursor_t cursor;

    lh_str_cursor_init(&cursor, lh_str_view_lit("<a"));
    EXPECT_FALSE(lh_str_cursor_skip_char(&cursor, 'a'));
    EXPECT_EQ(lh_str_cursor_get_pos(&cursor), 0U);
    EXPECT_TRUE(lh_str_cursor_skip_char(&cursor, '<'));
    EXPECT_TRUE(lh_str_cursor_skip_char(&cursor, 'a'));
    EXPECT_FALSE(lh_str_cursor_skip_char(&cursor, '\0'));
}

TEST(str_cursor, words_match_exactly_or_case_folded)
{
    lh_str_cursor_t cursor;

    lh_str_cursor_init(&cursor, lh_str_view_lit("<!DOCTYPE html>"));
    EXPECT_TRUE(lh_str_cursor_is_word(&cursor, lh_str_view_lit("<!DOC"), lh_bool_false));
    EXPECT_FALSE(lh_str_cursor_skip_word(&cursor, lh_str_view_lit("<!doctype"), lh_bool_false));
    EXPECT_EQ(lh_str_cursor_get_pos(&cursor), 0U);
    EXPECT_TRUE(lh_str_cursor_skip_word(&cursor, lh_str_view_lit("<!doctype"), lh_bool_true));
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), ' ');
    EXPECT_FALSE(lh_str_cursor_is_word(&cursor, lh_str_view_lit(" html>>"), lh_bool_false));
}

TEST(str_cursor, find_counts_from_the_position)
{
    lh_str_cursor_t cursor;

    lh_str_cursor_init(&cursor, lh_str_view_lit("a-->b-->"));
    EXPECT_EQ(lh_str_cursor_find(&cursor, lh_str_view_lit("-->"), lh_bool_false), 1U);
    lh_str_cursor_advance(&cursor, 4U);
    EXPECT_EQ(lh_str_cursor_find(&cursor, lh_str_view_lit("-->"), lh_bool_false), 1U);
    EXPECT_EQ(lh_str_cursor_find(&cursor, lh_str_view_lit("<"), lh_bool_false),
              LH_STR_VIEW_INVALID);
    EXPECT_EQ(lh_str_cursor_find(&cursor, lh_str_view_lit("B"), lh_bool_true), 0U);
}

TEST(str_cursor, skip_whitespace_counts_what_it_skipped)
{
    lh_str_cursor_t cursor;

    lh_str_cursor_init(&cursor, lh_str_view_lit(" \t\r\nx "));
    EXPECT_EQ(lh_str_cursor_skip_whitespace(&cursor), 4U);
    EXPECT_EQ(lh_str_cursor_get_char(&cursor), 'x');
    EXPECT_EQ(lh_str_cursor_skip_whitespace(&cursor), 0U);
    lh_str_cursor_advance(&cursor, 1U);
    EXPECT_EQ(lh_str_cursor_skip_whitespace(&cursor), 1U);
    EXPECT_TRUE(lh_str_cursor_is_end(&cursor));
}
