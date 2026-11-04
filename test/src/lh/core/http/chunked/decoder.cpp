/**
 * @file decoder.cpp
 * @brief Incremental chunked decoding across arbitrary input and output sizes.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/core/http/chunked/decoder.h>

#include <string>

TEST(http_chunked_decoder_feed, decodes_one_byte_at_a_time_and_stops_before_next_message)
{
    const std::string wire("2\r\na\0\r\n2;z=1\r\nbc\r\n0\r\nX: y\r\n\r\nNEXT", 33U);
    char line[32];
    lh_http_chunked_decoder_t decoder;
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 4U, 64U);
    std::string result;
    lh_usize_t offset = 0U;
    lh_http_parse_status_t status = lh_http_parse_incomplete;
    while (offset < wire.size() && status == lh_http_parse_incomplete)
    {
        lh_str_view_t input;
        lh_str_view_init_by_size(&input, wire.data() + offset, 1U);
        char output[1];
        lh_usize_t consumed = 0U;
        lh_usize_t written = 0U;
        status = lh_http_chunked_decoder_feed(&decoder, input, output, sizeof(output), &consumed,
                                              &written);
        ASSERT_LE(consumed, 1U);
        ASSERT_LE(written, 1U);
        ASSERT_TRUE(consumed || written);
        offset += consumed;
        result.append(output, written);
    }
    EXPECT_EQ(status, lh_http_parse_complete);
    EXPECT_EQ(result, std::string("a\0bc", 4U));
    EXPECT_EQ(wire.substr(offset), "NEXT");
    EXPECT_EQ(lh_http_chunked_decoder_get_decoded(&decoder), 4U);
}

TEST(http_chunked_decoder_feed, resumes_when_output_is_full)
{
    char line[16];
    lh_http_chunked_decoder_t decoder;
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 10U, 64U);
    const lh_str_view_t wire = lh_str_view_lit("4\r\ndata\r\n0\r\n\r\n");
    char output[4];
    lh_usize_t consumed = 0U, written = 0U;
    EXPECT_EQ(lh_http_chunked_decoder_feed(&decoder, wire, output, 2U, &consumed, &written),
              lh_http_parse_incomplete);
    EXPECT_EQ(consumed, 5U);
    EXPECT_EQ(written, 2U);
    lh_str_view_t rest;
    lh_str_view_tail(&wire, consumed, &rest);
    lh_usize_t tail = 0U, more = 0U;
    EXPECT_EQ(lh_http_chunked_decoder_feed(&decoder, rest, output + 2, 2U, &tail, &more),
              lh_http_parse_complete);
    EXPECT_EQ(more, 2U);
    EXPECT_EQ(std::string(output, 4U), "data");
}

TEST(http_chunked_decoder_feed, enforces_body_metadata_and_line_limits)
{
    char line[4];
    char output[8];
    lh_http_chunked_decoder_t decoder;
    lh_usize_t consumed = 0U, written = 0U;
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 1U, 64U);
    EXPECT_EQ(lh_http_chunked_decoder_feed(&decoder, lh_str_view_lit("2\r\nab\r\n0\r\n\r\n"),
                                           output, sizeof(output), &consumed, &written),
              lh_http_parse_error);
    EXPECT_EQ(written, 0U);
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 8U, 3U);
    EXPECT_EQ(lh_http_chunked_decoder_feed(&decoder, lh_str_view_lit("0\r\n\r\n"), output,
                                           sizeof(output), &consumed, &written),
              lh_http_parse_error);
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 8U, 64U);
    EXPECT_EQ(lh_http_chunked_decoder_feed(&decoder, lh_str_view_lit("00000\r\n"), output,
                                           sizeof(output), &consumed, &written),
              lh_http_parse_error);
    EXPECT_EQ(lh_http_chunked_decoder_feed(&decoder, lh_str_view_lit(""), output, sizeof(output),
                                           &consumed, &written),
              lh_http_parse_error);
}

TEST(http_chunked_decoder_finish_line, checks_invalid_state)
{
    char line[8];
    lh_http_chunked_decoder_t decoder;
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 8U, 64U);
    EXPECT_FALSE(lh_http_chunked_decoder_try_finish_line(&decoder));
    LH_EXPECT_CHECK_DEATH(lh_http_chunked_decoder_finish_line(&decoder));
}
