/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>
#include <lh/util/str/ptr.h>
#include <lh/char/map.h>
#include <lh/ui/web/page.h>

#include <lh/cast/const.h>
#include <lh/crypto/sha.h>
#include <lh/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/net.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/logger.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/text.h>
#include <lh/ui/view/controller.h>
#include <lh/util/memory.h>
#include <lh/str/format/base64.h>
#include <lh/util/return.h>

/** The player page, built once by ::lh_ui_web_page_build. */
static lh_byte_t m_ui_web_host_page[LH_UI_WEB_HOST_PAGE_CAP];
/** Bytes of ::m_ui_web_host_page in use. */
static lh_usize_t m_ui_web_host_page_len;

lh_void
lh_ui_web_host_reply(lh_os_net_socket_t *client, lh_uint_t code, lh_str_cptr type, lh_str_cptr body,
                     lh_usize_t len)
{
    lh_char_t head[256];
    lh_str_cptr reason =
        (code == 200u)
            ? "OK"
            : ((code == 304u) ? "Not Modified" : ((code == 500u) ? "Error" : "Not Found"));
    const lh_usize_t n = lh_str_ptr_format_text(
        head, sizeof(head),
        "HTTP/1.1 %u %s\r\nContent-Type: %s\r\nContent-Length: %u\r\n"
        "Connection: close\r\nCache-Control: no-store\r\n\r\n",
        lh_cast_static(lh_uint_t, code), reason, type, lh_cast_static(lh_uint_t, len));

    (void)lh_os_net_socket_send_all(client, head, n);
    (void)lh_os_net_socket_send_all(client, body, len);
}

lh_void
lh_ui_web_host_reply_text(lh_os_net_socket_t *client, lh_uint_t code, lh_str_cptr text,
                          lh_usize_t len)
{
    lh_ui_web_host_reply(client, code, "text/plain; charset=utf-8", text, len);
}

lh_str_cptr
lh_ui_web_host_find(lh_str_cptr p, lh_usize_t len, lh_str_cptr needle, lh_usize_t needle_len)
{
    lh_str_cptr at =
        lh_ptr_rcast(const lh_char_t, lh_memory_find(lh_cast_const(lh_ptr, p), len,
                                                     lh_cast_const(lh_ptr, needle), needle_len));

    return (at != lh_null && lh_cast_static(lh_usize_t, (at - p)) + needle_len <= len) ? at
                                                                                       : lh_null;
}

lh_bool_t
lh_ui_web_host_route(lh_str_cptr req, lh_usize_t len, lh_str_cptr path, lh_usize_t path_len)
{
    if (lh_str_ptr_starts_with_by_size(req, len, "GET ", 4u, lh_bool_false))
    {
        return lh_str_ptr_starts_with_by_size(req + 4u, len - 4u, path, path_len, lh_bool_false);
    }
    if (lh_str_ptr_starts_with_by_size(req, len, "POST ", 5u, lh_bool_false))
    {
        return lh_str_ptr_starts_with_by_size(req + 5u, len - 5u, path, path_len, lh_bool_false);
    }
    return lh_bool_false;
}

lh_usize_t
lh_ui_web_host_query(lh_str_cptr req, lh_usize_t len, lh_str_cptr key, lh_usize_t key_len,
                     lh_str_cptr *value)
{
    lh_usize_t off = 0u;

    for (;;)
    {
        lh_str_cptr at = lh_ui_web_host_find(req + off, len - off, key, key_len);
        lh_usize_t n = 0u;

        if (at == lh_null)
        {
            return 0u;
        }
        off = lh_cast_static(lh_usize_t, (at - req)) + key_len;
        if (at == req || (at[-1] != lh_char_map_question && at[-1] != lh_char_map_ampersand))
        {
            continue;
        }
        while (off + n < len && req[off + n] != lh_char_map_ampersand && req[off + n] != lh_char_map_space &&
               req[off + n] != lh_char_map_cr && req[off + n] != lh_char_map_lf)
        {
            ++n;
        }
        lh_ptr_deref(value) = req + off;
        return n;
    }
}

lh_sint_t
lh_ui_web_host_query_int(lh_str_cptr req, lh_usize_t len, lh_str_cptr key, lh_usize_t key_len)
{
    lh_str_cptr value = lh_null;
    const lh_usize_t n = lh_ui_web_host_query(req, len, key, key_len, lh_addr_of(value));
    lh_sint_t v = 0;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%d", lh_addr_of(v)) == 0u)
    {
        return 0;
    }
    return lh_cast_static(lh_sint_t, v);
}

lh_uint_t
lh_ui_web_host_query_uint(lh_str_cptr req, lh_usize_t len, lh_str_cptr key, lh_usize_t key_len)
{
    lh_str_cptr value = lh_null;
    const lh_usize_t n = lh_ui_web_host_query(req, len, key, key_len, lh_addr_of(value));
    lh_uint_t v = 0u;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%u", lh_addr_of(v)) == 0u)
    {
        return 0u;
    }
    return lh_cast_static(lh_uint_t, v);
}

lh_bool_t
lh_ui_web_host_query_flag(lh_str_cptr req, lh_usize_t len, lh_str_cptr key, lh_usize_t key_len,
                          lh_bool_t absent)
{
    lh_str_cptr value = lh_null;

    if (lh_ui_web_host_query(req, len, key, key_len, lh_addr_of(value)) == 0u)
    {
        return absent;
    }
    return (lh_ui_web_host_query_int(req, len, key, key_len) != 0) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_web_host_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
                     const lh_os_input_event_t *event)
{
    lh_bool_t changed = lh_bool_false;

    lh_return_if(hooks == lh_null || (lh_ui_web_host_hooks_get_input_fn(hooks) == lh_null && lh_ui_web_host_hooks_get_controller(hooks) == lh_null),
                 lh_ui_view_input(view, event));
    if (lh_ui_web_host_hooks_get_input_fn(hooks) != lh_null)
    {
        lh_ui_web_host_hooks_get_input_fn(hooks)(lh_ui_web_host_hooks_get_ctx(hooks), event);
        changed = lh_bool_true;
    }
    if (lh_ui_web_host_hooks_get_controller(hooks) != lh_null && lh_os_controller_feed(lh_ui_web_host_hooks_get_controller(hooks), event))
    {
        changed = lh_bool_true;
    }
    return changed;
}

lh_bool_t
lh_ui_web_host_pointer(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
                       lh_os_input_kind_t kind, lh_sint_t x, lh_sint_t y)
{
    lh_os_input_event_t event;
    const lh_bool_t up = lh_type_cast(lh_bool_t, kind == lh_os_input_kind_pointer_up);
    const lh_os_input_button_t button = kind == lh_os_input_kind_pointer_move
                                            ? lh_os_input_button_none
                                            : lh_os_input_button_primary;

    lh_os_input_event_init_pointer(lh_addr_of(event), kind, lh_os_input_device_kind_mouse, 0U,
                                   button, lh_os_input_coord_from_int(x),
                                   lh_os_input_coord_from_int(y), lh_os_tick_ms());
    lh_os_input_event_set_buttons(lh_addr_of(event),
                                  up ? 0U : lh_os_input_button_to_mask(lh_os_input_button_primary));
    return lh_ui_web_host_input(view, hooks, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_host_wheel(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_sint_t x,
                     lh_sint_t y, lh_sint_t dx, lh_sint_t dy)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_wheel(
        lh_addr_of(event), lh_os_input_coord_from_int(x), lh_os_input_coord_from_int(y),
        lh_type_cast(lh_s32_t, dx) * LH_OS_INPUT_WHEEL_NOTCH,
        lh_type_cast(lh_s32_t, dy) * LH_OS_INPUT_WHEEL_NOTCH, lh_os_tick_ms());
    return lh_ui_web_host_input(view, hooks, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_host_resize(const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width, lh_uint_t *height,
                      lh_sint_t w, lh_sint_t h)
{
    if (width == lh_null || height == lh_null)
    {
        return lh_bool_false;
    }
    if (w < LH_UI_WEB_HOST_SIZE_MIN)
    {
        w = LH_UI_WEB_HOST_SIZE_MIN;
    }
    if (h < LH_UI_WEB_HOST_SIZE_MIN)
    {
        h = LH_UI_WEB_HOST_SIZE_MIN;
    }
    if (w > LH_UI_WEB_HOST_SIZE_MAX)
    {
        w = LH_UI_WEB_HOST_SIZE_MAX;
    }
    if (h > LH_UI_WEB_HOST_SIZE_MAX)
    {
        h = LH_UI_WEB_HOST_SIZE_MAX;
    }
    if (lh_cast_static(lh_uint_t, w) == lh_ptr_deref(width) &&
        lh_cast_static(lh_uint_t, h) == lh_ptr_deref(height))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(width) = lh_cast_static(lh_uint_t, w);
    lh_ptr_deref(height) = lh_cast_static(lh_uint_t, h);
    if (hooks != lh_null && hooks->resize != lh_null)
    {
        hooks->resize(w, h, lh_ui_web_host_hooks_get_ctx(hooks));
    }
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_host_key(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_sint_t k,
                   lh_bool_t pressed)
{
    lh_os_input_event_t event;

    if (k < lh_cast_static(lh_sint_t, lh_os_key_other) ||
        k > lh_cast_static(lh_sint_t, lh_os_key_alt))
    {
        k = lh_cast_static(lh_sint_t, lh_os_key_other);
    }
    lh_os_input_event_init_key(lh_addr_of(event), lh_type_cast(lh_os_key_t, k), pressed,
                               lh_bool_false, 0U, lh_os_tick_ms());
    return lh_ui_web_host_input(view, hooks, lh_addr_of(event));
}

lh_bool_t
lh_ui_web_host_text(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_u32_t code)
{
    lh_os_input_event_t event;

    lh_return_if(code == 0U, lh_bool_false);
    lh_os_input_event_init_text(lh_addr_of(event), code, lh_os_tick_ms());
    return lh_ui_web_host_input(view, hooks, lh_addr_of(event));
}

lh_void
lh_ui_web_host_bump(lh_uint_t *gen)
{
    if (gen != lh_null)
    {
        ++lh_ptr_deref(gen);
    }
}

lh_bool_t
lh_ui_web_host_ws_upgrade(lh_os_net_socket_t *client, lh_str_cptr req, lh_usize_t len)
{
    static const lh_char_t guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    lh_str_cptr k = lh_ui_web_host_find(req, len, lh_str_lit("Sec-WebSocket-Key:"));
    lh_char_t cat[80];
    lh_byte_t digest[LH_CRYPTO_SHA1_DIGEST_SIZE];
    lh_char_t accept[32];
    lh_char_t head[280];
    lh_usize_t n = 0u;
    lh_usize_t h;

    if (k == lh_null)
    {
        k = lh_ui_web_host_find(req, len, lh_str_lit("sec-websocket-key:"));
    }
    if (k == lh_null)
    {
        return lh_bool_false;
    }
    k += sizeof("Sec-WebSocket-Key:") - 1u;
    while (k < req + len && lh_ptr_deref(k) == lh_char_map_space)
    {
        ++k;
    }
    while (k + n < req + len && k[n] != lh_char_map_cr && k[n] != lh_char_map_lf && n < sizeof(cat) - sizeof(guid))
    {
        cat[n] = k[n];
        ++n;
    }
    lh_memory_copy(cat + n, sizeof(cat) - n, lh_cast_const(lh_ptr, guid), sizeof(guid) - 1u);
    n += sizeof(guid) - 1u;
    if (lh_crypto_sha_hash(lh_crypto_sha_kind_1, cat, n, digest, sizeof(digest)) !=
        LH_CRYPTO_SHA1_DIGEST_SIZE)
    {
        return lh_bool_false;
    }
    accept[lh_str_ptr_format_base64(digest, sizeof(digest), accept, sizeof(accept) - 1u)] = lh_char_map_nul;
    h = lh_str_ptr_format_text(head, sizeof(head),
                               "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n"
                               "Connection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
                               accept);
    if (h == 0u)
    {
        return lh_bool_false;
    }
    (void)lh_os_net_socket_send_all(client, head, h);
    (void)lh_os_net_socket_set_blocking(client, lh_bool_false);
    return lh_bool_true;
}

lh_void
lh_ui_web_host_event_line(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width,
                          lh_uint_t *height, lh_uint_t *gen, lh_str_cptr line, lh_usize_t len)
{
    lh_sint_t x = 0;
    lh_sint_t y = 0;
    lh_sint_t dx = 0;
    lh_sint_t dy = 0;
    lh_sint_t k = 0;
    lh_sint_t pressed = 0;
    lh_uint_t code = 0u;

    if (lh_str_ptr_parse_text(line, len, "p %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_down,
                                     lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y));
    }
    else if (lh_str_ptr_parse_text(line, len, "r %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_up,
                                     lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y));
    }
    else if (lh_str_ptr_parse_text(line, len, "w %d %d %d %d", lh_addr_of(x), lh_addr_of(y),
                                   lh_addr_of(dx), lh_addr_of(dy)) != 0u)
    {
        (void)lh_ui_web_host_wheel(view, hooks, lh_cast_static(lh_sint_t, x),
                                   lh_cast_static(lh_sint_t, y), lh_cast_static(lh_sint_t, dx),
                                   lh_cast_static(lh_sint_t, dy));
    }
    else if (lh_str_ptr_parse_text(line, len, "m %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* A drag sends one every frame; one that changed nothing is not worth a draw. */
        if (!lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_move,
                                    lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y)))
        {
            return;
        }
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d %d", lh_addr_of(k), lh_addr_of(pressed)) != 0u)
    {
        (void)lh_ui_web_host_key(view, hooks, lh_cast_static(lh_sint_t, k),
                                 (pressed != 0) ? lh_bool_true : lh_bool_false);
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d", lh_addr_of(k)) != 0u)
    {
        (void)lh_ui_web_host_key(view, hooks, lh_cast_static(lh_sint_t, k), lh_bool_true);
    }
    else if (lh_str_ptr_parse_text(line, len, "t %u", lh_addr_of(code)) != 0u)
    {
        (void)lh_ui_web_host_text(view, hooks, lh_cast_static(lh_u32_t, code));
    }
    else if (lh_str_ptr_parse_text(line, len, "z %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* Always bump: an unchanged size still needs a frame for a new tab, or the
           player keeps the design 980×620 it got before `z` arrived. */
        (void)lh_ui_web_host_resize(hooks, width, height, lh_cast_static(lh_sint_t, x),
                                    lh_cast_static(lh_sint_t, y));
    }
    else
    {
        return;
    }
    lh_ui_web_host_bump(gen);
}

lh_void
lh_ui_web_host_ws_drop(lh_ui_web_host_client_t *c)
{
    lh_ui_web_host_frame_t *q;

    if (c == lh_null || !lh_ui_web_host_client_is_used(c))
    {
        return;
    }
    lh_os_net_socket_close(lh_ui_web_host_client_get_sock_mut(c));
    lh_os_net_socket_init(lh_ui_web_host_client_get_sock_mut(c));
    q = lh_ui_web_host_client_get_qhead(c);
    while (q != lh_null)
    {
        lh_ui_web_host_frame_t *n = lh_ui_web_host_frame_get_next(q);

        lh_runtime_allocator_free(lh_ui_web_host_frame_get_bytes(q));
        lh_runtime_allocator_free(q);
        q = n;
    }
    lh_runtime_allocator_free(lh_ui_web_host_client_get_rx(c));
    lh_runtime_allocator_free(lh_ui_web_host_client_get_msg(c));
    lh_ui_web_host_client_set_rx(c, lh_null);
    lh_ui_web_host_client_set_msg(c, lh_null);
    lh_ui_web_host_client_set_rx_len(c, 0u);
    lh_ui_web_host_client_set_msg_len(c, 0u);
    lh_ui_web_host_client_set_msg_cap(c, 0u);
    lh_ui_web_host_client_set_msg_on(c, lh_bool_false);
    lh_ui_web_host_client_set_qhead(c, lh_null);
    lh_ui_web_host_client_set_qtail(c, lh_null);
    lh_ui_web_host_client_set_queued(c, 0u);
    lh_ui_web_host_client_set_used(c, lh_bool_false);
    lh_ui_web_host_client_set_closing(c, lh_bool_false);
}

lh_bool_t
lh_ui_web_host_ws_enqueue(lh_ui_web_host_client_t *c, lh_uint_t opcode, const void *payload,
                          lh_usize_t plen, lh_bool_t front)
{
    const lh_usize_t need = lh_ui_web_ws_frame_size(plen);
    lh_ui_web_host_frame_t *q;
    lh_byte_ptr bytes;

    if (c == lh_null || !lh_ui_web_host_client_is_used(c))
    {
        return lh_bool_false;
    }
    if (need == 0u || lh_ui_web_host_client_get_queued(c) + lh_cast_static(lh_uint_t, need) > LH_UI_WEB_HOST_WS_QMAX)
    {
        lh_ui_web_host_ws_drop(c);
        return lh_bool_false;
    }
    q = lh_ptr_rcast(lh_ui_web_host_frame_t, lh_os_alloc(sizeof(lh_ptr_deref(q))));
    bytes = lh_ptr_rcast(lh_byte_t, lh_os_alloc(need));
    if (q == lh_null || bytes == lh_null ||
        lh_ui_web_ws_write(bytes, need, opcode, lh_bool_true, payload, plen) != need)
    {
        lh_runtime_allocator_free(q);
        lh_runtime_allocator_free(bytes);
        lh_ui_web_host_ws_drop(c);
        return lh_bool_false;
    }
    lh_ui_web_host_frame_set_bytes(q, bytes);
    lh_ui_web_host_frame_set_len(q, lh_cast_static(lh_uint_t, need));
    lh_ui_web_host_frame_set_off(q, 0u);
    if (front)
    {
        lh_ui_web_host_frame_set_next(q, lh_ui_web_host_client_get_qhead(c));
        lh_ui_web_host_client_set_qhead(c, q);
        if (lh_ui_web_host_client_get_qtail(c) == lh_null)
        {
            lh_ui_web_host_client_set_qtail(c, q);
        }
    }
    else
    {
        lh_ui_web_host_frame_set_next(q, lh_null);
        if (lh_ui_web_host_client_get_qtail(c) != lh_null)
        {
            lh_ui_web_host_frame_set_next(lh_ui_web_host_client_get_qtail(c), q);
        }
        else
        {
            lh_ui_web_host_client_set_qhead(c, q);
        }
        lh_ui_web_host_client_set_qtail(c, q);
    }
    lh_ui_web_host_client_set_queued(c, lh_ui_web_host_client_get_queued(c) + (lh_cast_static(lh_uint_t, need)));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_ws_flush(lh_ui_web_host_client_t *c)
{
    while (c != lh_null && lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_get_qhead(c) != lh_null)
    {
        lh_ui_web_host_frame_t *q = lh_ui_web_host_client_get_qhead(c);
        const lh_ssize_t w = lh_os_net_socket_send(lh_ui_web_host_client_get_sock_mut(c), lh_ui_web_host_frame_get_bytes(q) + lh_ui_web_host_frame_get_off(q),
                                                   lh_cast_static(lh_usize_t, (lh_ui_web_host_frame_get_len(q) - lh_ui_web_host_frame_get_off(q))));

        if (w == LH_OS_RESULT_PENDING)
        {
            return;
        }
        if (w <= 0)
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        lh_ui_web_host_frame_set_off(q, lh_ui_web_host_frame_get_off(q) + (lh_cast_static(lh_uint_t, w)));
        lh_ui_web_host_client_set_queued(c, lh_ui_web_host_client_get_queued(c) - (lh_cast_static(lh_uint_t, w)));
        if (lh_ui_web_host_frame_get_off(q) >= lh_ui_web_host_frame_get_len(q))
        {
            lh_ui_web_host_client_set_qhead(c, lh_ui_web_host_frame_get_next(q));
            if (lh_ui_web_host_client_get_qhead(c) == lh_null)
            {
                lh_ui_web_host_client_set_qtail(c, lh_null);
            }
            lh_runtime_allocator_free(lh_ui_web_host_frame_get_bytes(q));
            lh_runtime_allocator_free(q);
        }
    }
    if (c != lh_null && lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_is_closing(c) && lh_ui_web_host_client_get_qhead(c) == lh_null)
    {
        lh_ui_web_host_ws_drop(c);
    }
}

lh_bool_t
lh_ui_web_host_ws_msg_add(lh_ui_web_host_client_t *c, lh_byte_cptr p, lh_usize_t n)
{
    lh_byte_ptr grow;

    if (lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n) > LH_UI_WEB_WS_MAX_MSG)
    {
        return lh_bool_false;
    }
    if (lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n) > lh_ui_web_host_client_get_msg_cap(c))
    {
        const lh_uint_t cap = lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n);
        const lh_uint_t next = (lh_ui_web_host_client_get_msg_cap(c) == 0u) ? 256u : (lh_ui_web_host_client_get_msg_cap(c) * 2u);
        const lh_uint_t want = (cap > next) ? cap : next;

        grow = lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_realloc(lh_ui_web_host_client_get_msg(c), lh_ui_web_host_client_get_msg_cap(c), want));
        if (grow == lh_null)
        {
            return lh_bool_false;
        }
        lh_ui_web_host_client_set_msg(c, grow);
        lh_ui_web_host_client_set_msg_cap(c, want);
    }
    lh_memory_copy(lh_ui_web_host_client_get_msg(c) + lh_ui_web_host_client_get_msg_len(c), lh_ui_web_host_client_get_msg_cap(c) - lh_ui_web_host_client_get_msg_len(c), lh_cast_const(lh_ptr, p), n);
    lh_ui_web_host_client_set_msg_len(c, lh_ui_web_host_client_get_msg_len(c) + (lh_cast_static(lh_uint_t, n)));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_ws_ingest(lh_ui_web_host_client_t *c, lh_ui_view_t *view,
                         const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width, lh_uint_t *height,
                         lh_uint_t *gen, lh_uint_t now)
{
    while (lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_get_rx_len(c) >= 2u)
    {
        lh_uint_t opcode = 0;
        lh_bool_t fin = lh_bool_false;
        lh_byte_cptr payload = lh_null;
        lh_usize_t plen = 0;
        lh_byte_cptr mask = lh_null;
        const lh_usize_t n =
            lh_ui_web_ws_read(lh_ui_web_host_client_get_rx(c), lh_ui_web_host_client_get_rx_len(c), lh_addr_of(opcode), lh_addr_of(fin),
                              lh_addr_of(payload), lh_addr_of(plen), lh_addr_of(mask));
        lh_byte_ptr body;

        if (n == 0u)
        {
            return;
        }
        if (n == lh_cast_static(lh_usize_t, -1))
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        body = lh_cast_const(lh_byte_ptr, payload);
        if (mask != lh_null)
        {
            lh_ui_web_ws_unmask(body, plen, mask);
        }
        lh_ui_web_host_client_set_last_rx_ms(c, now);
        if (opcode >= 8u)
        {
            if (!fin || plen > 125u)
            {
                lh_ui_web_host_ws_drop(c);
                return;
            }
            if (opcode == LH_UI_WEB_WS_PING)
            {
                if (!lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_PONG, body, plen, lh_bool_true))
                {
                    return;
                }
            }
            else if (opcode == LH_UI_WEB_WS_CLOSE)
            {
                lh_ui_web_host_client_set_closing(c, lh_bool_true);
                if (!lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_CLOSE, body, plen, lh_bool_true))
                {
                    return;
                }
            }
        }
        else if (opcode == LH_UI_WEB_WS_CONT)
        {
            if (!lh_ui_web_host_client_is_msg_on(c) || !lh_ui_web_host_ws_msg_add(c, body, plen))
            {
                lh_ui_web_host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (lh_ui_web_host_client_get_msg_op(c) == LH_UI_WEB_WS_TEXT)
                {
                    lh_ui_web_host_event_line(view, hooks, width, height, gen,
                                              lh_ptr_rcast(const lh_char_t, lh_ui_web_host_client_get_msg(c)), lh_ui_web_host_client_get_msg_len(c));
                }
                lh_ui_web_host_client_set_msg_on(c, lh_bool_false);
                lh_ui_web_host_client_set_msg_len(c, 0u);
            }
        }
        else if (opcode == LH_UI_WEB_WS_TEXT || opcode == LH_UI_WEB_WS_BIN)
        {
            if (lh_ui_web_host_client_is_msg_on(c))
            {
                lh_ui_web_host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (opcode == LH_UI_WEB_WS_TEXT)
                {
                    lh_ui_web_host_event_line(view, hooks, width, height, gen,
                                              lh_ptr_rcast(const lh_char_t, body), plen);
                }
            }
            else
            {
                lh_ui_web_host_client_set_msg_on(c, lh_bool_true);
                lh_ui_web_host_client_set_msg_op(c, opcode);
                lh_ui_web_host_client_set_msg_len(c, 0u);
                if (!lh_ui_web_host_ws_msg_add(c, body, plen))
                {
                    lh_ui_web_host_ws_drop(c);
                    return;
                }
            }
        }
        else
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        lh_memory_move(lh_ui_web_host_client_get_rx(c), LH_UI_WEB_HOST_WS_RX, lh_ui_web_host_client_get_rx(c) + n,
                       lh_cast_static(lh_usize_t, (lh_ui_web_host_client_get_rx_len(c) - lh_cast_static(lh_uint_t, n))));
        lh_ui_web_host_client_set_rx_len(c, lh_ui_web_host_client_get_rx_len(c) - (lh_cast_static(lh_uint_t, n)));
    }
}

lh_void
lh_ui_web_host_ws_attach(lh_ui_web_host_client_t *slots, lh_os_net_socket_t *client, lh_uint_t now)
{
    lh_uint_t i;

    for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        lh_ui_web_host_client_t *c = slots + i;

        if (lh_ui_web_host_client_is_used(c))
        {
            continue;
        }
        lh_ui_web_host_client_set_rx(c, lh_ptr_rcast(lh_byte_t, lh_os_alloc(LH_UI_WEB_HOST_WS_RX)));
        if (lh_ui_web_host_client_get_rx(c) == lh_null)
        {
            lh_os_net_socket_close(client);
            return;
        }
        lh_ui_web_host_client_set_sock(c, lh_ptr_deref(client));
        lh_ui_web_host_client_set_rx_len(c, 0u);
        lh_ui_web_host_client_set_msg(c, lh_null);
        lh_ui_web_host_client_set_msg_len(c, 0u);
        lh_ui_web_host_client_set_msg_cap(c, 0u);
        lh_ui_web_host_client_set_msg_on(c, lh_bool_false);
        lh_ui_web_host_client_set_qhead(c, lh_null);
        lh_ui_web_host_client_set_qtail(c, lh_null);
        lh_ui_web_host_client_set_queued(c, 0u);
        lh_ui_web_host_client_set_seen(c, 0u);
        lh_ui_web_host_client_set_last_rx_ms(c, now);
        lh_ui_web_host_client_set_last_ping_ms(c, now);
        lh_ui_web_host_client_set_used(c, lh_bool_true);
        lh_ui_web_host_client_set_closing(c, lh_bool_false);
        lh_ui_web_host_client_set_synced(c, lh_bool_false);
        return;
    }
    lh_os_net_socket_close(client);
}

lh_void
lh_ui_web_host_ws_pump(lh_ui_web_host_client_t *c, lh_ui_view_t *view,
                       const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width, lh_uint_t *height,
                       lh_uint_t *gen, lh_uint_t now)
{
    lh_ssize_t got;

    if (!lh_ui_web_host_client_is_used(c))
    {
        return;
    }
    if ((now - lh_ui_web_host_client_get_last_rx_ms(c)) > LH_UI_WEB_HOST_WS_DEAD_MS)
    {
        lh_ui_web_host_ws_drop(c);
        return;
    }
    if (!lh_ui_web_host_client_is_closing(c) && (now - lh_ui_web_host_client_get_last_ping_ms(c)) >= LH_UI_WEB_HOST_WS_PING_MS)
    {
        if (!lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_PING, lh_null, 0u, lh_bool_true))
        {
            return;
        }
        lh_ui_web_host_client_set_last_ping_ms(c, now);
    }
    if (lh_ui_web_host_client_get_rx_len(c) >= LH_UI_WEB_HOST_WS_RX)
    {
        lh_ui_web_host_ws_drop(c);
        return;
    }
    got = lh_os_net_socket_recv(lh_ui_web_host_client_get_sock_mut(c), lh_ui_web_host_client_get_rx(c) + lh_ui_web_host_client_get_rx_len(c),
                                LH_UI_WEB_HOST_WS_RX - lh_ui_web_host_client_get_rx_len(c));
    if (got == LH_OS_RESULT_PENDING)
    {
        lh_ui_web_host_ws_flush(c);
        return;
    }
    if (got <= 0)
    {
        lh_ui_web_host_ws_drop(c);
        return;
    }
    lh_ui_web_host_client_set_rx_len(c, lh_ui_web_host_client_get_rx_len(c) + (lh_cast_static(lh_uint_t, got)));
    lh_ui_web_host_ws_ingest(c, view, hooks, width, height, gen, now);
    if (lh_ui_web_host_client_is_used(c))
    {
        lh_ui_web_host_ws_flush(c);
    }
}

lh_bool_t
lh_ui_web_host_draw(lh_ui_view_t *view, lh_uint_t width, lh_uint_t height, lh_str_ptr json,
                    lh_usize_t json_cap, lh_ui_canvas_web_t *web, lh_ui_canvas_web_glyphs_t *glyphs)
{
    lh_ui_canvas_t canvas;

    if (!lh_ui_canvas_web_open_mem(web, json, json_cap, width, height))
    {
        return lh_bool_false;
    }
    lh_ui_canvas_web_set_glyphs_v(web, glyphs);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), web);
    lh_ui_view_set_canvas(view, lh_addr_of(canvas));
    lh_ui_view_draw(view, lh_null);
    lh_ui_view_set_canvas(view, lh_null);
    return lh_ui_canvas_web_close(web);
}

lh_bool_t
lh_ui_web_host_handle_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
                            lh_uint_t *width, lh_uint_t *height, lh_str_cptr req, lh_usize_t len)
{
    const lh_sint_t x = lh_ui_web_host_query_int(req, len, lh_str_lit("x="));
    const lh_sint_t y = lh_ui_web_host_query_int(req, len, lh_str_lit("y="));

    if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/click")))
    {
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_down, x, y);
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_up, x, y);
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/press")))
    {
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_down, x, y);
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/release")))
    {
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_up, x, y);
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/move")))
    {
        (void)lh_ui_web_host_pointer(view, hooks, lh_os_input_kind_pointer_move, x, y);
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/wheel")))
    {
        (void)lh_ui_web_host_wheel(view, hooks, x, y,
                                   lh_ui_web_host_query_int(req, len, lh_str_lit("dx=")),
                                   lh_ui_web_host_query_int(req, len, lh_str_lit("dy=")));
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/key")))
    {
        /* `p=` is the direction; without it the request is a key going down, as before. */
        (void)lh_ui_web_host_key(
            view, hooks, lh_ui_web_host_query_int(req, len, lh_str_lit("k=")),
            lh_ui_web_host_query_flag(req, len, lh_str_lit("p="), lh_bool_true));
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/text")))
    {
        (void)lh_ui_web_host_text(
            view, hooks,
            lh_cast_static(lh_u32_t, lh_ui_web_host_query_uint(req, len, lh_str_lit("c="))));
    }
    else if (lh_ui_web_host_route(req, len, lh_str_lit("/ui/size")))
    {
        /* Always handled: an unchanged size is still a valid no-op, not a 404. */
        (void)lh_ui_web_host_resize(hooks, width, height,
                                    lh_ui_web_host_query_int(req, len, lh_str_lit("w=")),
                                    lh_ui_web_host_query_int(req, len, lh_str_lit("h=")));
    }
    else
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_ui_web_host_handle(lh_ui_view_t *view, lh_uint_t *width, lh_uint_t *height, lh_str_ptr json,
                      lh_usize_t json_cap, const lh_ui_web_host_hooks_t *hooks, lh_uint_t *gen,
                      lh_os_net_socket_t *client)
{
    lh_char_t req[1024];
    lh_os_net_socket_t *const watch[1] = {client};
    lh_ui_canvas_web_t web;
    lh_ssize_t got;
    lh_usize_t len;

    /* Accepted the moment it connected, so the request may not be here yet; and
       a socket accepted from a non-blocking listener is non-blocking on Windows,
       so a bare recv would answer "not yet" and drop the request. */
    (void)lh_os_net_socket_set_blocking(client, lh_bool_true);
    (void)lh_os_net_socket_set_no_delay(client, lh_bool_true);
    if (lh_os_net_socket_wait(watch, 1u, lh_null, 0u, LH_UI_WEB_HOST_REQUEST_MS) <= 0)
    {
        return lh_bool_false;
    }
    got = lh_os_net_socket_recv(client, req, sizeof(req));
    if (got <= 0)
    {
        return lh_bool_false;
    }
    len = lh_cast_static(lh_usize_t, got);
    if (lh_ui_web_host_route(req, len, lh_str_lit("/ws")))
    {
        return lh_ui_web_host_ws_upgrade(client, req, len);
    }
    if (lh_ui_web_host_handle_input(view, hooks, width, height, req, len))
    {
        lh_ui_web_host_bump(gen);
        lh_ui_web_host_reply_text(client, 200u, lh_str_lit("ok\n"));
        return lh_bool_false;
    }
    if (lh_ui_web_host_route(req, len, lh_str_lit("/ui.rev")))
    {
        lh_char_t body[32];
        const lh_usize_t n = lh_str_ptr_format_text(
            body, sizeof(body), "%u\n",
            lh_cast_static(lh_uint_t, (gen != lh_null ? lh_ptr_deref(gen) : 0u)));

        lh_ui_web_host_reply_text(client, 200u, body, n);
        return lh_bool_false;
    }
    if (lh_ui_web_host_route(req, len, lh_str_lit("/ui.json")))
    {
        if (gen != lh_null &&
            lh_ui_web_host_query_uint(req, len, lh_str_lit("rev=")) == lh_ptr_deref(gen))
        {
            lh_ui_web_host_reply_text(client, 304u, lh_str_lit(""));
            return lh_bool_false;
        }
        if (!lh_ui_web_host_draw(view, lh_ptr_deref(width), lh_ptr_deref(height), json, json_cap,
                                 lh_addr_of(web), lh_null))
        {
            lh_ui_web_host_reply_text(client, 500u, lh_str_lit("draw failed\n"));
            return lh_bool_false;
        }
        lh_ui_web_host_reply(client, 200u, "application/json; charset=utf-8",
                             lh_ui_canvas_web_bytes(lh_addr_of(web)),
                             lh_ui_canvas_web_length(lh_addr_of(web)));
        return lh_bool_false;
    }
    /* `/` and `/?scale=…` — not a bare `/` prefix, or `/ui.json` would match too. */
    if (lh_ui_web_host_route(req, len, lh_str_lit("/ ")) ||
        lh_ui_web_host_route(req, len, lh_str_lit("/?")) ||
        lh_ui_web_host_route(req, len, lh_str_lit("/index.html")))
    {
        lh_ui_web_host_reply(client, 200u, "text/html; charset=utf-8",
                             lh_ptr_rcast(const lh_char_t, m_ui_web_host_page),
                             m_ui_web_host_page_len);
        return lh_bool_false;
    }
    lh_ui_web_host_reply_text(client, 404u, lh_str_lit("not found\n"));
    return lh_bool_false;
}

lh_bool_t
lh_ui_web_host_ws_wants(const lh_ui_web_host_client_t *c, lh_uint_t gen)
{
    return (lh_ui_web_host_client_is_used(c) && !lh_ui_web_host_client_is_closing(c) && lh_ui_web_host_client_get_seen(c) != gen && lh_ui_web_host_client_get_qhead(c) == lh_null) ? lh_bool_true
                                                                             : lh_bool_false;
}

lh_void
lh_ui_web_host_ws_remember(lh_ui_web_host_stream_t *stream, lh_str_cptr body, lh_usize_t len)
{
    if (len > lh_ui_web_host_stream_get_last_cap(stream))
    {
        lh_str_ptr grow = lh_ptr_rcast(
            lh_char_t, lh_runtime_allocator_realloc(lh_ui_web_host_stream_get_last(stream), lh_ui_web_host_stream_get_last_cap(stream), len));

        if (grow == lh_null)
        {
            lh_ui_web_host_stream_set_last_len(stream, 0u);
            return;
        }
        lh_ui_web_host_stream_set_last(stream, grow);
        lh_ui_web_host_stream_set_last_cap(stream, len);
    }
    lh_memory_copy(lh_ui_web_host_stream_get_last(stream), lh_ui_web_host_stream_get_last_cap(stream), lh_cast_const(lh_ptr, body), len);
    lh_ui_web_host_stream_set_last_len(stream, len);
}

lh_bool_t
lh_ui_web_host_ws_same(const lh_ui_web_host_stream_t *stream, lh_str_cptr body, lh_usize_t len)
{
    return (len > 0u && len == lh_ui_web_host_stream_get_last_len(stream) &&
            lh_memory_compare(lh_cast_const(lh_ptr, body), len, lh_ui_web_host_stream_get_last(stream), len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_web_host_ws_frame(lh_ui_web_host_client_t *cli, lh_ui_web_host_stream_t *stream,
                        lh_ui_view_t *view, lh_uint_t width, lh_uint_t height, lh_str_ptr json,
                        lh_usize_t json_cap, lh_uint_t gen)
{
    lh_ui_canvas_web_t web;
    lh_bool_t want = lh_bool_false;
    lh_bool_t same;
    lh_str_cptr body;
    lh_usize_t len;
    lh_uint_t i;

    for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        if (lh_ui_web_host_ws_wants(cli + i, gen))
        {
            want = lh_bool_true;
            if (!cli[i].synced)
            {
                lh_ui_canvas_web_glyphs_reset(lh_ui_web_host_stream_get_glyphs_mut(stream));
            }
        }
    }
    if (!want || !lh_ui_web_host_draw(view, width, height, json, json_cap, lh_addr_of(web),
                                      lh_ui_web_host_stream_get_glyphs_mut(stream)))
    {
        return;
    }
    body = lh_ui_canvas_web_bytes(lh_addr_of(web));
    len = lh_ui_canvas_web_length(lh_addr_of(web));
    same = lh_ui_web_host_ws_same(stream, body, len);
    for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        lh_ui_web_host_client_t *c = cli + i;

        if (!lh_ui_web_host_client_is_used(c))
        {
            continue;
        }
        if (!lh_ui_web_host_ws_wants(c, gen))
        {
            /* Not sent this one: any cover it introduced, this client lacks. */
            if (!same)
            {
                lh_ui_web_host_client_set_synced(c, lh_bool_false);
            }
            continue;
        }
        if (same && lh_ui_web_host_client_is_synced(c))
        {
            lh_ui_web_host_client_set_seen(c, gen);
            continue;
        }
        if (lh_ui_web_host_ws_enqueue(c, LH_UI_WEB_WS_TEXT, body, len, lh_bool_false))
        {
            lh_ui_web_host_client_set_seen(c, gen);
            lh_ui_web_host_client_set_synced(c, lh_bool_true);
            lh_ui_web_host_ws_flush(c);
        }
    }
    if (!same)
    {
        lh_ui_web_host_ws_remember(stream, body, len);
    }
}

lh_void
lh_ui_web_host_accept_all(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                          lh_ui_view_t *view, lh_uint_t *width, lh_uint_t *height, lh_str_ptr json,
                          lh_usize_t json_cap, const lh_ui_web_host_hooks_t *hooks, lh_uint_t *gen,
                          lh_uint_t now)
{
    for (;;)
    {
        lh_os_net_socket_t client;
        lh_net_ip4_socket_addr_t peer;

        lh_os_net_socket_init(lh_addr_of(client));
        if (!lh_os_net_socket_accept(listen_fd, lh_addr_of(client), lh_addr_of(peer)))
        {
            return;
        }
        if (lh_ui_web_host_handle(view, width, height, json, json_cap, hooks, gen,
                                  lh_addr_of(client)))
        {
            lh_ui_web_host_ws_attach(cli, lh_addr_of(client), now);
        }
        else
        {
            lh_os_net_socket_close(lh_addr_of(client));
        }
    }
}

lh_void
lh_ui_web_host_wait(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                    lh_uint_t timeout_ms)
{
    lh_os_net_socket_t *read[LH_UI_WEB_HOST_WS_MAX + 1u];
    lh_os_net_socket_t *write[LH_UI_WEB_HOST_WS_MAX];
    lh_usize_t nr = 0u;
    lh_usize_t nw = 0u;
    lh_uint_t i;

    read[nr++] = listen_fd;
    for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        if (!cli[i].used)
        {
            continue;
        }
        read[nr++] = lh_addr_of(cli[i].sock);
        if (cli[i].qhead != lh_null)
        {
            write[nw++] = lh_addr_of(cli[i].sock);
        }
    }
    if (lh_os_net_socket_wait(read, nr, write, nw, lh_cast_static(lh_u32_t, timeout_ms)) ==
        LH_OS_RESULT_INVALID)
    {
        (void)lh_os_net_socket_wait(read, 1u, lh_null, 0u, lh_cast_static(lh_u32_t, timeout_ms));
    }
}

lh_void
lh_ui_web_host_route_hooks(const lh_ui_web_host_hooks_t *hooks, lh_ui_view_t *view,
                           lh_os_controller_t *view_controller, lh_ui_web_host_hooks_t *out)
{
    lh_memory_set(out, sizeof(lh_ptr_deref(out)), 0);
    if (hooks != lh_null)
    {
        lh_ptr_deref(out) = lh_ptr_deref(hooks);
    }
    lh_return_if(lh_ui_web_host_hooks_get_input_fn(out) != lh_null || lh_ui_web_host_hooks_get_controller(out) != lh_null);
    lh_os_controller_init(view_controller, lh_ui_view_controller_handle, view, lh_null);
    lh_os_controller_set_cancel_cb(view_controller, lh_ui_view_controller_cancel);
    lh_ui_web_host_hooks_set_controller(out, view_controller);
}

lh_bool_t
lh_ui_web_host_run(lh_ui_view_t *view, lh_uint_t width, lh_uint_t height, lh_str_ptr json,
                   lh_usize_t json_cap, lh_net_port_t port, const lh_ui_web_host_hooks_t *hooks)
{
    lh_os_controller_t view_controller;
    lh_ui_web_host_hooks_t routed;
    lh_os_net_socket_t listen_fd;
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;

    if (view == lh_null || json == lh_null || json_cap < 64u)
    {
        return lh_bool_false;
    }
    lh_ui_web_host_route_hooks(hooks, view, lh_addr_of(view_controller), lh_addr_of(routed));
    if (!lh_os_net_init())
    {
        return lh_bool_false;
    }
    lh_os_net_socket_init(lh_addr_of(listen_fd));
    if (!lh_os_net_socket_open(lh_addr_of(listen_fd), lh_os_system_net_socket_type_tcp))
    {
        return lh_bool_false;
    }
    (void)lh_os_net_socket_set_reuse_addr(lh_addr_of(listen_fd), lh_bool_true);
    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), port);
    if (!lh_os_net_socket_bind(lh_addr_of(listen_fd), lh_addr_of(addr)) ||
        !lh_os_net_socket_listen(lh_addr_of(listen_fd), 8))
    {
        lh_os_net_socket_close(lh_addr_of(listen_fd));
        return lh_bool_false;
    }
    if (!lh_ui_web_page_build(m_ui_web_host_page,
                              lh_cast_static(lh_usize_t, sizeof(m_ui_web_host_page)),
                              lh_addr_of(m_ui_web_host_page_len)))
    {
        lh_logger_error(lh_runtime_logger(), "[lh] ui web player page would not build");
        lh_os_net_socket_close(lh_addr_of(listen_fd));
        return lh_bool_false;
    }
    (void)lh_os_net_socket_set_blocking(lh_addr_of(listen_fd), lh_bool_false);
    lh_logger_notice(lh_runtime_logger(),
                     "[lh] ui web host http://127.0.0.1:%u/ (layout default, ?scale=fit)",
                     lh_cast_static(lh_uint_t, port));
    {
        lh_uint_t gen = 1u;
        lh_uint_t i;
        lh_uint_t frame_w = width;
        lh_uint_t frame_h = height;
        lh_ui_web_host_client_t cli[LH_UI_WEB_HOST_WS_MAX];
        lh_ui_web_host_stream_t stream;

        lh_memory_set(cli, sizeof(cli), 0);
        lh_memory_set(lh_addr_of(stream), sizeof(stream), 0);
        for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
        {
            lh_os_net_socket_init(lh_addr_of(cli[i].sock));
        }
        for (;;)
        {
            lh_ui_rect_t damage;
            const lh_uint_t now = lh_cast_static(lh_uint_t, lh_os_tick_ms());
            lh_uint_t spent;

            /* The controller's timers before the app's tick, as a window does: a long
               press that came due is in the view by the time the app looks. */
            if (lh_ui_web_host_hooks_get_controller(lh_addr_of(routed)) != lh_null)
            {
                (void)lh_os_controller_update(lh_ui_web_host_hooks_get_controller(lh_addr_of(routed)), lh_os_tick_ms());
            }
            if (routed.tick != lh_null)
            {
                routed.tick(lh_ui_web_host_hooks_get_ctx(lh_addr_of(routed)));
            }
            lh_ui_web_host_accept_all(lh_addr_of(listen_fd), cli, view, lh_addr_of(frame_w),
                                      lh_addr_of(frame_h), json, json_cap, lh_addr_of(routed),
                                      lh_addr_of(gen), now);
            for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
            {
                lh_ui_web_host_ws_pump(lh_addr_of(cli[i]), view, lh_addr_of(routed),
                                       lh_addr_of(frame_w), lh_addr_of(frame_h), lh_addr_of(gen),
                                       now);
            }
            /* After the input, not before it: an event and the damage it caused
               are one change and one frame, not two. */
            if (lh_ui_view_take_damage(view, lh_addr_of(damage)))
            {
                ++gen;
            }
            lh_ui_web_host_ws_frame(cli, lh_addr_of(stream), view, frame_w, frame_h, json, json_cap,
                                    gen);
            spent = lh_cast_static(lh_uint_t, lh_os_tick_ms()) - now;
            lh_ui_web_host_wait(lh_addr_of(listen_fd), cli,
                                (spent >= LH_UI_WEB_HOST_TICK_MS) ? 0u
                                                                  : LH_UI_WEB_HOST_TICK_MS - spent);
        }
    }
}
