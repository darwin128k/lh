/**
 * @file link.c
 * @brief Framing, and a link that never waits.
 */

#include <lh/net/modbus/link.h>

#include <lh/assert/runtime.h>
#include <lh/memory/std.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Framing ──────────────────────────────────────────────────────────────────
   The PDU is the part that is the same everywhere: function code, then data. The
   framing is the part that is not — a serial line has no header and ends a frame
   with a CRC, a network has a header with a length in it and no CRC. */

lh_u16_t
lh_mb_encode(lh_mb_framing_t framing, lh_u8_t unit, lh_u16_t tx, const lh_byte_t *pdu,
             lh_u16_t pdu_length, lh_byte_t *out, lh_u16_t cap)
{
    lh_return_if(lh_null_eq(out) || lh_null_eq(pdu), (lh_u16_t)0);

    if (framing == lh_mb_framing_mbap)
    {
        lh_return_if(cap < (lh_u16_t)(LH_MB_MBAP_HEAD + pdu_length), (lh_u16_t)0);
        /* Transaction id, protocol id (0 is Modbus), length, unit. The length is
           everything after itself: the unit and the PDU, and **not** the six bytes
           before it. A length that counts the wrong thing is a frame no device
           will accept.

           The length *field* and the length *of the frame* count different things,
           and that is the whole trap: the field is `1 + pdu_length` because it
           counts the unit, while the frame is `HEAD + pdu_length` because the unit
           is already one of the header's seven bytes. Returning `HEAD + 1 + ...`
           tells the caller to write one byte more than was filled in, and the byte
           it writes is whatever was in the buffer before -- a stale tail on a
           frame that is otherwise perfect.

           It is worse than a wrong number, because it hides: the reader in
           ::lh_mb_frame_length counted the same extra byte, so sender and receiver
           agreed with each other and disagreed with the protocol, and every test
           that only ever round-tripped our own frames was green. The device that
           finally showed it was a Modbus server written by somebody else. */
        out[0] = (lh_byte_t)(tx >> 8);
        out[1] = (lh_byte_t)(tx & 0xFF);
        out[2] = 0;
        out[3] = 0;
        out[4] = (lh_byte_t)((1 + pdu_length) >> 8);
        out[5] = (lh_byte_t)((1 + pdu_length) & 0xFF);
        out[6] = unit;
        lh_memory_std_copy(lh_addr_of(out[7]), pdu, pdu_length);
        return (lh_u16_t)(LH_MB_MBAP_HEAD + pdu_length);
    }
    /* RTU: unit, PDU, CRC low byte first. */
    lh_return_if(cap < (lh_u16_t)(2 + pdu_length), (lh_u16_t)0);
    out[0] = unit;
    lh_memory_std_copy(lh_addr_of(out[1]), pdu, pdu_length);
    {
        const lh_u16_t crc = lh_mb_crc16(out, (lh_u16_t)(1 + pdu_length));

        out[1 + pdu_length] = (lh_byte_t)(crc & 0xFF);
        out[2 + pdu_length] = (lh_byte_t)(crc >> 8);
    }
    return (lh_u16_t)(3 + pdu_length);
}

/* How long an RTU answer is, from its function code: a read says how many bytes
   in its third byte, a write echoes its request, and an exception is five. 125
   registers is the most, so 255 bytes is the most a read answer can be. */
static lh_u16_t
rtu_length(const lh_byte_t *frame, lh_u16_t have, lh_u8_t fc)
{
    if (have < 3)
    {
        return 0;
    }
    if ((frame[1] & 0x80) != 0)
    {
        return 5; /* unit, code|80, code, CRC */
    }
    switch (fc)
    {
        case LH_MB_FC_READ_HOLDING:
        case LH_MB_FC_READ_INPUT:
            return (lh_u16_t)(5 + frame[2]); /* unit, code, byte count, words, CRC */
        case LH_MB_FC_WRITE_ONE:
        case LH_MB_FC_WRITE_MANY:
            return 8; /* the write echoes its own header back */
        default:
            return 0; /* a code this program never sent */
    }
}

lh_u16_t
lh_mb_frame_length(lh_mb_framing_t framing, const lh_byte_t *frame, lh_u16_t have, lh_u8_t fc)
{
    lh_return_if(lh_null_eq(frame), (lh_u16_t)0);

    if (framing == lh_mb_framing_mbap)
    {
        lh_u16_t length;

        if (have < LH_MB_MBAP_HEAD)
        {
            return 0;
        }
        /* The header carries the length, so "have I got it all" is an arithmetic
           question and not a guess. This is the whole reason a network frame can
           be read one byte at a time without ever mistaking its middle for an
           edge.

           The arithmetic is `HEAD + length - 1`, and the **-1 is the unit byte**:
           Modbus TCP's length field counts the unit identifier and everything after
           it, while ::LH_MB_MBAP_HEAD already counts the unit among the bytes ahead
           of the PDU. Counting both is how a fifteen-byte answer is waited for as
           though it were sixteen, and a link that never completes a frame never
           parses one. The frame captured from a device that answered the read of
           three holding registers at address 10 is

               00 01 | 00 00 | 00 09 | 01 | 03 06 | 00 01 00 01 00 01

           -- 15 bytes, length field 9 -- and that is what this returns. */
        if (frame[2] != 0 || frame[3] != 0)
        {
            return 0; /* protocol id 0 is Modbus; anything else is not a frame we can size */
        }
        length = (lh_u16_t)(((lh_u16_t)frame[4] << 8) | frame[5]);
        if (length < 2 || length > (lh_u16_t)(LH_MB_ADU_MAX - LH_MB_MBAP_HEAD + 1))
        {
            /* Unit and a function code at least, and no more than the buffer holds.
               A length past that is not a big frame, it is a header that is not one:
               waiting for it would wait for bytes that can never fit. */
            return 0;
        }
        /* Whose frame it is does not matter here. Another unit's answer, or a late
           answer to a question that has timed out, is still exactly this long, and
           knowing that is what lets the link step over it to the frame behind it.
           Calling it "not complete" instead left the link waiting out its timeout
           on bytes that had all arrived. */
        return (lh_u16_t)(LH_MB_MBAP_HEAD + length - 1);
    }
    return rtu_length(frame, have, fc);
}

lh_bool_t
lh_mb_decode(lh_mb_framing_t framing, lh_u8_t unit, const lh_byte_t *frame, lh_u16_t length,
             lh_byte_t *pdu, lh_u16_t *pdu_length)
{
    lh_return_ifn(lh_null_ne(frame) && lh_null_ne(pdu) && lh_null_ne(pdu_length), lh_bool_false);

    if (framing == lh_mb_framing_mbap)
    {
        lh_return_ifn(length > LH_MB_MBAP_HEAD, lh_bool_false);
        if (frame[2] != 0 || frame[3] != 0 || frame[6] != unit)
        {
            return lh_bool_false;
        }
        /* The PDU is everything past the seven-byte header -- including the unit,
           there is nothing else -- so its length is the frame length less the
           header. It is *not* less the header plus one: that would drop the last
           byte of every answer, which for a read is the top byte of the last
           register, and a number that is wrong in its last byte is a number that
           looks right. */
        *pdu_length = (lh_u16_t)(length - LH_MB_MBAP_HEAD);
        lh_memory_std_copy(pdu, lh_addr_of(frame[7]), *pdu_length);
        return lh_bool_true;
    }
    /* RTU: the CRC is checked before anything is taken out of the frame, for the
       same reason the reader checks it before the byte count — a corrupted length
       sends the copy off the end of the buffer. */
    lh_return_ifn(length >= 4, lh_bool_false);
    if (frame[0] != unit)
    {
        return lh_bool_false; /* a good frame for another unit on the same line */
    }
    if (lh_mb_crc16(frame, (lh_u16_t)(length - 2)) !=
        (lh_u16_t)((lh_u16_t)frame[length - 1] << 8 | frame[length - 2]))
    {
        return lh_bool_false;
    }
    *pdu_length = (lh_u16_t)(length - 3);
    lh_memory_std_copy(pdu, lh_addr_of(frame[1]), *pdu_length);
    return lh_bool_true;
}

/* ── A link ───────────────────────────────────────────────────────────────── */

lh_void
lh_mb_link_init(lh_mb_link_t *self, const lh_io_stream_t *stream, lh_mb_framing_t framing,
                lh_u8_t unit, lh_u64_t (*now_us)(lh_ptr self), lh_ptr clock)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(stream);

    lh_memory_std_set(lh_addr_of(*self), 0, sizeof(*self));
    lh_io_stream_assign(lh_addr_of(self->stream), stream);
    self->framing = framing;
    self->unit = unit;
    /* Which question this link asks. It is a field and not an argument to every
       call because an answer is only known by the question that was asked: a
       parser that had to be handed the code on the way in would be handed the
       wrong one exactly when it matters, on the frame that arrived late.

       Holding registers are the default, and ::lh_mb_link_set_fc changes it. The
       default is a default and not a restriction: the cards in the firmware are
       mostly input registers (JL204C5M has 761 of them against 646 holding), and
       a link that could only ever ask 0x03 could not poll most of what those
       devices have. */
    self->fc = LH_MB_FC_READ_HOLDING;
    self->asked_fc = LH_MB_FC_READ_HOLDING;
    self->state = (lh_u8_t)lh_mb_link_idle;
    self->reason = lh_mb_status_ok;
    self->now_us = now_us;
    self->clock = clock;
}

lh_void
lh_mb_link_set_fc(lh_mb_link_t *self, lh_u8_t fc)
{
    lh_assert_runtime_ref(self);

    /* Only while nothing is in flight. A link whose question is already out
       cannot be told to ask a different one: the answer to a 0x03 arriving under
       a link that now believes it asked 0x04 is a frame that parses as somebody
       else's, which is the one thing the field above exists to prevent. */
    lh_return_if(self->state != (lh_u8_t)lh_mb_link_idle);
    self->fc = fc;
}

lh_u8_t
lh_mb_link_get_fc(const lh_mb_link_t *self)
{
    lh_return_ifn(lh_null_ne(self), (lh_u8_t)0);
    return self->fc;
}

lh_void
lh_mb_link_set_stream(lh_mb_link_t *self, const lh_io_stream_t *stream)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(stream);
    lh_io_stream_assign(lh_addr_of(self->stream), stream);
    /* A new connection is a new stream: whatever was held belonged to the old one,
       and the first byte that arrives now is the first byte of a frame. */
    self->held = 0;
    self->state = (lh_u8_t)lh_mb_link_idle;
    self->reason = lh_mb_status_ok;
}

/* The stream is gone: the peer closed it, it failed, or it took half a request. A
   frame boundary cannot be found on it again, so nothing held is kept, and the
   reason says so -- the app opens a new stream and hands it to
   ::lh_mb_link_set_stream. */
static lh_mb_link_state_t
lh_mb_link_lose(lh_mb_link_t *self)
{
    self->held = 0;
    self->state = (lh_u8_t)lh_mb_link_idle;
    self->reason = lh_mb_status_lost;
    self->counters.failed++;
    return lh_mb_link_failed;
}

/* Drop the first @p n bytes held. Over MBAP what follows them is the start of the
   next frame and is kept: TCP is a stream, a recv hands over whatever has arrived,
   and an answer and the frame behind it come in one piece as often as not. Over RTU
   there is no length to find the next frame by, so everything goes. */
static lh_void
lh_mb_link_consume(lh_mb_link_t *self, lh_u16_t n)
{
    if (self->framing != lh_mb_framing_mbap || n >= self->held)
    {
        self->held = 0;
        return;
    }
    lh_memory_std_move(self->frame, lh_addr_of(self->frame[n]), (lh_usize_t)(self->held - n));
    self->held = (lh_u16_t)(self->held - n);
}

/* Everything after the PDU is the same for a read and a write: the envelope, the send,
   and the bookkeeping that makes the answer this question's. @p fc is the code the
   answer will carry, and ::lh_mb_link_poll reads it by that. */
static lh_bool_t
lh_mb_link_send(lh_mb_link_t *self, const lh_byte_t *pdu, lh_u16_t pdu_length, lh_u16_t address,
                lh_u16_t count, lh_u8_t fc)
{
    /* The request is built in a buffer of its own. ::lh_mb_link_t.frame is the
       receive side, and over MBAP it may already hold the start of the next frame:
       building the request there wrote it over those bytes, and the request -- which
       carries this question's own transaction id -- was then read back as the
       answer. */
    lh_byte_t out[LH_MB_ADU_MAX];
    lh_ssize_t wrote;

    if (pdu_length == 0)
    {
        return lh_bool_false;
    }
    pdu_length = lh_mb_encode(self->framing, self->unit, self->tx, pdu, pdu_length, out,
                              (lh_u16_t)sizeof(out));
    if (pdu_length == 0)
    {
        return lh_bool_false;
    }
    wrote = lh_io_stream_write(lh_addr_of(self->stream), out, (lh_usize_t)pdu_length);
    if (wrote == LH_IO_PENDING)
    {
        /* Nothing went out: the send buffer is full for the moment. Not a fault --
           the stream is fine and so is the device -- so it is not counted as one;
           the caller gets `false` and asks again on its next pass. */
        self->state = (lh_u8_t)lh_mb_link_idle;
        return lh_bool_false;
    }
    if (wrote != (lh_ssize_t)pdu_length)
    {
        /* A failure, or half a request out. The second is worse than none: what is
           on the line now is a frame nobody sent, and the device will read whatever
           comes next as the rest of it. Either way the stream is lost. */
        (void)lh_mb_link_lose(self);
        return lh_bool_false;
    }
    self->address = address;
    self->expected = count;
    /* Over MBAP the held bytes stay: they arrived before this question went out, so
       they are not its answer, but they may be the start of an answer to the last
       one, and dropping half a frame puts every frame after it off by that much.
       The transaction id is what keeps them from being taken for this answer. */
    if (self->framing != lh_mb_framing_mbap)
    {
        self->held = 0;
    }
    self->got = 0;
    self->asked_tx = self->tx;
    ++self->tx;
    self->counters.sent++;
    self->counters.bytes += (lh_u32_t)pdu_length;
    /* A new question clears the last answer's verdict: ::reason says how the
       previous one ended, and a caller that reads it after asking the next one is
       reading about the wrong question. */
    self->reason = lh_mb_status_ok;
    self->since_us = self->now_us != lh_null ? self->now_us(self->clock) : 0;
    self->asked_fc = fc;
    self->state = (lh_u8_t)lh_mb_link_waiting;
    return lh_bool_true;
}

lh_bool_t
lh_mb_link_ask(lh_mb_link_t *self, lh_u16_t address, lh_u16_t count)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length;

    lh_assert_runtime_ref(self);
    /* One question at a time. A second request sent while the first is unanswered
       comes back after its own answer and is read as somebody else's frame, which
       is a bug that looks like a device answering the wrong question. */
    lh_return_ifn(self->state == (lh_u8_t)lh_mb_link_idle, lh_bool_false);
    lh_return_ifn(count > 0 && count <= LH_MB_READ_MAX, lh_bool_false);

    /* `self->fc`, not a constant. ::lh_mb_link_poll reads the answer back with the same
       field, and a request built with anything else is a question the reader is not
       expecting the answer to: the device replies perfectly to a 0x03 read of
       holding registers, the answer comes back carrying 0x03, and the parser is
       holding 0x04 and calls it somebody else's frame. It looked like a device
       answering the wrong question, which is the most expensive kind of bug to
       chase and the least likely — the answer was fine, the reader was wrong. */
    pdu_length = lh_mb_build_pdu_read(pdu, (lh_u16_t)sizeof(pdu), self->fc, address, count);
    return lh_mb_link_send(self, pdu, pdu_length, address, count, self->fc);
}

lh_bool_t
lh_mb_link_write(lh_mb_link_t *self, lh_u16_t address, const lh_u16_t *values, lh_u16_t count)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];

    lh_assert_runtime_ref(self);
    lh_return_ifn(self->state == (lh_u8_t)lh_mb_link_idle, lh_bool_false);
    lh_return_ifn(lh_null_ne(values) && count > 0, lh_bool_false);
    return lh_mb_link_send(self, pdu,
                           lh_mb_build_pdu_write(pdu, (lh_u16_t)sizeof(pdu), address, values, count),
                           address, count, LH_MB_FC_WRITE_MANY);
}

lh_bool_t
lh_mb_link_write_one(lh_mb_link_t *self, lh_u16_t address, lh_u16_t value)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];

    lh_assert_runtime_ref(self);
    lh_return_ifn(self->state == (lh_u8_t)lh_mb_link_idle, lh_bool_false);
    lh_return_ifn(lh_mb_link_send(self, pdu, lh_mb_build_pdu_write_one(pdu, (lh_u16_t)sizeof(pdu), address, value),
                                  address, 1, LH_MB_FC_WRITE_ONE),
                  lh_bool_false);
    /* After the send, which starts a question by forgetting the last one's. */
    self->written = value;
    return lh_bool_true;
}

lh_mb_link_state_t
lh_mb_link_poll(lh_mb_link_t *self, lh_u32_t timeout_us)
{
    lh_ssize_t got;
    lh_u16_t need;
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length = 0;
    lh_mb_status_t status;

    lh_assert_runtime_ref(self);
    lh_return_ifn(self->state == (lh_u8_t)lh_mb_link_waiting, (lh_mb_link_state_t)self->state);

    /* Nothing to ask for when the buffer is full: it holds a whole frame at least,
       and the loop below consumes it. A read of zero bytes is not a harmless call on
       a real socket -- it returns 0, which is how a socket says the peer has
       closed. */
    got = self->held < (lh_u16_t)sizeof(self->frame)
              ? lh_io_stream_read(lh_addr_of(self->stream), lh_addr_of(self->frame[self->held]),
                                  (lh_usize_t)(sizeof(self->frame) - self->held))
              : LH_IO_PENDING;
    if (got == 0 || (got < 0 && got != LH_IO_PENDING))
    {
        /* Every failure below ends the same way: **back to idle, not stuck.**
           A question that came back wrong, came back short, or never came back is
           one question. Letting it decide the rest of the session is how a device
           that answers "illegal function" for one register bank gets dropped for
           good, and how a poll loop that keeps finding the link busy spins for
           ever instead of moving on to the other devices.

           The caller learns it happened from the state this poll returns, from
           ::lh_mb_link_t.reason and from ::lh_mb_link_counters_t.failed; deciding
           that a device is *off* rather than *slow* is the app's business, because
           only it knows how many failures in a row mean that.

           This one is not a question going wrong but the stream ending -- `0` is
           the peer closing, a negative value a failure -- and it is said as such,
           because the answer to it is a new connection and not another question. */
        return lh_mb_link_lose(self);
    }
    if (got > 0)
    {
        self->held = (lh_u16_t)(self->held + (lh_u16_t)got);
        self->counters.bytes += (lh_u32_t)got;
    }

    /* Every whole frame that is not this question's answer is stepped over, and the
       loop goes on to the one behind it: a recv can hand over another unit's frame,
       or the late answer to a question that timed out, and this answer right after
       it in the same bytes. */
    for (;;)
    {
        lh_bool_t ours;

        need = lh_mb_frame_length(self->framing, self->frame, self->held, self->asked_fc);
        if (need == 0 && self->framing == lh_mb_framing_mbap && self->held >= LH_MB_MBAP_HEAD)
        {
            /* Seven bytes and still no length: the header is not a Modbus header.
               On a stream that means the place is lost, and nothing after it can be
               framed until the connection starts over -- so this is said now, not
               after a timeout spent waiting for a frame that has no end. */
            self->held = 0;
            self->state = (lh_u8_t)lh_mb_link_idle;
            self->reason = lh_mb_status_garbage;
            self->counters.failed++;
            return (lh_mb_link_state_t)lh_mb_link_failed;
        }
        if (need == 0 || self->held < need)
        {
            /* Not yet. How long it has been unanswered is the only thing that turns
               "not yet" into "never", and that check is what stops one dead device
               from holding a place in every pass for ever. */
            const lh_u64_t now = self->now_us != lh_null ? self->now_us(self->clock) : 0;
            if (self->now_us != lh_null && timeout_us > 0 && now - self->since_us > (lh_u64_t)timeout_us)
            {
                /* Over MBAP what has arrived of a frame is kept: if the rest of a
                   slow answer comes later, it completes the frame and the frame is
                   stepped over by its transaction id. Dropping the half that came
                   would read the half that comes as a header. */
                if (self->framing != lh_mb_framing_mbap)
                {
                    self->held = 0;
                }
                self->state = (lh_u8_t)lh_mb_link_idle;
                self->reason = lh_mb_status_short;
                self->counters.failed++;
                return (lh_mb_link_state_t)lh_mb_link_failed;
            }
            return (lh_mb_link_state_t)self->state;
        }

        /* A frame that is whole and is not ours -- another device on the line, or
           over MBAP an answer to some other question -- is dropped and the wait goes
           on, rather than being counted as this device's answer: that is the
           difference between a bus and a fault. The transaction id is checked
           because nothing else can tell a late answer from this one: asked for the
           same registers, it parses perfectly, and its values are stale. */
        ours = lh_mb_decode(self->framing, self->unit, self->frame, need, pdu, lh_addr_of(pdu_length));
        if (ours && self->framing == lh_mb_framing_mbap)
        {
            ours = (lh_bool_t)((((lh_u16_t)self->frame[0] << 8) | self->frame[1]) == self->asked_tx);
        }
        lh_mb_link_consume(self, need);
        if (ours)
        {
            break;
        }
    }
    if (self->asked_fc == LH_MB_FC_WRITE_MANY)
    {
        /* A write's answer is its own header echoed and carries no values. */
        self->got = 0;
        status = lh_mb_parse_pdu_write(pdu, pdu_length, self->address, self->expected, lh_null);
    }
    else if (self->asked_fc == LH_MB_FC_WRITE_ONE)
    {
        /* And a single write's is the whole request echoed, the value with it. */
        self->got = 0;
        status = lh_mb_parse_pdu_write_one(pdu, pdu_length, self->address, self->written, lh_null);
    }
    else
    {
        status = lh_mb_parse_pdu(pdu, pdu_length, self->asked_fc, self->expected, self->values,
                                 (lh_u16_t)LH_MB_LINK_VALUES, lh_addr_of(self->got), lh_null);
    }
    self->reason = status;
    if (status != lh_mb_status_ok && status != lh_mb_status_partial)
    {
        /* No data this time, so the state says failed — but the two reasons want
           opposite things from the app above, and they go to opposite counters.
           A device that refused the address is *talking*: the line is good, the
           framing was right, and the honest thing to do is stop asking that
           address. A device whose answers do not parse is a fault. Counting an
           exception as a failure made a healthy controller indistinguishable from a
           dead one, and the client's rule for "off line" is a run of failures. */
        self->state = (lh_u8_t)lh_mb_link_idle;
        if (status == lh_mb_status_exception)
        {
            self->counters.refused++;
        }
        else
        {
            self->counters.failed++;
        }
        return (lh_mb_link_state_t)lh_mb_link_failed;
    }
    self->counters.done++;
    if (self->now_us != lh_null)
    {
        self->counters.us_busy += self->now_us(self->clock) - self->since_us;
    }
    self->state = (lh_u8_t)lh_mb_link_answered;
    return (lh_mb_link_state_t)self->state;
}

const lh_u16_t *
lh_mb_link_take(lh_mb_link_t *self, lh_u16_t *count)
{
    lh_assert_runtime_ref(self);
    lh_return_ifn(self->state == (lh_u8_t)lh_mb_link_answered, lh_ptr_ccast(lh_u16_t, lh_null));
    if (count != lh_null)
    {
        *count = self->got;
    }
    return self->values;
}

lh_void
lh_mb_link_release(lh_mb_link_t *self)
{
    lh_assert_runtime_ref(self);
    self->state = (lh_u8_t)lh_mb_link_idle;
    /* Over MBAP what is held after the answer is the next frame's start; see
       ::lh_mb_link_consume. */
    if (self->framing != lh_mb_framing_mbap)
    {
        self->held = 0;
    }
}