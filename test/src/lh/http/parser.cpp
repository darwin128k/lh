#include <gtest/gtest.h>
#include <lh/http/parser.h>
#include <cstring>
#include <string>

static lh_str_view_t
view(const char *text, lh_usize_t size)
{
    lh_str_view_t result = lh_str_view_initializer(text, size);
    return result;
}

static testing::AssertionResult
equal(lh_str_view_t value, const char *text)
{
    const std::string actual = value.size
                                   ? std::string(static_cast<const char *>(value.begin), value.size)
                                   : std::string();
    if (actual == text)
    {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << "actual view: " << testing::PrintToString(actual)
                                       << ", expected: " << testing::PrintToString(text);
}

TEST(http_parser, prefixes_complete_only_after_the_full_head)
{
    const char *valid[] = {"GET / HTTP/1.0\r\n\r\n",
                           "HEAD /maps/test.bsp HTTP/1.1\r\nHost: example\r\n\r\n",
                           "CUSTOM! /a?x=1 HTTP/1.1\r\nX-A: one\r\nx-a: two\r\nEmpty:\r\n\r\n"};
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_header_t headers[4];
    lh_http_parser_init(&parser);
    for (unsigned i = 0; i < sizeof valid / sizeof valid[0]; ++i)
    {
        SCOPED_TRACE(valid[i]);
        lh_usize_t length = std::strlen(valid[i]);
        for (lh_usize_t prefix = 0; prefix < length; ++prefix)
        {
            EXPECT_EQ(lh_http_parser_request(&parser, view(valid[i], prefix), &request, headers, 4),
                      lh_http_parse_incomplete);
            EXPECT_EQ(lh_http_parser_get_consumed(&parser), 0);
            EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_none);
        }
        ASSERT_EQ(lh_http_parser_request(&parser, view(valid[i], length), &request, headers, 4),
                  lh_http_parse_complete);
        EXPECT_EQ(lh_http_parser_get_consumed(&parser), length);
    }
    EXPECT_EQ(request.header_count, 3);
    EXPECT_TRUE(equal(request.method, "CUSTOM!"));
    EXPECT_TRUE(equal(headers[0].name, "X-A"));
    EXPECT_TRUE(equal(headers[1].name, "x-a"));
    EXPECT_TRUE(equal(headers[2].value, ""));
    const char whitespace[] = "GET / HTTP/1.1\r\nX:\t  value\t \r\n\r\nBODY\0DATA";
    ASSERT_EQ(lh_http_parser_request(&parser, view(whitespace, sizeof whitespace - 1), &request,
                                     headers, 4),
              lh_http_parse_complete);
    EXPECT_TRUE(equal(headers[0].value, "value"));
    EXPECT_EQ(lh_http_parser_get_consumed(&parser), std::strstr(whitespace, "BODY") - whitespace);
    const char high_byte[] = "GET / HTTP/1.1\r\nX: \x80\xff\r\n\r\n";
    ASSERT_EQ(lh_http_parser_request(&parser, view(high_byte, sizeof high_byte - 1), &request,
                                     headers, 4),
              lh_http_parse_complete);
    EXPECT_EQ(headers[0].value.size, 2);
}

TEST(http_parser, rejects_invalid_request_syntax)
{
    const char *invalid[] = {"GET / HTTP/1.1\n\n",
                             "GET / HTTP/1.1\rX",
                             "GET\t/ HTTP/1.1\r\n\r\n",
                             "GET  / HTTP/1.1\r\n\r\n",
                             "GET /  HTTP/1.1\r\n\r\n",
                             "GE(T / HTTP/1.1\r\n\r\n",
                             "GET / HTTP/2.0\r\n\r\n",
                             "GET / HTTP/1.10\r\n\r\n",
                             "GET / HTTP/1.1 \r\n\r\n",
                             "GET / HTTP/1.1\r\n folded: value\r\n\r\n",
                             "GET / HTTP/1.1\r\nX : value\r\n\r\n",
                             "GET / HTTP/1.1\r\n: value\r\n\r\n",
                             "GET / HTTP/1.1\r\nX\x01: value\r\n\r\n",
                             "GET / HTTP/1.1\r\nX: value\x7f\r\n\r\n",
                             "GET / HTTP/1.1\r\nX: value\r\n\tcontinued\r\n\r\n",
                             "GET /\x80 HTTP/1.1\r\n\r\n"};
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_header_t headers[4];
    lh_http_parser_init(&parser);
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; ++i)
    {
        SCOPED_TRACE(invalid[i]);
        EXPECT_EQ(lh_http_parser_request(&parser, view(invalid[i], std::strlen(invalid[i])),
                                         &request, headers, 4),
                  lh_http_parse_error);
        EXPECT_NE(lh_http_parser_get_error(&parser), lh_http_error_none);
        EXPECT_EQ(lh_http_parser_get_consumed(&parser), 0);
        EXPECT_LE(lh_http_parser_get_error_offset(&parser), std::strlen(invalid[i]));
    }
    const char nul[] = "GET / HTTP/1.1\r\nX: a\0b\r\n\r\n";
    EXPECT_EQ(lh_http_parser_request(&parser, view(nul, sizeof nul - 1), &request, headers, 4),
              lh_http_parse_error);
}

TEST(http_parser, enforces_limits_and_argument_contracts)
{
    const char *empty = "GET / HTTP/1.1\r\n\r\n";
    const char *field = "GET / HTTP/1.1\r\nX: a\r\n\r\n";
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_header_t header;
    lh_http_parser_init(&parser);
    ASSERT_EQ(
        lh_http_parser_request(&parser, view(empty, std::strlen(empty)), &request, nullptr, 0),
        lh_http_parse_complete);
    EXPECT_EQ(
        lh_http_parser_request(&parser, view(field, std::strlen(field)), &request, nullptr, 0),
        lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_header_capacity);
    EXPECT_EQ(
        lh_http_parser_request(&parser, view(field, std::strlen(field)), &request, nullptr, 1),
        lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_argument);
    EXPECT_EQ(lh_http_parser_request(&parser, view(nullptr, 1), &request, &header, 1),
              lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_request(&parser, view(nullptr, 0), &request, &header, 1),
              lh_http_parse_incomplete);
    EXPECT_EQ(
        lh_http_parser_request(nullptr, view(empty, std::strlen(empty)), &request, &header, 1),
        lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_request(&parser, view(empty, std::strlen(empty)), nullptr, &header, 1),
              lh_http_parse_error);
    lh_http_parser_set_limits(&parser, std::strlen(empty), 14);
    ASSERT_EQ(
        lh_http_parser_request(&parser, view(empty, std::strlen(empty)), &request, &header, 1),
        lh_http_parse_complete);
    lh_http_parser_set_limits(&parser, std::strlen(empty) - 1, 14);
    EXPECT_EQ(
        lh_http_parser_request(&parser, view(empty, std::strlen(empty)), &request, &header, 1),
        lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_head_size);
    lh_http_parser_set_limits(&parser, 8192, 13);
    EXPECT_EQ(
        lh_http_parser_request(&parser, view(empty, std::strlen(empty)), &request, &header, 1),
        lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_line_size);
    lh_http_parser_set_limits(&parser, 0, 14);
    EXPECT_EQ(
        lh_http_parser_request(&parser, view(empty, std::strlen(empty)), &request, &header, 1),
        lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_argument);
}

TEST(http_parser, parses_responses_and_rejects_invalid_status_lines)
{
    const char *text = "HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\n";
    lh_http_parser_t parser;
    lh_http_response_t response;
    lh_http_header_t headers[2];
    lh_http_parser_init(&parser);
    for (lh_usize_t size = 0; size < std::strlen(text); ++size)
    {
        EXPECT_EQ(lh_http_parser_response(&parser, view(text, size), &response, headers, 2),
                  lh_http_parse_incomplete);
    }
    ASSERT_EQ(
        lh_http_parser_response(&parser, view(text, std::strlen(text)), &response, headers, 2),
        lh_http_parse_complete);
    EXPECT_TRUE(response.status == 200 && response.version_minor == 1 &&
                response.header_count == 1);
    EXPECT_TRUE(equal(response.reason, "OK"));
    EXPECT_TRUE(equal(headers[0].name, "Content-Length"));
    const char *bad[] = {"HTTP/1.1 099 Bad\r\n\r\n", "HTTP/1.1 600 Bad\r\n\r\n",
                         "HTTP/1.1 20 OK\r\n\r\n", "HTTP/1.1 200\r\n\r\n",
                         "HTTP/1.1 200 Bad\x01\r\n\r\n"};
    for (unsigned i = 0; i < sizeof bad / sizeof bad[0]; ++i)
    {
        EXPECT_EQ(lh_http_parser_response(&parser, view(bad[i], std::strlen(bad[i])), &response,
                                          headers, 2),
                  lh_http_parse_error);
    }
    text = "HTTP/1.0 204 \r\n\r\n";
    ASSERT_EQ(
        lh_http_parser_response(&parser, view(text, std::strlen(text)), &response, headers, 2),
        lh_http_parse_complete);
    EXPECT_EQ(response.reason.size, 0);
}

TEST(http_parser, arbitrary_bytes_preserve_bounds)
{
    lh_u32_t seed = 0x594ea13U;
    char input[512];
    struct guarded
    {
        lh_u32_t before;
        lh_http_header_t headers[2];
        lh_u32_t after;
    } guarded;
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_response_t response;
    lh_http_parser_init(&parser);
    lh_http_parser_set_limits(&parser, 256, 128);
    for (unsigned iteration = 0; iteration < 10000; ++iteration)
    {
        SCOPED_TRACE(iteration);
        unsigned size = iteration % sizeof input;
        for (unsigned i = 0; i < size; ++i)
        {
            seed = seed * 1664525U + 1013904223U;
            input[i] = static_cast<char>(seed >> 24);
        }
        guarded.before = guarded.after = 0x12345678U;
        lh_http_parse_status_t result =
            lh_http_parser_request(&parser, view(input, size), &request, guarded.headers, 2);
        EXPECT_LE(result, lh_http_parse_error);
        EXPECT_LE(parser.consumed, size);
        EXPECT_TRUE(guarded.before == 0x12345678U && guarded.after == 0x12345678U);
        result = lh_http_parser_response(&parser, view(input, size), &response, guarded.headers, 2);
        EXPECT_LE(result, lh_http_parse_error);
        EXPECT_LE(parser.consumed, size);
        EXPECT_TRUE(guarded.before == 0x12345678U && guarded.after == 0x12345678U);
    }
}

/* -- Steps -------------------------------------------------------------------- */

TEST(http_parser_fail, records_and_reports_an_error)
{
    lh_http_parser_t parser;
    lh_http_parser_init(&parser);
    EXPECT_EQ(lh_http_parser_fail(&parser, lh_http_error_header, 12), lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_header);
    EXPECT_EQ(lh_http_parser_get_error_offset(&parser), 12u);
}

TEST(http_parser_begin, clears_diagnostics_and_checks_arguments)
{
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_parser_init(&parser);
    lh_http_parser_fail(&parser, lh_http_error_header, 3);
    EXPECT_TRUE(lh_http_parser_begin(&parser, view("", 0), &request, nullptr, 0));
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_none);
    EXPECT_FALSE(lh_http_parser_begin(&parser, view("", 0), nullptr, nullptr, 0));
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_argument);
    EXPECT_FALSE(lh_http_parser_begin(&parser, view("", 0), &request, nullptr, 2));
    EXPECT_FALSE(lh_http_parser_begin(nullptr, view("", 0), &request, nullptr, 0));
}

TEST(http_parser_find_line, finds_crlf_and_rejects_bare_endings)
{
    lh_http_parser_t parser;
    lh_http_parser_init(&parser);
    lh_usize_t end = 0;
    EXPECT_EQ(lh_http_parser_find_line(&parser, view("ab\r\ncd", 6), 0, &end),
              lh_http_parse_complete);
    EXPECT_EQ(end, 2u);
    EXPECT_EQ(lh_http_parser_find_line(&parser, view("ab\r", 3), 0, &end),
              lh_http_parse_incomplete);
    EXPECT_EQ(lh_http_parser_find_line(&parser, view("ab\ncd", 5), 0, &end), lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error(&parser), lh_http_error_line_ending);
    EXPECT_EQ(lh_http_parser_get_error_offset(&parser), 2u);
}

TEST(http_parser_parse_field, splits_name_and_trims_the_value)
{
    lh_http_parser_t parser;
    lh_http_header_t header;
    lh_http_parser_init(&parser);
    const char line[] = "X-Name: \t value \t";
    ASSERT_EQ(lh_http_parser_parse_field(&parser, view(line, sizeof line - 1), 0, sizeof line - 1,
                                         &header),
              lh_http_parse_complete);
    EXPECT_TRUE(equal(header.name, "X-Name"));
    EXPECT_TRUE(equal(header.value, "value"));
    EXPECT_EQ(lh_http_parser_parse_field(&parser, view("Bad Name: v", 11), 0, 11, &header),
              lh_http_parse_error);
    EXPECT_EQ(lh_http_parser_get_error_offset(&parser), 3u);
}

TEST(http_parser_parse_status_line, reads_code_and_reason)
{
    lh_http_parser_t parser;
    lh_http_response_t response;
    lh_http_parser_init(&parser);
    const char line[] = "HTTP/1.1 404 Not Found";
    ASSERT_EQ(lh_http_parser_parse_status_line(&parser, view(line, sizeof line - 1),
                                               sizeof line - 1, &response),
              lh_http_parse_complete);
    EXPECT_EQ(response.status, 404);
    EXPECT_EQ(response.version_minor, 1);
    EXPECT_TRUE(equal(response.reason, "Not Found"));
    EXPECT_EQ(lh_http_parser_parse_status_line(&parser, view("HTTP/1.1 600 X", 14), 14, &response),
              lh_http_parse_error);
}
