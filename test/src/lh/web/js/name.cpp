/**
 * @file name.cpp
 * @brief ASCII JavaScript name classification.
 */

#include <gtest/gtest.h>

#include <lh/web/js/lexer/name.h>

TEST(js_lexer_is_name_start, accepts_ascii_letters_dollar_and_underscore)
{
    EXPECT_TRUE(lh_web_js_lexer_is_name_start('a'));
    EXPECT_TRUE(lh_web_js_lexer_is_name_start('Z'));
    EXPECT_TRUE(lh_web_js_lexer_is_name_start('$'));
    EXPECT_TRUE(lh_web_js_lexer_is_name_start('_'));
    EXPECT_FALSE(lh_web_js_lexer_is_name_start('1'));
    EXPECT_FALSE(lh_web_js_lexer_is_name_start('-'));
}

TEST(js_lexer_is_name_part, also_accepts_ascii_digits)
{
    EXPECT_TRUE(lh_web_js_lexer_is_name_part('_'));
    EXPECT_TRUE(lh_web_js_lexer_is_name_part('0'));
    EXPECT_TRUE(lh_web_js_lexer_is_name_part('9'));
    EXPECT_FALSE(lh_web_js_lexer_is_name_part(' '));
}

TEST(js_lexer_name_is_ascii_identifier, recognizes_the_complete_ascii_name)
{
    const lh_str_view_t valid = lh_str_view_initializer_lit("a_$Z09");
    const lh_str_view_t keyword = lh_str_view_initializer_lit("null");
    const lh_str_view_t empty = lh_str_view_initializer_lit("");
    const lh_str_view_t initial_digit = lh_str_view_initializer_lit("1x");
    const lh_str_view_t punctuation = lh_str_view_initializer_lit("a-b");
    const lh_str_view_t utf8 = lh_str_view_initializer_lit("\xc3\xa9");
    EXPECT_TRUE(lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(valid)));
    EXPECT_TRUE(lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(keyword)));
    EXPECT_FALSE(lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(empty)));
    EXPECT_FALSE(lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(initial_digit)));
    EXPECT_FALSE(lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(punctuation)));
    EXPECT_FALSE(lh_web_js_lexer_name_is_ascii_identifier(lh_addr_of(utf8)));
}
