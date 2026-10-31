/**
 * @file ws.c
 * @brief Implementation of `lh/ui/web/ws.h`.
 */

#include <lh/ui/web/ws.h>

#include <lh/math.h>

lh_bool_t
lh_ui_web_ws_is_ok_opcode(lh_uint_t opcode)
{
    return lh_cast_static(lh_bool_t, lh_math_le(opcode, 2U) ||
                                         (lh_math_ge(opcode, 8U) && lh_math_le(opcode, 10U)));
}

lh_usize_t
lh_ui_web_ws_frame_size(lh_usize_t payload_len)
{
    if (payload_len < 126u)
    {
        return 2u + payload_len;
    }
    if (payload_len <= 65535u)
    {
        return 4u + payload_len;
    }
    return 10u + payload_len;
}

lh_usize_t
lh_ui_web_ws_write(lh_byte_ptr out, lh_usize_t cap, lh_uint_t opcode, lh_bool_t fin,
                   const void *payload, lh_usize_t payload_len)
{
    const lh_usize_t need = lh_ui_web_ws_frame_size(payload_len);
    lh_byte_cptr src = lh_ptr_ccast(lh_byte_t, payload);

    if (out == lh_null || !lh_ui_web_ws_is_ok_opcode(opcode) || need == 0u || need > cap)
    {
        return 0;
    }
    if (payload_len > 0u && src == lh_null)
    {
        return 0;
    }
    out[0] = lh_cast_static(lh_byte_t, ((fin ? 0x80u : 0u) | (opcode & 0x0fu)));
    lh_usize_t off;
    if (payload_len < 126u)
    {
        out[1] = lh_cast_static(lh_byte_t, payload_len);
        off = 2u;
    }
    else if (payload_len <= 65535u)
    {
        out[1] = 126u;
        out[2] = lh_cast_static(lh_byte_t, (payload_len >> 8));
        out[3] = lh_cast_static(lh_byte_t, payload_len);
        off = 4u;
    }
    else
    {
        out[1] = 127u;
        for (lh_usize_t i = 0; i < 8u; ++i)
        {
            out[9u - i] =
                lh_cast_static(lh_byte_t, (lh_cast_static(lh_u64_t, payload_len) >> (8u * i)));
        }
        off = 10u;
    }
    for (lh_usize_t i = 0; i < payload_len; ++i)
    {
        out[off + i] = src[i];
    }
    return need;
}

lh_usize_t
lh_ui_web_ws_read(lh_byte_cptr p, lh_usize_t n, lh_uint_t *opcode, lh_bool_t *fin,
                  lh_byte_cptr *payload, lh_usize_t *payload_len, lh_byte_cptr *mask)
{
    lh_usize_t off = 2u;

    if (p == lh_null || n < 2u)
    {
        return 0;
    }
    if (opcode != lh_null)
    {
        lh_ptr_deref(opcode) = lh_cast_static(lh_uint_t, (p[0] & 0x0fu));
    }
    if (fin != lh_null)
    {
        lh_ptr_deref(fin) = (p[0] & 0x80u) ? lh_bool_true : lh_bool_false;
    }
    if (!lh_ui_web_ws_is_ok_opcode(lh_cast_static(lh_uint_t, (p[0] & 0x0fu))))
    {
        return (lh_usize_t)-1;
    }
    lh_uint_t masked = lh_cast_static(lh_uint_t, (p[1] & 0x80u));
    lh_uint_t len7 = lh_cast_static(lh_uint_t, (p[1] & 0x7fu));
    lh_usize_t plen;
    if (len7 == 126u)
    {
        if (n < 4u)
        {
            return 0;
        }
        plen = (lh_cast_static(lh_usize_t, p[2]) << 8) | lh_cast_static(lh_usize_t, p[3]);
        off = 4u;
    }
    else if (len7 == 127u)
    {
        if (n < 10u)
        {
            return 0;
        }
        lh_u64_t wide = 0;
        for (lh_uint_t i = 0; i < 8u; ++i)
        {
            wide = (wide << 8) | lh_cast_static(lh_u64_t, p[2u + i]);
        }
        if ((wide >> 63) != 0u || wide > lh_cast_static(lh_u64_t, LH_UI_WEB_WS_MAX_MSG))
        {
            return (lh_usize_t)-1;
        }
        plen = lh_cast_static(lh_usize_t, wide);
        off = 10u;
    }
    else
    {
        plen = lh_cast_static(lh_usize_t, len7);
    }
    if (plen > LH_UI_WEB_WS_MAX_MSG)
    {
        return (lh_usize_t)-1;
    }
    if (masked != 0u)
    {
        if (n < off + 4u)
        {
            return 0;
        }
        if (mask != lh_null)
        {
            lh_ptr_deref(mask) = p + off;
        }
        off += 4u;
    }
    else if (mask != lh_null)
    {
        lh_ptr_deref(mask) = lh_null;
    }
    if (n < off + plen)
    {
        return 0;
    }
    if (payload != lh_null)
    {
        lh_ptr_deref(payload) = p + off;
    }
    if (payload_len != lh_null)
    {
        lh_ptr_deref(payload_len) = plen;
    }
    return off + plen;
}

lh_void
lh_ui_web_ws_unmask(lh_byte_ptr data, lh_usize_t len, const lh_byte_t mask[4])
{
    lh_usize_t i;

    if (data == lh_null || mask == lh_null)
    {
        return;
    }
    for (i = 0; i < len; ++i)
    {
        data[i] = lh_cast_static(lh_byte_t, (data[i] ^ mask[i & 3u]));
    }
}
