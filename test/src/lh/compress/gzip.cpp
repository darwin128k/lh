/**
 * @file gzip.cpp
 * @brief Gzip member checks and independent compression vectors.
 */

#include <gtest/gtest.h>
#include <lh/compress/gzip.h>
#include <lh/compress/gzip/reader.h>
#include <lh/hash/crc.h>
#include <lh/compress/gzip/header/decoder.h>
#include <lh/expect/death.h>
#include <lh/size/limits.h>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

const lh_byte_t gzip_sample[] = {31,  139, 8,   0,   0,   0,   0,   0,   2,   255, 243,
                                 72,  205, 201, 201, 87,  72,  175, 202, 44,  80,  100,
                                 240, 24,  101, 143, 178, 71,  217, 163, 236, 65,  204,
                                 6,   0,   141, 195, 147, 162, 176, 4,   0,   0};

TEST(compress_gzip, decodes_concatenated_members_and_checks_trailer)
{
    std::vector<lh_byte_t> input(gzip_sample, gzip_sample + sizeof(gzip_sample));
    input.insert(input.end(), gzip_sample, gzip_sample + sizeof(gzip_sample));
    std::string expected;
    for (int i = 0; i < 200; ++i)
    {
        expected.append("Hello gzip!\0", 12);
    }
    std::vector<lh_byte_t> output(expected.size());
    lh_usize_t written = 0;
    ASSERT_TRUE(lh_compress_gzip_try_decode(input.data(), input.size(), output.data(),
                                            output.size(), &written));
    EXPECT_EQ(written, expected.size());
    EXPECT_EQ(std::string(reinterpret_cast<char *>(output.data()), output.size()), expected);
    input.back() ^= 1U;
    written = 42;
    EXPECT_FALSE(lh_compress_gzip_try_decode(input.data(), input.size(), output.data(),
                                             output.size(), &written));
    EXPECT_EQ(written, 42U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_decode(input.data(), input.size(), output.data(),
                                                  output.size(), &written));
    EXPECT_FALSE(lh_compress_gzip_try_decode(input.data(), input.size() - 1, output.data(),
                                             output.size(), &written));
}

TEST(compress_gzip, validates_optional_header_fields_and_header_crc)
{
    std::vector<lh_byte_t> wire = {31, 139, 8,   30,  0,   0,   0,   0,   2,   255, 3,   0,
                                   97, 98,  99,  102, 105, 108, 101, 110, 97,  109, 101, 0,
                                   99, 111, 109, 109, 101, 110, 116, 0,   121, 41,  203, 47,
                                   40, 201, 204, 207, 75,  204, 81,  200, 72,  77,  76,  73,
                                   45, 2,   0,   135, 48,  239, 52,  15,  0,   0,   0};
    lh_byte_t out[64];
    lh_usize_t written = 0;
    ASSERT_TRUE(lh_compress_gzip_try_decode(wire.data(), wire.size(), out, sizeof(out), &written));
    EXPECT_EQ(std::string(reinterpret_cast<char *>(out), written), "optional header");
    wire[13] ^= 1U;
    EXPECT_FALSE(lh_compress_gzip_try_decode(wire.data(), wire.size(), out, sizeof(out), &written));
    wire[3] |= 128U;
    EXPECT_FALSE(lh_compress_gzip_try_decode(wire.data(), wire.size(), out, sizeof(out), &written));
}

TEST(compress_gzip_header, validates_and_preserves_result_on_failure)
{
    lh_usize_t size = 77U;
    lh_compress_gzip_header(gzip_sample, sizeof(gzip_sample), &size);
    EXPECT_EQ(size, 10U);
    size = 77U;
    EXPECT_FALSE(lh_compress_gzip_try_header(gzip_sample, 9U, &size));
    EXPECT_EQ(size, 77U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_header(gzip_sample, 9U, &size));
}

TEST(compress_gzip_trailer, verifies_content_and_length)
{
    std::string decoded;
    for (int i = 0; i < 100; ++i)
    {
        decoded.append("Hello gzip!\0", 12);
    }
    const lh_byte_t *trailer = gzip_sample + sizeof(gzip_sample) - 8U;
    lh_compress_gzip_trailer(trailer, 8U, reinterpret_cast<const lh_byte_t *>(decoded.data()),
                             decoded.size());
    EXPECT_FALSE(lh_compress_gzip_try_trailer(
        trailer, 7U, reinterpret_cast<const lh_byte_t *>(decoded.data()), decoded.size()));
    EXPECT_FALSE(lh_compress_gzip_try_trailer(
        trailer, 8U, reinterpret_cast<const lh_byte_t *>(decoded.data()), decoded.size() - 1U));
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_trailer(
        trailer, 7U, reinterpret_cast<const lh_byte_t *>(decoded.data()), decoded.size()));
}

TEST(compress_gzip_header_decoder, resumes_one_byte_at_a_time_and_stops_at_payload)
{
    const lh_byte_t input[] = {31, 139, 8,   30,  0,   0,   0,   0,   2,   255, 3,   0,
                               97, 98,  99,  102, 105, 108, 101, 110, 97,  109, 101, 0,
                               99, 111, 109, 109, 101, 110, 116, 0,   121, 41,  75,  73,
                               44, 73,  4,   0,   99,  243, 243, 173, 4,   0,   0,   0};
    lh_compress_gzip_header_decoder_t decoder;
    lh_compress_gzip_header_decoder_init(&decoder, sizeof(input));
    lh_usize_t total = 0U;
    while (lh_compress_gzip_header_decoder_get_phase(&decoder) !=
           lh_compress_gzip_header_decoder_phase_done)
    {
        lh_usize_t consumed = 99U;
        ASSERT_LT(total, sizeof(input));
        const auto phase =
            lh_compress_gzip_header_decoder_feed(&decoder, input + total, 1U, &consumed);
        ASSERT_NE(phase, lh_compress_gzip_header_decoder_phase_error);
        ASSERT_EQ(consumed, 1U);
        total += consumed;
    }
    EXPECT_EQ(total, 34U);
    EXPECT_EQ(lh_compress_gzip_header_decoder_get_size(&decoder), total);
    lh_usize_t consumed = 99U;
    EXPECT_EQ(lh_compress_gzip_header_decoder_feed(&decoder, input + total, sizeof(input) - total,
                                                   &consumed),
              lh_compress_gzip_header_decoder_phase_done);
    EXPECT_EQ(consumed, 0U);
}

TEST(compress_gzip_header_decoder, enforces_header_limit_and_sticky_errors)
{
    lh_compress_gzip_header_decoder_t decoder;
    lh_compress_gzip_header_decoder_init(&decoder, 9U);
    lh_usize_t consumed = 99U;
    EXPECT_EQ(lh_compress_gzip_header_decoder_try_feed(&decoder, gzip_sample, sizeof(gzip_sample),
                                                       &consumed),
              lh_compress_gzip_header_decoder_phase_error);
    EXPECT_EQ(consumed, 9U);
    EXPECT_EQ(lh_compress_gzip_header_decoder_get_size(&decoder), 9U);
    EXPECT_EQ(lh_compress_gzip_header_decoder_try_feed(&decoder, gzip_sample, sizeof(gzip_sample),
                                                       &consumed),
              lh_compress_gzip_header_decoder_phase_error);
    EXPECT_EQ(consumed, 0U);
    lh_compress_gzip_header_decoder_init(&decoder, 10U);
    EXPECT_EQ(
        lh_compress_gzip_header_decoder_feed(&decoder, gzip_sample, sizeof(gzip_sample), &consumed),
        lh_compress_gzip_header_decoder_phase_done);
    EXPECT_EQ(consumed, 10U);
}

TEST(compress_gzip_header_decoder, rejects_bad_magic_and_bad_header_crc)
{
    lh_byte_t bad_magic[] = {0, 139, 8, 0};
    lh_compress_gzip_header_decoder_t decoder;
    lh_compress_gzip_header_decoder_init(&decoder, 100U);
    lh_usize_t consumed = 0U;
    EXPECT_EQ(
        lh_compress_gzip_header_decoder_try_feed(&decoder, bad_magic, sizeof(bad_magic), &consumed),
        lh_compress_gzip_header_decoder_phase_error);
    EXPECT_EQ(consumed, 1U);
    lh_compress_gzip_header_decoder_init(&decoder, 100U);
    LH_EXPECT_CHECK_DEATH(
        lh_compress_gzip_header_decoder_feed(&decoder, bad_magic, sizeof(bad_magic), &consumed));
    lh_byte_t bad_crc[] = {31, 139, 8,   30,  0,   0,   0,   0,   2,   255, 3,   0,
                           97, 98,  99,  102, 105, 108, 101, 110, 97,  109, 101, 0,
                           99, 111, 109, 109, 101, 110, 116, 0,   121, 40};
    lh_compress_gzip_header_decoder_init(&decoder, sizeof(bad_crc));
    EXPECT_EQ(
        lh_compress_gzip_header_decoder_try_feed(&decoder, bad_crc, sizeof(bad_crc), &consumed),
        lh_compress_gzip_header_decoder_phase_error);
    EXPECT_EQ(consumed, sizeof(bad_crc));
}

TEST(compress_gzip_trailer_values, validates_incrementally_computed_crc)
{
    std::string decoded;
    for (int i = 0; i < 100; ++i)
    {
        decoded.append("Hello gzip!\0", 12);
    }
    const lh_u32_t first = lh_hash_crc32_update(0U, decoded.data(), 37U);
    const lh_u32_t crc = lh_hash_crc32_update(first, decoded.data() + 37U, decoded.size() - 37U);
    const lh_byte_t *trailer = gzip_sample + sizeof(gzip_sample) - 8U;
    lh_compress_gzip_trailer_values(trailer, 8U, crc, decoded.size());
    EXPECT_FALSE(lh_compress_gzip_try_trailer_values(trailer, 8U, crc ^ 1U, decoded.size()));
    EXPECT_FALSE(lh_compress_gzip_try_trailer_values(trailer, 7U, crc, decoded.size()));
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_trailer_values(trailer, 8U, crc ^ 1U, decoded.size()));
}

struct gzip_reader_test_source
{
    const lh_byte_t *data;
    size_t size;
    size_t position;
    bool pending;
};

lh_ssize_t
gzip_reader_test_read(lh_self_ptr context, lh_ptr output, lh_usize_t size)
{
    auto *source = static_cast<gzip_reader_test_source *>(context);
    if (size == 0U)
    {
        return 0;
    }
    if (source->pending)
    {
        source->pending = false;
        return LH_IO_PENDING;
    }
    source->pending = true;
    if (source->position == source->size)
    {
        return 0;
    }
    *static_cast<lh_byte_t *>(output) = source->data[source->position++];
    return 1;
}

TEST(compress_gzip_reader, streams_concatenated_members_across_pending_boundaries)
{
    std::vector<lh_byte_t> input(gzip_sample, gzip_sample + sizeof(gzip_sample));
    input.insert(input.end(), gzip_sample, gzip_sample + sizeof(gzip_sample));
    gzip_reader_test_source source{input.data(), input.size(), 0U, false};
    lh_io_reader_t io;
    lh_io_reader_init(&io, gzip_reader_test_read, &source);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t gzip;
    lh_compress_gzip_reader_init(&gzip, &io, &header, &deflate, scratch, history, 2400U, 36U);
    lh_io_reader_t reader;
    lh_compress_gzip_reader_get_reader(&gzip, &reader);
    std::string result;
    for (;;)
    {
        lh_byte_t out[7];
        const lh_ssize_t n = lh_io_reader_read(&reader, out, sizeof(out));
        if (n == LH_IO_PENDING)
        {
            continue;
        }
        ASSERT_GE(n, 0);
        if (n == 0)
        {
            break;
        }
        result.append(reinterpret_cast<char *>(out), static_cast<size_t>(n));
    }
    std::string expected;
    for (int i = 0; i < 200; ++i)
    {
        expected.append("Hello gzip!\0", 12);
    }
    EXPECT_EQ(result, expected);
    EXPECT_EQ(source.position, input.size());
    EXPECT_EQ(lh_compress_gzip_reader_get_total(&gzip), 2400U);
    EXPECT_EQ(lh_compress_gzip_reader_get_metadata(&gzip), 36U);
}

TEST(compress_gzip_reader, reports_bad_crc_and_independent_limits_after_partial_output)
{
    for (int scenario = 0; scenario < 4; ++scenario)
    {
        std::vector<lh_byte_t> input(gzip_sample, gzip_sample + sizeof(gzip_sample));
        if (scenario == 0)
        {
            input[input.size() - 8] ^= 1U;
        }
        if (scenario == 3)
        {
            input.pop_back();
        }
        gzip_reader_test_source source{input.data(), input.size(), 0U, false};
        lh_io_reader_t io;
        lh_io_reader_init(&io, gzip_reader_test_read, &source);
        lh_compress_gzip_header_decoder_t header;
        lh_compress_deflate_reader_t deflate;
        lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
        lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
        lh_compress_gzip_reader_t reader;
        lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history,
                                     scenario == 1 ? 100U : 1200U, scenario == 2 ? 17U : 18U);
        lh_byte_t out[31];
        size_t total = 0U;
        for (;;)
        {
            const lh_ssize_t n = lh_compress_gzip_reader_read(&reader, out, sizeof(out));
            if (n == LH_IO_PENDING)
            {
                continue;
            }
            if (n > 0)
            {
                total += static_cast<size_t>(n);
                continue;
            }
            EXPECT_EQ(n, LH_IO_ERROR) << scenario;
            break;
        }
        EXPECT_GT(total, 0U) << scenario;
        EXPECT_EQ(lh_compress_gzip_reader_read(&reader, out, sizeof(out)), LH_IO_ERROR);
    }
}

TEST(compress_gzip_reader_header_byte, checks_invalid_header)
{
    lh_io_reader_t source;
    lh_io_reader_init(&source, gzip_reader_test_read, nullptr);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t reader;
    lh_compress_gzip_reader_init(&reader, &source, &header, &deflate, scratch, history, 1U, 18U);
    EXPECT_FALSE(lh_compress_gzip_reader_try_header_byte(&reader, 0U));
    lh_compress_gzip_reader_init(&reader, &source, &header, &deflate, scratch, history, 1U, 18U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_reader_header_byte(&reader, 0U));
}

TEST(compress_gzip_reader, streams_optional_header_and_rejects_bad_header_crc)
{
    const lh_byte_t fixture[] = {31, 139, 8,   30,  0,   0,   0,   0,   2,   255, 3,   0,
                                 97, 98,  99,  102, 105, 108, 101, 110, 97,  109, 101, 0,
                                 99, 111, 109, 109, 101, 110, 116, 0,   121, 41,  203, 47,
                                 40, 201, 204, 207, 75,  204, 81,  200, 72,  77,  76,  73,
                                 45, 2,   0,   135, 48,  239, 52,  15,  0,   0,   0};
    for (bool corrupt : {false, true})
    {
        std::vector<lh_byte_t> data(fixture, fixture + sizeof(fixture));
        if (corrupt)
        {
            data[33] ^= 1U;
        }
        gzip_reader_test_source source{data.data(), data.size(), 0U, false};
        lh_io_reader_t io;
        lh_io_reader_init(&io, gzip_reader_test_read, &source);
        lh_compress_gzip_header_decoder_t header;
        lh_compress_deflate_reader_t deflate;
        lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
        lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
        lh_compress_gzip_reader_t reader;
        lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history, 100U, 100U);
        std::string result;
        for (;;)
        {
            lh_byte_t out[3];
            const lh_ssize_t n = lh_compress_gzip_reader_read(&reader, out, sizeof(out));
            if (n == LH_IO_PENDING)
            {
                continue;
            }
            if (n <= 0)
            {
                EXPECT_EQ(n, corrupt ? LH_IO_ERROR : 0);
                break;
            }
            result.append(reinterpret_cast<char *>(out), static_cast<size_t>(n));
        }
        EXPECT_EQ(result, corrupt ? "" : "optional header");
    }
}

TEST(compress_gzip_reader_step_header, handles_pending_and_completes_header)
{
    gzip_reader_test_source source{gzip_sample, sizeof(gzip_sample), 0U, false};
    lh_io_reader_t io;
    lh_io_reader_init(&io, gzip_reader_test_read, &source);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t reader;
    lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history, 1200U, 18U);
    EXPECT_EQ(lh_compress_gzip_reader_step_header(&reader), 0);
    EXPECT_EQ(lh_compress_gzip_reader_step_header(&reader), LH_IO_PENDING);
    while (lh_compress_gzip_reader_get_phase(&reader) == lh_compress_gzip_reader_phase_header)
    {
        const lh_ssize_t n = lh_compress_gzip_reader_step_header(&reader);
        ASSERT_TRUE(n == 0 || n == LH_IO_PENDING);
    }
    EXPECT_EQ(lh_compress_gzip_reader_get_phase(&reader), lh_compress_gzip_reader_phase_deflate);
    EXPECT_EQ(source.position, 10U);
}

TEST(compress_gzip_reader_step_deflate, accumulates_crc_before_trailer)
{
    gzip_reader_test_source source{gzip_sample, sizeof(gzip_sample), 0U, false};
    lh_io_reader_t io;
    lh_io_reader_init(&io, gzip_reader_test_read, &source);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t reader;
    lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history, 1200U, 18U);
    while (lh_compress_gzip_reader_get_phase(&reader) == lh_compress_gzip_reader_phase_header)
    {
        const lh_ssize_t n = lh_compress_gzip_reader_step_header(&reader);
        ASSERT_TRUE(n == 0 || n == LH_IO_PENDING);
    }
    std::string decoded;
    while (lh_compress_gzip_reader_get_phase(&reader) == lh_compress_gzip_reader_phase_deflate)
    {
        lh_byte_t out[17];
        const lh_ssize_t n = lh_compress_gzip_reader_step_deflate(&reader, out, sizeof(out));
        if (n == LH_IO_PENDING)
        {
            continue;
        }
        ASSERT_GE(n, 0);
        if (n > 0)
        {
            decoded.append(reinterpret_cast<char *>(out), static_cast<size_t>(n));
        }
    }
    EXPECT_EQ(decoded.size(), 1200U);
    EXPECT_EQ(lh_compress_gzip_reader_get_member_size(&reader), decoded.size());
    EXPECT_EQ(lh_compress_gzip_reader_get_crc(&reader),
              lh_hash_crc32(decoded.data(), decoded.size()));
    EXPECT_EQ(lh_compress_gzip_reader_get_phase(&reader), lh_compress_gzip_reader_phase_trailer);
}

TEST(compress_gzip_reader_step_trailer, validates_crc_and_advances_member)
{
    gzip_reader_test_source source{gzip_sample + sizeof(gzip_sample) - 8U, 8U, 0U, false};
    lh_io_reader_t io;
    lh_io_reader_init(&io, gzip_reader_test_read, &source);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t reader;
    lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history, 1200U, 18U);
    std::string decoded;
    for (int i = 0; i < 100; ++i)
    {
        decoded.append("Hello gzip!\0", 12);
    }
    lh_compress_gzip_reader_set_crc(&reader, lh_hash_crc32(decoded.data(), decoded.size()));
    lh_compress_gzip_reader_set_member_size(&reader, decoded.size());
    lh_compress_gzip_reader_set_metadata(&reader, 10U);
    lh_compress_gzip_reader_set_phase(&reader, lh_compress_gzip_reader_phase_trailer);
    while (lh_compress_gzip_reader_get_phase(&reader) == lh_compress_gzip_reader_phase_trailer)
    {
        const lh_ssize_t n = lh_compress_gzip_reader_step_trailer(&reader);
        ASSERT_TRUE(n == 0 || n == LH_IO_PENDING);
    }
    EXPECT_EQ(lh_compress_gzip_reader_get_total(&reader), 1200U);
    EXPECT_EQ(lh_compress_gzip_reader_get_metadata(&reader), 18U);
    EXPECT_EQ(lh_compress_gzip_reader_get_phase(&reader), lh_compress_gzip_reader_phase_next);
}

TEST(compress_gzip_reader_step, marks_source_error_sticky)
{
    gzip_reader_test_source source{gzip_sample, 0U, 0U, false};
    lh_io_reader_t io;
    lh_io_reader_init(&io, gzip_reader_test_read, &source);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t reader;
    lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history, 1U, 18U);
    lh_byte_t out;
    EXPECT_EQ(lh_compress_gzip_reader_try_step(&reader, &out, 1U), LH_IO_ERROR);
    EXPECT_EQ(lh_compress_gzip_reader_get_phase(&reader), lh_compress_gzip_reader_phase_error);
    EXPECT_EQ(lh_compress_gzip_reader_try_step(&reader, &out, 1U), LH_IO_ERROR);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_reader_step(&reader, &out, 1U));
}

TEST(compress_gzip_reader_checked_steps, report_header_deflate_and_trailer_errors)
{
    gzip_reader_test_source source{nullptr, 0U, 0U, false};
    lh_io_reader_t io;
    lh_io_reader_init(&io, gzip_reader_test_read, &source);
    lh_compress_gzip_header_decoder_t header;
    lh_compress_deflate_reader_t deflate;
    lh_byte_t scratch[LH_COMPRESS_DEFLATE_READER_INPUT_SIZE];
    lh_byte_t history[LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE];
    lh_compress_gzip_reader_t reader;
    lh_compress_gzip_reader_init(&reader, &io, &header, &deflate, scratch, history, 10U, 18U);
    EXPECT_EQ(lh_compress_gzip_reader_try_step_header(&reader), LH_IO_ERROR);
    source.pending = false;
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_reader_step_header(&reader));
    lh_compress_deflate_reader_init(&deflate, &io, scratch, sizeof(scratch), history, 10U);
    lh_compress_deflate_reader_set_phase(&deflate, lh_compress_deflate_reader_phase_error);
    lh_compress_gzip_reader_set_phase(&reader, lh_compress_gzip_reader_phase_deflate);
    lh_byte_t out;
    EXPECT_EQ(lh_compress_gzip_reader_try_step_deflate(&reader, &out, 1U), LH_IO_ERROR);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_reader_step_deflate(&reader, &out, 1U));
    lh_compress_gzip_reader_set_phase(&reader, lh_compress_gzip_reader_phase_trailer);
    lh_compress_gzip_reader_set_metadata(&reader, 18U);
    EXPECT_EQ(lh_compress_gzip_reader_try_step_trailer(&reader), LH_IO_ERROR);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_reader_step_trailer(&reader));
}

TEST(compress_gzip_put_header, appends_minimal_header_the_decoder_accepts)
{
    lh_byte_t output[12] = {'x', 'y'};
    lh_usize_t written = 2U;
    lh_compress_gzip_put_header(output, sizeof(output), &written);
    EXPECT_EQ(written, 12U);
    const lh_byte_t expected[] = {31, 139, 8, 0, 0, 0, 0, 0, 0, 255};
    EXPECT_TRUE(std::equal(expected, expected + sizeof(expected), output + 2));
    lh_usize_t consumed = 0U;
    lh_compress_gzip_header(output + 2, 10U, &consumed);
    EXPECT_EQ(consumed, LH_COMPRESS_GZIP_HEADER_SIZE);
    written = 3U;
    EXPECT_FALSE(lh_compress_gzip_try_put_header(output, sizeof(output), &written));
    EXPECT_EQ(written, 3U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_put_header(output, sizeof(output), &written));
}

TEST(compress_gzip_put_trailer, appends_values_the_trailer_check_accepts)
{
    lh_byte_t output[9] = {0};
    lh_usize_t written = 1U;
    lh_compress_gzip_put_trailer(0x11223344U, 0x100000005ULL, output, sizeof(output), &written);
    EXPECT_EQ(written, 9U);
    EXPECT_EQ(output[1], 0x44U);
    EXPECT_EQ(output[5], 0x05U);
    EXPECT_TRUE(lh_compress_gzip_try_trailer_values(output + 1, 8U, 0x11223344U, 5U));
    written = 2U;
    EXPECT_FALSE(lh_compress_gzip_try_put_trailer(1U, 1U, output, sizeof(output), &written));
    EXPECT_EQ(written, 2U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_put_trailer(1U, 1U, output, sizeof(output), &written));
}

TEST(compress_gzip_bound, adds_header_and_trailer_to_stored_size)
{
    lh_usize_t bound = 0U;
    lh_compress_gzip_bound(0U, &bound);
    EXPECT_EQ(bound, 23U);
    lh_compress_gzip_bound(65536U, &bound);
    EXPECT_EQ(bound, 65564U);
    bound = 1U;
    EXPECT_FALSE(lh_compress_gzip_try_bound(LH_USIZE_T_MAX - 20U, &bound));
    EXPECT_EQ(bound, 1U);
    LH_EXPECT_CHECK_DEATH(lh_compress_gzip_bound(LH_USIZE_T_MAX, &bound));
}

TEST(compress_gzip_encode, writes_members_the_decoders_accept)
{
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher(new lh_compress_deflate_matcher_t);
    lh_compress_deflate_matcher_init(matcher.get(), LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT);
    std::string text;
    for (int i = 0; i < 400; ++i)
    {
        text += "<li>item " + std::to_string(i % 17) + "</li>\n";
    }
    const std::string inputs[] = {"", "Hello", text};
    std::vector<lh_byte_t> joined;
    std::string expected;
    for (const std::string &input : inputs)
    {
        lh_usize_t bound = 0U;
        lh_compress_gzip_bound(input.size(), &bound);
        std::vector<lh_byte_t> member(bound);
        lh_usize_t written = 0U;
        lh_compress_gzip_encode(matcher.get(), reinterpret_cast<const lh_byte_t *>(input.data()),
                                input.size(), member.data(), member.size(), &written);
        member.resize(written);
        std::vector<lh_byte_t> decoded(input.size() + 1U);
        lh_usize_t size = 0U;
        ASSERT_TRUE(lh_compress_gzip_try_decode(member.data(), member.size(), decoded.data(),
                                                decoded.size(), &size));
        EXPECT_EQ(std::string(reinterpret_cast<char *>(decoded.data()), size), input);
        joined.insert(joined.end(), member.begin(), member.end());
        expected += input;
    }
    EXPECT_LT(joined.size(), text.size() / 4U);
    std::vector<lh_byte_t> decoded(expected.size());
    lh_usize_t size = 0U;
    lh_compress_gzip_decode(joined.data(), joined.size(), decoded.data(), decoded.size(), &size);
    EXPECT_EQ(std::string(reinterpret_cast<char *>(decoded.data()), size), expected);
}

TEST(compress_gzip_encode, reports_insufficient_output)
{
    std::unique_ptr<lh_compress_deflate_matcher_t> matcher(new lh_compress_deflate_matcher_t);
    lh_compress_deflate_matcher_init(matcher.get(), LH_COMPRESS_DEFLATE_MATCHER_CHAIN_LIMIT);
    const lh_byte_t input[] = {'a', 'b', 'c'};
    lh_byte_t output[64];
    lh_usize_t written = 7U;
    EXPECT_FALSE(
        lh_compress_gzip_try_encode(matcher.get(), input, sizeof(input), output, 12U, &written));
    EXPECT_FALSE(
        lh_compress_gzip_try_encode(matcher.get(), input, sizeof(input), output, 20U, &written));
    EXPECT_EQ(written, 7U);
    EXPECT_TRUE(lh_compress_gzip_try_encode(matcher.get(), input, sizeof(input), output,
                                            sizeof(output), &written));
    EXPECT_EQ(written, 23U);
    LH_EXPECT_CHECK_DEATH(
        lh_compress_gzip_encode(matcher.get(), input, sizeof(input), output, 12U, &written));
}
