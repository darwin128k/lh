/**
 * @file format.cpp
 * @brief HTTP response and field serialization tests.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/core/http/format.h>
#include <lh/core/http/parser.h>

#include <string>

TEST(http_header_set, stores_borrowed_name_and_value)
{
    lh_http_header_t header;
    const lh_str_view_t name = lh_str_view_initializer_lit("Content-Type");
    const lh_str_view_t value = lh_str_view_initializer_lit("text/plain");
    lh_http_header_set(lh_addr_of(header), name, value);
    EXPECT_TRUE(lh_str_view_equals(lh_http_header_get_name(lh_addr_of(header)), lh_addr_of(name),
                                   lh_bool_false));
    EXPECT_TRUE(lh_str_view_equals(lh_http_header_get_value(lh_addr_of(header)), lh_addr_of(value),
                                   lh_bool_false));
}

TEST(http_format_status_line, writes_a_valid_line_and_rejects_invalid_status)
{
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), 200);
    lh_http_response_set_version_minor(lh_addr_of(response), 1);
    const lh_str_view_t reason = lh_str_view_initializer_lit("OK");
    lh_http_response_set_reason(lh_addr_of(response), reason);
    char out[64];
    lh_usize_t offset = 0;
    ASSERT_TRUE(
        lh_http_format_try_status_line(lh_addr_of(response), out, sizeof(out), lh_addr_of(offset)));
    EXPECT_EQ(std::string(out, offset), "HTTP/1.1 200 OK\r\n");
    lh_http_response_set_status(lh_addr_of(response), 99);
    offset = 0;
    EXPECT_FALSE(
        lh_http_format_try_status_line(lh_addr_of(response), out, sizeof(out), lh_addr_of(offset)));
    EXPECT_EQ(offset, 0u);
    LH_EXPECT_CHECK_DEATH(
        lh_http_format_status_line(lh_addr_of(response), out, sizeof(out), lh_addr_of(offset)));
    lh_http_response_set_status(lh_addr_of(response), 200);
    lh_http_response_set_version_minor(lh_addr_of(response), 10);
    EXPECT_FALSE(
        lh_http_format_try_status_line(lh_addr_of(response), out, sizeof(out), lh_addr_of(offset)));
}

TEST(http_format_header, writes_fields_and_rejects_insufficient_space)
{
    lh_http_header_t header;
    const lh_str_view_t name = lh_str_view_initializer_lit("X-Test");
    const lh_str_view_t value = lh_str_view_initializer_lit("hello");
    lh_http_header_set(lh_addr_of(header), name, value);
    char out[32];
    lh_usize_t offset = 0;
    ASSERT_TRUE(
        lh_http_format_try_header(lh_addr_of(header), out, sizeof(out), lh_addr_of(offset)));
    EXPECT_EQ(std::string(out, offset), "X-Test: hello\r\n");
    offset = 0;
    EXPECT_FALSE(lh_http_format_try_header(lh_addr_of(header), out, 1, lh_addr_of(offset)));
    offset = 0;
    LH_EXPECT_CHECK_DEATH(lh_http_format_header(lh_addr_of(header), out, 1, lh_addr_of(offset)));
}

TEST(http_format_response_head, roundtrips_status_and_fields)
{
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), 101);
    lh_http_response_set_version_minor(lh_addr_of(response), 1);
    const lh_str_view_t reason = lh_str_view_initializer_lit("Switching Protocols");
    lh_http_response_set_reason(lh_addr_of(response), reason);
    lh_http_response_set_header_count(lh_addr_of(response), 2);
    lh_http_header_t fields[2];
    const lh_str_view_t name1 = lh_str_view_initializer_lit("Upgrade");
    const lh_str_view_t value1 = lh_str_view_initializer_lit("websocket");
    const lh_str_view_t name2 = lh_str_view_initializer_lit("X-Empty");
    const lh_str_view_t value2 = lh_str_view_initializer_lit("");
    lh_http_header_set(lh_addr_of(fields[0]), name1, value1);
    lh_http_header_set(lh_addr_of(fields[1]), name2, value2);
    char out[128];
    lh_usize_t offset = 0;
    ASSERT_TRUE(lh_http_format_try_response_head(lh_addr_of(response), fields, out, sizeof(out),
                                                 lh_addr_of(offset)));
    EXPECT_EQ(std::string(out, offset),
              "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nX-Empty: \r\n\r\n");
    lh_http_parser_t parser;
    lh_http_parser_init(lh_addr_of(parser));
    lh_http_response_t parsed;
    lh_http_header_t parsed_fields[2];
    const lh_str_view_t input = lh_str_view_initializer(out, offset);
    ASSERT_EQ(
        lh_http_parser_response(lh_addr_of(parser), input, lh_addr_of(parsed), parsed_fields, 2),
        lh_http_parse_complete);
    EXPECT_EQ(lh_http_response_get_status(lh_addr_of(parsed)), 101u);
    EXPECT_EQ(lh_http_response_get_header_count(lh_addr_of(parsed)), 2u);
    EXPECT_TRUE(lh_str_view_equals(lh_http_header_get_value(lh_addr_of(parsed_fields[0])),
                                   lh_addr_of(value1), lh_bool_false));
    offset = 0;
    EXPECT_FALSE(
        lh_http_format_try_response_head(lh_addr_of(response), fields, out, 4, lh_addr_of(offset)));
    offset = 0;
    LH_EXPECT_CHECK_DEATH(
        lh_http_format_response_head(lh_addr_of(response), fields, out, 4, lh_addr_of(offset)));
}

TEST(http_format_response_head, supports_empty_reason_no_fields_and_prefix)
{
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), 418);
    lh_http_response_set_version_minor(lh_addr_of(response), 0);
    lh_http_response_set_reason(lh_addr_of(response), lh_str_view_empty());
    lh_http_response_set_header_count(lh_addr_of(response), 0);
    char out[32] = {'x', 'y'};
    lh_usize_t offset = 2;

    ASSERT_TRUE(lh_http_format_try_response_head(lh_addr_of(response), nullptr, out, sizeof(out),
                                                 lh_addr_of(offset)));
    EXPECT_EQ(std::string(out, offset), "xyHTTP/1.0 418 \r\n\r\n");
}
