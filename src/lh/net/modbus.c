/**
 * @file modbus.c
 * @brief Modbus RTU: frames, and the fewest requests that cover a map.
 */

#include <lh/net/modbus.h>

#include <string.h>

#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* ── CRC ─────────────────────────────────────────────────────────────────────
   Reflected 0x8005, which Modbus writes as 0xA001 with the low byte first. This
   is the whole reason a frame from a device nobody trusts can be checked at all,
   so it is the one function here that gets its own test with the vectors from the
   specification rather than a round trip through the builder. */

lh_u16_t
lh_mb_crc16(const lh_byte_t *bytes, lh_u16_t length)
{
    lh_u16_t crc = 0xFFFF;

    lh_return_if(lh_null_eq(bytes), (lh_u16_t)0xFFFF);
    while (length > 0)
    {
        lh_u8_t bit;

        crc ^= (lh_u16_t)(*bytes);
        ++bytes;
        --length;
        for (bit = 0; bit < 8; ++bit)
        {
            if ((crc & 1) != 0)
            {
                crc = (lh_u16_t)((crc >> 1) ^ 0xA001);
            }
            else
            {
                crc = (lh_u16_t)(crc >> 1);
            }
        }
    }
    return crc;
}

/* ── Frames ──────────────────────────────────────────────────────────────────
   An RTU frame is unit, function, data, and two CRC bytes low first. There is no
   length field, which is why a parser has to be told how much it has: a frame that
   has not all arrived is not a short frame, it is a frame still on the wire.

   What is built here is the PDU, and the envelope goes on afterwards
   (`lh/net/modbus/link.h`): the same request has to go out over a COM port and
   over a network, and only the envelope differs. */

lh_u16_t
lh_mb_build_pdu_read(lh_byte_t *pdu, lh_u16_t cap, lh_u8_t fc, lh_u16_t address, lh_u16_t count)
{
    lh_return_if(lh_null_eq(pdu), (lh_u16_t)0);
    lh_return_if(cap < 6 || count == 0 || count > LH_MB_READ_MAX, (lh_u16_t)0);
    lh_return_ifn(fc == LH_MB_FC_READ_HOLDING || fc == LH_MB_FC_READ_INPUT, (lh_u16_t)0);

    pdu[0] = fc;
    pdu[1] = (lh_byte_t)((address >> 8) & 0xFF);
    pdu[2] = (lh_byte_t)(address & 0xFF);
    pdu[3] = (lh_byte_t)((count >> 8) & 0xFF);
    pdu[4] = (lh_byte_t)(count & 0xFF);
    return 5;
}

lh_u16_t
lh_mb_build_pdu_write(lh_byte_t *pdu, lh_u16_t cap, lh_u16_t address, const lh_u16_t *values,
                      lh_u16_t count)
{
    lh_u16_t i;

    lh_return_if(lh_null_eq(pdu) || lh_null_eq(values), (lh_u16_t)0);
    lh_return_if(count == 0 || count > LH_MB_READ_MAX, (lh_u16_t)0);
    lh_return_if(cap < (lh_u16_t)(6 + count * 2), (lh_u16_t)0);

    pdu[0] = LH_MB_FC_WRITE_MANY;
    pdu[1] = (lh_byte_t)((address >> 8) & 0xFF);
    pdu[2] = (lh_byte_t)(address & 0xFF);
    pdu[3] = (lh_byte_t)((count >> 8) & 0xFF);
    pdu[4] = (lh_byte_t)(count & 0xFF);
    pdu[5] = (lh_byte_t)(count * 2);
    for (i = 0; i < count; ++i)
    {
        pdu[6 + i * 2] = (lh_byte_t)((values[i] >> 8) & 0xFF);
        pdu[7 + i * 2] = (lh_byte_t)(values[i] & 0xFF);
    }
    return (lh_u16_t)(6 + count * 2);
}

lh_u16_t
lh_mb_build_read(lh_byte_t *adu, lh_u16_t cap, lh_u8_t unit, lh_u8_t fc, lh_u16_t address,
                 lh_u16_t count)
{
    lh_byte_t pdu[16];
    lh_u16_t pdu_length;
    lh_u16_t crc;

    lh_return_if(lh_null_eq(adu), (lh_u16_t)0);
    /* unit, PDU, CRC. */
    lh_return_if(cap < 8, (lh_u16_t)0);
    pdu_length = lh_mb_build_pdu_read(pdu, (lh_u16_t)sizeof(pdu), fc, address, count);
    lh_return_if(pdu_length == 0, (lh_u16_t)0);

    adu[0] = unit;
    memcpy(lh_addr_of(adu[1]), pdu, pdu_length);
    crc = lh_mb_crc16(adu, (lh_u16_t)(1 + pdu_length));
    adu[1 + pdu_length] = (lh_byte_t)(crc & 0xFF);
    adu[2 + pdu_length] = (lh_byte_t)((crc >> 8) & 0xFF);
    return (lh_u16_t)(3 + pdu_length);
}

lh_u16_t
lh_mb_build_write(lh_byte_t *adu, lh_u16_t cap, lh_u8_t unit, lh_u16_t address,
                  const lh_u16_t *values, lh_u16_t count)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length;
    lh_u16_t crc;

    lh_return_if(lh_null_eq(adu) || lh_null_eq(values), (lh_u16_t)0);
    /* unit, PDU, CRC. */
    lh_return_if(count == 0 || count > LH_MB_READ_MAX, (lh_u16_t)0);
    lh_return_if(cap < (lh_u16_t)(9 + count * 2), (lh_u16_t)0);
    pdu_length = lh_mb_build_pdu_write(pdu, (lh_u16_t)sizeof(pdu), address, values, count);
    lh_return_if(pdu_length == 0, (lh_u16_t)0);

    adu[0] = unit;
    memcpy(lh_addr_of(adu[1]), pdu, pdu_length);
    crc = lh_mb_crc16(adu, (lh_u16_t)(1 + pdu_length));
    adu[1 + pdu_length] = (lh_byte_t)(crc & 0xFF);
    adu[2 + pdu_length] = (lh_byte_t)((crc >> 8) & 0xFF);
    return (lh_u16_t)(3 + pdu_length);
}

lh_mb_status_t
lh_mb_parse_pdu(const lh_byte_t *pdu, lh_u16_t length, lh_u8_t fc, lh_u16_t expect,
                lh_u16_t *values, lh_u16_t cap, lh_u16_t *out, lh_u8_t *exception)
{
    lh_u8_t bytes;
    lh_u16_t got;
    lh_u16_t i;

    if (out != lh_null)
    {
        *out = 0;
    }
    if (exception != lh_null)
    {
        *exception = 0;
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

    if (pdu[0] == (lh_u8_t)(fc | 0x80))
    {
        if (exception != lh_null)
        {
            *exception = pdu[1];
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
    bytes = pdu[1];
    if ((bytes % 2) != 0)
    {
        return lh_mb_status_garbage; /* not a whole number of words */
    }
    got = (lh_u16_t)(bytes / 2);
    /* The bytes that arrived decide this, not the byte count inside them: a
       device that says eight words and sends four has not answered, and taking
       its word for how much came is how a parser reads the next frame as its
       own. */
    if ((lh_u16_t)(length - 2) < bytes)
    {
        return lh_mb_status_short;
    }
    if (values != lh_null)
    {
        const lh_u16_t take = got < cap ? got : cap;

        for (i = 0; i < take; ++i)
        {
            values[i] = (lh_u16_t)((lh_u16_t)pdu[2 + i * 2] << 8 | pdu[3 + i * 2]);
        }
    }
    if (out != lh_null)
    {
        *out = got;
    }
    return got < expect ? lh_mb_status_partial : lh_mb_status_ok;
}

lh_mb_status_t
lh_mb_parse_pdu_write(const lh_byte_t *pdu, lh_u16_t length, lh_u16_t address, lh_u16_t count,
                      lh_u8_t *exception)
{
    if (exception != lh_null)
    {
        *exception = 0;
    }
    lh_return_if(lh_null_eq(pdu), lh_mb_status_garbage);
    /* The refusal first, for the reason ::lh_mb_parse_pdu gives: it is two bytes,
       shorter than any answer, and checking the length first calls "no" "not yet". */
    lh_return_if(length < 2, lh_mb_status_short);
    if (pdu[0] == (lh_u8_t)(LH_MB_FC_WRITE_MANY | 0x80))
    {
        if (exception != lh_null)
        {
            *exception = pdu[1];
        }
        return lh_mb_status_exception;
    }
    lh_return_if(length < 5, lh_mb_status_short);
    lh_return_if(pdu[0] != LH_MB_FC_WRITE_MANY, lh_mb_status_wrong_code);
    /* The echo has to be this write: the same address and the same count. */
    lh_return_if(((lh_u16_t)((lh_u16_t)pdu[1] << 8 | pdu[2])) != address ||
                     ((lh_u16_t)((lh_u16_t)pdu[3] << 8 | pdu[4])) != count,
                 lh_mb_status_garbage);
    return lh_mb_status_ok;
}

lh_mb_status_t
lh_mb_parse_read(const lh_byte_t *adu, lh_u16_t length, lh_u8_t unit, lh_u8_t fc, lh_u16_t expect,
                 lh_u16_t *values, lh_u16_t cap, lh_u16_t *out, lh_u8_t *exception)
{
    lh_byte_t pdu[LH_MB_ADU_MAX];
    lh_u16_t pdu_length = 0;

    if (out != lh_null)
    {
        *out = 0;
    }
    if (exception != lh_null)
    {
        *exception = 0;
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
    if (adu[1] == (lh_u8_t)(fc | 0x80))
    {
        if (exception != lh_null)
        {
            *exception = adu[2];
        }
        return lh_mb_status_exception;
    }
    /* Not an exception, so it is a read answer and has to be as long as one:
       unit, code, byte count, one word, CRC. */
    lh_return_if(length < 5, lh_mb_status_short);

    /* The CRC covers everything but the last two bytes, and it is checked before
       anything is taken out of the frame: a corrupted byte count would send the
       copy off the end of the buffer on the strength of a bad byte. */
    if (lh_mb_crc16(adu, (lh_u16_t)(length - 2)) !=
        (lh_u16_t)((lh_u16_t)adu[length - 1] << 8 | adu[length - 2]))
    {
        return lh_mb_status_crc;
    }
    pdu[0] = adu[1];
    memcpy(lh_addr_of(pdu[1]), lh_addr_of(adu[2]), (size_t)(length - 4));
    pdu_length = (lh_u16_t)(length - 3);
    return lh_mb_parse_pdu(pdu, pdu_length, fc, expect, values, cap, out, lh_null);
}

/* ── The plan ────────────────────────────────────────────────────────────────
   One pass over an address-ordered list. The list is already sorted because a
   device map is sorted for exactly this reason, and sorting it here would put a
   second pass on the one path that runs for every device.

   The rule for merging is written once, in `mb_reach`, because the planner and the
   counter have to agree: a counter that disagrees with the planner is a number on
   a slide that the code does not produce, and nobody finds out which of the two is
   wrong until a device is asked for something it was never sent. */

/* How far a run that currently ends at `end` reaches once `next` is folded in, or
   0 when it must not be — the two reasons being a gap worth crossing and a run
   longer than one read may ask for. */
static lh_u16_t
mb_reach(lh_u16_t start, lh_u16_t end, const lh_mb_want_t *next)
{
    const lh_u16_t next_end = (lh_u16_t)(next->address + (next->registers ? next->registers : 1));
    const lh_u16_t reach = next_end > end ? next_end : end;

    if (next->address > end && next->address - end > LH_MB_GAP_MAX)
    {
        return 0; /* nobody wants what is between, and there is a lot of it */
    }
    if (reach - start > LH_MB_READ_MAX)
    {
        return 0; /* one read may not ask for this much */
    }
    return reach;
}

lh_u16_t
lh_mb_plan_reads(const lh_mb_want_t *wants, lh_u16_t count, lh_mb_read_t *reads, lh_u16_t cap)
{
    lh_u16_t made = 0;
    lh_u16_t i = 0;

    lh_return_if(lh_null_eq(wants) || lh_null_eq(reads), (lh_u16_t)0);
    lh_return_if(count == 0 || cap == 0, (lh_u16_t)0);

    while (i < count && made < cap)
    {
        const lh_u16_t start = wants[i].address;
        lh_u16_t end = (lh_u16_t)(start + (wants[i].registers ? wants[i].registers : 1));
        lh_u16_t j = i;
        lh_mb_read_t *one;

        /* Take every want this request can carry for free. */
        while (j + 1 < count)
        {
            const lh_u16_t reach = mb_reach(start, end, lh_addr_of(wants[j + 1]));

            if (reach == 0)
            {
                break;
            }
            end = reach;
            ++j;
        }

        one = lh_addr_of(reads[made]);
        one->start = start;
        one->count = (lh_u16_t)(end - start);
        one->first = i;
        one->last = j;
        ++made;
        i = (lh_u16_t)(j + 1);
    }
    return made;
}

lh_u16_t
lh_mb_count_requests(const lh_mb_want_t *wants, lh_u16_t count)
{
    lh_u16_t made = 0;
    lh_u16_t i = 0;

    lh_return_if(lh_null_eq(wants), (lh_u16_t)0);
    lh_return_if(count == 0, (lh_u16_t)0);

    while (i < count)
    {
        const lh_u16_t start = wants[i].address;
        lh_u16_t end = (lh_u16_t)(start + (wants[i].registers ? wants[i].registers : 1));
        lh_u16_t j = i;

        while (j + 1 < count)
        {
            const lh_u16_t reach = mb_reach(start, end, lh_addr_of(wants[j + 1]));

            if (reach == 0)
            {
                break;
            }
            end = reach;
            ++j;
        }
        ++made;
        i = (lh_u16_t)(j + 1);
    }
    return made;
}