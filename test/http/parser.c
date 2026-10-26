#include <lh/http/parser.h>
#include <stdio.h>
#include <string.h>

static unsigned failures, checks;
#define CHECK(expression)                                                                          \
    do                                                                                             \
    {                                                                                              \
        ++checks;                                                                                  \
        if (!(expression))                                                                         \
        {                                                                                          \
            ++failures;                                                                            \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression);                       \
        }                                                                                          \
    } while (0)

static lh_str_view_t
view(const char *text, lh_usize_t size)
{
    lh_str_view_t result = lh_str_view_initializer(text, size);
    return result;
}

static int
equal(lh_str_view_t value, const char *text)
{
    return value.size == strlen(text) && !memcmp(value.begin, text, value.size);
}

static void
prefixes(void)
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
        lh_usize_t length = strlen(valid[i]);
        for (lh_usize_t prefix = 0; prefix < length; ++prefix)
        {
            CHECK(lh_http_parser_request(&parser, view(valid[i], prefix), &request, headers, 4) ==
                  lh_http_parse_incomplete);
            CHECK(lh_http_parser_get_consumed(&parser) == 0);
            CHECK(lh_http_parser_get_error(&parser) == lh_http_error_none);
        }
        CHECK(lh_http_parser_request(&parser, view(valid[i], length), &request, headers, 4) ==
              lh_http_parse_complete);
        CHECK(lh_http_parser_get_consumed(&parser) == length);
    }
    CHECK(request.header_count == 3);
    CHECK(equal(request.method, "CUSTOM!"));
    CHECK(equal(headers[0].name, "X-A"));
    CHECK(equal(headers[1].name, "x-a"));
    CHECK(equal(headers[2].value, ""));
    const char whitespace[] = "GET / HTTP/1.1\r\nX:\t  value\t \r\n\r\nBODY\0DATA";
    CHECK(lh_http_parser_request(&parser, view(whitespace, sizeof whitespace - 1), &request,
                                 headers, 4) == lh_http_parse_complete);
    CHECK(equal(headers[0].value, "value"));
    CHECK(lh_http_parser_get_consumed(&parser) == strstr(whitespace, "BODY") - whitespace);
    const char high_byte[] = "GET / HTTP/1.1\r\nX: \x80\xff\r\n\r\n";
    CHECK(lh_http_parser_request(&parser, view(high_byte, sizeof high_byte - 1), &request, headers,
                                 4) == lh_http_parse_complete);
    CHECK(headers[0].value.size == 2);
}

static void
invalid_requests(void)
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
        CHECK(lh_http_parser_request(&parser, view(invalid[i], strlen(invalid[i])), &request,
                                     headers, 4) == lh_http_parse_error);
        CHECK(lh_http_parser_get_error(&parser) != lh_http_error_none);
        CHECK(lh_http_parser_get_consumed(&parser) == 0);
        CHECK(lh_http_parser_get_error_offset(&parser) <= strlen(invalid[i]));
    }
    const char nul[] = "GET / HTTP/1.1\r\nX: a\0b\r\n\r\n";
    CHECK(lh_http_parser_request(&parser, view(nul, sizeof nul - 1), &request, headers, 4) ==
          lh_http_parse_error);
}

static void
limits(void)
{
    const char *empty = "GET / HTTP/1.1\r\n\r\n";
    const char *field = "GET / HTTP/1.1\r\nX: a\r\n\r\n";
    lh_http_parser_t parser;
    lh_http_request_t request;
    lh_http_header_t header;
    lh_http_parser_init(&parser);
    CHECK(lh_http_parser_request(&parser, view(empty, strlen(empty)), &request, NULL, 0) ==
          lh_http_parse_complete);
    CHECK(lh_http_parser_request(&parser, view(field, strlen(field)), &request, NULL, 0) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_get_error(&parser) == lh_http_error_header_capacity);
    CHECK(lh_http_parser_request(&parser, view(field, strlen(field)), &request, NULL, 1) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_get_error(&parser) == lh_http_error_argument);
    CHECK(lh_http_parser_request(&parser, view(NULL, 1), &request, &header, 1) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_request(&parser, view(NULL, 0), &request, &header, 1) ==
          lh_http_parse_incomplete);
    CHECK(lh_http_parser_request(NULL, view(empty, strlen(empty)), &request, &header, 1) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_request(&parser, view(empty, strlen(empty)), NULL, &header, 1) ==
          lh_http_parse_error);
    lh_http_parser_set_limits(&parser, strlen(empty), 14);
    CHECK(lh_http_parser_request(&parser, view(empty, strlen(empty)), &request, &header, 1) ==
          lh_http_parse_complete);
    lh_http_parser_set_limits(&parser, strlen(empty) - 1, 14);
    CHECK(lh_http_parser_request(&parser, view(empty, strlen(empty)), &request, &header, 1) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_get_error(&parser) == lh_http_error_head_size);
    lh_http_parser_set_limits(&parser, 8192, 13);
    CHECK(lh_http_parser_request(&parser, view(empty, strlen(empty)), &request, &header, 1) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_get_error(&parser) == lh_http_error_line_size);
    lh_http_parser_set_limits(&parser, 0, 14);
    CHECK(lh_http_parser_request(&parser, view(empty, strlen(empty)), &request, &header, 1) ==
          lh_http_parse_error);
    CHECK(lh_http_parser_get_error(&parser) == lh_http_error_argument);
}

static void
responses(void)
{
    const char *text = "HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\n";
    lh_http_parser_t parser;
    lh_http_response_t response;
    lh_http_header_t headers[2];
    lh_http_parser_init(&parser);
    for (lh_usize_t size = 0; size < strlen(text); ++size)
    {
        CHECK(lh_http_parser_response(&parser, view(text, size), &response, headers, 2) ==
              lh_http_parse_incomplete);
    }
    CHECK(lh_http_parser_response(&parser, view(text, strlen(text)), &response, headers, 2) ==
          lh_http_parse_complete);
    CHECK(response.status == 200 && response.version_minor == 1 && response.header_count == 1);
    CHECK(equal(response.reason, "OK"));
    CHECK(equal(headers[0].name, "Content-Length"));
    const char *bad[] = {"HTTP/1.1 099 Bad\r\n\r\n", "HTTP/1.1 600 Bad\r\n\r\n",
                         "HTTP/1.1 20 OK\r\n\r\n", "HTTP/1.1 200\r\n\r\n",
                         "HTTP/1.1 200 Bad\x01\r\n\r\n"};
    for (unsigned i = 0; i < sizeof bad / sizeof bad[0]; ++i)
    {
        CHECK(lh_http_parser_response(&parser, view(bad[i], strlen(bad[i])), &response, headers,
                                      2) == lh_http_parse_error);
    }
    text = "HTTP/1.0 204 \r\n\r\n";
    CHECK(lh_http_parser_response(&parser, view(text, strlen(text)), &response, headers, 2) ==
          lh_http_parse_complete);
    CHECK(response.reason.size == 0);
}

static void
arbitrary_bytes(void)
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
        unsigned size = iteration % sizeof input;
        for (unsigned i = 0; i < size; ++i)
        {
            seed = seed * 1664525U + 1013904223U;
            input[i] = (char)(seed >> 24);
        }
        guarded.before = guarded.after = 0x12345678U;
        lh_http_parse_status_t result =
            lh_http_parser_request(&parser, view(input, size), &request, guarded.headers, 2);
        CHECK(result <= lh_http_parse_error);
        CHECK(parser.consumed <= size);
        CHECK(guarded.before == 0x12345678U && guarded.after == 0x12345678U);
        result = lh_http_parser_response(&parser, view(input, size), &response, guarded.headers, 2);
        CHECK(result <= lh_http_parse_error);
        CHECK(parser.consumed <= size);
        CHECK(guarded.before == 0x12345678U && guarded.after == 0x12345678U);
    }
}

int
main(void)
{
    prefixes();
    invalid_requests();
    limits();
    responses();
    arbitrary_bytes();
    printf("HTTP parser: %u checks, %u failures\n", checks, failures);
    return failures ? 1 : 0;
}
