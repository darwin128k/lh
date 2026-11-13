/**
 * @file modbus.c
 * @brief Modbus RTU: frames, and the fewest requests that cover a map.
 *
 * Ordered by level (AGENTS.md 0.0): what calls nothing of this file first, then what
 * calls only that, and so on. The frames, the server's side of the wire and the plan
 * therefore interleave; the comments below say which is which.
 */

#include <lh/core/net/modbus.h>
#include <lh/core/hash/crc.h>
#include <lh/core/bit/endian.h>

#include <lh/core/memory/std.h>
#include <lh/core/util/return.h>

/* -- CRC -------------------------------------------------------------------
   Reflected 0x8005, which Modbus writes as 0xA001 with the low byte first. This
   is the whole reason a frame from a device nobody trusts can be checked at all,
   so it is the one function here that gets its own test with the vectors from the
   specification rather than a round trip through the builder. */

lh_u16_t
lh_mb_crc16(lh_byte_cptr bytes, lh_u16_t length)
{
    lh_return_if(lh_null_eq(bytes), lh_cast_static(lh_u16_t, 0xFFFFU));
    return lh_cast_static(lh_u16_t, lh_hash_crc_reflected_update(0xFFFFU, bytes, length, 0xA001U));
}

/* -- Frames ----------------------------------------------------------------
   An RTU frame is unit, function, data, and two CRC bytes low first. There is no
   length field, which is why a parser has to be told how much it has: a frame that
   has not all arrived is not a short frame, it is a frame still on the wire.

   What is built here is the PDU, and the envelope goes on afterwards
   (`lh/core/net/modbus/link.h`): the same request has to go out over a COM port and
   over a network, and only the envelope differs. */

lh_u16_t
lh_mb_build_pdu_read(lh_byte_ptr pdu, lh_u16_t cap, lh_u8_t fc, lh_u16_t address, lh_u16_t count)
{
    lh_return_if(lh_null_eq(pdu), lh_cast_static(lh_u16_t, 0));
    lh_return_if(cap < 6 || count == 0 || count > LH_MB_READ_MAX, lh_cast_static(lh_u16_t, 0));
    lh_return_ifn(fc == LH_MB_FC_READ_HOLDING || fc == LH_MB_FC_READ_INPUT,
                  lh_cast_static(lh_u16_t, 0));

    pdu[0] = fc;
    lh_bit_pack_be16(address, lh_addr_of(pdu[1]));
    lh_bit_pack_be16(count, lh_addr_of(pdu[3]));
    return 5;
}

lh_u16_t
lh_mb_build_pdu_write(lh_byte_ptr pdu, lh_u16_t cap, lh_u16_t address, const lh_u16_t *values,
                      lh_u16_t count)
{
    lh_return_if(lh_null_eq(pdu) || lh_null_eq(values), lh_cast_static(lh_u16_t, 0));
    lh_return_if(count == 0 || count > LH_MB_WRITE_MAX, lh_cast_static(lh_u16_t, 0));
    lh_return_if(cap < lh_cast_static(lh_u16_t, (6 + count * 2)), lh_cast_static(lh_u16_t, 0));

    pdu[0] = LH_MB_FC_WRITE_MANY;
    lh_bit_pack_be16(address, lh_addr_of(pdu[1]));
    lh_bit_pack_be16(count, lh_addr_of(pdu[3]));
    pdu[5] = lh_cast_static(lh_byte_t, (count * 2));
    for (lh_u16_t i = 0; i < count; ++i)
    {
        lh_bit_pack_be16(values[i], lh_addr_of(pdu[6 + i * 2]));
    }
    return (lh_u16_t)(6 + count * 2);
}

lh_u16_t
lh_mb_build_pdu_write_one(lh_byte_ptr pdu, lh_u16_t cap, lh_u16_t address, lh_u16_t value)
{
    lh_return_if(lh_null_eq(pdu) || cap < 5, lh_cast_static(lh_u16_t, 0));
    pdu[0] = LH_MB_FC_WRITE_ONE;
    lh_bit_pack_be16(address, lh_addr_of(pdu[1]));
    lh_bit_pack_be16(value, lh_addr_of(pdu[3]));
    return 5;
}

lh_mb_status_t
lh_mb_parse_pdu(lh_byte_cptr pdu, lh_u16_t length, lh_u8_t fc, lh_u16_t expect, lh_u16_t *values,
                lh_u16_t cap, lh_u16_t *out, lh_u8_t *exception)
{
    if (out != lh_null)
    {
        lh_ptr_deref(out) = 0;
    }
    if (exception != lh_null)
    {
        lh_ptr_deref(exception) = 0;
    }
    lh_return_if(lh_null_eq(pdu), lh_mb_status_garbage);
    /* The **exception first, and the length of an exception is two.** That is the
       whole PDU: the function code with its high bit set, and the reason. A device
       taken off a panel with nothing more to say than "illegal data value" puts two
       bytes on the wire, and the check below -- function code, byte count, one word,
       four bytes -- called that *unfinished*.

       It was invisible for the same reason the other eight were: our own encoder
       never builds an exception, so a round trip never carries one, and every test
       that fed this function an exception had to invent its length. The first thing
       to notice was a real controller, which answered a 126-register read with
       `00 07 00 00 00 03 01 83 03` and was reported by this parser as a device that
       had not finished talking -- so "no" and "not yet" were the same word, and a
       client that has to tell a healthy device from a broken one could not. */
    lh_return_if(length < 2, lh_mb_status_short);

    if (pdu[0] == lh_cast_static(lh_u8_t, (fc | 0x80)))
    {
        if (exception != lh_null)
        {
            lh_ptr_deref(exception) = pdu[1];
        }
        return lh_mb_status_exception;
    }
    /* Not an exception, so it is a read answer and has to be as long as one:
       function code, byte count, one word. */
    lh_return_if(length < 4, lh_mb_status_short);

    if (pdu[0] != fc)
    {
        return lh_mb_status_wrong_code;
    }
    lh_u8_t bytes = pdu[1];
    if (!lh_math_is_multiple_of(bytes, 2))
    {
        return lh_mb_status_garbage; /* not a whole number of words */
    }
    lh_u16_t got = lh_cast_static(lh_u16_t, (bytes / 2));
    /* The bytes that arrived decide this, not the byte count inside them: a
       device that says eight words and sends four has not answered, and taking
       its word for how much came is how a parser reads the next frame as its
       own. */
    if (lh_cast_static(lh_u16_t, (length - 2)) < bytes)
    {
        return lh_mb_status_short;
    }
    /* More words than were asked for is not a generous answer, it is an answer to
       some other question -- or a byte count that is wrong and a frame whose end is
       somewhere else. Neither can be taken: the words cannot be told from the bytes
       of whatever was mistaken for them. */
    if (got > expect)
    {
        return lh_mb_status_garbage;
    }
    if (values != lh_null)
    {
        const lh_u16_t take = lh_math_min(got, cap);

        for (lh_u16_t i = 0; i < take; ++i)
        {
            values[i] = lh_bit_unpack_be16(lh_addr_of(pdu[2 + i * 2]));
        }
    }
    if (out != lh_null)
    {
        lh_ptr_deref(out) = got;
    }
    return got < expect ? lh_mb_status_partial : lh_mb_status_ok;
}

lh_mb_status_t
lh_mb_parse_pdu_write_one(lh_byte_cptr pdu, lh_u16_t length, lh_u16_t address, lh_u16_t value,
                          lh_u8_t *exception)
{
    if (exception != lh_null)
    {
        lh_ptr_deref(exception) = 0;
    }
    lh_return_if(lh_null_eq(pdu), lh_mb_status_garbage);
    lh_return_if(length < 2, lh_mb_status_short);
    if (pdu[0] == lh_cast_static(lh_u8_t, (LH_MB_FC_WRITE_ONE | 0x80)))
    {
        if (exception != lh_null)
        {
            lh_ptr_deref(exception) = pdu[1];
        }
        return lh_mb_status_exception;
    }
    lh_return_if(length < 5, lh_mb_status_short);
    lh_return_if(pdu[0] != LH_MB_FC_WRITE_ONE, lh_mb_status_wrong_code);
    /* The whole request comes back, so the value is checked as well as the
       address: a device that stored something else says so here. */
    lh_return_if(lh_bit_unpack_be16(lh_addr_of(pdu[1])) != address ||
                     lh_bit_unpack_be16(lh_addr_of(pdu[3])) != value,
                 lh_mb_status_garbage);
    return lh_mb_status_ok;
}

/* The server's side of the wire: which codes a request may carry, and the answers
   a device builds. */

lh_bool_t
lh_mb_is_fc_supported(lh_u8_t fc)
{
    return lh_cast_static(lh_bool_t, (fc == LH_MB_FC_READ_HOLDING || fc == LH_MB_FC_READ_INPUT ||
                                      fc == LH_MB_FC_WRITE_ONE || fc == LH_MB_FC_WRITE_MANY));
}

lh_u16_t
lh_mb_build_pdu_exception(lh_byte_ptr pdu, lh_u16_t cap, lh_u8_t fc, lh_u8_t code)
{
    lh_return_if(lh_null_eq(pdu) || cap < 2, lh_cast_static(lh_u16_t, 0));
    pdu[0] = lh_cast_static(lh_byte_t, (fc | 0x80));
    pdu[1] = code;
    return 2;
}

lh_u16_t
lh_mb_build_pdu_read_answer(lh_byte_ptr pdu, lh_u16_t cap, lh_u8_t fc, const lh_u16_t *values,
                            lh_u16_t count)
{
    lh_return_if(lh_null_eq(pdu) || lh_null_eq(values), lh_cast_static(lh_u16_t, 0));
    lh_return_if(count == 0 || count > LH_MB_READ_MAX, lh_cast_static(lh_u16_t, 0));
    lh_return_if(cap < lh_cast_static(lh_u16_t, (2 + count * 2)), lh_cast_static(lh_u16_t, 0));

    pdu[0] = fc;
    pdu[1] = lh_cast_static(lh_byte_t, (count * 2));
    for (lh_u16_t i = 0; i < count; ++i)
    {
        lh_bit_pack_be16(values[i], lh_addr_of(pdu[2 + i * 2]));
    }
    return lh_cast_static(lh_u16_t, (2 + count * 2));
}

/* -- The plan --------------------------------------------------------------
   One pass over an address-ordered list. The list is already sorted because a
   device map is sorted for exactly this reason, and sorting it here would put a
   second pass on the one path that runs for every device.

   The rule for merging is written once, in `lh_mb_reach` and
   `lh_mb_find_run_last`, because the planner and the counter have to agree: a counter that disagrees with the planner is a number on
   a slide that the code does not produce, and nobody finds out which of the two is
   wrong until a device is asked for something it was never sent. */

lh_u32_t
lh_mb_reach(lh_u16_t start, lh_u32_t end, const lh_mb_want_t *next)
{
    const lh_u32_t address = lh_mb_want_get_address(next);
    const lh_u32_t reach = lh_math_max(lh_mb_want_get_end(next), end);

    if (address > end && address - end > LH_MB_GAP_MAX)
    {
        return 0; /* nobody wants what is between, and there is a lot of it */
    }
    if (reach - start > LH_MB_READ_MAX)
    {
        return 0; /* one read may not ask for this much */
    }
    return reach;
}

lh_bool_t
lh_mb_rtu_is_crc_valid(lh_byte_cptr frame, lh_u16_t length)
{
    lh_return_ifn(lh_null_ne(frame) && length >= 4, lh_bool_false);
    return lh_cast_static(lh_bool_t, lh_mb_crc16(frame, lh_cast_static(lh_u16_t, (length - 2))) ==
                                         lh_bit_unpack_le16(lh_addr_of(frame[length - 2])));
}

lh_u16_t
lh_mb_build_read(lh_byte_ptr adu, lh_u16_t cap, lh_u8_t unit, lh_u8_t fc, lh_u16_t address,
                 lh_u16_t count)
{
    lh_return_if(lh_null_eq(adu), lh_cast_static(lh_u16_t, 0));
    /* unit, PDU, CRC. */
    lh_return_if(cap < 8, lh_cast_static(lh_u16_t, 0));
    lh_byte_t pdu[16];
    lh_u16_t pdu_length =
        lh_mb_build_pdu_read(pdu, lh_cast_static(lh_u16_t, sizeof(pdu)), fc, address, count);
    lh_return_if(pdu_length == 0, lh_cast_static(lh_u16_t, 0));

    adu[0] = unit;
    lh_memory_std_copy(lh_addr_of(adu[1]), pdu, pdu_length);
    lh_u16_t crc = lh_mb_crc16(adu, lh_cast_static(lh_u16_t, (1 + pdu_length)));
    lh_bit_pack_le16(crc, lh_addr_of(adu[1 + pdu_length]));
    return (lh_u16_t)(3 + pdu_length);
}

lh_u16_t
lh_mb_build_write(lh_byte_ptr adu, lh_u16_t cap, lh_u8_t unit, lh_u16_t address,
                  const lh_u16_t *values, lh_u16_t count)
{
    lh_return_if(lh_null_eq(adu) || lh_null_eq(values), lh_cast_static(lh_u16_t, 0));
    /* unit, PDU, CRC. */
    lh_return_if(count == 0 || count > LH_MB_READ_MAX, lh_cast_static(lh_u16_t, 0));
    lh_return_if(cap < lh_cast_static(lh_u16_t, (9 + count * 2)), lh_cast_static(lh_u16_t, 0));
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length =
        lh_mb_build_pdu_write(pdu, lh_cast_static(lh_u16_t, sizeof(pdu)), address, values, count);
    lh_return_if(pdu_length == 0, lh_cast_static(lh_u16_t, 0));

    adu[0] = unit;
    lh_memory_std_copy(lh_addr_of(adu[1]), pdu, pdu_length);
    lh_u16_t crc = lh_mb_crc16(adu, lh_cast_static(lh_u16_t, (1 + pdu_length)));
    lh_bit_pack_le16(crc, lh_addr_of(adu[1 + pdu_length]));
    return (lh_u16_t)(3 + pdu_length);
}

lh_mb_status_t
lh_mb_parse_pdu_write(lh_byte_cptr pdu, lh_u16_t length, lh_u16_t address, lh_u16_t count,
                      lh_u8_t *exception)
{
    if (exception != lh_null)
    {
        lh_ptr_deref(exception) = 0;
    }
    lh_return_if(lh_null_eq(pdu), lh_mb_status_garbage);
    /* The refusal first, for the reason ::lh_mb_parse_pdu gives: it is two bytes,
       shorter than any answer, and checking the length first calls "no" "not yet". */
    lh_return_if(length < 2, lh_mb_status_short);
    if (pdu[0] == lh_cast_static(lh_u8_t, (LH_MB_FC_WRITE_MANY | 0x80)))
    {
        if (exception != lh_null)
        {
            lh_ptr_deref(exception) = pdu[1];
        }
        return lh_mb_status_exception;
    }
    lh_return_if(length < 5, lh_mb_status_short);
    lh_return_if(pdu[0] != LH_MB_FC_WRITE_MANY, lh_mb_status_wrong_code);
    /* The echo has to be this write: the same address and the same count. */
    lh_return_if(lh_bit_unpack_be16(lh_addr_of(pdu[1])) != address ||
                     lh_bit_unpack_be16(lh_addr_of(pdu[3])) != count,
                 lh_mb_status_garbage);
    return lh_mb_status_ok;
}

lh_u16_t
lh_mb_rtu_request_length(lh_byte_cptr frame, lh_u16_t have)
{
    lh_return_if(lh_null_eq(frame) || have < 2, lh_cast_static(lh_u16_t, 0));
    lh_return_ifn(lh_mb_is_fc_supported(frame[1]), lh_cast_static(lh_u16_t, 0));
    if (frame[1] != LH_MB_FC_WRITE_MANY)
    {
        return 8; /* unit, code, address, count or value, CRC */
    }
    /* unit, code, address, count, byte count, the words, CRC */
    lh_return_if(have < 7, lh_cast_static(lh_u16_t, 0));
    return lh_cast_static(lh_u16_t, (9 + frame[6]));
}

lh_u16_t
lh_mb_find_run_last(const lh_mb_want_t *wants, lh_u16_t count, lh_u16_t first, lh_u32_t *end)
{
    const lh_u16_t start = lh_mb_want_get_address(lh_addr_of(wants[first]));
    lh_u32_t reached = lh_mb_want_get_end(lh_addr_of(wants[first]));
    lh_u16_t last = first;

    /* Take every want this request can carry for free. */
    while (last + 1 < count)
    {
        const lh_u32_t reach = lh_mb_reach(start, reached, lh_addr_of(wants[last + 1]));

        if (reach == 0)
        {
            break;
        }
        reached = reach;
        ++last;
    }
    lh_ptr_deref(end) = reached;
    return last;
}

lh_mb_status_t
lh_mb_parse_read(lh_byte_cptr adu, lh_u16_t length, lh_u8_t unit, lh_u8_t fc, lh_u16_t expect,
                 lh_u16_t *values, lh_u16_t cap, lh_u16_t *out, lh_u8_t *exception)
{
    lh_u16_t pdu_length = 0;

    if (out != lh_null)
    {
        lh_ptr_deref(out) = 0;
    }
    if (exception != lh_null)
    {
        lh_ptr_deref(exception) = 0;
    }
    lh_return_if(lh_null_eq(adu), lh_mb_status_garbage);
    /* unit, code, reason: under that there is nothing to look at. */
    lh_return_if(length < 3, lh_mb_status_short);

    /* Somebody else's answer. A bus carries every unit's traffic, and this is not
       a fault in our line — it is the bus doing what a bus does. Asked before the
       CRC, because a frame for unit 7 is not ours to spend time checking. */
    if (adu[0] != unit)
    {
        return lh_mb_status_wrong_unit;
    }
    /* An exception is five bytes: unit, code | 80, code, CRC. It is read here and
       not by ::lh_mb_parse_pdu, because the exception of an RTU frame carries the
       unit in front of it and a PDU's does not — the two shapes are one field
       apart, and taking the exception from the wrong one reports the unit number
       as the reason the device refused.

       Asked before the five-byte check for the same reason ::lh_mb_parse_pdu asks
       it before its four: the reason is at byte 3, and a device that has only a
       reason to give has given a whole answer. */
    if (adu[1] == lh_cast_static(lh_u8_t, (fc | 0x80)))
    {
        if (exception != lh_null)
        {
            lh_ptr_deref(exception) = adu[2];
        }
        return lh_mb_status_exception;
    }
    /* Not an exception, so it is a read answer and has to be as long as one:
       unit, code, byte count, one word, CRC. */
    lh_return_if(length < 5, lh_mb_status_short);

    /* The CRC covers everything but the last two bytes, and it is checked before
       anything is taken out of the frame: a corrupted byte count would send the
       copy off the end of the buffer on the strength of a bad byte. */
    if (!lh_mb_rtu_is_crc_valid(adu, length))
    {
        return lh_mb_status_crc;
    }
    lh_byte_t pdu[LH_MB_ADU_MAX];
    pdu[0] = adu[1];
    lh_memory_std_copy(lh_addr_of(pdu[1]), lh_addr_of(adu[2]),
                       lh_cast_static(lh_usize_t, (length - 4)));
    pdu_length = lh_cast_static(lh_u16_t, (length - 3));
    return lh_mb_parse_pdu(pdu, pdu_length, fc, expect, values, cap, out, lh_null);
}

lh_u16_t
lh_mb_plan_reads(const lh_mb_want_t *wants, lh_u16_t count, lh_mb_read_t *reads, lh_u16_t cap)
{
    lh_u16_t made = 0;
    lh_u16_t i = 0;

    lh_return_if(lh_null_eq(wants) || lh_null_eq(reads), lh_cast_static(lh_u16_t, 0));
    lh_return_if(count == 0 || cap == 0, lh_cast_static(lh_u16_t, 0));

    while (i < count && made < cap)
    {
        const lh_u16_t start = lh_mb_want_get_address(lh_addr_of(wants[i]));
        lh_u32_t end = 0;
        const lh_u16_t j = lh_mb_find_run_last(wants, count, i, lh_addr_of(end));
        lh_mb_read_t *one = lh_addr_of(reads[made]);

        lh_mb_read_set_start(one, start);
        lh_mb_read_set_count(one, lh_cast_static(lh_u16_t, (end - start)));
        lh_mb_read_set_first(one, i);
        lh_mb_read_set_last(one, j);
        ++made;
        i = lh_cast_static(lh_u16_t, (j + 1));
    }
    return made;
}

lh_u16_t
lh_mb_count_requests(const lh_mb_want_t *wants, lh_u16_t count)
{
    lh_u16_t made = 0;
    lh_u16_t i = 0;

    lh_return_if(lh_null_eq(wants), lh_cast_static(lh_u16_t, 0));
    lh_return_if(count == 0, lh_cast_static(lh_u16_t, 0));

    while (i < count)
    {
        lh_u32_t end = 0;

        ++made;
        i = lh_cast_static(lh_u16_t, (lh_mb_find_run_last(wants, count, i, lh_addr_of(end)) + 1));
    }
    return made;
}