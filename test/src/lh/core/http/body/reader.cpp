/**
 * @file reader.cpp
 * @brief Streaming HTTP body reader tests.
 */

#include <gtest/gtest.h>

#include <lh/core/http/body/reader.h>
#include <lh/core/memory/std.h>
#include <lh/core/str/cursor.h>

#include <string>

lh_ssize_t
http_body_test_read(lh_self_ptr context, lh_ptr out, lh_usize_t capacity)
{
    lh_str_cursor_t *cursor = lh_ptr_cast(lh_str_cursor_t, context);
    lh_str_view_t rest;
    lh_str_cursor_get_rest(cursor, &rest);
    const lh_usize_t n = lh_math_min(2U, lh_math_min(capacity, lh_str_view_get_size(&rest)));
    if (n != 0U)
    {
        lh_memory_std_copy(out, lh_str_view_get_data(&rest), n);
        lh_str_cursor_advance(cursor, n);
    }
    return lh_cast_static(lh_ssize_t, n);
}

lh_ssize_t
http_body_test_pending(lh_self_ptr, lh_ptr, lh_usize_t)
{
    return LH_IO_PENDING;
}

TEST(http_body_reader_read, streams_length_and_preserves_following_bytes)
{
    lh_str_cursor_t source_cursor;
    lh_str_cursor_init(&source_cursor, lh_str_view_lit(""));
    lh_io_reader_t source;
    lh_io_reader_init(&source, http_body_test_read, &source_cursor);
    char wire[] = "abcTAIL";
    lh_http_body_reader_t body;
    lh_http_body_reader_init_length(&body, &source, wire, sizeof(wire) - 1U, 7U, 3U);
    lh_io_reader_t reader;
    lh_http_body_reader_get_reader(&body, &reader);
    char out[4];
    EXPECT_EQ(lh_io_reader_read(&reader, out, 2U), 2);
    EXPECT_EQ(lh_io_reader_read(&reader, out + 2, 2U), 1);
    EXPECT_EQ(lh_io_reader_read(&reader, out, 1U), 0);
    EXPECT_EQ(std::string(out, 3U), "abc");
    lh_str_view_t unread;
    lh_http_body_reader_get_unread(&body, &unread);
    EXPECT_EQ(std::string(lh_str_view_get_data(&unread), lh_str_view_get_size(&unread)), "TAIL");
}

TEST(http_body_reader_read, streams_chunked_across_fragments_and_reports_truncation)
{
    lh_str_cursor_t source_cursor;
    lh_str_cursor_init(&source_cursor, lh_str_view_lit("3\r\nabc\r\n2\r\nde\r\n0\r\n\r\n"));
    lh_io_reader_t source;
    lh_io_reader_init(&source, http_body_test_read, &source_cursor);
    char wire[3];
    char line[16];
    lh_http_chunked_decoder_t decoder;
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 20U, 64U);
    lh_http_body_reader_t body;
    lh_http_body_reader_init_chunked(&body, &source, wire, sizeof(wire), 0U, &decoder);
    char out[3];
    std::string result;
    for (;;)
    {
        const lh_ssize_t n = lh_http_body_reader_read(&body, out, sizeof(out));
        ASSERT_GE(n, 0);
        if (n == 0)
        {
            break;
        }
        result.append(out, static_cast<size_t>(n));
    }
    EXPECT_EQ(result, "abcde");
    lh_str_cursor_init(&source_cursor, lh_str_view_lit("3\r\na"));
    lh_http_chunked_decoder_init(&decoder, line, sizeof(line), 20U, 64U);
    lh_http_body_reader_init_chunked(&body, &source, wire, sizeof(wire), 0U, &decoder);
    EXPECT_EQ(lh_http_body_reader_read(&body, out, sizeof(out)), 1);
    EXPECT_EQ(lh_http_body_reader_read(&body, out, sizeof(out)), LH_IO_ERROR);
    EXPECT_EQ(lh_http_body_reader_read(&body, out, sizeof(out)), LH_IO_ERROR);
}

TEST(http_body_reader_read, preserves_pending_source_status)
{
    lh_io_reader_t source;
    lh_io_reader_init(&source, http_body_test_pending, nullptr);
    char wire[8];
    lh_http_body_reader_t body;
    lh_http_body_reader_init_length(&body, &source, wire, sizeof(wire), 0U, 3U);
    char out[8];
    EXPECT_EQ(lh_http_body_reader_read(&body, out, sizeof(out)), LH_IO_PENDING);
    EXPECT_FALSE(lh_http_body_reader_get_failed(&body));
}
