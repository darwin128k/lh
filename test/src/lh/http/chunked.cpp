/**
 * @file chunked.cpp
 * @brief Chunked body parsing, validation and in-place decoding.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/http/chunked.h>

#include <string>

TEST(http_chunked_are_extensions_valid, handles_tokens_quoted_values_and_whitespace)
{
    EXPECT_TRUE(lh_http_chunked_are_extensions_valid(lh_str_view_lit("")));
    EXPECT_TRUE(
        lh_http_chunked_are_extensions_valid(lh_str_view_lit(";x; a = token; q=\"a\\\"b\"")));
    EXPECT_FALSE(lh_http_chunked_are_extensions_valid(lh_str_view_lit(";=x")));
    EXPECT_FALSE(lh_http_chunked_are_extensions_valid(lh_str_view_lit(";x=\"unterminated")));
    EXPECT_FALSE(lh_http_chunked_are_extensions_valid(lh_str_view_lit(";x=")));
    EXPECT_FALSE(lh_http_chunked_are_extensions_valid(lh_str_view_lit(";x ")));
}

TEST(http_chunked_parse_size, checks_size_extensions_and_overflow)
{
    EXPECT_EQ(lh_http_chunked_parse_size(lh_str_view_lit("aF;x=y")), 175U);
    lh_usize_t size = 9U;
    EXPECT_FALSE(lh_http_chunked_try_parse_size(lh_str_view_lit(""), &size));
    EXPECT_FALSE(lh_http_chunked_try_parse_size(lh_str_view_lit("1g"), &size));
    EXPECT_FALSE(
        lh_http_chunked_try_parse_size(lh_str_view_lit("ffffffffffffffffffffffffffffffff"), &size));
    EXPECT_EQ(size, 9U);
    LH_EXPECT_CHECK_DEATH(lh_http_chunked_parse_size(lh_str_view_lit("q")));
}

TEST(http_chunked_parse_part, reports_data_range_and_rejects_bad_delimiter)
{
    lh_usize_t data = 9U, length = 9U, next = 9U;
    EXPECT_EQ(lh_http_chunked_parse_part(lh_str_view_lit("3\r\nab"), 0U, &data, &length, &next),
              lh_http_parse_incomplete);
    EXPECT_EQ(data, 9U);
    EXPECT_EQ(lh_http_chunked_parse_part(lh_str_view_lit("3\r\nabcX"), 0U, &data, &length, &next),
              lh_http_parse_error);
    EXPECT_EQ(lh_http_chunked_parse_part(lh_str_view_lit("3\r\nabc\r"), 0U, &data, &length, &next),
              lh_http_parse_incomplete);
    EXPECT_EQ(lh_http_chunked_parse_part(lh_str_view_lit("3\r\nabcXX"), 0U, &data, &length, &next),
              lh_http_parse_error);
    EXPECT_EQ(
        lh_http_chunked_parse_part(lh_str_view_lit("3\r\nabc\r\n"), 0U, &data, &length, &next),
        lh_http_parse_complete);
    EXPECT_EQ(data, 3U);
    EXPECT_EQ(length, 3U);
    EXPECT_EQ(next, 8U);
}

TEST(http_chunked_parse_trailers, validates_fields_and_preserves_outputs_on_failure)
{
    lh_usize_t next = 99U;
    EXPECT_EQ(lh_http_chunked_parse_trailers(lh_str_view_lit("X: yes\r\n\r\nTAIL"), 0U, &next),
              lh_http_parse_complete);
    EXPECT_EQ(next, 10U);
    next = 99U;
    EXPECT_EQ(lh_http_chunked_parse_trailers(lh_str_view_lit("X: yes\r\n"), 0U, &next),
              lh_http_parse_incomplete);
    EXPECT_EQ(next, 99U);
    EXPECT_EQ(lh_http_chunked_parse_trailers(lh_str_view_lit("bad field\r\n\r\n"), 0U, &next),
              lh_http_parse_error);
    EXPECT_EQ(next, 99U);
}

TEST(http_chunked_scan, handles_partial_input_trailers_and_following_bytes)
{
    const std::string input = "4\r\nWiki\r\n5;ext=ok\r\npedia\r\n0\r\nX-Trailer: yes\r\n\r\nNEXT";
    const lh_usize_t expected = input.size() - 4U;
    for (lh_usize_t n = 0U; lh_math_lt(n, expected); n = lh_math_add_one(n))
    {
        lh_str_view_t view;
        lh_str_view_init_by_size(&view, input.data(), n);
        lh_usize_t consumed = 99U, decoded = 99U;
        EXPECT_EQ(lh_http_chunked_scan(view, &consumed, &decoded), lh_http_parse_incomplete) << n;
        EXPECT_EQ(consumed, 99U);
        EXPECT_EQ(decoded, 99U);
    }
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, input.data(), input.size());
    lh_usize_t consumed = 0U, decoded = 0U;
    EXPECT_EQ(lh_http_chunked_scan(view, &consumed, &decoded), lh_http_parse_complete);
    EXPECT_EQ(consumed, expected);
    EXPECT_EQ(decoded, 9U);
    EXPECT_EQ(
        lh_http_chunked_scan(lh_str_view_lit("0\r\nbad trailer\r\n\r\n"), &consumed, &decoded),
        lh_http_parse_error);
}

TEST(http_chunked_decode, compacts_binary_data_only_after_complete_validation)
{
    std::string input("2\r\na\0\r\n2\r\nbc\r\n0\r\n\r\nTAIL", 23U);
    lh_usize_t consumed = 17U, decoded = 17U;
    const std::string original = input;
    EXPECT_EQ(lh_http_chunked_decode(&input[0], 5U, &consumed, &decoded), lh_http_parse_incomplete);
    EXPECT_EQ(input, original);
    EXPECT_EQ(consumed, 17U);
    EXPECT_EQ(decoded, 17U);
    ASSERT_EQ(lh_http_chunked_decode(&input[0], input.size(), &consumed, &decoded),
              lh_http_parse_complete);
    EXPECT_EQ(consumed, 19U);
    EXPECT_EQ(decoded, 4U);
    EXPECT_EQ(input.substr(0, decoded), std::string("a\0bc", 4U));
    EXPECT_EQ(input.substr(consumed), "TAIL");
}

TEST(http_chunked_decode, rejects_invalid_framing_without_modifying_input)
{
    const char *cases[] = {"1\r\naXX0\r\n\r\n", "0\r\nBad trailer\r\n\r\n",
                           "1;bad=\r\na\r\n0\r\n\r\n", "0\n\n"};
    for (const char *value : cases)
    {
        std::string input(value);
        const std::string original = input;
        lh_usize_t consumed = 42U;
        lh_usize_t decoded = 42U;
        EXPECT_EQ(lh_http_chunked_decode(&input[0], input.size(), &consumed, &decoded),
                  lh_http_parse_error);
        EXPECT_EQ(input, original);
        EXPECT_EQ(consumed, 42U);
        EXPECT_EQ(decoded, 42U);
    }
}

TEST(http_chunked_decode, accepts_empty_body)
{
    char input[] = "0\r\n\r\n";
    lh_usize_t consumed = 0U;
    lh_usize_t decoded = 99U;
    EXPECT_EQ(lh_http_chunked_decode(input, sizeof(input) - 1U, &consumed, &decoded),
              lh_http_parse_complete);
    EXPECT_EQ(consumed, 5U);
    EXPECT_EQ(decoded, 0U);
}
