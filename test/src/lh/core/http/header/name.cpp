/**
 * @file name.cpp
 * @brief Standard HTTP field names and values are reusable borrowed views.
 */

#include <gtest/gtest.h>

#include <lh/core/http/header/name.h>
#include <lh/core/http/header/value.h>

TEST(http_header_name, provides_the_protocol_names)
{
    const lh_str_view_t actual[] = {
        lh_http_header_name_content_type,      lh_http_header_name_content_length,
        lh_http_header_name_transfer_encoding, lh_http_header_name_connection,
        lh_http_header_name_cache_control,     lh_http_header_name_upgrade,
        lh_http_header_name_websocket_key,     lh_http_header_name_websocket_accept};
    const lh_str_view_t expected[] = {
        lh_str_view_lit("Content-Type"),      lh_str_view_lit("Content-Length"),
        lh_str_view_lit("Transfer-Encoding"), lh_str_view_lit("Connection"),
        lh_str_view_lit("Cache-Control"),     lh_str_view_lit("Upgrade"),
        lh_str_view_lit("Sec-WebSocket-Key"), lh_str_view_lit("Sec-WebSocket-Accept")};
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_array_ptr_get_size(actual)); i = lh_math_add_one(i))
    {
        EXPECT_TRUE(
            lh_str_view_equals(lh_addr_of(actual[i]), lh_addr_of(expected[i]), lh_bool_false));
    }
}

TEST(http_header_value, provides_the_protocol_tokens)
{
    const lh_str_view_t actual[] = {lh_http_header_value_close, lh_http_header_value_no_store,
                                    lh_http_header_value_websocket, lh_http_header_value_upgrade,
                                    lh_http_header_value_chunked};
    const lh_str_view_t expected[] = {lh_str_view_lit("close"), lh_str_view_lit("no-store"),
                                      lh_str_view_lit("websocket"), lh_str_view_lit("Upgrade"),
                                      lh_str_view_lit("chunked")};
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_array_ptr_get_size(actual)); i = lh_math_add_one(i))
    {
        EXPECT_TRUE(
            lh_str_view_equals(lh_addr_of(actual[i]), lh_addr_of(expected[i]), lh_bool_false));
    }
}
