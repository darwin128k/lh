#include <gtest/gtest.h>

#include <lh/core/net/modbus/server.h>

#include <cstring>
#include <deque>
#include <memory>
#include <random>
#include <vector>

/* -- A server on a pair of queues -------------------------------------------
   What the master writes is what the server reads, and the other way round. No
   process, no line, no clock but the test's own: every request below is built by
   hand byte for byte, or asked by an LH link on the other end of the pipe. */
namespace
{

struct pipe_end
{
    std::deque<lh_byte_t> *in;
    std::deque<lh_byte_t> *out;
    size_t chunk = 0; /* 0: everything at once; else at most this much per read */
    bool closed = false;
    lh_ssize_t send_result = 0; /* 0: normal; else what the next write returns, once */
};

lh_ssize_t
end_read(lh_self_ptr self, lh_ptr bytes, lh_usize_t cap)
{
    pipe_end *e = (pipe_end *)self;

    if (e->closed)
    {
        return 0;
    }
    size_t take = e->in->size() < cap ? e->in->size() : cap;
    if (e->chunk != 0 && take > e->chunk)
    {
        take = e->chunk;
    }
    if (take == 0)
    {
        return LH_IO_PENDING;
    }
    for (size_t i = 0; i < take; ++i)
    {
        ((lh_byte_t *)bytes)[i] = e->in->front();
        e->in->pop_front();
    }
    return (lh_ssize_t)take;
}

lh_ssize_t
end_write(lh_self_ptr self, const lh_ptr bytes, lh_usize_t length)
{
    pipe_end *e = (pipe_end *)self;

    if (e->send_result != 0)
    {
        const lh_ssize_t r = e->send_result;
        e->send_result = 0;
        return r;
    }
    e->out->insert(e->out->end(), (const lh_byte_t *)bytes, (const lh_byte_t *)bytes + length);
    return (lh_ssize_t)length;
}

lh_io_stream_t
stream_of(pipe_end *e)
{
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, end_read, e);
    lh_io_writer_t writer;
    lh_io_writer_init(&writer, end_write, e);
    lh_io_stream_t stream;
    lh_io_stream_init(&stream, &reader, &writer);
    return stream;
}

/* The device the server fronts: 65536 holding registers, input registers that are
   arithmetic, and a switch that makes it refuse. */
struct device
{
    std::vector<lh_u16_t> holding = std::vector<lh_u16_t>(65536);
    lh_u8_t refuse = 0;
    int reads = 0;
    int writes = 0;

    device()
    {
        for (lh_u32_t a = 0; a < 65536; ++a)
        {
            holding[a] = (lh_u16_t)(a * 3U);
        }
    }
};

lh_u8_t
device_read(lh_ptr owner, lh_u8_t fc, lh_u16_t address, lh_u16_t count, lh_u16_t *values)
{
    device *d = (device *)owner;

    ++d->reads;
    if (d->refuse != 0)
    {
        return d->refuse;
    }
    for (lh_u16_t i = 0; i < count; ++i)
    {
        const lh_u32_t a = (lh_u32_t)address + i;
        values[i] = fc == LH_MB_FC_READ_HOLDING ? d->holding[a] : (lh_u16_t)(a ^ 0x5A5AU);
    }
    return 0;
}

lh_u8_t
device_write(lh_ptr owner, lh_u16_t address, lh_u16_t count, const lh_u16_t *values)
{
    device *d = (device *)owner;

    ++d->writes;
    if (d->refuse != 0)
    {
        return d->refuse;
    }
    for (lh_u16_t i = 0; i < count; ++i)
    {
        d->holding[(lh_u32_t)address + i] = values[i];
    }
    return 0;
}

struct bench
{
    std::deque<lh_byte_t> to_server;
    std::deque<lh_byte_t> to_master;
    pipe_end server_end{&to_server, &to_master};
    pipe_end master_end{&to_master, &to_server};
    lh_io_stream_t server_stream;
    lh_io_stream_t master_stream;
    device dev;
    lh_mb_server_t server;
    lh_u64_t now = 1000000;

    explicit bench(lh_mb_framing_t framing)
    {
        server_stream = stream_of(&server_end);
        master_stream = stream_of(&master_end);
        lh_mb_server_init(&server, &server_stream, framing, 17, device_read, device_write, &dev,
                          clock_of, this);
    }

    static lh_u64_t
    clock_of(lh_ptr self)
    {
        return ((bench *)self)->now;
    }

    void
    put(const std::vector<lh_byte_t> &bytes)
    {
        to_server.insert(to_server.end(), bytes.begin(), bytes.end());
    }

    std::vector<lh_byte_t>
    take()
    {
        std::vector<lh_byte_t> out(to_master.begin(), to_master.end());
        to_master.clear();
        return out;
    }
};

/* An RTU frame: @p body, then its CRC low byte first. */
std::vector<lh_byte_t>
rtu(std::vector<lh_byte_t> body)
{
    const lh_u16_t crc = lh_mb_crc16(body.data(), (lh_u16_t)body.size());

    body.push_back((lh_byte_t)(crc & 0xFF));
    body.push_back((lh_byte_t)(crc >> 8));
    return body;
}

/* An MBAP frame for transaction @p tx. */
std::vector<lh_byte_t>
mbap(lh_u16_t tx, lh_u8_t unit, const std::vector<lh_byte_t> &pdu)
{
    std::vector<lh_byte_t> out = {
        (lh_byte_t)(tx >> 8),        (lh_byte_t)tx, 0, 0, (lh_byte_t)((pdu.size() + 1) >> 8),
        (lh_byte_t)(pdu.size() + 1), unit};
    out.insert(out.end(), pdu.begin(), pdu.end());
    return out;
}

} // namespace

/* -- The wire bricks -------------------------------------------------------- */

TEST(lh_mb_server_wire, a_request_says_how_long_it_is)
{
    const lh_byte_t read[] = {17, 0x03, 0, 0, 0, 1};
    const lh_byte_t write_many[] = {17, 0x10, 0, 0, 0, 2, 4};
    const lh_byte_t unknown[] = {17, 0x2B, 0x0E};

    EXPECT_EQ(lh_mb_rtu_request_length(read, 1), 0) << "not even the code yet";
    EXPECT_EQ(lh_mb_rtu_request_length(read, 2), 8);
    EXPECT_EQ(lh_mb_rtu_request_length(write_many, 6), 0) << "the byte count is the seventh byte";
    EXPECT_EQ(lh_mb_rtu_request_length(write_many, 7), 9 + 4);
    EXPECT_EQ(lh_mb_rtu_request_length(unknown, 3), 0) << "a code nobody here can size";
    EXPECT_EQ(lh_mb_rtu_request_length(lh_null, 8), 0);
    EXPECT_TRUE(lh_mb_is_fc_supported(LH_MB_FC_WRITE_ONE));
    EXPECT_FALSE(lh_mb_is_fc_supported(0x01));
}

TEST(lh_mb_server_wire, a_refusal_is_two_bytes)
{
    lh_byte_t pdu[2];

    EXPECT_EQ(
        lh_mb_build_pdu_exception(pdu, 2, LH_MB_FC_READ_INPUT, LH_MB_EXCEPTION_ILLEGAL_ADDRESS), 2);
    EXPECT_EQ(pdu[0], 0x84);
    EXPECT_EQ(pdu[1], 0x02);
    EXPECT_EQ(lh_mb_build_pdu_exception(pdu, 1, 0x03, 0x02), 0);
}

TEST(lh_mb_server_wire, a_read_answer_is_read_back_by_the_parser)
{
    const lh_u16_t values[3] = {0x0102, 0xFFFF, 0};
    lh_byte_t pdu[8];
    lh_u16_t back[3] = {};
    lh_u16_t got = 0;

    ASSERT_EQ(lh_mb_build_pdu_read_answer(pdu, sizeof(pdu), LH_MB_FC_READ_HOLDING, values, 3), 8);
    EXPECT_EQ(lh_mb_parse_pdu(pdu, 8, LH_MB_FC_READ_HOLDING, 3, back, 3, &got, lh_null),
              lh_mb_status_ok);
    EXPECT_EQ(memcmp(back, values, sizeof(values)), 0);
    EXPECT_EQ(lh_mb_build_pdu_read_answer(pdu, 7, LH_MB_FC_READ_HOLDING, values, 3), 0)
        << "one byte short";
    EXPECT_EQ(lh_mb_build_pdu_read_answer(pdu, sizeof(pdu), LH_MB_FC_READ_HOLDING, values, 0), 0);
}

/* -- RTU, request by request ------------------------------------------------- */

TEST(lh_mb_server, a_read_is_answered_with_the_registers)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({17, 0x03, 0x00, 0x0A, 0x00, 0x02}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take(), rtu({17, 0x03, 0x04, 0x00, 30, 0x00, 33}));
    EXPECT_EQ(lh_mb_server_counters_get_answered(lh_mb_server_get_counters(&b.server)), 1U);
}

TEST(lh_mb_server, an_input_read_reads_the_input_registers)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({17, 0x04, 0xFF, 0xFF, 0x00, 0x01}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take(), rtu({17, 0x04, 0x02, 0xA5, 0xA5}));
}

TEST(lh_mb_server, a_single_write_is_stored_and_echoed_whole)
{
    bench b(lh_mb_framing_rtu);
    const std::vector<lh_byte_t> request = rtu({17, 0x06, 0x12, 0x34, 0xBE, 0xEF});

    b.put(request);
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take(), request);
    EXPECT_EQ(b.dev.holding[0x1234], 0xBEEF);
}

TEST(lh_mb_server, a_write_of_many_is_stored_and_its_head_echoed)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({17, 0x10, 0x00, 0x05, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take(), rtu({17, 0x10, 0x00, 0x05, 0x00, 0x02}));
    EXPECT_EQ(b.dev.holding[5], 0x1122);
    EXPECT_EQ(b.dev.holding[6], 0x3344);
}

TEST(lh_mb_server, the_widest_requests_the_protocol_allows)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({17, 0x03, 0xFF, 0x83, 0x00, 125}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take().size(), 255U) << "unit, code, byte count, 250 bytes, CRC";

    std::vector<lh_byte_t> write = {17, 0x10, 0xFF, 0x85, 0x00, 123, 246};
    write.resize(7 + 246, 0x5A);
    b.put(rtu(write));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.dev.holding[65535], 0x5A5A);
}

TEST(lh_mb_server, what_the_protocol_forbids_is_refused_with_its_own_exception)
{
    const struct
    {
        std::vector<lh_byte_t> request;
        lh_byte_t code;
    } cases[] = {
        {{17, 0x03, 0x00, 0x00, 0x00, 0x00}, 0x03},                   /* count 0 */
        {{17, 0x03, 0x00, 0x00, 0x00, 126}, 0x03},                    /* count 126 */
        {{17, 0x04, 0xFF, 0xFF, 0x00, 0x02}, 0x02},                   /* past 65535 */
        {{17, 0x10, 0x00, 0x00, 0x00, 124, 248}, 0x03},               /* write 124 */
        {{17, 0x10, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x01}, 0x03}, /* byte count 2 for 2 */
        {{17, 0x10, 0xFF, 0xFF, 0x00, 0x02, 0x04, 0, 1, 0, 2}, 0x02}, /* write past 65535 */
        {{17, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x03},             /* write 0 */
    };

    for (const auto &c : cases)
    {
        bench b(lh_mb_framing_rtu);
        std::vector<lh_byte_t> request = c.request;
        if (request[1] == 0x10)
        {
            request.resize(7 + request[6], 0);
        }
        b.put(rtu(request));
        ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused) << (int)request[1];
        EXPECT_EQ(b.take(), rtu({17, (lh_byte_t)(request[1] | 0x80), c.code})) << (int)request[1];
        EXPECT_EQ(b.dev.reads + b.dev.writes, 0) << "the application is never asked";
    }
}

TEST(lh_mb_server, the_application_refuses_with_its_own_code)
{
    bench b(lh_mb_framing_rtu);

    b.dev.refuse = LH_MB_EXCEPTION_DEVICE_BUSY;
    b.put(rtu({17, 0x03, 0x00, 0x00, 0x00, 0x01}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused);
    EXPECT_EQ(b.take(), rtu({17, 0x83, 0x06}));
    b.put(rtu({17, 0x06, 0x00, 0x00, 0x00, 0x01}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused);
    EXPECT_EQ(b.take(), rtu({17, 0x86, 0x06}));
}

TEST(lh_mb_server, a_server_with_no_map_refuses_as_an_illegal_function)
{
    bench b(lh_mb_framing_rtu);

    lh_mb_server_set_read(&b.server, lh_null);
    lh_mb_server_set_write(&b.server, lh_null);
    b.put(rtu({17, 0x03, 0x00, 0x00, 0x00, 0x01}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused);
    EXPECT_EQ(b.take(), rtu({17, 0x83, 0x01}));
    b.put(rtu({17, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x01}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused);
    EXPECT_EQ(b.take(), rtu({17, 0x90, 0x01}));
}

TEST(lh_mb_server, another_units_request_is_stepped_over_and_ours_behind_it_served)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({18, 0x03, 0x00, 0x00, 0x00, 0x01}));
    b.put(rtu({17, 0x03, 0x00, 0x01, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_ignored);
    EXPECT_TRUE(b.take().empty()) << "not ours to answer";
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take(), rtu({17, 0x03, 0x02, 0x00, 3}));
}

TEST(lh_mb_server, a_broadcast_write_is_done_and_not_answered)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({0, 0x06, 0x00, 0x09, 0x12, 0x34}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_broadcast);
    EXPECT_TRUE(b.take().empty());
    EXPECT_EQ(b.dev.holding[9], 0x1234);
    b.put(rtu({0, 0x03, 0x00, 0x09, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_broadcast);
    EXPECT_TRUE(b.take().empty());
    EXPECT_EQ(b.dev.reads, 0) << "a read to everybody has nobody to answer";
}

TEST(lh_mb_server, a_bad_crc_is_dropped_unanswered_and_the_next_request_served)
{
    bench b(lh_mb_framing_rtu);
    std::vector<lh_byte_t> bad = rtu({17, 0x06, 0x00, 0x09, 0x12, 0x34});

    bad[4] ^= 0x01;
    b.put(bad);
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_broken);
    EXPECT_TRUE(b.take().empty());
    EXPECT_EQ(b.dev.writes, 0) << "a broken write is not done";
    b.put(rtu({17, 0x03, 0x00, 0x00, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
}

TEST(lh_mb_server, a_request_in_pieces_is_served_when_whole)
{
    bench b(lh_mb_framing_rtu);
    const std::vector<lh_byte_t> request =
        rtu({17, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0xAB, 0xCD});

    b.server_end.chunk = 1;
    b.put(request);
    for (size_t i = 0; i + 1 < request.size(); ++i)
    {
        ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_idle) << "byte " << i;
    }
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.dev.holding[0], 0xABCD);
}

TEST(lh_mb_server, silence_inside_a_request_breaks_it)
{
    bench b(lh_mb_framing_rtu);
    const std::vector<lh_byte_t> request = rtu({17, 0x06, 0x00, 0x09, 0x12, 0x34});

    lh_mb_server_set_gap_us(&b.server, 4000);
    b.put({request.begin(), request.begin() + 4});
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_idle);
    b.now += 5000;
    b.put({request.begin() + 4, request.end()});
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_idle);
    EXPECT_EQ(b.dev.writes, 0);
    EXPECT_EQ(lh_mb_server_counters_get_broken(lh_mb_server_get_counters(&b.server)), 1U);
}

TEST(lh_mb_server, an_unknown_code_is_refused_at_the_silence)
{
    bench b(lh_mb_framing_rtu);

    lh_mb_server_set_gap_us(&b.server, 4000);
    b.put(rtu({17, 0x2B, 0x0E, 0x01, 0x00}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_idle) << "not until the line is quiet";
    b.now += 5000;
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused);
    EXPECT_EQ(b.take(), rtu({17, 0xAB, 0x01}));
}

TEST(lh_mb_server, an_unknown_code_with_no_silence_to_end_it_is_dropped)
{
    bench b(lh_mb_framing_rtu);

    b.put(rtu({17, 0x01, 0x00, 0x00, 0x00, 0x08}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_broken);
    EXPECT_EQ(lh_mb_server_get_held(&b.server), 0);
    b.put(rtu({17, 0x03, 0x00, 0x00, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered) << "and the server is not stuck";
}

TEST(lh_mb_server, a_byte_count_no_frame_can_hold_is_dropped)
{
    bench b(lh_mb_framing_rtu);

    b.put({17, 0x10, 0x00, 0x00, 0x00, 0x7F, 0xFE});
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_broken);
    EXPECT_EQ(lh_mb_server_get_held(&b.server), 0);
}

TEST(lh_mb_server, a_full_send_buffer_loses_the_stream)
{
    bench b(lh_mb_framing_rtu);

    b.server_end.send_result = 3; /* three bytes of seven */
    b.put(rtu({17, 0x03, 0x00, 0x00, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_lost);
}

TEST(lh_mb_server, the_end_of_the_stream_is_lost_and_a_new_one_is_served)
{
    bench b(lh_mb_framing_mbap);

    b.server_end.closed = true;
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_lost);
    b.server_end.closed = false;
    lh_mb_server_set_stream(&b.server, &b.server_stream);
    b.put(mbap(1, 17, {0x03, 0x00, 0x00, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
}

/* -- Modbus TCP --------------------------------------------------------------- */

TEST(lh_mb_server, mbap_the_answer_carries_the_transaction_id)
{
    bench b(lh_mb_framing_mbap);

    b.put(mbap(0xBEEF, 17, {0x03, 0x00, 0x0A, 0x00, 0x01}));
    ASSERT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(b.take(), mbap(0xBEEF, 17, {0x03, 0x02, 0x00, 30}));
}

TEST(lh_mb_server, mbap_requests_that_come_together_are_served_one_by_one)
{
    bench b(lh_mb_framing_mbap);

    b.put(mbap(1, 17, {0x03, 0x00, 0x00, 0x00, 0x01}));
    b.put(mbap(2, 0xFF, {0x06, 0x00, 0x00, 0x00, 0x07}));
    b.put(mbap(3, 0, {0x03, 0x00, 0x00, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered);
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered)
        << "unit 255 is the server itself";
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_answered) << "and over TCP so is 0";
    std::vector<lh_byte_t> want = mbap(1, 17, {0x03, 0x02, 0x00, 0x00});
    const std::vector<lh_byte_t> two = mbap(2, 0xFF, {0x06, 0x00, 0x00, 0x00, 0x07});
    const std::vector<lh_byte_t> three = mbap(3, 0, {0x03, 0x02, 0x00, 0x07});
    want.insert(want.end(), two.begin(), two.end());
    want.insert(want.end(), three.begin(), three.end());
    EXPECT_EQ(b.take(), want);
}

TEST(lh_mb_server, mbap_a_header_that_is_not_modbus_loses_the_stream)
{
    bench b(lh_mb_framing_mbap);

    b.put({0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 17, 0x03, 0, 0, 0, 1});
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_lost);
}

TEST(lh_mb_server, mbap_an_unknown_code_is_refused_at_once)
{
    bench b(lh_mb_framing_mbap);

    b.put(mbap(9, 17, {0x2B, 0x0E, 0x01, 0x00}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_refused) << "the header gives the length";
    EXPECT_EQ(b.take(), mbap(9, 17, {0xAB, 0x01}));
}

TEST(lh_mb_server, mbap_another_unit_is_not_answered)
{
    bench b(lh_mb_framing_mbap);

    b.put(mbap(4, 5, {0x03, 0x00, 0x00, 0x00, 0x01}));
    EXPECT_EQ(lh_mb_server_poll(&b.server), lh_mb_server_ignored);
    EXPECT_TRUE(b.take().empty());
}

/* -- An LH link on the other end -------------------------------------------- */

namespace
{

lh_u64_t
bench_clock(lh_ptr self)
{
    return ((bench *)self)->now;
}

/* Poll both ends until the link stops waiting. */
lh_mb_link_state_t
exchange(bench &b, lh_mb_link_t *link)
{
    lh_mb_link_state_t state = lh_mb_link_waiting;

    for (int i = 0; i < 1000 && state == lh_mb_link_waiting; ++i)
    {
        (void)lh_mb_server_poll(&b.server);
        state = lh_mb_link_poll(link, 1000000);
        b.now += 100;
    }
    return state;
}

} // namespace

class lh_mb_server_with_link : public ::testing::TestWithParam<lh_mb_framing_t>
{
};

INSTANTIATE_TEST_SUITE_P(framing, lh_mb_server_with_link,
                         ::testing::Values(lh_mb_framing_rtu, lh_mb_framing_mbap));

TEST_P(lh_mb_server_with_link, five_thousand_random_questions_agree_with_the_map)
{
    bench b(GetParam());
    lh_mb_link_t link;
    std::mt19937 rng(GetParam() == lh_mb_framing_rtu ? 3U : 5U);
    lh_u16_t count = 0;

    lh_mb_link_init(&link, &b.master_stream, GetParam(), 17, bench_clock, &b);
    b.master_end.chunk = 7; /* bytes arrive in pieces, never a frame at a time */
    b.server_end.chunk = 5;
    for (int step = 0; step < 5000; ++step)
    {
        const int what = (int)(rng() % 4U);
        const lh_u16_t n = (lh_u16_t)(1U + rng() % (what == 2 ? 123U : 125U));
        const lh_u16_t address = (lh_u16_t)(rng() % (65536U - n + 1U));

        if (what <= 1)
        {
            const lh_u8_t fc = what == 0 ? LH_MB_FC_READ_HOLDING : LH_MB_FC_READ_INPUT;
            lh_mb_link_set_fc(&link, fc);
            ASSERT_TRUE(lh_mb_link_ask(&link, address, n));
            ASSERT_EQ(exchange(b, &link), lh_mb_link_answered) << "step " << step;
            const lh_u16_t *values = lh_mb_link_take(&link, &count);
            ASSERT_EQ(count, n);
            for (lh_u16_t i = 0; i < n; ++i)
            {
                const lh_u32_t a = (lh_u32_t)address + i;
                ASSERT_EQ(values[i],
                          fc == LH_MB_FC_READ_HOLDING ? b.dev.holding[a] : (lh_u16_t)(a ^ 0x5A5AU))
                    << "step " << step << " register " << a;
            }
        }
        else if (what == 2)
        {
            std::vector<lh_u16_t> words(n);
            for (auto &w : words)
            {
                w = (lh_u16_t)rng();
            }
            ASSERT_TRUE(lh_mb_link_write(&link, address, words.data(), n));
            ASSERT_EQ(exchange(b, &link), lh_mb_link_answered) << "step " << step;
            ASSERT_TRUE(std::equal(words.begin(), words.end(), b.dev.holding.begin() + address));
        }
        else
        {
            const lh_u16_t value = (lh_u16_t)rng();
            ASSERT_TRUE(lh_mb_link_write_one(&link, address, value));
            ASSERT_EQ(exchange(b, &link), lh_mb_link_answered) << "step " << step;
            ASSERT_EQ(b.dev.holding[address], value);
        }
        lh_mb_link_release(&link);
    }
    const lh_mb_server_counters_t *c = lh_mb_server_get_counters(&b.server);
    EXPECT_EQ(lh_mb_server_counters_get_answered(c), 5000U);
    EXPECT_EQ(lh_mb_server_counters_get_broken(c) + lh_mb_server_counters_get_refused(c) +
                  lh_mb_server_counters_get_ignored(c),
              0U);
}

TEST_P(lh_mb_server_with_link, a_refusal_reaches_the_link_with_its_code)
{
    bench b(GetParam());
    lh_mb_link_t link;

    lh_mb_link_init(&link, &b.master_stream, GetParam(), 17, bench_clock, &b);
    ASSERT_TRUE(lh_mb_link_ask(&link, 65535, 1));
    ASSERT_EQ(exchange(b, &link), lh_mb_link_answered);
    lh_mb_link_release(&link);
    b.dev.refuse = LH_MB_EXCEPTION_DEVICE_BUSY;
    ASSERT_TRUE(lh_mb_link_write_one(&link, 3, 3));
    ASSERT_EQ(exchange(b, &link), lh_mb_link_failed);
    EXPECT_EQ(lh_mb_link_get_reason(&link), lh_mb_status_exception);
    EXPECT_EQ(lh_mb_link_get_exception(&link), LH_MB_EXCEPTION_DEVICE_BUSY);
}

/* Many masters, one thread, one map: a server per connection, all polled in turn by
   the same loop, every link with a question in flight at once. Nothing may be lost
   or answered to the wrong master. */
TEST(lh_mb_server, eight_masters_on_one_thread_share_one_map_without_losing_a_frame)
{
    const int kMasters = 8;
    std::vector<std::unique_ptr<bench>> benches;
    device shared;
    std::vector<lh_mb_link_t> links(kMasters);
    std::mt19937 rng(9U);

    for (int m = 0; m < kMasters; ++m)
    {
        benches.emplace_back(new bench(lh_mb_framing_mbap));
        lh_mb_server_set_owner(&benches[m]->server, &shared);
        benches[m]->master_end.chunk = 3 + m;
        benches[m]->server_end.chunk = 2 + m;
        lh_mb_link_init(&links[m], &benches[m]->master_stream, lh_mb_framing_mbap, 17, bench_clock,
                        benches[m].get());
    }
    for (int round = 0; round < 500; ++round)
    {
        /* Every master writes its own register block and asks back what it wrote. */
        std::vector<std::vector<lh_u16_t>> wrote(kMasters);
        for (int m = 0; m < kMasters; ++m)
        {
            wrote[m].resize(1 + rng() % 60U);
            for (auto &w : wrote[m])
            {
                w = (lh_u16_t)rng();
            }
            ASSERT_TRUE(lh_mb_link_write(&links[m], (lh_u16_t)(m * 1000), wrote[m].data(),
                                         (lh_u16_t)wrote[m].size()));
        }
        for (int phase = 0; phase < 2; ++phase)
        {
            std::vector<bool> done(kMasters, false);
            for (int spin = 0; spin < 2000; ++spin)
            {
                int left = 0;
                for (int m = 0; m < kMasters; ++m)
                {
                    (void)lh_mb_server_poll(&benches[m]->server);
                    if (!done[m])
                    {
                        const lh_mb_link_state_t s = lh_mb_link_poll(&links[m], 1000000);
                        ASSERT_NE(s, lh_mb_link_failed) << "round " << round << " master " << m;
                        done[m] = s == lh_mb_link_answered;
                    }
                    left += done[m] ? 0 : 1;
                }
                if (left == 0)
                {
                    break;
                }
            }
            for (int m = 0; m < kMasters; ++m)
            {
                ASSERT_TRUE(done[m]) << "round " << round << " master " << m << " lost its answer";
                if (phase == 1)
                {
                    lh_u16_t count = 0;
                    const lh_u16_t *values = lh_mb_link_take(&links[m], &count);
                    ASSERT_EQ(count, wrote[m].size());
                    ASSERT_TRUE(std::equal(wrote[m].begin(), wrote[m].end(), values))
                        << "round " << round << " master " << m;
                }
                lh_mb_link_release(&links[m]);
                if (phase == 0)
                {
                    ASSERT_TRUE(
                        lh_mb_link_ask(&links[m], (lh_u16_t)(m * 1000), (lh_u16_t)wrote[m].size()));
                }
            }
        }
    }
    for (int m = 0; m < kMasters; ++m)
    {
        const lh_mb_server_counters_t *c = lh_mb_server_get_counters(&benches[m]->server);
        EXPECT_EQ(lh_mb_server_counters_get_answered(c), 1000U) << "master " << m;
        EXPECT_EQ(lh_mb_link_counters_get_failed(lh_mb_link_get_counters(&links[m])), 0U);
    }
}
