#include <gtest/gtest.h>

#include <lh/net/modbus/link.h>
#include <lh/null.h>
#include <lh/util/addr.h>

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

/* ── A transport that hands out bytes one recv at a time ────────────────────────
   No sockets, no device, no clock: what goes out is recorded and what comes back is
   whatever the test put in the queue. That is the whole reason a link is a link. */

struct lh_test_transport
{
    lh_u8_t in[512];
    lh_u16_t in_size;
    lh_u16_t in_at;
    lh_u16_t chunk;         /**< 0: as much as fits. Otherwise: at most this much per recv */
    lh_u8_t out[512];
    lh_u16_t out_size;
    lh_s32_t fail_next_recv; /**< 0 to be normal, otherwise the value recv returns once */
};

struct lh_test_link
{
    lh_test_transport tcp;
    lh_mb_transport_t transport;
    lh_mb_link_t link;
    lh_u64_t now_us;
};

lh_bool_t
test_open(lh_ptr self, const lh_char_t *target, lh_u16_t port)
{
    (void)self;
    (void)target;
    (void)port;
    return lh_bool_true;
}

lh_void
test_close(lh_ptr self)
{
    (void)self;
}

lh_s32_t
test_send(lh_ptr self, const lh_byte_t *bytes, lh_u16_t length)
{
    lh_test_transport *t = (lh_test_transport *)self;

    if ((lh_u16_t)(t->out_size + length) > (lh_u16_t)sizeof(t->out))
    {
        return -1;
    }
    memcpy(lh_addr_of(t->out[t->out_size]), bytes, length);
    t->out_size = (lh_u16_t)(t->out_size + length);
    return (lh_s32_t)length;
}

lh_s32_t
test_recv(lh_ptr self, lh_byte_t *bytes, lh_u16_t cap)
{
    lh_test_transport *t = (lh_test_transport *)self;
    lh_u16_t left = (lh_u16_t)(t->in_size - t->in_at);
    lh_u16_t take = left < cap ? left : cap;

    if (t->chunk != 0 && take > t->chunk)
    {
        take = t->chunk;
    }
    if (t->fail_next_recv != 0)
    {
        const lh_s32_t rc = t->fail_next_recv;

        t->fail_next_recv = 0;
        return rc;
    }
    if (take == 0)
    {
        return 0; /* nothing yet */
    }
    memcpy(bytes, lh_addr_of(t->in[t->in_at]), take);
    t->in_at = (lh_u16_t)(t->in_at + take);
    return (lh_s32_t)take;
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
    memset(self, 0, sizeof(*self));
    self->transport.self = (lh_ptr)lh_addr_of(self->tcp);
    self->transport.open = test_open;
    self->transport.close = test_close;
    self->transport.send = test_send;
    self->transport.recv = test_recv;
    self->now_us = 1000000;
    lh_mb_link_init(lh_addr_of(self->link), lh_addr_of(self->transport), lh_mb_framing_mbap, 1,
                    test_clock, (lh_ptr)self);
    lh_mb_link_set_fc(lh_addr_of(self->link), fc);
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
                                 (lh_u16_t)sizeof(kRead3Holding), 1, LH_MB_FC_READ_HOLDING),
              (lh_u16_t)15)
        << "header 7 + length field 9 - the unit byte that is in both";
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kRead1Holding,
                                 (lh_u16_t)sizeof(kRead1Holding), 1, LH_MB_FC_READ_HOLDING),
              (lh_u16_t)11)
        << "header 7 + length field 5 - 1";
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kException, (lh_u16_t)sizeof(kException), 1,
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
        need = lh_mb_frame_length(lh_mb_framing_mbap, partial, have, 1, LH_MB_FC_READ_HOLDING);

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
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, kRead3Holding,
                                 (lh_u16_t)sizeof(kRead3Holding), 2, LH_MB_FC_READ_HOLDING),
              (lh_u16_t)0)
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

    answer[4] = 0;
    answer[5] = (lh_u8_t)(total - header + 1);
    answer[6] = 1;
    answer[7] = LH_MB_FC_READ_HOLDING;
    answer[8] = (lh_u8_t)(LH_MB_READ_MAX * 2);
    for (lh_u16_t i = 0; i < (lh_u16_t)(LH_MB_READ_MAX * 2); ++i)
    {
        answer[9 + i] = (lh_u8_t)(i & 0xFF);
    }
    EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_mbap, answer, total, 1, LH_MB_FC_READ_HOLDING),
              total);

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
        EXPECT_EQ(lh_mb_frame_length(lh_mb_framing_rtu, answer, answer_length, 1,
                                     LH_MB_FC_READ_HOLDING),
                  answer_length);
    }
}