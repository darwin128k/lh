#include <gtest/gtest.h>

#include <lh/net/modbus/link.h>

#include <cstring>

/* ── Frames captured from a device ──────────────────────────────────────────────
   These bytes are not written down from memory. They were taken off a socket from
   a Modbus server somebody else wrote, by asking it to read three holding
   registers at address 10, and they are here exactly as they arrived:

       00 01 | 00 00 | 00 09 | 01 | 03 06 | 00 01 00 01 00 01

   A frame built from the specification and a frame captured from a device are the
   same object, and this file exists because they were **not**: five separate
   mistakes below — in how long a Modbus TCP frame is, in how long its PDU is, in
   what a link asks for versus what it reads back for, in how wide the buffer for a
   full answer has to be, and in how wide a timestamp has to be — were all
   invisible to a test that only ever round-tripped our own encoder against our own
   decoder. Both halves were wrong in the same direction, so they agreed with each
   other, and the first thing that noticed was a device we did not write.

   Every expectation below is therefore the arithmetic on these bytes, and the
   arithmetic is written out in full. */
namespace
{

/** The answer above, byte for byte. */
const lh_byte_t kRead3Holding[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x03,
                                   0x06, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};

/** The same read of input registers: function code 0x04, one byte different. */
const lh_byte_t kRead3Input[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x04,
                                  0x06, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};

/** One register, the smallest answer there is. */
const lh_byte_t kRead1Holding[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x00, 0x01};

/** "Illegal function" to a read of 126 registers, which the protocol forbids. */
const lh_byte_t kException[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x01};

/**
 * @brief A refusal taken off a controller, not out of a specification.
 *
 * Asked of a real device on a panel for 126 holding registers at 30259 — one more
 * than the protocol allows — and this is every byte that came back:
 *
 *     00 07 | 00 00 | 00 03 | 01 | 83 03
 *
 * Transaction 7 (we asked for 7), protocol 0, length 3, unit 1, function code
 * 0x83 = the 0x03 we sent with the high bit set, and 0x03 = *illegal data value*.
 * Nine bytes, and every one of them means the device is alive and understood the
 * question.
 *
 * That last part is why this frame is in the file rather than only in a comment.
 * `counters.failed` used to be incremented here, on the grounds that no registers
 * came back; and the app above counts failures in a row to decide that a device
 * has gone off line. So a controller that is answering every question, correctly,
 * on time, was being taken off line **by its own client** for the sin of saying
 * that an address is not on it — and a card is a union of everything a device
 * family ever had, so that is not a corner case, it is what happens on the first
 * morning somebody adds a register to the card.
 */
const lh_byte_t kRefusedByDevice[] = {0x00, 0x07, 0x00, 0x00, 0x00, 0x03,
                                      0x01, 0x83, 0x03};

/* ── A stream that hands out bytes one read at a time ───────────────────────────
   No sockets, no device, no clock: what goes out is recorded and what comes back is
   whatever the test put in the queue. That is the whole reason a link reads a
   stream and not a socket. Nothing queued is ::LH_IO_PENDING, as on a quiet socket
   or serial line; `0` is the end of the stream, and only `closed` says that. */

struct lh_test_transport
{
    lh_u8_t in[512];
    lh_u16_t in_size;
    lh_u16_t in_at;
    lh_u16_t chunk;          /**< 0: as much as fits. Otherwise: at most this much per read */
    lh_u8_t out[512];
    lh_u16_t out_size;
    lh_ssize_t fail_next_recv;  /**< 0 to be normal, otherwise the value a read returns once */
    lh_ssize_t fail_next_send;  /**< 0 to be normal, otherwise the value a write returns once */
    bool closed;             /**< the peer has gone: every read is the end of the stream */
};

struct lh_test_link
{
    lh_test_transport tcp;
    lh_io_stream_t stream;
    lh_mb_link_t link;
    lh_u64_t now_us;
};

lh_ssize_t
test_send(lh_self_ptr self, const lh_ptr bytes, lh_usize_t length)
{
    lh_test_transport *t = (lh_test_transport *)self;

    if (t->fail_next_send != 0)
    {
        const lh_ssize_t rc = t->fail_next_send;

        t->fail_next_send = 0;
        return rc;
    }
    if (t->out_size + length > sizeof(t->out))
    {
        return -1;
    }
    memcpy(lh_addr_of(t->out[t->out_size]), bytes, length);
    t->out_size = (lh_u16_t)(t->out_size + length);
    return (lh_ssize_t)length;
}

lh_ssize_t
test_recv(lh_self_ptr self, lh_ptr bytes, lh_usize_t cap)
{
    lh_test_transport *t = (lh_test_transport *)self;
    lh_usize_t left = (lh_usize_t)(t->in_size - t->in_at);
    lh_usize_t take = left < cap ? left : cap;

    if (t->chunk != 0 && take > t->chunk)
    {
        take = t->chunk;
    }
    if (t->fail_next_recv != 0)
    {
        const lh_ssize_t rc = t->fail_next_recv;

        t->fail_next_recv = 0;
        return rc;
    }
    if (t->closed)
    {
        return 0;
    }
    if (take == 0)
    {
        return LH_IO_PENDING; /* nothing yet */
    }
    memcpy(bytes, lh_addr_of(t->in[t->in_at]), take);
    t->in_at = (lh_u16_t)(t->in_at + take);
    return (lh_ssize_t)take;
}

lh_u64_t
test_clock(lh_ptr self)
{
    return ((lh_test_link *)self)->now_us;
}

void
feed(lh_test_link *self, const lh_byte_t *bytes, lh_u16_t length)
{
    memcpy(lh_addr_of(self->tcp.in), bytes, length);
    self->tcp.in_size = length;
    self->tcp.in_at = 0;
}

void
setup(lh_test_link *self, lh_u8_t fc)
{
    lh_io_reader_t reader;
    lh_io_writer_t writer;

    memset(self, 0, sizeof(*self));
    lh_io_reader_init(lh_addr_of(reader), test_recv, lh_addr_of(self->tcp));
    lh_io_writer_init(lh_addr_of(writer), test_send, lh_addr_of(self->tcp));
    lh_io_stream_init(lh_addr_of(self->stream), lh_addr_of(reader), lh_addr_of(writer));
    self->now_us = 1000000;
    lh_mb_link_init(lh_addr_of(self->link), lh_addr_of(self->stream), lh_mb_framing_mbap, 1, test_clock,
                    (lh_ptr)self);
    lh_mb_link_set_fc(lh_addr_of(self->link), fc);
    /* The captured answers are answers to transaction 1, so the first question asked
       here is transaction 1. An answer is only an answer to the question with its
       transaction id, and a test that asked transaction 0 would be testing what
       happens to somebody else's frame. */
    self->link.tx = 1;
}

/* Put @p bytes after whatever the stream has not handed over yet: two frames in
   one recv, the way a stream delivers them. */
void
append(lh_test_link *self, const lh_byte_t *bytes, lh_u16_t length)
{
    memcpy(lh_addr_of(self->tcp.in[self->tcp.in_size]), bytes, length);
    self->tcp.in_size = (lh_u16_t)(self->tcp.in_size + length);
}

} /* namespace */

/* ── How long a frame is ────────────────────────────────────────────────────────
   Modbus TCP's length field counts the unit identifier and everything after it.
   The seven-byte header counts the unit identifier too. Adding the field to the
   header therefore counts one byte twice, and the frame that comes out is one byte
   longer than the device sent — which is a frame whose last byte is never read and
   an answer that never completes. */

TEST(lh_mb_link, a_captured_tcp_frame_is_fifteen_bytes_and_not_sixteen)
{
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kRead3Holding,
                                 (lh_u16_t)sizeof(kRead3Holding), LH_MB_FC_READ_HOLDING),
              (lh_u16_t)15)
        << "header 7 + length field 9 - the unit byte that is in both";
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kRead1Holding,
                                 (lh_u16_t)sizeof(kRead1Holding), LH_MB_FC_READ_HOLDING),
              (lh_u16_t)11)
        << "header 7 + length field 5 - 1";
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kException, (lh_u16_t)sizeof(kException),
                                 LH_MB_FC_READ_HOLDING),
              (lh_u16_t)9)
        << "header 7 + length field 3 - 1";
}

TEST(lh_mb_link, a_frame_is_not_complete_before_the_arithmetic_says_so)
{
    /* The contract is two numbers, not one: `need` says how long the frame is, and
       the caller compares it with what it has. Answering "how long" as soon as the
       seven-byte header is there is the point -- a frame spread over three packets
       still has one known length. What must not happen is the length being one more
       than the device sent, which this asserts at every prefix. */
    lh_byte_t partial[16];

    for (lh_u16_t i = 0; i < (lh_u16_t)sizeof(kRead3Holding); ++i)
    {
        const lh_u16_t have = (lh_u16_t)(i + 1);
        lh_u16_t need;

        memcpy(partial, kRead3Holding, (size_t)have);
        need = lh_mb_frame_length(lh_mb_framing_mbap, partial, have, LH_MB_FC_READ_HOLDING);

        if (have < LH_MB_MBAP_HEAD)
        {
            EXPECT_EQ(need, (lh_u16_t)0) << "under the header there is nothing to know";
            continue;
        }
        EXPECT_EQ(need, (lh_u16_t)15) << "with " << have << " bytes: the frame is 15 bytes long";
        EXPECT_LE(need, (lh_u16_t)LH_MB_ADU_MAX)
            << "and it has to fit the buffer, or the last bytes are never even asked for";
        if (have < 15)
        {
            EXPECT_GT(need, have) << "not all of it yet at " << have << " bytes";
        }
    }
}

TEST(lh_mb_link, another_devices_frame_is_not_ours)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length = 0;

    /* Its length is still its length: that is what lets a link step over it. Whose
       it is is the decoder's question, and the answer is "not unit 2's". */
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kRead3Holding,
                                 (lh_u16_t)sizeof(kRead3Holding), LH_MB_FC_READ_HOLDING),
              (lh_u16_t)15)
        << "another unit's frame is exactly as long as ours";
    EXPECT_FALSE(lh_mb_decode(lh_mb_framing_mbap, 2, kRead3Holding, (lh_u16_t)sizeof(kRead3Holding),
                              pdu, lh_addr_of(pdu_length)))
        << "unit 1 on the wire, asked for by unit 2";
}

/* ── The PDU out of a frame ──────────────────────────────────────────────────────
   The PDU is everything past the seven-byte header and there is nothing else. A
   reader that counts the header plus one is dropping the last byte of every answer,
   and for a read that is the top byte of the last register: a number that is wrong
   in its most significant byte is a number that looks entirely right. */

TEST(lh_mb_link, the_pdu_of_a_captured_frame_is_what_is_left_after_the_header)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length = 0;

    ASSERT_TRUE(lh_mb_decode(lh_mb_framing_mbap, 1, kRead3Holding,
                             (lh_u16_t)sizeof(kRead3Holding), pdu, lh_addr_of(pdu_length)));
    EXPECT_EQ(pdu_length, (lh_u16_t)8) << "function code, byte count, three words";
    EXPECT_EQ(pdu[0], (lh_u8_t)LH_MB_FC_READ_HOLDING);
    EXPECT_EQ(pdu[1], (lh_u8_t)6) << "three registers is six bytes";
    EXPECT_EQ(pdu[2], (lh_u8_t)0x00);
    EXPECT_EQ(pdu[7], (lh_u8_t)0x01) << "the last byte of the last register, not the second last";
}

TEST(lh_mb_link, decoding_does_not_truncate_the_final_register)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length = 0;
    lh_u16_t values[LH_MB_LINK_VALUES];
    lh_u16_t got = 0;

    /* One register whose high byte is 0xAB and whose low byte is 0xCD: a parser that
       drops the last byte of the PDU reads 0x00AB. */
    const lh_byte_t frame[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0xAB, 0xCD};

    ASSERT_TRUE(lh_mb_decode(lh_mb_framing_mbap, 1, frame, (lh_u16_t)sizeof(frame), pdu,
                             lh_addr_of(pdu_length)));
    EXPECT_EQ(lh_mb_parse_pdu(pdu, pdu_length, LH_MB_FC_READ_HOLDING, 1, values,
                              (lh_u16_t)(sizeof(values) / sizeof(values[0])), lh_addr_of(got),
                              lh_null),
              lh_mb_status_ok);
    ASSERT_EQ(got, (lh_u16_t)1);
    EXPECT_EQ(values[0], (lh_u16_t)0xABCD) << "0x00AB is the answer of a parser that drops a byte";
}

/* ── How long a frame we build is ────────────────────────────────────────────────
   The length **field** counts the unit and the PDU. The frame is the header and the
   PDU, and the unit is already one of the header's seven bytes. Adding one byte too
   many puts a stale byte from the buffer on the wire, and it hides: the reader below
   counted the same extra byte, so our own round trip was green and every real device
   ignored us. */

TEST(lh_mb_link, an_encoded_tcp_request_is_twelve_bytes_with_no_byte_after_the_pdu)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_byte_t frame[LH_MB_ADU_MAX];
    const lh_u16_t pdu_length = lh_mb_build_pdu_read(pdu, (lh_u16_t)sizeof(pdu),
                                                     LH_MB_FC_READ_HOLDING, 10, 19);
    const lh_u16_t length =
        lh_mb_encode(lh_mb_framing_mbap, 1, 0, pdu, pdu_length, frame, (lh_u16_t)sizeof(frame));

    ASSERT_EQ(pdu_length, (lh_u16_t)5) << "function code, address, count";
    EXPECT_EQ(length, (lh_u16_t)12) << "7 header + 5 PDU, and the unit is not a ninth thing";
    EXPECT_EQ(frame[4], (lh_u8_t)0x00);
    EXPECT_EQ(frame[5], (lh_u8_t)0x06) << "the field counts the unit and the PDU: 1 + 5";
    EXPECT_EQ(frame[6], (lh_u8_t)1);
    EXPECT_EQ(frame[7], (lh_u8_t)LH_MB_FC_READ_HOLDING);
    EXPECT_EQ(frame[11], (lh_u8_t)19);
}

/* ── How wide the frame buffer has to be ────────────────────────────────────────
   A read of 125 registers is the most Modbus allows. Over RTU its answer is 255
   bytes; over TCP it is 259, because the seven-byte header replaces a one-byte unit
   and a two-byte CRC and so comes out five bytes the longer. A buffer of 256 asks
   `recv` for at most 256 bytes, never requests the last three, and waits out its
   timeout on a device that answered perfectly. */

TEST(lh_mb_link, the_frame_buffer_holds_the_widest_answer_the_link_may_ask_for)
{
    lh_byte_t answer[LH_MB_ADU_MAX];
    lh_byte_t pdu[LH_MB_ADU_MAX];
    const lh_u16_t header = LH_MB_MBAP_HEAD;
    const lh_u16_t total = (lh_u16_t)(header + 2 + LH_MB_READ_MAX * 2);

    /* The widest answer there is, the way a device sends it: seven header bytes, then
       the PDU of a read of 125 registers -- code, byte count, 250 bytes of data. */
    ASSERT_EQ(LH_MB_READ_MAX, (lh_u16_t)125);
    ASSERT_EQ(total, (lh_u16_t)259) << "7 header + unit + code + count + 250 data bytes";
    ASSERT_LE(total, (lh_u16_t)LH_MB_ADU_MAX)
        << "a 125-register answer over TCP does not fit the buffer, and the request that asks"
           " for it waits for ever on a device that answered perfectly";
    ASSERT_GT(LH_MB_ADU_MAX, total) << "with room to spare, so this is not a coincidence of sizes";

    answer[0] = 0;
    answer[1] = 1;
    answer[2] = 0;
    answer[3] = 0;
    answer[4] = 0;
    answer[5] = (lh_u8_t)(total - header + 1);
    answer[6] = 1;
    answer[7] = LH_MB_FC_READ_HOLDING;
    answer[8] = (lh_u8_t)(LH_MB_READ_MAX * 2);
    for (lh_u16_t i = 0; i < (lh_u16_t)(LH_MB_READ_MAX * 2); ++i)
    {
        answer[9 + i] = (lh_u8_t)(i & 0xFF);
    }
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, answer, total, LH_MB_FC_READ_HOLDING), total);

    /* And it parses: the buffer being big enough is only half of it. */
    {
        lh_u16_t values[LH_MB_LINK_VALUES];
        lh_u16_t got = 0;
        lh_u8_t decoded[LH_MB_ADU_MAX];
        lh_u16_t pdu_length = 0;

        ASSERT_TRUE(lh_mb_decode(lh_mb_framing_mbap, 1, answer, total, decoded,
                                 lh_addr_of(pdu_length)));
        EXPECT_EQ(pdu_length, (lh_u16_t)252);
        EXPECT_EQ(lh_mb_parse_pdu(decoded, pdu_length, LH_MB_FC_READ_HOLDING, LH_MB_READ_MAX, values,
                                  (lh_u16_t)(sizeof(values) / sizeof(values[0])),
                                  lh_addr_of(got), lh_null),
                  lh_mb_status_ok);
        EXPECT_EQ(got, (lh_u16_t)125);
        /* Register 124 sits at PDU offset 2 + 248, and answer[9 + i] carries i & 0xFF:
           0xF8F9, the last register of the widest answer, whole. */
        EXPECT_EQ(values[124], (lh_u16_t)((248 << 8) | 249));
        EXPECT_EQ(values[0], (lh_u16_t)0x0001);
    }

    /* A request still builds inside the same buffer. */
    EXPECT_EQ(lh_mb_build_pdu_read(pdu, (lh_u16_t)sizeof(pdu), LH_MB_FC_READ_HOLDING, 0,
                                   LH_MB_READ_MAX),
              (lh_u16_t)5);
}

/* ── The question a link asks ─────────────────────────────────────────────────────
   The function code was a field on the link and a constant in the code that built
   the request. The answer came back carrying 0x03, the reader was holding 0x04, and
   the frame was called somebody else's — on a device that had answered the question
   it was actually asked, correctly, in under a millisecond. */

TEST(lh_mb_link, a_link_asks_with_the_function_code_it_was_given)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_INPUT);
    EXPECT_EQ(lh_mb_link_get_fc(lh_addr_of(t.link)), (lh_u8_t)LH_MB_FC_READ_INPUT);

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    ASSERT_EQ(t.tcp.out_size, (lh_u16_t)12);
    EXPECT_EQ(t.tcp.out[7], (lh_u8_t)LH_MB_FC_READ_INPUT)
        << "the request carries the code the reader will use to parse the answer";

    lh_mb_link_release(lh_addr_of(t.link));
    lh_mb_link_set_fc(lh_addr_of(t.link), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_EQ(t.tcp.out_size, (lh_u16_t)24);
    EXPECT_EQ(t.tcp.out[19], (lh_u8_t)LH_MB_FC_READ_HOLDING)
        << "the second request, which starts twelve bytes in";
}

TEST(lh_mb_link, an_input_answer_does_not_answer_a_holding_question)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    feed(lh_addr_of(t), kRead3Input, (lh_u16_t)sizeof(kRead3Input));

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed)
        << "one byte of function code apart, and it is not our answer";
    EXPECT_EQ(t.link.counters.done, (lh_u32_t)0);
}

TEST(lh_mb_link, a_holding_answer_answers_a_holding_question_in_both_banks)
{
    lh_test_link t;

    /* The captured input-register answer, read by a link asking exactly that. */
    setup(lh_addr_of(t), LH_MB_FC_READ_INPUT);
    feed(lh_addr_of(t), kRead3Input, (lh_u16_t)sizeof(kRead3Input));

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered)
        << "an answer to the question that was asked, read with the code it was asked under";
}

/* ── What an answer is worth ────────────────────────────────────────────────────── */

TEST(lh_mb_link, the_answer_a_device_sent_is_the_answer_the_link_holds)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);

    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    ASSERT_EQ(count, (lh_u16_t)3);
    EXPECT_EQ(values[0], (lh_u16_t)1);
    EXPECT_EQ(values[1], (lh_u16_t)1);
    EXPECT_EQ(values[2], (lh_u16_t)1);
    EXPECT_EQ(t.link.counters.sent, (lh_u32_t)1);
    EXPECT_EQ(t.link.counters.done, (lh_u32_t)1);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0);
    EXPECT_EQ(t.link.address, (lh_u16_t)10);
}

TEST(lh_mb_link, an_answer_arriving_one_byte_at_a_time_is_the_same_answer)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;
    lh_mb_link_state_t state = lh_mb_link_waiting;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    t.tcp.chunk = 1; /* one byte per recv, which is what a socket hands over when the
                        frame is spread across two packets */
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));

    for (int i = 0; i < (int)sizeof(kRead3Holding) + 4 && state == lh_mb_link_waiting; ++i)
    {
        t.now_us += 10;
        state = lh_mb_link_poll(lh_addr_of(t.link), 1000000);
    }
    ASSERT_EQ(state, lh_mb_link_answered) << "15 bytes in 15 pieces is still one answer";
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(count, (lh_u16_t)3);
}

/* ── A failure is one question ─────────────────────────────────────────────────────
   A link left in `failed` is a link that can never be asked anything again, because
   asking needs idle. One would-block on a full send buffer, one reset, one timeout,
   and that device is out for the rest of the session while every other device keeps
   being polled. Whether a device is *off* rather than *slow* is the app's decision,
   and only the app has the counter to make it from. */

TEST(lh_mb_link, a_timeout_leaves_the_link_able_to_be_asked_again)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));

    t.now_us += 2000000; /* two seconds later, nothing came back */
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1);
    EXPECT_EQ(t.link.state, (lh_u8_t)lh_mb_link_idle) << "and idle, so the next pass can use it";

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 20, 3)) << "the same device, one question later";
    EXPECT_EQ(t.link.counters.sent, (lh_u32_t)2);
}

TEST(lh_mb_link, a_device_that_stops_answering_still_counts_as_failed)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 30259, 125));
    t.now_us += 2000000;

    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1);
    EXPECT_EQ(t.link.counters.refused, (lh_u32_t)0)
        << "nothing came back at all, so there is nothing to have refused";
    EXPECT_EQ(t.link.reason, lh_mb_status_short) << "not yet a frame, and never one";
}

/* ── A refusal is an answer ────────────────────────────────────────────────────
   The ninth thing a real controller showed, and the only one a simulator cannot
   show at all: it answers every address. Every question in this file until now was
   answered, so "no registers came back" and "the device is gone" were the same
   sentence, and the app above has no way to write the second one differently. */

TEST(lh_mb_link, a_refusal_is_counted_as_a_refusal_and_not_as_a_failure)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    /* 125, not the 126 that provoked the captured frame: ::lh_mb_link_ask refuses
       to send a question the protocol forbids, and this test is about what a
       refusal **looks like on the wire**, not about replaying its cause. The nine
       bytes are the device's. */
    t.link.tx = 7; /* the captured refusal answers transaction 7 */
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 30259, 125));
    feed(lh_addr_of(t), kRefusedByDevice, (lh_u16_t)sizeof(kRefusedByDevice));

    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed)
        << "no registers came back, so this question produced no data";
    EXPECT_EQ(t.link.reason, lh_mb_status_exception) << "and the device said exactly what it meant";
    EXPECT_EQ(t.link.counters.refused, (lh_u32_t)1) << "it understood us and answered";
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0) << "a failure is a device that did not answer";
    EXPECT_EQ(t.link.counters.done, (lh_u32_t)0) << "and this one carried no registers either";
    EXPECT_EQ(t.link.state, (lh_u8_t)lh_mb_link_idle) << "idle, because it is not off";
}

TEST(lh_mb_link, eight_refusals_in_a_row_are_not_eight_failures)
{
    lh_test_link t;

    /* The rule this whole distinction exists for: the app decides a device is *off*
       after a run of failures in a row. ::BENCH_STRIKES in the bench is 8. A card
       is the union of everything a device family ever had, so on the first morning
       somebody widens it, a perfectly healthy controller answers every question
       perfectly and is taken off line by its own client — while the only thing it
       ever said was "that address is not on me". */
    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    for (lh_u32_t i = 0; i < 8; ++i)
    {
        t.link.tx = 7; /* the same captured refusal each time, so the same transaction */
        ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), (lh_u16_t)(30259 + i), 125))
            << "still asking after refusal " << i << ": the device is talking, so it is still there";
        feed(lh_addr_of(t), kRefusedByDevice, (lh_u16_t)sizeof(kRefusedByDevice));
        EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    }
    EXPECT_EQ(t.link.counters.refused, (lh_u32_t)8);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0)
        << "eight answers, zero failures. A run of failures is what takes a device off line and "
           "this device has not given us one";
}

TEST(lh_mb_link, asking_again_clears_the_verdict_of_the_last_question)
{
    lh_test_link t;

    /* `reason` answers "how did the previous question end". Left stale, it answers
       about the wrong question: an app that checks it after asking the next one
       reads a refusal for a request that has not been sent yet. */
    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    t.link.tx = 7;
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 30259, 125));
    feed(lh_addr_of(t), kRefusedByDevice, (lh_u16_t)sizeof(kRefusedByDevice));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    ASSERT_EQ(t.link.reason, lh_mb_status_exception);

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_EQ(t.link.reason, lh_mb_status_ok) << "the next question has not been answered yet";
    EXPECT_EQ(t.link.counters.refused, (lh_u32_t)1) << "and the old refusal is still counted";
}

TEST(lh_mb_link, a_socket_error_leaves_the_link_able_to_be_asked_again)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));

    t.tcp.fail_next_recv = -1;
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.state, (lh_u8_t)lh_mb_link_idle);
    EXPECT_EQ(t.link.reason, lh_mb_status_lost) << "a stream that failed is a stream to open again";
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 20, 3));
}

TEST(lh_mb_link, an_answer_to_the_other_question_is_not_an_answer_to_this_one)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    /* A holding answer arrives while the link is asking for input registers. */
    t.link.fc = LH_MB_FC_READ_INPUT;
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed)
        << "a frame carrying 0x03 is not the answer to a 0x04 question";
}

/* ── How wide a timestamp has to be ───────────────────────────────────────────────
   `lh_mb_link_t.since_us` held microseconds since boot in 32 bits. Thirty-two bits of
   microseconds run out after 4294 seconds, so on a machine that had been up for
   seventy-one minutes the stored time came back wrapped and `now - since_us` read
   five and a half hours. Every request then timed out the instant it was sent, and
   not one answer was ever parsed. */

TEST(lh_mb_link, a_link_works_on_a_machine_that_has_been_up_for_days)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    /* Three days of uptime: 259 200 000 000 microseconds, which is sixty times past
       what 32 bits of them can hold. */
    t.now_us = 259200000000ull;
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    t.now_us += 50;
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered)
        << "a 32-bit since_us reads this as a question in flight for five and a half hours";
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(count, (lh_u16_t)3);
}

TEST(lh_mb_link, the_timestamp_is_as_wide_as_the_clock_that_fills_it)
{
    lh_test_link t;
    lh_u64_t far_future = 259200000000ull;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    t.now_us = far_future;
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));

    /* If since_us were narrower than the clock, this is where it would differ. */
    EXPECT_EQ((lh_u64_t)t.link.since_us, far_future)
        << "stored narrower than it was read: the difference is the whole bug";
    EXPECT_LT(far_future, 4294967296ull * 64u) << "the test value has to be past 32 bits to say"
                                                   " anything";
    EXPECT_GT(far_future, 4294967295ull) << "and it has to be past the end of a 32-bit field";
}

/* ── Only one question at a time ────────────────────────────────────────────────────
   A second request sent while the first is unanswered comes back after its own
   answer and is read as somebody else's frame. */

TEST(lh_mb_link, a_link_asks_one_question_at_a_time)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_FALSE(lh_mb_link_ask(lh_addr_of(t.link), 20, 3))
        << "two questions in flight is one frame read as another's answer";
    EXPECT_EQ(t.tcp.out_size, (lh_u16_t)12) << "and only one went out";

    lh_mb_link_release(lh_addr_of(t.link));
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 20, 3));
    EXPECT_EQ(t.tcp.out_size, (lh_u16_t)24);
}

TEST(lh_mb_link, a_link_does_not_wait_for_an_answer_it_has_not_asked_for)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_idle)
        << "an idle link has nothing to poll and must not eat the answer waiting on the wire";
    EXPECT_EQ(t.link.state, (lh_u8_t)lh_mb_link_idle);
}

/* ── RTU still holds ───────────────────────────────────────────────────────────────
   The length field is the only thing that is different about TCP, and the frame
   arithmetic below is the RTU one, checked against the CRC rather than a header. */

TEST(lh_mb_link, an_rtu_request_is_the_unit_the_pdu_and_two_crc_bytes)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_byte_t frame[LH_MB_ADU_MAX];
    const lh_u16_t pdu_length =
        lh_mb_build_pdu_read(pdu, (lh_u16_t)sizeof(pdu), LH_MB_FC_READ_HOLDING, 0x0010, 3);
    const lh_u16_t length = lh_mb_encode(lh_mb_framing_rtu, 1, 0, pdu, pdu_length, frame,
                                         (lh_u16_t)sizeof(frame));
    const lh_u16_t crc = lh_mb_crc16(frame, (lh_u16_t)(1 + pdu_length));

    ASSERT_EQ(pdu_length, (lh_u16_t)5);
    EXPECT_EQ(length, (lh_u16_t)8) << "unit, code, address, count, two CRC bytes";
    EXPECT_EQ(frame[0], (lh_u8_t)1);
    EXPECT_EQ(frame[1], (lh_u8_t)LH_MB_FC_READ_HOLDING);
    EXPECT_EQ(frame[6], (lh_u8_t)(crc & 0xFF)) << "low byte first";
    EXPECT_EQ(frame[7], (lh_u8_t)(crc >> 8));

    /* And the answer to it. RTU has no length field, so the reader works it out from
       the function code and the byte count — which is exactly why a request is not
       something it can be asked about, and this frame is an answer. */
    {
        lh_byte_t answer[LH_MB_ADU_MAX];
        /* The PDU of the captured answer: code, byte count, six bytes of data. */
        const lh_u16_t pdu_out = 8;
        const lh_u16_t answer_length =
            lh_mb_encode(lh_mb_framing_rtu, 1, 0, kRead3Holding + 7, pdu_out, answer,
                         (lh_u16_t)sizeof(answer));

        EXPECT_EQ(answer_length, (lh_u16_t)11) << "unit, code, count, six data bytes, two CRC";
        EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_rtu, answer, answer_length, LH_MB_FC_READ_HOLDING),
                  answer_length);
    }
}
/* ── Writing ──────────────────────────────────────────────────────────────────
   Every frame below came off a socket from the write-capable copy of the emulator
   (a separate implementation that shares no code with this link): a write of 1 and 2
   at address 10, its echo, a read of the same two registers back, and the refusal of a
   write of zero registers. */
namespace
{
const lh_byte_t kWrite2At10[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x01, 0x10, 0x00,
                                 0x0a, 0x00, 0x02, 0x04, 0x00, 0x01, 0x00, 0x02};
const lh_byte_t kWrite2At10Echo[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x06,
                                     0x01, 0x10, 0x00, 0x0a, 0x00, 0x02};
const lh_byte_t kWriteRefused[] = {0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x01, 0x90, 0x03};
} /* namespace */

TEST(lh_mb_link, a_write_puts_the_captured_request_on_the_wire)
{
    lh_test_link t;
    const lh_u16_t values[] = {1, 2};

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    t.link.tx = 1; /* the captured request is transaction 1 */
    ASSERT_TRUE(lh_mb_link_write(lh_addr_of(t.link), 10, values, 2));
    ASSERT_EQ(t.tcp.out_size, (lh_u16_t)sizeof(kWrite2At10));
    EXPECT_EQ(memcmp(t.tcp.out, kWrite2At10, sizeof(kWrite2At10)), 0);
}

TEST(lh_mb_link, the_echo_of_a_write_answers_it_and_carries_no_values)
{
    lh_test_link t;
    const lh_u16_t values[] = {1, 2};
    lh_u16_t count = 99;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write(lh_addr_of(t.link), 10, values, 2));
    feed(lh_addr_of(t), kWrite2At10Echo, (lh_u16_t)sizeof(kWrite2At10Echo));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered)
        << "twelve bytes, length field 6: the write's header echoed";
    EXPECT_NE(lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count)), lh_null);
    EXPECT_EQ(count, (lh_u16_t)0) << "a write's answer has no registers in it";
    EXPECT_EQ(t.link.reason, lh_mb_status_ok);
}

/* The echo is checked against what was written: an echo of another address is not this
   write having happened. Asked at 11, answered with the echo of 10. */
TEST(lh_mb_link, the_echo_of_another_write_is_not_this_one)
{
    lh_test_link t;
    const lh_u16_t values[] = {1, 2};

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write(lh_addr_of(t.link), 11, values, 2));
    feed(lh_addr_of(t), kWrite2At10Echo, (lh_u16_t)sizeof(kWrite2At10Echo));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_garbage);
}

TEST(lh_mb_link, a_refused_write_is_a_refusal_and_not_a_failure)
{
    lh_test_link t;
    const lh_u16_t values[] = {1};

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    t.link.tx = 3; /* the captured refusal answers transaction 3 */
    ASSERT_TRUE(lh_mb_link_write(lh_addr_of(t.link), 10, values, 1));
    feed(lh_addr_of(t), kWriteRefused, (lh_u16_t)sizeof(kWriteRefused));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_exception) << "0x90 0x03: the device said no";
    EXPECT_EQ(t.link.counters.refused, (lh_u32_t)1);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0);
}

/* A write does not change what the link reads: after it, the next question is a read
   with the link's own function code, and its answer is read as one. */
TEST(lh_mb_link, after_a_write_the_link_reads_with_its_own_code_again)
{
    lh_test_link t;
    const lh_u16_t values[] = {1, 2};
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write(lh_addr_of(t.link), 10, values, 2));
    feed(lh_addr_of(t), kWrite2At10Echo, (lh_u16_t)sizeof(kWrite2At10Echo));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    lh_mb_link_release(lh_addr_of(t.link));
    EXPECT_EQ(lh_mb_link_get_fc(lh_addr_of(t.link)), LH_MB_FC_READ_HOLDING);

    t.link.tx = 1; /* the captured read answers transaction 1, as the write did */
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    EXPECT_NE(lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count)), lh_null);
    EXPECT_EQ(count, (lh_u16_t)3);
}

/* ── A stream, not a sequence of frames ─────────────────────────────────────────
   Over TCP a recv hands over whatever has arrived: half a frame, a frame and the
   start of the next, or the answer to a question that already timed out followed by
   the answer to this one. A link that treats each recv as one frame reads the wrong
   values the first time a device is slow, and loses its place the first time two
   frames come in one packet. */
namespace
{
/** The captured read of three registers at 10, transaction 2, with other values in
    it: what the device sends to the *second* question. */
const lh_byte_t kRead3HoldingTx2[] = {0x00, 0x02, 0x00, 0x00, 0x00, 0x09, 0x01, 0x03,
                                      0x06, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C};

/** The captured read, answered by unit 2 instead of unit 1. */
const lh_byte_t kRead3HoldingUnit2[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x02, 0x03,
                                        0x06, 0x00, 0x07, 0x00, 0x07, 0x00, 0x07};
} /* namespace */

TEST(lh_mb_link, a_late_answer_is_not_the_answer_to_the_next_question)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3)); /* transaction 1 */
    t.now_us += 2000000;
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed) << "timed out";

    /* The same question again, which is what a poll loop does on its next pass. The
       slow device's answer to the first one arrives now, and then the answer to this. */
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3)); /* transaction 2 */
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_waiting)
        << "transaction 1's answer, whole and well formed, is not transaction 2's";
    EXPECT_EQ(t.link.counters.done, (lh_u32_t)0);

    append(lh_addr_of(t), kRead3HoldingTx2, (lh_u16_t)sizeof(kRead3HoldingTx2));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    ASSERT_EQ(count, (lh_u16_t)3);
    EXPECT_EQ(values[0], (lh_u16_t)0x0A) << "1 is the stale value from the first question";
    EXPECT_EQ(values[2], (lh_u16_t)0x0C);
}

TEST(lh_mb_link, a_late_answer_and_this_one_in_one_recv_is_this_one)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    t.now_us += 2000000;
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    append(lh_addr_of(t), kRead3HoldingTx2, (lh_u16_t)sizeof(kRead3HoldingTx2));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered)
        << "the stale frame is stepped over and the one behind it is read, in one poll";
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(values[0], (lh_u16_t)0x0A);
}

TEST(lh_mb_link, half_a_late_answer_is_completed_and_stepped_over)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    /* The timeout falls in the middle of the slow answer. What came of it is kept,
       the rest completes it, and the frame is skipped by its transaction id -- where
       dropping the first half would read the second half as a header. */
    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, 9);
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_waiting);
    t.now_us += 2000000;
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.held, (lh_u16_t)9) << "nine bytes of a frame, kept";

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    append(lh_addr_of(t), kRead3Holding + 9, (lh_u16_t)(sizeof(kRead3Holding) - 9));
    append(lh_addr_of(t), kRead3HoldingTx2, (lh_u16_t)sizeof(kRead3HoldingTx2));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(values[0], (lh_u16_t)0x0A);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1) << "only the timeout";
}

TEST(lh_mb_link, the_bytes_after_an_answer_are_the_next_frame)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    /* Two answers in one recv. The second is not this question's, but it is the
       next one's, and it must still be there, whole, when the next one is asked. */
    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    append(lh_addr_of(t), kRead3HoldingTx2, (lh_u16_t)sizeof(kRead3HoldingTx2));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    EXPECT_EQ(t.link.held, (lh_u16_t)sizeof(kRead3HoldingTx2)) << "the second frame, kept";
    lh_mb_link_release(lh_addr_of(t.link));

    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered)
        << "answered from what was already held, with nothing new on the wire";
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(values[0], (lh_u16_t)0x0A);
    EXPECT_EQ(t.link.held, (lh_u16_t)0);
}

TEST(lh_mb_link, another_units_frame_is_stepped_over_without_a_timeout)
{
    lh_test_link t;
    const lh_u16_t *values;
    lh_u16_t count = 0;

    /* Behind a gateway every unit's answer comes down the same connection. A frame
       for unit 2 used to have no length as far as unit 1 was concerned, and the
       link sat on it until the timeout -- with its own answer right behind it. */
    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3HoldingUnit2, (lh_u16_t)sizeof(kRead3HoldingUnit2));
    append(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    values = lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count));
    ASSERT_NE(values, lh_null);
    EXPECT_EQ(values[0], (lh_u16_t)1) << "unit 1's values, not unit 2's 7";
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0);
}

TEST(lh_mb_link, a_header_that_is_not_modbus_fails_at_once)
{
    lh_test_link t;
    lh_byte_t frame[sizeof(kRead3Holding)];

    /* Protocol id 1. Seven bytes in, there is nothing to wait for: the stream has
       lost its place, and saying so now beats saying it a timeout later. */
    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    memcpy(frame, kRead3Holding, sizeof(frame));
    frame[3] = 1;
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), frame, (lh_u16_t)sizeof(frame));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_garbage);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1);
    EXPECT_EQ(t.link.held, (lh_u16_t)0) << "nothing after a lost place can be framed";
    EXPECT_EQ(t.link.state, (lh_u8_t)lh_mb_link_idle);
}

TEST(lh_mb_link, a_length_no_frame_can_have_fails_at_once)
{
    lh_test_link t;
    lh_byte_t frame[sizeof(kRead3Holding)];

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    memcpy(frame, kRead3Holding, sizeof(frame));
    frame[4] = 0xFF;
    frame[5] = 0xFF;
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, frame, (lh_u16_t)sizeof(frame),
                                 LH_MB_FC_READ_HOLDING),
              (lh_u16_t)0)
        << "65535 bytes would never fit, and would never arrive";
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), frame, (lh_u16_t)sizeof(frame));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_garbage);
}

TEST(lh_mb_link, a_new_connection_starts_with_nothing_held)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, 9);
    t.now_us += 2000000;
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    ASSERT_EQ(t.link.held, (lh_u16_t)9);

    lh_mb_link_set_stream(lh_addr_of(t.link), lh_addr_of(t.stream)); /* the connection, made again */
    EXPECT_EQ(t.link.held, (lh_u16_t)0) << "half a frame from the old connection is not a frame";
}

TEST(lh_mb_link, an_rtu_frame_for_another_unit_is_not_ours)
{
    lh_byte_t answer[LH_MB_ADU_MAX];
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length = 0;
    const lh_u16_t length = lh_mb_encode(lh_mb_framing_rtu, 2, 0, kRead3Holding + 7, 8, answer,
                                         (lh_u16_t)sizeof(answer));

    /* A good CRC says the line is clean, not that the frame is for us. */
    EXPECT_FALSE(lh_mb_decode(lh_mb_framing_rtu, 1, answer, length, pdu, lh_addr_of(pdu_length)));
    EXPECT_TRUE(lh_mb_decode(lh_mb_framing_rtu, 2, answer, length, pdu, lh_addr_of(pdu_length)));
}

/* ── Write one register over a link ─────────────────────────────────────────────
   Transaction 1, unit 1: 0x1234 to register 10. The request and its echo are the
   same twelve bytes, header and all. */
namespace
{
const lh_byte_t kWriteOne1234At10[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x06, 0x00, 0x0A, 0x12, 0x34};
const lh_byte_t kWriteOneRefused[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x86, 0x02};
} /* namespace */

TEST(lh_mb_link, a_single_write_puts_twelve_bytes_on_the_wire)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write_one(lh_addr_of(t.link), 10, 0x1234));
    ASSERT_EQ(t.tcp.out_size, (lh_u16_t)sizeof(kWriteOne1234At10));
    EXPECT_EQ(memcmp(t.tcp.out, kWriteOne1234At10, sizeof(kWriteOne1234At10)), 0);
    EXPECT_FALSE(lh_mb_link_write_one(lh_addr_of(t.link), 11, 1)) << "one question at a time, writes included";
}

TEST(lh_mb_link, the_echo_of_a_single_write_answers_it)
{
    lh_test_link t;
    lh_u16_t count = 99;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write_one(lh_addr_of(t.link), 10, 0x1234));
    feed(lh_addr_of(t), kWriteOne1234At10, (lh_u16_t)sizeof(kWriteOne1234At10));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    EXPECT_NE(lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count)), lh_null);
    EXPECT_EQ(count, (lh_u16_t)0) << "a write's answer has no registers in it";
    EXPECT_EQ(t.link.counters.done, (lh_u32_t)1);
}

TEST(lh_mb_link, the_echo_of_another_value_is_not_this_single_write)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write_one(lh_addr_of(t.link), 10, 0x4321));
    feed(lh_addr_of(t), kWriteOne1234At10, (lh_u16_t)sizeof(kWriteOne1234At10));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_garbage) << "asked to store 0x4321, it says it stored 0x1234";
}

TEST(lh_mb_link, a_refused_single_write_is_a_refusal_and_not_a_failure)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write_one(lh_addr_of(t.link), 10, 0x1234));
    feed(lh_addr_of(t), kWriteOneRefused, (lh_u16_t)sizeof(kWriteOneRefused));
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_exception);
    EXPECT_EQ(t.link.counters.refused, (lh_u32_t)1);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0);
}

TEST(lh_mb_link, after_a_single_write_the_link_reads_again)
{
    lh_test_link t;
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_write_one(lh_addr_of(t.link), 10, 0x1234));
    feed(lh_addr_of(t), kWriteOne1234At10, (lh_u16_t)sizeof(kWriteOne1234At10));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    lh_mb_link_release(lh_addr_of(t.link));

    t.link.tx = 1; /* the captured read answers transaction 1 */
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    EXPECT_NE(lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count)), lh_null);
    EXPECT_EQ(count, (lh_u16_t)3);
}

/* ── The stream, not the question ───────────────────────────────────────────────
   `0` from a read is the end of the stream, ::LH_IO_PENDING is "nothing yet", and
   the two have opposite answers: one is a new connection, the other is patience. A
   link that mixed them up either waits for ever on a socket the device closed, or
   gives up on a device that is merely quiet. */

TEST(lh_mb_link, nothing_yet_is_waiting_and_not_a_failure)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_waiting);
    }
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0);
}

TEST(lh_mb_link, the_end_of_the_stream_is_a_lost_stream)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    t.tcp.closed = true;
    EXPECT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);
    EXPECT_EQ(t.link.reason, lh_mb_status_lost) << "the device closed the connection";
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1);
    EXPECT_EQ(t.link.state, (lh_u8_t)lh_mb_link_idle);
}

TEST(lh_mb_link, a_full_send_buffer_is_not_a_failure)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    t.tcp.fail_next_send = LH_IO_PENDING;
    EXPECT_FALSE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3)) << "nothing went out";
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)0) << "and nothing is wrong";
    EXPECT_EQ(t.link.counters.sent, (lh_u32_t)0);
    EXPECT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3)) << "the next pass asks again";
}

TEST(lh_mb_link, half_a_request_out_is_a_lost_stream)
{
    lh_test_link t;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    t.tcp.fail_next_send = 5; /* five bytes of twelve */
    EXPECT_FALSE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    EXPECT_EQ(t.link.reason, lh_mb_status_lost) << "the device will read the next request as the rest of this one";
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1);
}

TEST(lh_mb_link, a_new_stream_forgets_the_question_in_flight)
{
    lh_test_link t;
    lh_u16_t count = 0;

    setup(lh_addr_of(t), LH_MB_FC_READ_HOLDING);
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    t.tcp.closed = true;
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_failed);

    /* Reconnected: the same device, a new stream, asked again. */
    t.tcp.closed = false;
    lh_mb_link_set_stream(lh_addr_of(t.link), lh_addr_of(t.stream));
    EXPECT_EQ(t.link.reason, lh_mb_status_ok);
    t.link.tx = 1;
    ASSERT_TRUE(lh_mb_link_ask(lh_addr_of(t.link), 10, 3));
    feed(lh_addr_of(t), kRead3Holding, (lh_u16_t)sizeof(kRead3Holding));
    ASSERT_EQ(lh_mb_link_poll(lh_addr_of(t.link), 1000000), lh_mb_link_answered);
    EXPECT_NE(lh_mb_link_take(lh_addr_of(t.link), lh_addr_of(count)), lh_null);
    EXPECT_EQ(count, (lh_u16_t)3);
    EXPECT_EQ(t.link.counters.failed, (lh_u32_t)1) << "the counters are the device's and are kept";
}
