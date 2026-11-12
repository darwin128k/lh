#include <lh/core/net/modbus/server.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/bit/endian.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/return.h>

lh_mb_framing_t
lh_mb_server_get_framing(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->framing;
}

lh_void
lh_mb_server_set_framing(lh_mb_server_t *self, lh_mb_framing_t framing)
{
    lh_assert_runtime_ref(self);
    self->framing = framing;
}

lh_u8_t
lh_mb_server_get_unit(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->unit;
}

lh_void
lh_mb_server_set_unit(lh_mb_server_t *self, lh_u8_t unit)
{
    lh_assert_runtime_ref(self);
    self->unit = unit;
}

lh_mb_server_read_cb
lh_mb_server_get_read(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->read;
}

lh_void
lh_mb_server_set_read(lh_mb_server_t *self, lh_mb_server_read_cb read)
{
    lh_assert_runtime_ref(self);
    self->read = read;
}

lh_mb_server_write_cb
lh_mb_server_get_write(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->write;
}

lh_void
lh_mb_server_set_write(lh_mb_server_t *self, lh_mb_server_write_cb write)
{
    lh_assert_runtime_ref(self);
    self->write = write;
}

lh_ptr
lh_mb_server_get_owner(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_void
lh_mb_server_set_owner(lh_mb_server_t *self, lh_ptr owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

lh_mb_clock_cb
lh_mb_server_get_now_us(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->now_us;
}

lh_void
lh_mb_server_set_now_us(lh_mb_server_t *self, lh_mb_clock_cb now_us)
{
    lh_assert_runtime_ref(self);
    self->now_us = now_us;
}

lh_ptr
lh_mb_server_get_clock(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clock;
}

lh_void
lh_mb_server_set_clock(lh_mb_server_t *self, lh_ptr clock)
{
    lh_assert_runtime_ref(self);
    self->clock = clock;
}

lh_u32_t
lh_mb_server_get_gap_us(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gap_us;
}

lh_void
lh_mb_server_set_gap_us(lh_mb_server_t *self, lh_u32_t gap_us)
{
    lh_assert_runtime_ref(self);
    self->gap_us = gap_us;
}

lh_u64_t
lh_mb_server_get_heard_us(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->heard_us;
}

lh_void
lh_mb_server_set_heard_us(lh_mb_server_t *self, lh_u64_t heard_us)
{
    lh_assert_runtime_ref(self);
    self->heard_us = heard_us;
}

lh_u16_t
lh_mb_server_get_held(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->held;
}

lh_void
lh_mb_server_set_held(lh_mb_server_t *self, lh_u16_t held)
{
    lh_assert_runtime_ref(self);
    self->held = held;
}

const lh_io_stream_t *
lh_mb_server_get_stream(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->stream);
}

lh_io_stream_t *
lh_mb_server_get_stream_mut(lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->stream);
}

const lh_mb_server_counters_t *
lh_mb_server_get_counters(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->counters);
}

lh_mb_server_counters_t *
lh_mb_server_get_counters_mut(lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->counters);
}

const lh_byte_t *
lh_mb_server_get_frame(const lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frame;
}

lh_byte_t *
lh_mb_server_get_frame_mut(lh_mb_server_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frame;
}

lh_u64_t
lh_mb_server_get_time_us(const lh_mb_server_t *self)
{
    return lh_mb_server_get_now_us(self) != lh_null
               ? lh_mb_server_get_now_us(self)(lh_mb_server_get_clock(self))
               : 0;
}

lh_bool_t
lh_mb_server_is_for_unit(const lh_mb_server_t *self, lh_u8_t unit)
{
    /* 255 is what Modbus TCP calls "no unit" -- the server itself. Over RTU it is
       not an address a device can have, and nobody else's either. */
    return lh_cast_static(lh_bool_t,
                          (unit == lh_mb_server_get_unit(self) || unit == LH_MB_UNIT_BROADCAST ||
                           (lh_mb_server_get_framing(self) == lh_mb_framing_mbap && unit == 0xFF)));
}

lh_bool_t
lh_mb_server_is_broadcast(const lh_mb_server_t *self, lh_u8_t unit)
{
    return lh_cast_static(lh_bool_t, (lh_mb_server_get_framing(self) == lh_mb_framing_rtu &&
                                      unit == LH_MB_UNIT_BROADCAST));
}

lh_u16_t
lh_mb_server_get_request_length(const lh_mb_server_t *self)
{
    if (lh_mb_server_get_framing(self) == lh_mb_framing_mbap)
    {
        return lh_mb_frame_length(lh_mb_framing_mbap, lh_mb_server_get_frame(self),
                                  lh_mb_server_get_held(self), 0);
    }
    return lh_mb_rtu_request_length(lh_mb_server_get_frame(self), lh_mb_server_get_held(self));
}

lh_void
lh_mb_server_consume(lh_mb_server_t *self, lh_u16_t n)
{
    if (n >= lh_mb_server_get_held(self))
    {
        lh_mb_server_set_held(self, 0);
        return;
    }
    lh_memory_std_move(lh_mb_server_get_frame_mut(self), lh_mb_server_get_frame(self) + n,
                       lh_cast_static(lh_usize_t, (lh_mb_server_get_held(self) - n)));
    lh_mb_server_set_held(self, lh_cast_static(lh_u16_t, (lh_mb_server_get_held(self) - n)));
}

lh_mb_server_result_t
lh_mb_server_lose(lh_mb_server_t *self)
{
    lh_mb_server_set_held(self, 0);
    return lh_mb_server_lost;
}

lh_void
lh_mb_server_set_stream(lh_mb_server_t *self, const lh_io_stream_t *stream)
{
    lh_io_stream_assign(lh_mb_server_get_stream_mut(self), stream);
    /* A new connection is a new stream: whatever was held belonged to the old one. */
    lh_mb_server_set_held(self, 0);
}

lh_u16_t
lh_mb_server_answer_read(lh_mb_server_t *self, lh_byte_cptr pdu, lh_u16_t length, lh_byte_ptr out,
                         lh_u16_t cap)
{
    lh_return_if(lh_null_eq(pdu) || lh_null_eq(out) || length == 0, lh_cast_static(lh_u16_t, 0));
    const lh_u8_t fc = pdu[0];

    /* The specification's order: the code, then the quantity, then the address,
       then the device. A request that is wrong in two ways is refused for the first
       of them, which is what a master that tests a device expects to see. */
    if (length != 5)
    {
        return lh_mb_build_pdu_exception(out, cap, fc, LH_MB_EXCEPTION_ILLEGAL_VALUE);
    }
    const lh_u16_t address = lh_bit_unpack_be16(lh_addr_of(pdu[1]));
    const lh_u16_t count = lh_bit_unpack_be16(lh_addr_of(pdu[3]));
    if (count == 0 || count > LH_MB_READ_MAX)
    {
        return lh_mb_build_pdu_exception(out, cap, fc, LH_MB_EXCEPTION_ILLEGAL_VALUE);
    }
    if (lh_cast_static(lh_u32_t, address) + count > 0x10000U)
    {
        return lh_mb_build_pdu_exception(out, cap, fc, LH_MB_EXCEPTION_ILLEGAL_ADDRESS);
    }
    if (lh_mb_server_get_read(self) == lh_null)
    {
        return lh_mb_build_pdu_exception(out, cap, fc, LH_MB_EXCEPTION_ILLEGAL_FUNCTION);
    }
    lh_u16_t values[LH_MB_READ_MAX];
    const lh_u8_t refused =
        lh_mb_server_get_read(self)(lh_mb_server_get_owner(self), fc, address, count, values);
    if (refused != 0)
    {
        return lh_mb_build_pdu_exception(out, cap, fc, refused);
    }
    return lh_mb_build_pdu_read_answer(out, cap, fc, values, count);
}

lh_u16_t
lh_mb_server_answer_write_one(lh_mb_server_t *self, lh_byte_cptr pdu, lh_u16_t length,
                              lh_byte_ptr out, lh_u16_t cap)
{
    lh_return_if(lh_null_eq(pdu) || lh_null_eq(out) || length == 0, lh_cast_static(lh_u16_t, 0));
    if (length != 5)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_ONE,
                                         LH_MB_EXCEPTION_ILLEGAL_VALUE);
    }
    if (lh_mb_server_get_write(self) == lh_null)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_ONE,
                                         LH_MB_EXCEPTION_ILLEGAL_FUNCTION);
    }
    const lh_u16_t value = lh_bit_unpack_be16(lh_addr_of(pdu[3]));
    const lh_u8_t refused = lh_mb_server_get_write(self)(
        lh_mb_server_get_owner(self), lh_bit_unpack_be16(lh_addr_of(pdu[1])), 1, lh_addr_of(value));
    if (refused != 0)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_ONE, refused);
    }
    /* The answer is the request, all five bytes of it. */
    lh_return_if(cap < 5, lh_cast_static(lh_u16_t, 0));
    lh_memory_std_copy(out, pdu, 5);
    return 5;
}

lh_u16_t
lh_mb_server_answer_write_many(lh_mb_server_t *self, lh_byte_cptr pdu, lh_u16_t length,
                               lh_byte_ptr out, lh_u16_t cap)
{
    lh_return_if(lh_null_eq(pdu) || lh_null_eq(out) || length == 0, lh_cast_static(lh_u16_t, 0));
    if (length < 6)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_MANY,
                                         LH_MB_EXCEPTION_ILLEGAL_VALUE);
    }
    const lh_u16_t address = lh_bit_unpack_be16(lh_addr_of(pdu[1]));
    const lh_u16_t count = lh_bit_unpack_be16(lh_addr_of(pdu[3]));
    /* Three numbers that must agree: the count, the byte count, and the bytes that
       came. A byte count that disagrees with the count is a request that would
       write words nobody sent, or leave out words somebody did. */
    if (count == 0 || count > LH_MB_WRITE_MAX || pdu[5] != count * 2U ||
        length != lh_cast_static(lh_u16_t, (6 + pdu[5])))
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_MANY,
                                         LH_MB_EXCEPTION_ILLEGAL_VALUE);
    }
    if (lh_cast_static(lh_u32_t, address) + count > 0x10000U)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_MANY,
                                         LH_MB_EXCEPTION_ILLEGAL_ADDRESS);
    }
    if (lh_mb_server_get_write(self) == lh_null)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_MANY,
                                         LH_MB_EXCEPTION_ILLEGAL_FUNCTION);
    }
    lh_u16_t values[LH_MB_WRITE_MAX];
    for (lh_u16_t i = 0; i < count; ++i)
    {
        values[i] = lh_bit_unpack_be16(lh_addr_of(pdu[6 + i * 2]));
    }
    const lh_u8_t refused =
        lh_mb_server_get_write(self)(lh_mb_server_get_owner(self), address, count, values);
    if (refused != 0)
    {
        return lh_mb_build_pdu_exception(out, cap, LH_MB_FC_WRITE_MANY, refused);
    }
    /* The answer is the request's code, address and count. */
    lh_return_if(cap < 5, lh_cast_static(lh_u16_t, 0));
    lh_memory_std_copy(out, pdu, 5);
    return 5;
}

lh_void
lh_mb_server_init(lh_mb_server_t *self, const lh_io_stream_t *stream, lh_mb_framing_t framing,
                  lh_u8_t unit, lh_mb_server_read_cb read, lh_mb_server_write_cb write,
                  lh_ptr owner, lh_mb_clock_cb now_us, lh_ptr clock)
{
    lh_assert_runtime_ref(self);

    lh_memory_std_set(lh_addr_of(lh_ptr_deref(self)), 0, sizeof(lh_ptr_deref(self)));
    lh_io_stream_assign(lh_mb_server_get_stream_mut(self), stream);
    lh_mb_server_set_framing(self, framing);
    lh_mb_server_set_unit(self, unit);
    lh_mb_server_set_read(self, read);
    lh_mb_server_set_write(self, write);
    lh_mb_server_set_owner(self, owner);
    lh_mb_server_set_now_us(self, now_us);
    lh_mb_server_set_clock(self, clock);
}

lh_u16_t
lh_mb_server_answer(lh_mb_server_t *self, lh_byte_cptr pdu, lh_u16_t length, lh_byte_ptr out,
                    lh_u16_t cap)
{
    lh_return_if(lh_null_eq(pdu) || length == 0, lh_cast_static(lh_u16_t, 0));
    if (pdu[0] == LH_MB_FC_READ_HOLDING || pdu[0] == LH_MB_FC_READ_INPUT)
    {
        return lh_mb_server_answer_read(self, pdu, length, out, cap);
    }
    if (pdu[0] == LH_MB_FC_WRITE_ONE)
    {
        return lh_mb_server_answer_write_one(self, pdu, length, out, cap);
    }
    if (pdu[0] == LH_MB_FC_WRITE_MANY)
    {
        return lh_mb_server_answer_write_many(self, pdu, length, out, cap);
    }
    return lh_mb_build_pdu_exception(out, cap, pdu[0], LH_MB_EXCEPTION_ILLEGAL_FUNCTION);
}

lh_bool_t
lh_mb_server_try_reply(lh_mb_server_t *self, lh_u8_t unit, lh_u16_t tx, lh_byte_cptr pdu,
                       lh_u16_t length)
{
    lh_byte_t out[LH_MB_ADU_MAX];
    const lh_u16_t framed = lh_mb_encode(lh_mb_server_get_framing(self), unit, tx, pdu, length, out,
                                         lh_cast_static(lh_u16_t, sizeof(out)));

    lh_return_if(framed == 0, lh_bool_false);
    if (lh_io_stream_write(lh_mb_server_get_stream_mut(self), out,
                           lh_cast_static(lh_usize_t, framed)) !=
        lh_cast_static(lh_ssize_t, framed))
    {
        /* Nothing, or half an answer: either way the master will not get this one,
           and half of one on the line is a frame nobody sent. */
        LH_ATTRIBUTE_UNUSED(lh_mb_server_lose(self));
        return lh_bool_false;
    }
    lh_mb_server_counters_add_bytes(lh_mb_server_get_counters_mut(self),
                                    lh_cast_static(lh_u32_t, framed));
    return lh_bool_true;
}

lh_mb_server_result_t
lh_mb_server_serve(lh_mb_server_t *self, lh_u16_t length)
{
    const lh_byte_t *frame = lh_mb_server_get_frame(self);
    lh_u8_t unit;
    lh_u16_t tx = 0;
    lh_byte_cptr pdu;
    lh_u16_t pdu_length;

    if (lh_mb_server_get_framing(self) == lh_mb_framing_mbap)
    {
        tx = lh_bit_unpack_be16(frame);
        unit = frame[6];
        pdu = lh_addr_of(frame[7]);
        pdu_length = lh_cast_static(lh_u16_t, (length - LH_MB_MBAP_HEAD));
    }
    else
    {
        /* Checked before the unit: a frame whose CRC fails may have a wrong unit as
           well, and a frame for us that arrived broken must not be answered. */
        if (!lh_mb_rtu_is_crc_valid(frame, length))
        {
            lh_mb_server_counters_count_broken(lh_mb_server_get_counters_mut(self));
            return lh_mb_server_broken;
        }
        unit = frame[0];
        pdu = lh_addr_of(frame[1]);
        pdu_length = lh_cast_static(lh_u16_t, (length - 3));
    }
    if (!lh_mb_server_is_for_unit(self, unit))
    {
        lh_mb_server_counters_count_ignored(lh_mb_server_get_counters_mut(self));
        return lh_mb_server_ignored;
    }
    lh_mb_server_counters_count_received(lh_mb_server_get_counters_mut(self));
    lh_byte_t answer[LH_MB_ADU_MAX];
    if (lh_mb_server_is_broadcast(self, unit))
    {
        /* Done by every device, answered by none. A read to every unit has nothing
           to do and nobody to tell, so it is not even asked of the application. */
        if (pdu[0] == LH_MB_FC_WRITE_ONE || pdu[0] == LH_MB_FC_WRITE_MANY)
        {
            LH_ATTRIBUTE_UNUSED(lh_mb_server_answer(self, pdu, pdu_length, answer,
                                                    lh_cast_static(lh_u16_t, sizeof(answer))));
        }
        lh_mb_server_counters_count_broadcast(lh_mb_server_get_counters_mut(self));
        return lh_mb_server_broadcast;
    }
    const lh_u16_t answer_length = lh_mb_server_answer(self, pdu, pdu_length, answer,
                                                       lh_cast_static(lh_u16_t, sizeof(answer)));
    lh_return_ifn(lh_mb_server_try_reply(self, unit, tx, answer, answer_length), lh_mb_server_lost);
    if ((answer[0] & 0x80) != 0)
    {
        lh_mb_server_counters_count_refused(lh_mb_server_get_counters_mut(self));
        return lh_mb_server_refused;
    }
    lh_mb_server_counters_count_answered(lh_mb_server_get_counters_mut(self));
    return lh_mb_server_answered;
}

lh_mb_server_result_t
lh_mb_server_serve_unsized(lh_mb_server_t *self)
{
    const lh_u16_t held = lh_mb_server_get_held(self);

    if (lh_mb_server_get_framing(self) == lh_mb_framing_mbap)
    {
        /* Seven bytes and no length: not a Modbus header, and nothing after it can
           be framed on this stream. */
        return held >= LH_MB_MBAP_HEAD ? lh_mb_server_lose(self) : lh_mb_server_idle;
    }
    if (held < 2 || lh_mb_is_fc_supported(lh_mb_server_get_frame(self)[1]))
    {
        return lh_mb_server_idle; /* the code, or a byte count, still to come */
    }
    if (lh_mb_server_get_gap_us(self) == 0 || lh_mb_server_get_now_us(self) == lh_null)
    {
        /* No silence to end it: the frame cannot be told from the next one. */
        lh_mb_server_set_held(self, 0);
        lh_mb_server_counters_count_broken(lh_mb_server_get_counters_mut(self));
        return lh_mb_server_broken;
    }
    if (lh_mb_server_get_time_us(self) - lh_mb_server_get_heard_us(self) <
        lh_mb_server_get_gap_us(self))
    {
        return lh_mb_server_idle;
    }
    /* The line has been silent: everything held is one frame, and its code is one
       this server does not speak. ::lh_mb_server_serve checks it and refuses it. */
    const lh_mb_server_result_t result = lh_mb_server_serve(self, held);
    lh_mb_server_set_held(self, 0);
    return result;
}

lh_mb_server_result_t
lh_mb_server_poll(lh_mb_server_t *self)
{
    lh_u16_t need = lh_mb_server_get_request_length(self);

    if (need == 0 || lh_mb_server_get_held(self) < need)
    {
        /* Nothing to ask for when the buffer is full; see ::lh_mb_link_poll. */
        const lh_u16_t before = lh_mb_server_get_held(self);
        const lh_ssize_t got =
            before < lh_cast_static(lh_u16_t, LH_MB_ADU_MAX)
                ? lh_io_stream_read(lh_mb_server_get_stream_mut(self),
                                    lh_mb_server_get_frame_mut(self) + before,
                                    lh_cast_static(lh_usize_t, (LH_MB_ADU_MAX - before)))
                : LH_IO_PENDING;

        if (got == 0 || (got < 0 && got != LH_IO_PENDING))
        {
            return lh_mb_server_lose(self);
        }
        if (got > 0)
        {
            const lh_u64_t now = lh_mb_server_get_time_us(self);

            lh_mb_server_set_held(self, lh_cast_static(lh_u16_t, (before + got)));
            lh_mb_server_counters_add_bytes(lh_mb_server_get_counters_mut(self),
                                            lh_cast_static(lh_u32_t, got));
            /* Bytes after a silence longer than the gap begin a new frame: what was
               held before them was a request broken off, and goes. */
            if (lh_mb_server_get_framing(self) == lh_mb_framing_rtu &&
                lh_mb_server_get_gap_us(self) > 0 && before > 0 &&
                now - lh_mb_server_get_heard_us(self) > lh_mb_server_get_gap_us(self))
            {
                lh_mb_server_counters_count_broken(lh_mb_server_get_counters_mut(self));
                lh_mb_server_consume(self, before);
            }
            lh_mb_server_set_heard_us(self, now);
        }
        need = lh_mb_server_get_request_length(self);
    }
    if (need == 0)
    {
        return lh_mb_server_serve_unsized(self);
    }
    if (need > LH_MB_ADU_MAX)
    {
        /* A "write many" whose byte count no request can have: it would never fit. */
        lh_mb_server_set_held(self, 0);
        lh_mb_server_counters_count_broken(lh_mb_server_get_counters_mut(self));
        return lh_mb_server_broken;
    }
    if (lh_mb_server_get_held(self) < need)
    {
        return lh_mb_server_idle;
    }
    const lh_mb_server_result_t result = lh_mb_server_serve(self, need);
    if (result == lh_mb_server_broken)
    {
        /* A bad CRC: the length was read off a frame that is not what it says, so
           where the next one starts is not known. Everything held goes, and the next
           request is found after it. */
        lh_mb_server_set_held(self, 0);
    }
    else if (result != lh_mb_server_lost)
    {
        lh_mb_server_consume(self, need);
    }
    return result;
}
