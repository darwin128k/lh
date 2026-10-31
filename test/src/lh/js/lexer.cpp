#include <gtest/gtest.h>

#include <lh/js.h>

#include <string>
#include <vector>

namespace
{

struct token_info
{
    lh_js_token_kind_t kind;
    std::string text;
    lh_real_t value;
};

/* The text of a token: a pointer into the source and a length, not owned. */
std::string
text_of(const lh_js_token_t *token)
{
    const char *text = lh_js_token_get_text(token);

    if (text == nullptr)
    {
        return std::string();
    }
    return std::string(text, lh_js_token_get_text_len(token));
}

/* Scan the whole text and keep what the lexer saw. */
std::vector<token_info>
scan(const std::string &source)
{
    std::vector<token_info> out;
    lh_js_lexer_t lexer;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    while (lh_js_lexer_next(&lexer))
    {
        const lh_js_token_t *token = lh_js_lexer_get_token(&lexer);
        token_info info;

        info.kind = lh_js_token_get_kind(token);
        info.text = text_of(token);
        info.value = lh_js_token_get_value(token);
        out.push_back(info);
    }
    return out;
}

/* Scan the whole text and return the failure, if there is one. */
std::string
failure_of(const std::string &source)
{
    lh_js_lexer_t lexer;
    lh_str_ptr message = nullptr;
    lh_usize_t len = 0;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    while (lh_js_lexer_next(&lexer))
    {
    }
    if (!lh_js_lexer_get_error(&lexer, &message, &len))
    {
        return std::string();
    }
    return std::string(message, len);
}

TEST(js_token, holds_its_kind_text_and_value)
{
    lh_js_token_t token;

    lh_js_token_set_kind(&token, lh_js_token_kind_number);
    lh_js_token_set_text(&token, "12");
    lh_js_token_set_text_len(&token, 2);
    lh_js_token_set_value(&token, 12.0);
    EXPECT_EQ(lh_js_token_get_kind(&token), lh_js_token_kind_number);
    EXPECT_EQ(std::string(lh_js_token_get_text(&token), lh_js_token_get_text_len(&token)), "12");
    EXPECT_EQ(lh_js_token_get_value(&token), 12.0);
}

TEST(js_lexer, scans_names)
{
    std::vector<token_info> tokens = scan("count $name _name name2");

    ASSERT_EQ(tokens.size(), 4U);
    EXPECT_EQ(tokens[0].kind, lh_js_token_kind_identifier);
    EXPECT_EQ(tokens[0].text, "count");
    EXPECT_EQ(tokens[1].kind, lh_js_token_kind_identifier);
    EXPECT_EQ(tokens[1].text, "$name");
    EXPECT_EQ(tokens[2].kind, lh_js_token_kind_identifier);
    EXPECT_EQ(tokens[2].text, "_name");
    EXPECT_EQ(tokens[3].kind, lh_js_token_kind_identifier);
    EXPECT_EQ(tokens[3].text, "name2");
}

TEST(js_lexer, scans_keywords)
{
    std::vector<token_info> tokens = scan("if while function return");

    ASSERT_EQ(tokens.size(), 4U);
    EXPECT_EQ(tokens[0].kind, lh_js_token_kind_keyword);
    EXPECT_EQ(tokens[0].text, "if");
    EXPECT_EQ(tokens[1].kind, lh_js_token_kind_keyword);
    EXPECT_EQ(tokens[1].text, "while");
    EXPECT_EQ(tokens[2].kind, lh_js_token_kind_keyword);
    EXPECT_EQ(tokens[2].text, "function");
    EXPECT_EQ(tokens[3].kind, lh_js_token_kind_keyword);
    EXPECT_EQ(tokens[3].text, "return");
}

TEST(js_lexer, scans_every_number_form)
{
    EXPECT_EQ(scan("0")[0].value, 0.0);
    EXPECT_EQ(scan("12")[0].value, 12.0);
    EXPECT_EQ(scan("1.5")[0].value, 1.5);
    EXPECT_EQ(scan(".5")[0].value, 0.5);
    EXPECT_EQ(scan("1e10")[0].value, 1e10);
    EXPECT_EQ(scan("1.5e-3")[0].value, 0.0015);
    EXPECT_EQ(scan("1E+3")[0].value, 1000.0);
    EXPECT_EQ(scan("0x1f")[0].value, 31.0);
    EXPECT_EQ(scan("0X1F")[0].value, 31.0);
    EXPECT_EQ(scan("0o17")[0].value, 15.0);
    EXPECT_EQ(scan("0b1010")[0].value, 10.0);
}

TEST(js_lexer, keeps_the_text_of_a_number)
{
    std::vector<token_info> tokens = scan("1.5e-3");

    ASSERT_EQ(tokens.size(), 1U);
    EXPECT_EQ(tokens[0].kind, lh_js_token_kind_number);
    EXPECT_EQ(tokens[0].text, "1.5e-3");
}

TEST(js_lexer, scans_a_number_above_uint32)
{
    EXPECT_EQ(scan("0x100000000")[0].value, 4294967296.0);
}

TEST(js_lexer, scans_strings_with_every_escape)
{
    const char *escapes[] = {"\\\\", "\\'", "\\\"", "\\n",   "\\r",     "\\t",       "\\b",
                             "\\f",  "\\v", "\\0",  "\\x41", "\\u0041", "\\u{1F600}"};

    for (const char *escape : escapes)
    {
        std::string source = std::string("'") + escape + "'";
        std::vector<token_info> tokens = scan(source);

        ASSERT_EQ(tokens.size(), 1U) << escape;
        EXPECT_EQ(tokens[0].kind, lh_js_token_kind_string) << escape;
        EXPECT_EQ(tokens[0].text, source) << escape;
    }
}

TEST(js_lexer, scans_a_string_in_both_quotes)
{
    std::vector<token_info> tokens = scan("'one' \"two\"");

    ASSERT_EQ(tokens.size(), 2U);
    EXPECT_EQ(tokens[0].kind, lh_js_token_kind_string);
    EXPECT_EQ(tokens[0].text, "'one'");
    EXPECT_EQ(tokens[1].kind, lh_js_token_kind_string);
    EXPECT_EQ(tokens[1].text, "\"two\"");
}

TEST(js_lexer, skips_a_line_comment)
{
    std::vector<token_info> tokens = scan("a // the rest\nb");

    ASSERT_EQ(tokens.size(), 3U);
    EXPECT_EQ(tokens[0].text, "a");
    EXPECT_EQ(tokens[1].kind, lh_js_token_kind_newline);
    EXPECT_EQ(tokens[2].text, "b");
}

TEST(js_lexer, skips_a_block_comment)
{
    std::vector<token_info> tokens = scan("a /* the rest */ b");

    ASSERT_EQ(tokens.size(), 2U);
    EXPECT_EQ(tokens[0].text, "a");
    EXPECT_EQ(tokens[1].text, "b");
}

TEST(js_lexer, counts_the_line_inside_a_block_comment)
{
    std::string source = "/* one\ntwo */ x";
    lh_js_lexer_t lexer;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    EXPECT_EQ(text_of(lh_js_lexer_get_token(&lexer)), "x");
    EXPECT_EQ(lh_js_lexer_get_line(&lexer), 2U);
    EXPECT_EQ(lh_js_lexer_get_column(&lexer), 9U);
}

TEST(js_lexer, counts_the_line_and_the_column)
{
    std::string source = "a\nb";
    lh_js_lexer_t lexer;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    EXPECT_EQ(text_of(lh_js_lexer_get_token(&lexer)), "a");
    EXPECT_EQ(lh_js_lexer_get_line(&lexer), 1U);
    EXPECT_EQ(lh_js_lexer_get_column(&lexer), 2U);

    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    EXPECT_EQ(lh_js_token_get_kind(lh_js_lexer_get_token(&lexer)), lh_js_token_kind_newline);
    EXPECT_EQ(lh_js_lexer_get_line(&lexer), 2U);
    EXPECT_EQ(lh_js_lexer_get_column(&lexer), 1U);

    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    EXPECT_EQ(text_of(lh_js_lexer_get_token(&lexer)), "b");
    EXPECT_EQ(lh_js_lexer_get_line(&lexer), 2U);
    EXPECT_EQ(lh_js_lexer_get_column(&lexer), 2U);
}

TEST(js_lexer, ends_at_the_end_of_the_text)
{
    std::string source = "a";
    lh_js_lexer_t lexer;
    lh_str_ptr message = nullptr;
    lh_usize_t len = 0;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    EXPECT_FALSE(lh_js_lexer_next(&lexer));
    EXPECT_EQ(lh_js_token_get_kind(lh_js_lexer_get_token(&lexer)), lh_js_token_kind_end_of_input);
    EXPECT_FALSE(lh_js_lexer_get_error(&lexer, &message, &len));
}

TEST(js_lexer, scans_the_operators_and_the_brackets)
{
    const char *operators[] = {"+",  "-",  "*",  "/",  "%",   "**", "++",  "--", "=",   "+=",
                               "-=", "*=", "/=", "%=", "**=", "==", "===", "!=", "!==", "<",
                               "<=", ">",  ">=", "&&", "||",  "??", "?",   ":",  ",",   ";",
                               ".",  "(",  ")",  "[",  "]",   "{",  "}",   "=>", "...", "&",
                               "|",  "^",  "~",  "<<", ">>",  ">>>"};

    for (const char *op : operators)
    {
        std::vector<token_info> tokens = scan(op);

        ASSERT_EQ(tokens.size(), 1U) << op;
        EXPECT_EQ(tokens[0].kind, lh_js_token_kind_punctuation) << op;
        EXPECT_EQ(tokens[0].text, op) << op;
    }
}

TEST(js_lexer, takes_the_longest_operator)
{
    std::vector<token_info> tokens = scan("===!==");

    ASSERT_EQ(tokens.size(), 2U);
    EXPECT_EQ(tokens[0].text, "===");
    EXPECT_EQ(tokens[1].text, "!==");
}

TEST(js_lexer, reports_an_unterminated_string)
{
    EXPECT_EQ(failure_of("'abc"), "unterminated string");
    EXPECT_EQ(failure_of("\"abc\n"), "unterminated string");
}

TEST(js_lexer, reports_an_unterminated_block_comment)
{
    EXPECT_EQ(failure_of("/* abc"), "unterminated block comment");
}

TEST(js_lexer, reports_a_bad_escape)
{
    EXPECT_EQ(failure_of("'\\q'"), "bad escape");
    EXPECT_EQ(failure_of("'\\x4'"), "bad escape");
    EXPECT_EQ(failure_of("'\\u041'"), "bad escape");
    EXPECT_EQ(failure_of("'\\u{110000}'"), "bad escape");
}

TEST(js_lexer, reports_a_bad_number)
{
    EXPECT_EQ(failure_of("017"), "bad number");
    EXPECT_EQ(failure_of("1e"), "bad number");
    EXPECT_EQ(failure_of("0x"), "bad number");
    EXPECT_EQ(failure_of("0b"), "bad number");
    EXPECT_EQ(failure_of("0o"), "bad number");
}

TEST(js_lexer, reports_a_character_that_starts_nothing)
{
    EXPECT_EQ(failure_of("@"), "unexpected character");
    EXPECT_EQ(failure_of("a # b"), "unexpected character");
}

TEST(js_lexer, forgets_the_last_failure)
{
    std::string source = "@";
    lh_js_lexer_t lexer;
    lh_str_ptr message = nullptr;
    lh_usize_t len = 0;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    EXPECT_FALSE(lh_js_lexer_next(&lexer));
    EXPECT_TRUE(lh_js_lexer_get_error(&lexer, &message, &len));
    lh_js_lexer_clear_error(&lexer);
    EXPECT_FALSE(lh_js_lexer_get_error(&lexer, &message, &len));
}

TEST(js_lexer, holds_where_the_failure_was)
{
    std::string source = "a\n@";
    lh_js_lexer_t lexer;

    lh_js_lexer_init(&lexer, source.c_str(), source.size());
    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    ASSERT_TRUE(lh_js_lexer_next(&lexer));
    EXPECT_FALSE(lh_js_lexer_next(&lexer));
    EXPECT_EQ(lh_js_lexer_get_error_line(&lexer), 2U);
    EXPECT_EQ(lh_js_lexer_get_error_column(&lexer), 1U);
}

} // namespace
