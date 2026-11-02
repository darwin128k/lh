/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>
#include <lh/attribute/unused.h>
#include <lh/http/format.h>
#include <lh/http/framing.h>
#include <lh/http/status.h>
#include <lh/str/format/ullong.h>
#include <lh/str/split/once.h>
#include <lh/char/map.h>

#include <lh/cast/const.h>
#include <lh/crypto/sha.h>
#include <lh/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/net.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/logger.h>
#include <lh/str/parse/text.h>
#include <lh/util/memory.h>
#include <lh/str/format/base64.h>
#include <lh/util/return.h>

lh_void
lh_ui_web_host_reply(lh_os_net_socket_t *client, const lh_http_request_t *request, lh_uint_t code,
                     lh_str_cptr type, lh_str_cptr body, lh_usize_t len)
{
    lh_char_t head[256];
    lh_char_t length_text[LH_STR_FORMAT_ULLONG_DIGITS_MAX];
    const lh_usize_t length_size = lh_str_ptr_format_ullong(lh_cast_static(lh_ullong_t, len),
                                                            length_text, sizeof(length_text));
    if (!lh_http_status_is_valid(code))
    {
        return;
    }
    const lh_str_view_t reason = lh_http_status_get_reason(code);
    const lh_bool_t content_forbidden =
        lh_http_status_is_content_forbidden(code) ||
        (lh_http_request_is_method(request, lh_http_method_connect) &&
         lh_http_status_is_success(code));
    const lh_bool_t is_head = lh_http_request_is_method(request, lh_http_method_head);
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), lh_cast_static(lh_u16_t, code));
    lh_http_response_set_version_minor(lh_addr_of(response), 1U);
    lh_http_response_set_reason(lh_addr_of(response), reason);
    lh_http_header_t headers[4];
    lh_usize_t count = 0U;
    lh_http_header_set(
        lh_addr_of(headers[count]), lh_str_view_lit("Content-Type"),
        lh_initializer_of(lh_str_view_t, lh_str_view_initializer, type, lh_str_ptr_len(type)));
    count = lh_math_add_one(count);
    if (!content_forbidden)
    {
        lh_http_header_set(
            lh_addr_of(headers[count]), lh_str_view_lit("Content-Length"),
            lh_initializer_of(lh_str_view_t, lh_str_view_initializer, length_text, length_size));
        count = lh_math_add_one(count);
    }
    lh_http_header_set(lh_addr_of(headers[count]), lh_str_view_lit("Connection"),
                       lh_str_view_lit("close"));
    count = lh_math_add_one(count);
    lh_http_header_set(lh_addr_of(headers[count]), lh_str_view_lit("Cache-Control"),
                       lh_str_view_lit("no-store"));
    count = lh_math_add_one(count);
    lh_http_response_set_header_count(lh_addr_of(response), count);
    lh_usize_t n = 0U;
    if (lh_math_is_zero(length_size) ||
        !lh_http_format_try_response_head(lh_addr_of(response), headers, head, sizeof(head),
                                          lh_addr_of(n)))
    {
        return;
    }
    if (!lh_os_net_socket_send_all(client, head, n))
    {
        return;
    }
    if (!content_forbidden && !is_head)
    {
        LH_ATTRIBUTE_UNUSED(lh_os_net_socket_send_all(client, body, len));
    }
}

lh_void
lh_ui_web_host_reply_text(lh_os_net_socket_t *client, const lh_http_request_t *request,
                          lh_uint_t code, lh_str_cptr text, lh_usize_t len)
{
    lh_ui_web_host_reply(client, request, code, "text/plain; charset=utf-8", text, len);
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
lh_ui_web_host_is_route(const lh_http_request_t *request, lh_str_cptr path, lh_usize_t path_len)
{
    if (!lh_http_request_is_method(request, lh_http_method_get) &&
        !lh_http_request_is_method(request, lh_http_method_post))
    {
        return lh_bool_false;
    }
    lh_ui_web_route_t route;
    lh_ui_web_route_set_path(lh_addr_of(route), path);
    lh_ui_web_route_set_path_len(lh_addr_of(route), path_len);
    lh_ui_web_route_set_method(lh_addr_of(route), lh_null);
    lh_ui_web_route_set_method_len(lh_addr_of(route), 0U);
    return lh_ui_web_route_is_match(lh_addr_of(route), request);
}

lh_usize_t
lh_ui_web_host_get_query_value_size(const lh_http_request_t *request, lh_str_cptr key,
                                    lh_usize_t key_len, lh_str_cptr *value)
{
    if (lh_math_is_zero(key_len))
    {
        return 0u;
    }
    lh_str_view_t path;
    lh_str_view_t query;
    if (!lh_str_view_split_once(lh_http_request_get_target(request), lh_char_map_question,
                                lh_addr_of(path), lh_addr_of(query)) ||
        lh_str_view_is_empty(lh_addr_of(query)))
    {
        return 0u;
    }
    const lh_str_ptr req = lh_str_view_get_data(lh_addr_of(query));
    const lh_usize_t len = lh_str_view_get_size(lh_addr_of(query));
    lh_usize_t off = 0u;

    for (;;)
    {
        lh_str_cptr at = lh_ui_web_host_find(req + off, len - off, key, key_len);

        if (at == lh_null)
        {
            return 0u;
        }
        off = lh_math_add(lh_ptr_udiff(at, req), key_len);
        if (at != req && at[-1] != lh_char_map_ampersand)
        {
            continue;
        }
        lh_ptr_deref(value) = req + off;
        const lh_str_ptr end = lh_str_ptr_find_of_char(req + off, len - off, lh_char_map_ampersand);
        return lh_null_ne(end) ? lh_ptr_udiff(end, req + off) : len - off;
    }
}

lh_sint_t
lh_ui_web_host_get_query_int(const lh_http_request_t *request, lh_str_cptr key, lh_usize_t key_len)
{
    lh_str_cptr value = lh_null;
    const lh_usize_t n =
        lh_ui_web_host_get_query_value_size(request, key, key_len, lh_addr_of(value));
    lh_sint_t v = 0;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%d", lh_addr_of(v)) == 0u)
    {
        return 0;
    }
    return lh_cast_static(lh_sint_t, v);
}

lh_uint_t
lh_ui_web_host_get_query_uint(const lh_http_request_t *request, lh_str_cptr key, lh_usize_t key_len)
{
    lh_str_cptr value = lh_null;
    const lh_usize_t n =
        lh_ui_web_host_get_query_value_size(request, key, key_len, lh_addr_of(value));
    lh_uint_t v = 0u;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%u", lh_addr_of(v)) == 0u)
    {
        return 0u;
    }
    return lh_cast_static(lh_uint_t, v);
}

lh_bool_t
lh_ui_web_host_get_query_flag(const lh_http_request_t *request, lh_str_cptr key, lh_usize_t key_len,
                              lh_bool_t absent)
{
    lh_str_cptr value = lh_null;

    if (lh_ui_web_host_get_query_value_size(request, key, key_len, lh_addr_of(value)) == 0u)
    {
        return absent;
    }
    return (lh_ui_web_host_get_query_int(request, key, key_len) != 0) ? lh_bool_true
                                                                      : lh_bool_false;
}

lh_bool_t
lh_ui_web_host_ws_upgrade(lh_os_net_socket_t *client, const lh_http_header_t *headers,
                          lh_usize_t count)
{
    static const lh_char_t guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const lh_str_view_t key_name = lh_str_view_lit("Sec-WebSocket-Key");

    const lh_http_header_t *key = lh_http_header_find(headers, count, lh_addr_of(key_name));
    if (key == lh_null)
    {
        return lh_bool_false;
    }
    const lh_str_view_t *value = lh_http_header_get_value(key);
    lh_usize_t n = lh_str_view_get_size(value);
    lh_char_t cat[80];

    if (n > sizeof(cat) - (sizeof(guid) - 1u))
    {
        return lh_bool_false;
    }
    lh_memory_copy(cat, sizeof(cat), lh_cast_const(lh_ptr, lh_str_view_get_data(value)), n);
    lh_memory_copy(cat + n, sizeof(cat) - n, lh_cast_const(lh_ptr, guid), sizeof(guid) - 1u);
    n += sizeof(guid) - 1u;
    lh_byte_t digest[LH_CRYPTO_SHA1_DIGEST_SIZE];
    if (lh_crypto_sha_hash(lh_crypto_sha_kind_1, cat, n, digest, sizeof(digest)) !=
        LH_CRYPTO_SHA1_DIGEST_SIZE)
    {
        return lh_bool_false;
    }
    lh_char_t accept[32];
    accept[lh_str_ptr_format_base64(digest, sizeof(digest), accept, sizeof(accept) - 1u)] =
        lh_char_map_nul;
    lh_char_t head[280];
    lh_http_response_t response;
    lh_http_response_set_status(lh_addr_of(response), lh_http_status_switching_protocols);
    lh_http_response_set_version_minor(lh_addr_of(response), 1U);
    lh_http_response_set_reason(lh_addr_of(response),
                                lh_http_status_get_reason(lh_http_status_switching_protocols));
    lh_http_response_set_header_count(lh_addr_of(response), 3U);
    lh_http_header_t response_headers[3];
    lh_http_header_set(lh_addr_of(response_headers[0]), lh_str_view_lit("Upgrade"),
                       lh_str_view_lit("websocket"));
    lh_http_header_set(lh_addr_of(response_headers[1]), lh_str_view_lit("Connection"),
                       lh_str_view_lit("Upgrade"));
    lh_http_header_set(
        lh_addr_of(response_headers[2]), lh_str_view_lit("Sec-WebSocket-Accept"),
        lh_initializer_of(lh_str_view_t, lh_str_view_initializer, accept, lh_str_ptr_len(accept)));
    lh_usize_t h = 0U;
    if (!lh_http_format_try_response_head(lh_addr_of(response), response_headers, head,
                                          sizeof(head), lh_addr_of(h)))
    {
        return lh_bool_false;
    }
    if (!lh_os_net_socket_send_all(client, head, h))
    {
        return lh_bool_false;
    }
    return lh_os_net_socket_set_blocking(client, lh_bool_false);
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

    if (c == lh_null || !lh_ui_web_host_client_is_used(c))
    {
        return lh_bool_false;
    }
    if (need == 0u || lh_ui_web_host_client_get_queued(c) + lh_cast_static(lh_uint_t, need) >
                          LH_UI_WEB_HOST_WS_QMAX)
    {
        lh_ui_web_host_ws_drop(c);
        return lh_bool_false;
    }
    lh_ui_web_host_frame_t *q =
        lh_ptr_rcast(lh_ui_web_host_frame_t, lh_os_alloc(sizeof(lh_ptr_deref(q))));
    lh_byte_ptr bytes = lh_ptr_rcast(lh_byte_t, lh_os_alloc(need));
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
    lh_ui_web_host_client_set_queued(c, lh_ui_web_host_client_get_queued(c) +
                                            (lh_cast_static(lh_uint_t, need)));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_ws_flush(lh_ui_web_host_client_t *c)
{
    while (c != lh_null && lh_ui_web_host_client_is_used(c) &&
           lh_ui_web_host_client_get_qhead(c) != lh_null)
    {
        lh_ui_web_host_frame_t *q = lh_ui_web_host_client_get_qhead(c);
        const lh_ssize_t w = lh_os_net_socket_send(
            lh_ui_web_host_client_get_sock_mut(c),
            lh_ui_web_host_frame_get_bytes(q) + lh_ui_web_host_frame_get_off(q),
            lh_cast_static(lh_usize_t,
                           (lh_ui_web_host_frame_get_len(q) - lh_ui_web_host_frame_get_off(q))));

        if (w == LH_OS_RESULT_PENDING)
        {
            return;
        }
        if (w <= 0)
        {
            lh_ui_web_host_ws_drop(c);
            return;
        }
        lh_ui_web_host_frame_set_off(q, lh_ui_web_host_frame_get_off(q) +
                                            (lh_cast_static(lh_uint_t, w)));
        lh_ui_web_host_client_set_queued(c, lh_ui_web_host_client_get_queued(c) -
                                                (lh_cast_static(lh_uint_t, w)));
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
    if (c != lh_null && lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_is_closing(c) &&
        lh_ui_web_host_client_get_qhead(c) == lh_null)
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
    if (lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n) >
        lh_ui_web_host_client_get_msg_cap(c))
    {
        const lh_uint_t cap = lh_ui_web_host_client_get_msg_len(c) + lh_cast_static(lh_uint_t, n);
        const lh_uint_t next = (lh_ui_web_host_client_get_msg_cap(c) == 0u)
                                   ? 256u
                                   : (lh_ui_web_host_client_get_msg_cap(c) * 2u);
        const lh_uint_t want = (cap > next) ? cap : next;

        grow = lh_ptr_rcast(
            lh_byte_t, lh_runtime_allocator_realloc(lh_ui_web_host_client_get_msg(c),
                                                    lh_ui_web_host_client_get_msg_cap(c), want));
        if (grow == lh_null)
        {
            return lh_bool_false;
        }
        lh_ui_web_host_client_set_msg(c, grow);
        lh_ui_web_host_client_set_msg_cap(c, want);
    }
    lh_memory_copy(lh_ui_web_host_client_get_msg(c) + lh_ui_web_host_client_get_msg_len(c),
                   lh_ui_web_host_client_get_msg_cap(c) - lh_ui_web_host_client_get_msg_len(c),
                   lh_cast_const(lh_ptr, p), n);
    lh_ui_web_host_client_set_msg_len(c, lh_ui_web_host_client_get_msg_len(c) +
                                             (lh_cast_static(lh_uint_t, n)));
    return lh_bool_true;
}

lh_void
lh_ui_web_host_ws_ingest(lh_ui_web_host_client_t *c, lh_ui_web_app_t *app, lh_uint_t now)
{
    while (lh_ui_web_host_client_is_used(c) && lh_ui_web_host_client_get_rx_len(c) >= 2u)
    {
        lh_uint_t opcode = 0;
        lh_bool_t fin = lh_bool_false;
        lh_byte_cptr payload = lh_null;
        lh_usize_t plen = 0;
        lh_byte_cptr mask = lh_null;
        const lh_usize_t n =
            lh_ui_web_ws_read(lh_ui_web_host_client_get_rx(c), lh_ui_web_host_client_get_rx_len(c),
                              lh_addr_of(opcode), lh_addr_of(fin), lh_addr_of(payload),
                              lh_addr_of(plen), lh_addr_of(mask));
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
                    lh_ui_web_app_message(app,
                                          lh_ptr_rcast(const lh_char_t,
                                                        lh_ui_web_host_client_get_msg(c)),
                                          lh_ui_web_host_client_get_msg_len(c));
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
                    lh_ui_web_app_message(app, lh_ptr_rcast(const lh_char_t, body), plen);
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
        lh_memory_move(lh_ui_web_host_client_get_rx(c), LH_UI_WEB_HOST_WS_RX,
                       lh_ui_web_host_client_get_rx(c) + n,
                       lh_cast_static(lh_usize_t, (lh_ui_web_host_client_get_rx_len(c) -
                                                   lh_cast_static(lh_uint_t, n))));
        lh_ui_web_host_client_set_rx_len(c, lh_ui_web_host_client_get_rx_len(c) -
                                                (lh_cast_static(lh_uint_t, n)));
    }
}

lh_void
lh_ui_web_host_ws_attach(lh_ui_web_host_client_t *slots, lh_os_net_socket_t *client, lh_uint_t now)
{
    for (lh_uint_t i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
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
lh_ui_web_host_ws_pump(lh_ui_web_host_client_t *c, lh_ui_web_app_t *app, lh_uint_t now)
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
    if (!lh_ui_web_host_client_is_closing(c) &&
        (now - lh_ui_web_host_client_get_last_ping_ms(c)) >= LH_UI_WEB_HOST_WS_PING_MS)
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
    got =
        lh_os_net_socket_recv(lh_ui_web_host_client_get_sock_mut(c),
                              lh_ui_web_host_client_get_rx(c) + lh_ui_web_host_client_get_rx_len(c),
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
    lh_ui_web_host_client_set_rx_len(c, lh_ui_web_host_client_get_rx_len(c) +
                                            (lh_cast_static(lh_uint_t, got)));
    lh_ui_web_host_ws_ingest(c, app, now);
    if (lh_ui_web_host_client_is_used(c))
    {
        lh_ui_web_host_ws_flush(c);
    }
}

lh_bool_t
lh_ui_web_host_try_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                                lh_u32_t timeout_ms, lh_usize_t *length, lh_str_view_t *body,
                                lh_http_request_t *request, lh_http_header_t *headers,
                                lh_usize_t header_capacity)
{
    lh_http_parser_t parser;
    lh_os_net_socket_t *const watch[] = {client};
    const lh_u64_t start = lh_os_tick_ms();
    lh_usize_t used = 0U;
    lh_usize_t required = 0U;
    lh_usize_t head_size = 0U;
    lh_bool_t head_complete = lh_bool_false;

    lh_http_parser_init(lh_addr_of(parser));
    while (used < capacity)
    {
        const lh_u64_t elapsed = lh_os_tick_ms() - start;
        if (elapsed >= timeout_ms ||
            lh_os_net_socket_wait(watch, 1U, lh_null, 0U,
                                  lh_cast_static(lh_u32_t, timeout_ms - elapsed)) <= 0)
        {
            return lh_bool_false;
        }
        const lh_ssize_t got = lh_os_net_socket_recv(client, buf + used, capacity - used);
        if (got <= 0)
        {
            return lh_bool_false;
        }
        used += lh_cast_static(lh_usize_t, got);
        if (!head_complete)
        {
            const lh_str_view_t input = lh_str_view_initializer(buf, used);
            const lh_http_parse_status_t status = lh_http_parser_request(
                lh_addr_of(parser), input, request, headers, header_capacity);
            if (status == lh_http_parse_error)
            {
                return lh_bool_false;
            }
            if (status == lh_http_parse_complete)
            {
                head_size = lh_http_parser_get_consumed(lh_addr_of(parser));
                lh_usize_t body_size;
                if (!lh_http_framing_try_get_content_length(
                        headers, lh_http_request_get_header_count(request),
                        lh_addr_of(body_size)) ||
                    body_size > capacity - head_size)
                {
                    return lh_bool_false;
                }
                required = head_size + body_size;
                head_complete = lh_bool_true;
            }
        }
        if (head_complete && used >= required)
        {
            lh_str_view_init_by_size(body, buf + head_size, required - head_size);
            lh_ptr_deref(length) = required;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_ui_web_host_read_request(lh_os_net_socket_t *client, lh_str_ptr buf, lh_usize_t capacity,
                            lh_u32_t timeout_ms, lh_usize_t *length, lh_str_view_t *body,
                            lh_http_request_t *request, lh_http_header_t *headers,
                            lh_usize_t header_capacity)
{
    lh_runtime_check_ifn(lh_ui_web_host_try_read_request(client, buf, capacity, timeout_ms, length,
                                                         body, request, headers, header_capacity),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_ui_web_host_handle_request_is_upgraded(lh_ui_web_app_t *app, lh_os_net_socket_t *client)
{
    lh_char_t req[1024];
    lh_usize_t len;
    lh_str_view_t body;
    lh_http_request_t request;
    lh_http_header_t headers[32];

    if (!lh_os_net_socket_set_blocking(client, lh_bool_true))
    {
        return lh_bool_false;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_net_socket_set_no_delay(client, lh_bool_true));
    if (!lh_ui_web_host_try_read_request(client, req, sizeof(req), LH_UI_WEB_HOST_REQUEST_MS,
                                         lh_addr_of(len), lh_addr_of(body), lh_addr_of(request),
                                         headers, lh_array_ptr_get_size(headers)))
    {
        return lh_bool_false;
    }
    if (lh_ui_web_app_is_websocket(app, lh_addr_of(request)))
    {
        return lh_ui_web_host_ws_upgrade(client, headers,
                                         lh_http_request_get_header_count(lh_addr_of(request)));
    }
    if (lh_ui_web_app_handle_request_is_handled(app, lh_addr_of(request), headers, lh_addr_of(body),
                                                client))
    {
        return lh_bool_false;
    }
    lh_ui_web_host_reply_text(client, lh_addr_of(request), lh_http_status_not_found,
                              lh_str_lit("not found\n"));
    return lh_bool_false;
}

lh_bool_t
lh_ui_web_host_ws_wants(const lh_ui_web_host_client_t *c, lh_uint_t gen)
{
    return (lh_ui_web_host_client_is_used(c) && !lh_ui_web_host_client_is_closing(c) &&
            lh_ui_web_host_client_get_seen(c) != gen &&
            lh_ui_web_host_client_get_qhead(c) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_web_host_ws_remember(lh_ui_web_host_stream_t *stream, lh_str_cptr body, lh_usize_t len)
{
    if (len > lh_ui_web_host_stream_get_last_cap(stream))
    {
        lh_str_ptr grow = lh_ptr_rcast(
            lh_char_t,
            lh_runtime_allocator_realloc(lh_ui_web_host_stream_get_last(stream),
                                         lh_ui_web_host_stream_get_last_cap(stream), len));

        if (grow == lh_null)
        {
            lh_ui_web_host_stream_set_last_len(stream, 0u);
            return;
        }
        lh_ui_web_host_stream_set_last(stream, grow);
        lh_ui_web_host_stream_set_last_cap(stream, len);
    }
    lh_memory_copy(lh_ui_web_host_stream_get_last(stream),
                   lh_ui_web_host_stream_get_last_cap(stream), lh_cast_const(lh_ptr, body), len);
    lh_ui_web_host_stream_set_last_len(stream, len);
}

lh_bool_t
lh_ui_web_host_ws_same(const lh_ui_web_host_stream_t *stream, lh_str_cptr body, lh_usize_t len)
{
    return (len > 0u && len == lh_ui_web_host_stream_get_last_len(stream) &&
            lh_memory_compare(lh_cast_const(lh_ptr, body), len,
                              lh_ui_web_host_stream_get_last(stream), len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_web_host_ws_frame(lh_ui_web_host_client_t *cli, lh_ui_web_host_stream_t *stream,
                        lh_ui_web_app_t *app)
{
    const lh_uint_t gen = lh_ui_web_app_get_generation(app);
    const lh_str_ptr body = lh_null;
    lh_usize_t len = 0u;
    lh_bool_t want = lh_bool_false;

    for (lh_uint_t i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
    {
        if (lh_ui_web_host_ws_wants(cli + i, gen))
        {
            want = lh_bool_true;
            if (!lh_ui_web_host_client_is_synced(cli + i))
            {
                lh_ui_canvas_web_glyphs_reset(lh_ui_web_host_stream_get_glyphs_mut(stream));
            }
        }
    }
    if (!want)
    {
        return;
    }
    lh_ui_web_app_frame(app, lh_ui_web_host_stream_get_glyphs_mut(stream), lh_addr_of(body),
                        lh_addr_of(len));
    if (len == 0u)
    {
        return;
    }
    {
        const lh_bool_t same = lh_ui_web_host_ws_same(stream, body, len);

        for (lh_uint_t i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
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
}

lh_void
lh_ui_web_host_accept_all(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                          lh_ui_web_app_t *app, lh_uint_t now)
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
        if (lh_ui_web_host_handle_request_is_upgraded(app, lh_addr_of(client)))
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
    lh_usize_t nr = 0u;
    lh_usize_t nw = 0u;
    lh_uint_t i;
    lh_os_net_socket_t *read[LH_UI_WEB_HOST_WS_MAX + 1u];

    read[nr++] = listen_fd;
    lh_os_net_socket_t *write[LH_UI_WEB_HOST_WS_MAX];
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
        LH_ATTRIBUTE_UNUSED(
            lh_os_net_socket_wait(read, 1u, lh_null, 0u, lh_cast_static(lh_u32_t, timeout_ms)));
    }
}

lh_bool_t
lh_ui_web_host_run(lh_ui_web_app_t *app, lh_net_port_t port)
{
    if (app == lh_null)
    {
        return lh_bool_false;
    }
    if (!lh_os_net_init())
    {
        return lh_bool_false;
    }
    lh_os_net_socket_t listen_fd;
    lh_os_net_socket_init(lh_addr_of(listen_fd));
    if (!lh_os_net_socket_open(lh_addr_of(listen_fd), lh_os_system_net_socket_type_tcp))
    {
        return lh_bool_false;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_net_socket_set_reuse_addr(lh_addr_of(listen_fd), lh_bool_true));
    lh_net_ip4_t ip;
    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), port);
    if (!lh_os_net_socket_bind(lh_addr_of(listen_fd), lh_addr_of(addr)) ||
        !lh_os_net_socket_listen(lh_addr_of(listen_fd), 8))
    {
        lh_os_net_socket_close(lh_addr_of(listen_fd));
        return lh_bool_false;
    }
    if (!lh_os_net_socket_set_blocking(lh_addr_of(listen_fd), lh_bool_false))
    {
        lh_os_net_socket_close(lh_addr_of(listen_fd));
        return lh_bool_false;
    }
    lh_logger_notice(lh_runtime_logger(), "[lh] ui web host http://127.0.0.1:%u/",
                     lh_cast_static(lh_uint_t, port));
    {
        lh_uint_t i;
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
            const lh_uint_t now = lh_cast_static(lh_uint_t, lh_os_tick_ms());
            lh_uint_t spent;

            lh_ui_web_app_tick(app);
            lh_ui_web_host_accept_all(lh_addr_of(listen_fd), cli, app, now);
            for (i = 0u; i < LH_UI_WEB_HOST_WS_MAX; ++i)
            {
                lh_ui_web_host_ws_pump(lh_addr_of(cli[i]), app, now);
            }
            lh_ui_web_host_ws_frame(cli, lh_addr_of(stream), app);
            spent = lh_cast_static(lh_uint_t, lh_os_tick_ms()) - now;
            lh_ui_web_host_wait(lh_addr_of(listen_fd), cli,
                                (spent >= LH_UI_WEB_HOST_TICK_MS) ? 0u
                                                                  : LH_UI_WEB_HOST_TICK_MS - spent);
        }
    }
}
