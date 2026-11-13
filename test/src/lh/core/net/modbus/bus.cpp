#include <gtest/gtest.h>

#include <lh/core/net/modbus/bus.h>

#include <cstring>

/* -- A line in memory -----------------------------------------------------------
   What the bus sends is recorded; what comes back is whatever the test puts in, built
   with the same encoder a server uses. The clock is a number the test moves. */
namespace
{

struct wire
{
    lh_byte_t in[1024];
    lh_usize_t in_size;
    lh_usize_t in_at;
    lh_byte_t out[1024];
    lh_usize_t out_size;
    lh_usize_t sends;
};

struct line
{
    wire w;
    lh_io_stream_t stream;
    lh_mb_bus_t bus;
    lh_u64_t now_us;
};

lh_ssize_t
wire_send(lh_self_ptr self, const lh_ptr bytes, lh_usize_t length)
{
    wire *w = (wire *)self;

    memcpy(w->out + w->out_size, bytes, length);
    w->out_size += length;
    w->sends += 1;
    return (lh_ssize_t)length;
}

lh_ssize_t
wire_recv(lh_self_ptr self, lh_ptr bytes, lh_usize_t cap)
{
    wire *w = (wire *)self;
    const lh_usize_t left = w->in_size - w->in_at;
    const lh_usize_t take = left < cap ? left : cap;

    if (take == 0)
    {
        return LH_IO_PENDING;
    }
    memcpy(bytes, w->in + w->in_at, take);
    w->in_at += take;
    return (lh_ssize_t)take;
}

lh_u64_t
line_clock(lh_ptr self)
{
    return ((line *)self)->now_us;
}

void
setup(line *self, lh_u8_t retries)
{
    memset(self, 0, sizeof(*self));
    lh_io_reader_t reader;
    lh_io_reader_init(&reader, wire_recv, &self->w);
    lh_io_writer_t writer;
    lh_io_writer_init(&writer, wire_send, &self->w);
    lh_io_stream_init(&self->stream, &reader, &writer);
    self->now_us = 1000000;
    lh_mb_bus_init(&self->bus, &self->stream, lh_mb_framing_mbap, line_clock, self, 1000000);
    lh_mb_bus_set_retries(&self->bus, retries);
}

/* The device answers the question on the line with @p pdu. */
void
answer(line *self, const lh_byte_t *pdu, lh_u16_t length)
{
    const lh_mb_link_t *link = lh_mb_bus_get_link(&self->bus);
    lh_byte_t frame[LH_MB_ADU_MAX];
    const lh_u16_t size =
        lh_mb_encode(lh_mb_framing_mbap, lh_mb_link_get_unit(link), lh_mb_link_get_asked_tx(link),
                     (lh_byte_ptr)pdu, length, frame, sizeof(frame));

    memcpy(self->w.in + self->w.in_size, frame, size);
    self->w.in_size += size;
}

void
answer_read(line *self, lh_u8_t fc, const lh_u16_t *values, lh_u16_t count)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];

    answer(self, pdu, lh_mb_build_pdu_read_answer(pdu, sizeof(pdu), fc, values, count));
}

lh_mb_job_t
read_job(lh_u8_t unit, lh_u16_t address, lh_u16_t count)
{
    lh_mb_job_t job;

    lh_mb_job_init_read(&job, unit, LH_MB_FC_READ_HOLDING, address, count);
    return job;
}

} // namespace

/* Which endings of a request are worth asking again. A timeout or a broken answer is:
   the device may simply have been busy (a real controller holds requests for a second
   every few seconds) or the frame was damaged. A refusal is not: the device answered,
   and the same question gets the same exception. A lost stream is not: it has to be
   opened again first. */
TEST(lh_mb_bus, retries_what_may_pass_the_second_time_only)
{
    EXPECT_TRUE(lh_mb_bus_is_retryable(lh_mb_status_short));
    EXPECT_TRUE(lh_mb_bus_is_retryable(lh_mb_status_crc));
    EXPECT_TRUE(lh_mb_bus_is_retryable(lh_mb_status_garbage));
    EXPECT_TRUE(lh_mb_bus_is_retryable(lh_mb_status_wrong_code));
    EXPECT_FALSE(lh_mb_bus_is_retryable(lh_mb_status_exception));
    EXPECT_FALSE(lh_mb_bus_is_retryable(lh_mb_status_lost));
    EXPECT_FALSE(lh_mb_bus_is_retryable(lh_mb_status_ok));
    EXPECT_FALSE(lh_mb_bus_is_retryable(lh_mb_status_partial));
}

TEST(lh_mb_bus, a_read_is_answered_and_its_words_are_on_the_link)
{
    line t;
    setup(&t, 0);
    const lh_mb_job_t job = read_job(1, 100, 2);

    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &job));
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_waiting) << "nothing has come back yet";
    const lh_u16_t words[2] = {7, 8};
    answer_read(&t, LH_MB_FC_READ_HOLDING, words, 2);
    ASSERT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_answered);
    lh_u16_t got = 0;
    const lh_u16_t *values = lh_mb_link_take(lh_mb_bus_get_link_mut(&t.bus), &got);
    ASSERT_EQ(got, 2);
    EXPECT_EQ(values[0], 7);
    EXPECT_EQ(values[1], 8);
    EXPECT_TRUE(lh_mb_bus_is_busy(&t.bus)) << "busy until the outcome is released";
    lh_mb_bus_release(&t.bus);
    EXPECT_FALSE(lh_mb_bus_is_busy(&t.bus));
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_idle);
}

TEST(lh_mb_bus, a_busy_line_takes_no_second_job)
{
    line t;
    setup(&t, 0);
    const lh_mb_job_t job = read_job(1, 100, 2);

    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &job));
    EXPECT_FALSE(lh_mb_bus_try_start(&t.bus, &job));
    EXPECT_EQ(t.w.sends, 1U);
}

/* A controller that holds a request for a second: with one retry it costs a resend,
   not a failure. */
TEST(lh_mb_bus, a_timeout_is_sent_again_and_the_second_answer_counts)
{
    line t;
    setup(&t, 1);
    const lh_mb_job_t job = read_job(1, 100, 1);

    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &job));
    t.now_us += 1500000;
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_waiting) << "sent again, still waiting";
    EXPECT_EQ(t.w.sends, 2U);
    EXPECT_EQ(lh_mb_bus_get_resent(&t.bus), 1U);
    const lh_u16_t word = 42;
    answer_read(&t, LH_MB_FC_READ_HOLDING, &word, 1);
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_answered);
}

TEST(lh_mb_bus, when_the_retries_run_out_the_job_fails)
{
    line t;
    setup(&t, 1);
    const lh_mb_job_t job = read_job(1, 100, 1);

    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &job));
    t.now_us += 1500000;
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_waiting);
    t.now_us += 1500000;
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_failed);
    EXPECT_EQ(t.w.sends, 2U) << "once and once again";
    EXPECT_TRUE(lh_mb_bus_is_busy(&t.bus)) << "the failure is the outcome, to be read";
    lh_mb_bus_release(&t.bus);
    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &job)) << "and the line serves the next job";
    EXPECT_EQ(lh_mb_bus_get_attempt(&t.bus), 0) << "with its own retries";
}

TEST(lh_mb_bus, a_refusal_is_an_answer_and_is_not_sent_again)
{
    line t;
    setup(&t, 3);
    const lh_mb_job_t job = read_job(1, 100, 1);

    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &job));
    lh_byte_t pdu[8];
    answer(&t, pdu, lh_mb_build_pdu_exception(pdu, sizeof(pdu), LH_MB_FC_READ_HOLDING, 2));
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_failed);
    EXPECT_EQ(lh_mb_link_get_reason(lh_mb_bus_get_link(&t.bus)), lh_mb_status_exception);
    EXPECT_EQ(lh_mb_link_get_exception(lh_mb_bus_get_link(&t.bus)), 2);
    EXPECT_EQ(t.w.sends, 1U) << "the same question gets the same no";
}

TEST(lh_mb_bus, jobs_for_different_units_go_down_one_line)
{
    line t;
    setup(&t, 0);
    const lh_u16_t words[2] = {0xA120, 0x0007};
    lh_mb_job_t write;

    lh_mb_job_init_write(&write, 5, LH_MB_FC_WRITE_MANY, 32772, words, 2);
    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &write));
    /* MBAP: 7 bytes of header with the unit last, then the PDU. */
    EXPECT_EQ(t.w.out[6], 5);
    EXPECT_EQ(t.w.out[7], LH_MB_FC_WRITE_MANY);
    const lh_byte_t echo[5] = {LH_MB_FC_WRITE_MANY, 0x80, 0x04, 0x00, 0x02};
    answer(&t, echo, 5);
    EXPECT_EQ(lh_mb_bus_poll(&t.bus), lh_mb_link_answered);
    lh_mb_bus_release(&t.bus);

    const lh_mb_job_t read = read_job(9, 1, 1);
    const lh_usize_t at = t.w.out_size;
    ASSERT_TRUE(lh_mb_bus_try_start(&t.bus, &read));
    EXPECT_EQ(t.w.out[at + 6], 9);
    EXPECT_EQ(t.w.out[at + 7], LH_MB_FC_READ_HOLDING);
}

TEST(lh_mb_bus, a_job_the_line_cannot_send_leaves_it_idle)
{
    line t;
    setup(&t, 0);
    lh_mb_job_t job = read_job(1, 0, 1);

    lh_mb_job_set_fc(&job, 0x01);
    EXPECT_FALSE(lh_mb_bus_try_start(&t.bus, &job)) << "a function the bus does not know";
    lh_mb_job_set_fc(&job, LH_MB_FC_READ_HOLDING);
    lh_mb_job_set_count(&job, 126);
    EXPECT_FALSE(lh_mb_bus_try_start(&t.bus, &job)) << "more than a read may ask for";
    EXPECT_FALSE(lh_mb_bus_is_busy(&t.bus));
    EXPECT_EQ(t.w.sends, 0U);
}
