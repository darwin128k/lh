#include <lh/str/utf8.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/util/return.h>

lh_u32_t
lh_str_utf8_get_encoded_size(lh_u32_t code)
{
    if (code == 0U || code > 0x10FFFFU || (code >= 0xD800U && code <= 0xDFFFU))
    {
        return 0U;
    }
    if (code < 0x80U)
    {
        return 1U;
    }
    if (code < 0x800U)
    {
        return 2U;
    }
    return code < 0x10000U ? 3U : 4U;
}

lh_u32_t
lh_str_utf8_get_lead_size(lh_byte_t lead)
{
    /* Two functions that look like one, and the difference between them is a bug that
       only shows up outside ASCII. ::lh_str_utf8_get_encoded_size answers for a **code
       point**; this one answers for the **first byte** of an encoded character, which
       is what you actually hold when you walk a buffer somebody else filled in.

       Handing a lead byte to the other function works by coincidence and lies:
       `0xD0` (a two-byte lead) is the code point `0xD0`, which is under `0x800`, so
       the two agree -- and every Cyrillic letter agrees with them, which is why a
       suite written with Cyrillic in it stayed green. `0xE0` is a **three**-byte lead,
       and as a code point it is also under `0x800`, so the wrong answer of two comes
       back: a field deleting a character like this removes two of its three bytes and
       leaves the tail of a code point behind as the next character. Same for `0xF0`.
       Our own encoder and our own decoder agreed with each other and both disagreed
       with UTF-8 -- the one thing a round-trip test cannot see. */
    if (lead < 0x80U)
    {
        return 1U;
    }
    if ((lead & 0xE0U) == 0xC0U)
    {
        return 2U;
    }
    if ((lead & 0xF0U) == 0xE0U)
    {
        return 3U;
    }
    if ((lead & 0xF8U) == 0xF0U)
    {
        return 4U;
    }
    /* A continuation byte, or anything that is not a lead byte at all. One is as far
       as it is safe to go: skipping more would step over a character somebody can see,
       and a walk one byte at a time over these is what a reader does with them. */
    return 1U;
}

lh_u32_t
lh_str_utf8_encode(lh_char_t *out, lh_u32_t bytes, lh_u32_t code)
{
    const lh_u32_t size = lh_str_utf8_get_encoded_size(code);

    lh_return_if(lh_null_eq(out), 0U);
    lh_return_if(size == 0U || bytes < size, 0U);

    switch (size)
    {
    case 1U:
        out[0] = lh_cast_static(lh_char_t, code);
        break;
    case 2U:
        out[0] = lh_cast_static(lh_char_t, 0xC0U | (code >> 6));
        out[1] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
        break;
    case 3U:
        out[0] = lh_cast_static(lh_char_t, 0xE0U | (code >> 12));
        out[1] = lh_cast_static(lh_char_t, 0x80U | ((code >> 6) & 0x3FU));
        out[2] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
        break;
    default:
        out[0] = lh_cast_static(lh_char_t, 0xF0U | (code >> 18));
        out[1] = lh_cast_static(lh_char_t, 0x80U | ((code >> 12) & 0x3FU));
        out[2] = lh_cast_static(lh_char_t, 0x80U | ((code >> 6) & 0x3FU));
        out[3] = lh_cast_static(lh_char_t, 0x80U | (code & 0x3FU));
        break;
    }
    return size;
}
