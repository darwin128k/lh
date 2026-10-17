/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>
#include <lh/byte/ptr.h>
#include <lh/ui/web/page.h>
#include <lh/ui/web/ws.h>

#include <lh/cast/const.h>
#include <lh/crypto/sha.h>
#include <lh/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/logger.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/text.h>
#include <lh/ui/canvas/web.h>
#include <lh/util/return.h>

/** WebSocket clients served at once; one more is closed on arrival. */
#define HOST_WS_MAX 8u
/** Receive buffer of one client: the largest message plus a frame header. */
#define HOST_WS_RX (LH_UI_WEB_WS_MAX_MSG + 32u)
/** Bytes a client may have queued for sending before it is dropped as too slow. */
#define HOST_WS_QMAX (4u * 1024u * 1024u)
/** How long a quiet client goes before the host pings it. */
#define HOST_WS_PING_MS 15000u
/** How long a client may send nothing at all before it is dropped. */
#define HOST_WS_DEAD_MS 45000u
/** How long the loop sleeps when nothing at all happens: the tick's period. */
#define HOST_TICK_MS 20u
/** How long an accepted connection may take to send its request line. */
#define HOST_REQUEST_MS 2000u
/** Player HTML from ::lh_ui_web_page_build — room for the script to grow. */
#define HOST_PAGE_CAP (48u * 1024u)

/** The player page, built once by ::lh_ui_web_page_build. */
static lh_byte_t host_page_mem[HOST_PAGE_CAP];
/** Bytes of ::host_page_mem in use. */
static lh_usize_t host_page_len;

/** One encoded WebSocket frame waiting to be sent. */
typedef struct host_ws_q
{
    struct host_ws_q *next; /**< next frame in the queue */
    lh_byte_ptr bytes;      /**< the whole frame, header included */
    unsigned len;           /**< bytes in ::bytes */
    unsigned off;           /**< bytes of it already sent */
} host_ws_q_t;              /**< One encoded WebSocket frame waiting to be sent. */

/** One WebSocket client: its socket, what it sent, and what is queued for it. */
typedef struct host_ws_cli
{
    lh_os_net_socket_t sock; /**< the connection, non-blocking */
    lh_byte_ptr rx;          /**< received bytes not yet parsed (::HOST_WS_RX) */
    unsigned rx_len;         /**< bytes in ::rx */
    lh_byte_ptr msg;         /**< a fragmented message being assembled */
    unsigned msg_len;        /**< bytes in ::msg */
    unsigned msg_cap;        /**< capacity of ::msg */
    unsigned msg_op;         /**< opcode of the message in ::msg */
    lh_bool_t msg_on;        /**< a fragmented message is in progress */
    host_ws_q_t *qhead;      /**< first frame to send */
    host_ws_q_t *qtail;      /**< last frame to send */
    unsigned queued;         /**< bytes queued, against ::HOST_WS_QMAX */
    unsigned seen;           /**< tree generation of the last frame it got */
    unsigned last_rx_ms;     /**< when it last sent anything */
    unsigned last_ping_ms;   /**< when it was last pinged */
    lh_bool_t used;          /**< this slot holds a client */
    lh_bool_t closing;       /**< a close was exchanged; drop once the queue drains */
    lh_bool_t synced; /**< got the last frame sent, so it holds every cover that frame assumed */
} host_ws_cli_t;      /**< One WebSocket client. */

/** What the stream of WS frames shares between iterations. */
typedef struct host_ws_stream
{
    lh_ui_canvas_web_glyphs_t glyphs; /**< covers every synced client already holds */
    char *last;                       /**< bytes of the last frame sent */
    lh_usize_t last_len;              /**< bytes in ::last */
    lh_usize_t last_cap;              /**< capacity of ::last */
} host_ws_stream_t; /**< What the stream of WS frames shares between iterations. */

/** A literal and its length, for the wrappers that take both. */
#define host_lit(s) (s), lh_cast_static(lh_usize_t, (sizeof(s) - 1u))

/** Send all of @p p, stopping early only when the socket fails. */
static lh_void
host_send_all(lh_os_net_socket_t *client, const char *p, lh_usize_t left)
{
    while (left > 0u)
    {
        const lh_ssize_t w = lh_os_net_socket_send(client, lh_cast_const(lh_ptr, p), left);

        if (w <= 0)
        {
            break;
        }
        p += lh_cast_static(lh_usize_t, w);
        left -= lh_cast_static(lh_usize_t, w);
    }
}

/** Answer an HTTP request with status @p code, content type @p type and @p body. */
static lh_void
host_reply(lh_os_net_socket_t *client, unsigned code, const char *type, const char *body,
           lh_usize_t len)
{
    char head[256];
    const char *reason =
        (code == 200u)
            ? "OK"
            : ((code == 304u) ? "Not Modified" : ((code == 500u) ? "Error" : "Not Found"));
    const lh_usize_t n = lh_str_ptr_format_text(
        head, sizeof(head),
        "HTTP/1.1 %u %s\r\nContent-Type: %s\r\nContent-Length: %u\r\n"
        "Connection: close\r\nCache-Control: no-store\r\n\r\n",
        lh_cast_static(lh_uint_t, code), reason, type, lh_cast_static(lh_uint_t, len));

    host_send_all(client, head, n);
    host_send_all(client, body, len);
}

/** ::host_reply with a plain-text body. */
static lh_void
host_reply_text(lh_os_net_socket_t *client, unsigned code, const char *text, lh_usize_t len)
{
    host_reply(client, code, "text/plain; charset=utf-8", text, len);
}

/** Does @p p (of @p len bytes) start with @p lit? */
static lh_bool_t
host_starts(const char *p, lh_usize_t len, const char *lit, lh_usize_t lit_len)
{
    return (len >= lit_len && lh_memory_compare(lh_cast_const(lh_ptr, p), lit_len,
                                                lh_cast_const(lh_ptr, lit), lit_len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

/** The first whole @p needle in @p p, or lh_null. lh_memory_find compares a
   shortened needle at the tail, so a match running past the end is no match. */
static const char *
host_find(const char *p, lh_usize_t len, const char *needle, lh_usize_t needle_len)
{
    const char *at =
        lh_ptr_rcast(const char, lh_memory_find(lh_cast_const(lh_ptr, p), len,
                                                lh_cast_const(lh_ptr, needle), needle_len));

    return (at != lh_null && lh_cast_static(lh_usize_t, (at - p)) + needle_len <= len) ? at
                                                                                       : lh_null;
}

/** `GET <path>` or `POST <path>` at the start of the request. */
static lh_bool_t
host_route(const char *req, lh_usize_t len, const char *path, lh_usize_t path_len)
{
    if (host_starts(req, len, host_lit("GET ")))
    {
        return host_starts(req + 4u, len - 4u, path, path_len);
    }
    if (host_starts(req, len, host_lit("POST ")))
    {
        return host_starts(req + 5u, len - 5u, path, path_len);
    }
    return lh_bool_false;
}

/** The value of query parameter @p key (written `x=`), as text up to the next
   `&`, space or line end. Only a key right after `?` or `&` counts, so `x=`
   is never found inside `dx=`. */
static lh_usize_t
host_query(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len, const char **value)
{
    lh_usize_t off = 0u;

    for (;;)
    {
        const char *at = host_find(req + off, len - off, key, key_len);
        lh_usize_t n = 0u;

        if (at == lh_null)
        {
            return 0u;
        }
        off = lh_cast_static(lh_usize_t, (at - req)) + key_len;
        if (at == req || (at[-1] != '?' && at[-1] != '&'))
        {
            continue;
        }
        while (off + n < len && req[off + n] != '&' && req[off + n] != ' ' &&
               req[off + n] != '\r' && req[off + n] != '\n')
        {
            ++n;
        }
        lh_ptr_deref(value) = req + off;
        return n;
    }
}

/** Query parameter @p key as a signed integer; 0 when absent or not a number. */
static int
host_query_int(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len)
{
    const char *value = lh_null;
    const lh_usize_t n = host_query(req, len, key, key_len, lh_addr_of(value));
    lh_sint_t v = 0;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%d", lh_addr_of(v)) == 0u)
    {
        return 0;
    }
    return (int)v;
}

/** Query parameter @p key as an unsigned integer; 0 when absent or not a number. */
static unsigned
host_query_uint(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len)
{
    const char *value = lh_null;
    const lh_usize_t n = host_query(req, len, key, key_len, lh_addr_of(value));
    lh_uint_t v = 0u;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%u", lh_addr_of(v)) == 0u)
    {
        return 0u;
    }
    return (unsigned)v;
}

/** Query parameter @p key as a flag: @p absent when it is not there, otherwise true for any
   number but 0. */
static lh_bool_t
host_query_flag(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len,
                lh_bool_t absent)
{
    const char *value = lh_null;

    if (host_query(req, len, key, key_len, lh_addr_of(value)) == 0u)
    {
        return absent;
    }
    return (host_query_int(req, len, key, key_len) != 0) ? lh_bool_true : lh_bool_false;
}

/** @p event to the app's input hook, or to ::lh_ui_view_input without one. True when
   the tree may have changed: always for a hook, which the host cannot see into. */
static lh_bool_t
host_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
           const lh_os_input_event_t *event)
{
    if (hooks != lh_null && hooks->input != lh_null)
    {
        hooks->input(event, hooks->ctx);
        return lh_bool_true;
    }
    return lh_ui_view_input(view, event);
}

/** The player's pointer, which is a mouse with one button: @p kind at view pixel
   (@p x, @p y). A move comes only while the button is down, so it holds it. */
static lh_bool_t
host_pointer(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_os_input_kind_t kind,
             int x, int y)
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
    return host_input(view, hooks, lh_addr_of(event));
}

/** A wheel notch at (@p x, @p y): @p dx, @p dy are notches, 1 is left / up. */
static lh_bool_t
host_wheel(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, int x, int y, int dx, int dy)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_wheel(
        lh_addr_of(event), lh_os_input_coord_from_int(x), lh_os_input_coord_from_int(y),
        lh_type_cast(lh_s32_t, dx) * LH_OS_INPUT_WHEEL_NOTCH,
        lh_type_cast(lh_s32_t, dy) * LH_OS_INPUT_WHEEL_NOTCH, lh_os_tick_ms());
    return host_input(view, hooks, lh_addr_of(event));
}

/** @{ */
/** Layout mode: the tab is the view. Clamp so a 1×1 or a 16k request cannot
   blow the JSON scratch or starve the host. Fit mode never calls this. */
#define HOST_SIZE_MIN 160
#define HOST_SIZE_MAX 4096
/** @} */

/** Layout mode: make the view @p w x @p h, clamped; true when the size changed. */
static lh_bool_t
host_resize(const lh_ui_web_host_hooks_t *hooks, unsigned *width, unsigned *height, int w, int h)
{
    if (width == lh_null || height == lh_null)
    {
        return lh_bool_false;
    }
    if (w < HOST_SIZE_MIN)
    {
        w = HOST_SIZE_MIN;
    }
    if (h < HOST_SIZE_MIN)
    {
        h = HOST_SIZE_MIN;
    }
    if (w > HOST_SIZE_MAX)
    {
        w = HOST_SIZE_MAX;
    }
    if (h > HOST_SIZE_MAX)
    {
        h = HOST_SIZE_MAX;
    }
    if (lh_cast_static(unsigned, w) == lh_ptr_deref(width) &&
        lh_cast_static(unsigned, h) == lh_ptr_deref(height))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(width) = lh_cast_static(unsigned, w);
    lh_ptr_deref(height) = lh_cast_static(unsigned, h);
    if (hooks != lh_null && hooks->resize != lh_null)
    {
        hooks->resize(w, h, hooks->ctx);
    }
    return lh_bool_true;
}

/** A key from the player (an ::lh_os_key_t, anything else is `other`) going down (@p pressed)
   or up. */
static lh_bool_t
host_key(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, int k, lh_bool_t pressed)
{
    lh_os_input_event_t event;

    if (k < lh_cast_static(int, lh_os_key_other) || k > lh_cast_static(int, lh_os_key_alt))
    {
        k = lh_cast_static(int, lh_os_key_other);
    }
    lh_os_input_event_init_key(lh_addr_of(event), lh_type_cast(lh_os_key_t, k), pressed,
                               lh_bool_false, 0U, lh_os_tick_ms());
    return host_input(view, hooks, lh_addr_of(event));
}

/** A typed code point from the player; 0 is ignored. */
static lh_bool_t
host_text(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_u32_t code)
{
    lh_os_input_event_t event;

    lh_return_if(code == 0U, lh_bool_false);
    lh_os_input_event_init_text(lh_addr_of(event), code, lh_os_tick_ms());
    return host_input(view, hooks, lh_addr_of(event));
}

/** Mark the tree changed: the next frame is due for every client. */
static lh_void
host_bump(unsigned *gen)
{
    if (gen != lh_null)
    {
        ++lh_ptr_deref(gen);
    }
}

/** Base64 of a 20-byte SHA-1 digest into @p out (29 bytes with the NUL). */
static lh_void
host_b64_sha1(lh_byte_cptr in, char *out)
{
    static const char tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned i;
    unsigned o = 0;

    for (i = 0; i < 18u; i += 3u)
    {
        const unsigned n = (lh_cast_static(unsigned, in[i]) << 16) |
                           (lh_cast_static(unsigned, in[i + 1u]) << 8) |
                           lh_cast_static(unsigned, in[i + 2u]);

        out[o++] = tab[(n >> 18) & 63u];
        out[o++] = tab[(n >> 12) & 63u];
        out[o++] = tab[(n >> 6) & 63u];
        out[o++] = tab[n & 63u];
    }
    {
        const unsigned n =
            (lh_cast_static(unsigned, in[18]) << 16) | (lh_cast_static(unsigned, in[19]) << 8);

        out[o++] = tab[(n >> 18) & 63u];
        out[o++] = tab[(n >> 12) & 63u];
        out[o++] = tab[(n >> 6) & 63u];
        out[o++] = '=';
        out[o] = 0;
    }
}

/** Answer a WebSocket handshake (RFC 6455) and make the socket non-blocking. */
static lh_bool_t
host_ws_upgrade(lh_os_net_socket_t *client, const char *req, lh_usize_t len)
{
    static const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const char *k = host_find(req, len, host_lit("Sec-WebSocket-Key:"));
    char cat[80];
    lh_byte_t digest[LH_CRYPTO_SHA1_DIGEST_SIZE];
    char accept[32];
    char head[280];
    lh_usize_t n = 0u;
    lh_usize_t h;

    if (k == lh_null)
    {
        k = host_find(req, len, host_lit("sec-websocket-key:"));
    }
    if (k == lh_null)
    {
        return lh_bool_false;
    }
    k += sizeof("Sec-WebSocket-Key:") - 1u;
    while (k < req + len && lh_ptr_deref(k) == ' ')
    {
        ++k;
    }
    while (k + n < req + len && k[n] != '\r' && k[n] != '\n' && n < sizeof(cat) - sizeof(guid))
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
    host_b64_sha1(digest, accept);
    h = lh_str_ptr_format_text(head, sizeof(head),
                               "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n"
                               "Connection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
                               accept);
    if (h == 0u)
    {
        return lh_bool_false;
    }
    host_send_all(client, head, h);
    (void)lh_os_net_socket_set_blocking(client, lh_bool_false);
    return lh_bool_true;
}

/** One input from the player: `p x y`, `r x y`, `m x y` (a move while a button is down),
   `w x y dx dy`, `k key pressed` (`1` down, `0` up), `t code`, `z w h` (layout-mode size).
   `k key` without the flag is what a player from before key-up sends, and is a key going down. */
static lh_void
host_event_line(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *width,
                unsigned *height, unsigned *gen, const char *line, lh_usize_t len)
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
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_down, lh_cast_static(int, x),
                           lh_cast_static(int, y));
    }
    else if (lh_str_ptr_parse_text(line, len, "r %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_up, lh_cast_static(int, x),
                           lh_cast_static(int, y));
    }
    else if (lh_str_ptr_parse_text(line, len, "w %d %d %d %d", lh_addr_of(x), lh_addr_of(y),
                                   lh_addr_of(dx), lh_addr_of(dy)) != 0u)
    {
        (void)host_wheel(view, hooks, lh_cast_static(int, x), lh_cast_static(int, y),
                         lh_cast_static(int, dx), lh_cast_static(int, dy));
    }
    else if (lh_str_ptr_parse_text(line, len, "m %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* A drag sends one every frame; one that changed nothing is not worth a draw. */
        if (!host_pointer(view, hooks, lh_os_input_kind_pointer_move, lh_cast_static(int, x),
                          lh_cast_static(int, y)))
        {
            return;
        }
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d %d", lh_addr_of(k), lh_addr_of(pressed)) != 0u)
    {
        (void)host_key(view, hooks, lh_cast_static(int, k),
                       (pressed != 0) ? lh_bool_true : lh_bool_false);
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d", lh_addr_of(k)) != 0u)
    {
        (void)host_key(view, hooks, lh_cast_static(int, k), lh_bool_true);
    }
    else if (lh_str_ptr_parse_text(line, len, "t %u", lh_addr_of(code)) != 0u)
    {
        (void)host_text(view, hooks, lh_cast_static(lh_u32_t, code));
    }
    else if (lh_str_ptr_parse_text(line, len, "z %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* Always bump: an unchanged size still needs a frame for a new tab, or the
           player keeps the design 980×620 it got before `z` arrived. */
        (void)host_resize(hooks, width, height, lh_cast_static(int, x), lh_cast_static(int, y));
    }
    else
    {
        return;
    }
    host_bump(gen);
}

/** Close a client and free everything it holds. */
static lh_void
host_ws_drop(host_ws_cli_t *c)
{
    host_ws_q_t *q;

    if (c == lh_null || !c->used)
    {
        return;
    }
    lh_os_net_socket_close(lh_addr_of(c->sock));
    lh_os_net_socket_init(lh_addr_of(c->sock));
    q = c->qhead;
    while (q != lh_null)
    {
        host_ws_q_t *n = q->next;

        lh_runtime_allocator_free(q->bytes);
        lh_runtime_allocator_free(q);
        q = n;
    }
    lh_runtime_allocator_free(c->rx);
    lh_runtime_allocator_free(c->msg);
    c->rx = lh_null;
    c->msg = lh_null;
    c->rx_len = 0u;
    c->msg_len = 0u;
    c->msg_cap = 0u;
    c->msg_on = lh_bool_false;
    c->qhead = lh_null;
    c->qtail = lh_null;
    c->queued = 0u;
    c->used = lh_bool_false;
    c->closing = lh_bool_false;
}

/** Queue one frame for a client, at the front for control frames; drops a client over
 * ::HOST_WS_QMAX. */
static lh_bool_t
host_ws_enqueue(host_ws_cli_t *c, unsigned opcode, const void *payload, lh_usize_t plen,
                lh_bool_t front)
{
    const lh_usize_t need = lh_ui_web_ws_frame_size(plen);
    host_ws_q_t *q;
    lh_byte_ptr bytes;

    if (c == lh_null || !c->used)
    {
        return lh_bool_false;
    }
    if (need == 0u || c->queued + lh_cast_static(unsigned, need) > HOST_WS_QMAX)
    {
        host_ws_drop(c);
        return lh_bool_false;
    }
    q = lh_ptr_rcast(host_ws_q_t, lh_os_alloc(sizeof(lh_ptr_deref(q))));
    bytes = lh_ptr_rcast(lh_byte_t, lh_os_alloc(need));
    if (q == lh_null || bytes == lh_null ||
        lh_ui_web_ws_write(bytes, need, opcode, lh_bool_true, payload, plen) != need)
    {
        lh_runtime_allocator_free(q);
        lh_runtime_allocator_free(bytes);
        host_ws_drop(c);
        return lh_bool_false;
    }
    q->bytes = bytes;
    q->len = lh_cast_static(unsigned, need);
    q->off = 0u;
    if (front)
    {
        q->next = c->qhead;
        c->qhead = q;
        if (c->qtail == lh_null)
        {
            c->qtail = q;
        }
    }
    else
    {
        q->next = lh_null;
        if (c->qtail != lh_null)
        {
            c->qtail->next = q;
        }
        else
        {
            c->qhead = q;
        }
        c->qtail = q;
    }
    c->queued += lh_cast_static(unsigned, need);
    return lh_bool_true;
}

/** Send what a client's socket takes now; drops it on error or once a close has drained. */
static lh_void
host_ws_flush(host_ws_cli_t *c)
{
    while (c != lh_null && c->used && c->qhead != lh_null)
    {
        host_ws_q_t *q = c->qhead;
        const lh_ssize_t w = lh_os_net_socket_send(lh_addr_of(c->sock), q->bytes + q->off,
                                                   lh_cast_static(lh_usize_t, (q->len - q->off)));

        if (w == LH_OS_RESULT_PENDING)
        {
            return;
        }
        if (w <= 0)
        {
            host_ws_drop(c);
            return;
        }
        q->off += lh_cast_static(unsigned, w);
        c->queued -= lh_cast_static(unsigned, w);
        if (q->off >= q->len)
        {
            c->qhead = q->next;
            if (c->qhead == lh_null)
            {
                c->qtail = lh_null;
            }
            lh_runtime_allocator_free(q->bytes);
            lh_runtime_allocator_free(q);
        }
    }
    if (c != lh_null && c->used && c->closing && c->qhead == lh_null)
    {
        host_ws_drop(c);
    }
}

/** Append a fragment to the message being assembled; false past ::LH_UI_WEB_WS_MAX_MSG. */
static lh_bool_t
host_ws_msg_add(host_ws_cli_t *c, lh_byte_cptr p, lh_usize_t n)
{
    lh_byte_ptr grow;

    if (c->msg_len + lh_cast_static(unsigned, n) > LH_UI_WEB_WS_MAX_MSG)
    {
        return lh_bool_false;
    }
    if (c->msg_len + lh_cast_static(unsigned, n) > c->msg_cap)
    {
        const unsigned cap = c->msg_len + lh_cast_static(unsigned, n);
        const unsigned next = (c->msg_cap == 0u) ? 256u : (c->msg_cap * 2u);
        const unsigned want = (cap > next) ? cap : next;

        grow = lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_realloc(c->msg, c->msg_cap, want));
        if (grow == lh_null)
        {
            return lh_bool_false;
        }
        c->msg = grow;
        c->msg_cap = want;
    }
    lh_memory_copy(c->msg + c->msg_len, c->msg_cap - c->msg_len, lh_cast_const(lh_ptr, p), n);
    c->msg_len += lh_cast_static(unsigned, n);
    return lh_bool_true;
}

/** Parse every whole frame received: answer pings and closes, run text messages as input lines. */
static lh_void
host_ws_ingest(host_ws_cli_t *c, lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
               unsigned *width, unsigned *height, unsigned *gen, unsigned now)
{
    while (c->used && c->rx_len >= 2u)
    {
        unsigned opcode = 0;
        lh_bool_t fin = lh_bool_false;
        lh_byte_cptr payload = lh_null;
        lh_usize_t plen = 0;
        lh_byte_cptr mask = lh_null;
        const lh_usize_t n =
            lh_ui_web_ws_read(c->rx, c->rx_len, lh_addr_of(opcode), lh_addr_of(fin),
                              lh_addr_of(payload), lh_addr_of(plen), lh_addr_of(mask));
        lh_byte_ptr body;

        if (n == 0u)
        {
            return;
        }
        if (n == lh_cast_static(lh_usize_t, -1))
        {
            host_ws_drop(c);
            return;
        }
        body = lh_cast_const(lh_byte_ptr, payload);
        if (mask != lh_null)
        {
            lh_ui_web_ws_unmask(body, plen, mask);
        }
        c->last_rx_ms = now;
        if (opcode >= 8u)
        {
            if (!fin || plen > 125u)
            {
                host_ws_drop(c);
                return;
            }
            if (opcode == LH_UI_WEB_WS_PING)
            {
                if (!host_ws_enqueue(c, LH_UI_WEB_WS_PONG, body, plen, lh_bool_true))
                {
                    return;
                }
            }
            else if (opcode == LH_UI_WEB_WS_CLOSE)
            {
                c->closing = lh_bool_true;
                if (!host_ws_enqueue(c, LH_UI_WEB_WS_CLOSE, body, plen, lh_bool_true))
                {
                    return;
                }
            }
        }
        else if (opcode == LH_UI_WEB_WS_CONT)
        {
            if (!c->msg_on || !host_ws_msg_add(c, body, plen))
            {
                host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (c->msg_op == LH_UI_WEB_WS_TEXT)
                {
                    host_event_line(view, hooks, width, height, gen,
                                    lh_ptr_rcast(const char, c->msg), c->msg_len);
                }
                c->msg_on = lh_bool_false;
                c->msg_len = 0u;
            }
        }
        else if (opcode == LH_UI_WEB_WS_TEXT || opcode == LH_UI_WEB_WS_BIN)
        {
            if (c->msg_on)
            {
                host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (opcode == LH_UI_WEB_WS_TEXT)
                {
                    host_event_line(view, hooks, width, height, gen, lh_ptr_rcast(const char, body),
                                    plen);
                }
            }
            else
            {
                c->msg_on = lh_bool_true;
                c->msg_op = opcode;
                c->msg_len = 0u;
                if (!host_ws_msg_add(c, body, plen))
                {
                    host_ws_drop(c);
                    return;
                }
            }
        }
        else
        {
            host_ws_drop(c);
            return;
        }
        lh_memory_move(c->rx, HOST_WS_RX, c->rx + n,
                       lh_cast_static(lh_usize_t, (c->rx_len - lh_cast_static(unsigned, n))));
        c->rx_len -= lh_cast_static(unsigned, n);
    }
}

/** Give an upgraded connection a free client slot, or close it when there is none. */
static lh_void
host_ws_attach(host_ws_cli_t *slots, lh_os_net_socket_t *client, unsigned now)
{
    unsigned i;

    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        host_ws_cli_t *c = slots + i;

        if (c->used)
        {
            continue;
        }
        c->rx = lh_ptr_rcast(lh_byte_t, lh_os_alloc(HOST_WS_RX));
        if (c->rx == lh_null)
        {
            lh_os_net_socket_close(client);
            return;
        }
        c->sock = lh_ptr_deref(client);
        c->rx_len = 0u;
        c->msg = lh_null;
        c->msg_len = 0u;
        c->msg_cap = 0u;
        c->msg_on = lh_bool_false;
        c->qhead = lh_null;
        c->qtail = lh_null;
        c->queued = 0u;
        c->seen = 0u;
        c->last_rx_ms = now;
        c->last_ping_ms = now;
        c->used = lh_bool_true;
        c->closing = lh_bool_false;
        c->synced = lh_bool_false;
        return;
    }
    lh_os_net_socket_close(client);
}

/** Ping, time out, read and answer one client; one pass, never blocks. */
static lh_void
host_ws_pump(host_ws_cli_t *c, lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
             unsigned *width, unsigned *height, unsigned *gen, unsigned now)
{
    lh_ssize_t got;

    if (!c->used)
    {
        return;
    }
    if ((now - c->last_rx_ms) > HOST_WS_DEAD_MS)
    {
        host_ws_drop(c);
        return;
    }
    if (!c->closing && (now - c->last_ping_ms) >= HOST_WS_PING_MS)
    {
        if (!host_ws_enqueue(c, LH_UI_WEB_WS_PING, lh_null, 0u, lh_bool_true))
        {
            return;
        }
        c->last_ping_ms = now;
    }
    if (c->rx_len >= HOST_WS_RX)
    {
        host_ws_drop(c);
        return;
    }
    got = lh_os_net_socket_recv(lh_addr_of(c->sock), c->rx + c->rx_len, HOST_WS_RX - c->rx_len);
    if (got == LH_OS_RESULT_PENDING)
    {
        host_ws_flush(c);
        return;
    }
    if (got <= 0)
    {
        host_ws_drop(c);
        return;
    }
    c->rx_len += lh_cast_static(unsigned, got);
    host_ws_ingest(c, view, hooks, width, height, gen, now);
    if (c->used)
    {
        host_ws_flush(c);
    }
}

/** Draw the whole view as one JSON frame into @p json. */
static lh_bool_t
host_draw(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
          lh_ui_canvas_web_t *web, lh_ui_canvas_web_glyphs_t *glyphs)
{
    lh_ui_canvas_t canvas;

    if (!lh_ui_canvas_web_open_mem(web, json, json_cap, width, height))
    {
        return lh_bool_false;
    }
    lh_ui_canvas_web_set_glyphs(web, glyphs);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), web);
    lh_ui_view_set_canvas(view, lh_addr_of(canvas));
    lh_ui_view_draw(view, lh_null);
    lh_ui_view_set_canvas(view, lh_null);
    return lh_ui_canvas_web_close(web);
}

/** An input over HTTP: the same lines as WS, with the numbers in the query. */
static lh_bool_t
host_handle_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *width,
                  unsigned *height, const char *req, lh_usize_t len)
{
    const int x = host_query_int(req, len, host_lit("x="));
    const int y = host_query_int(req, len, host_lit("y="));

    if (host_route(req, len, host_lit("/ui/click")))
    {
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_down, x, y);
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_up, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/press")))
    {
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_down, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/release")))
    {
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_up, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/move")))
    {
        (void)host_pointer(view, hooks, lh_os_input_kind_pointer_move, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/wheel")))
    {
        (void)host_wheel(view, hooks, x, y, host_query_int(req, len, host_lit("dx=")),
                         host_query_int(req, len, host_lit("dy=")));
    }
    else if (host_route(req, len, host_lit("/ui/key")))
    {
        /* `p=` is the direction; without it the request is a key going down, as before. */
        (void)host_key(view, hooks, host_query_int(req, len, host_lit("k=")),
                       host_query_flag(req, len, host_lit("p="), lh_bool_true));
    }
    else if (host_route(req, len, host_lit("/ui/text")))
    {
        (void)host_text(view, hooks,
                        lh_cast_static(lh_u32_t, host_query_uint(req, len, host_lit("c="))));
    }
    else if (host_route(req, len, host_lit("/ui/size")))
    {
        /* Always handled: an unchanged size is still a valid no-op, not a 404. */
        (void)host_resize(hooks, width, height, host_query_int(req, len, host_lit("w=")),
                          host_query_int(req, len, host_lit("h=")));
    }
    else
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

/** Serve one HTTP request; true when it became a WebSocket to keep. */
static lh_bool_t
host_handle(lh_ui_view_t *view, unsigned *width, unsigned *height, char *json, lh_usize_t json_cap,
            const lh_ui_web_host_hooks_t *hooks, unsigned *gen, lh_os_net_socket_t *client)
{
    char req[1024];
    lh_os_net_socket_t *const watch[1] = {client};
    lh_ui_canvas_web_t web;
    lh_ssize_t got;
    lh_usize_t len;

    /* Accepted the moment it connected, so the request may not be here yet; and
       a socket accepted from a non-blocking listener is non-blocking on Windows,
       so a bare recv would answer "not yet" and drop the request. */
    (void)lh_os_net_socket_set_blocking(client, lh_bool_true);
    (void)lh_os_net_socket_set_no_delay(client, lh_bool_true);
    if (lh_os_net_socket_wait(watch, 1u, lh_null, 0u, HOST_REQUEST_MS) <= 0)
    {
        return lh_bool_false;
    }
    got = lh_os_net_socket_recv(client, req, sizeof(req));
    if (got <= 0)
    {
        return lh_bool_false;
    }
    len = lh_cast_static(lh_usize_t, got);
    if (host_route(req, len, host_lit("/ws")))
    {
        return host_ws_upgrade(client, req, len);
    }
    if (host_handle_input(view, hooks, width, height, req, len))
    {
        host_bump(gen);
        host_reply_text(client, 200u, host_lit("ok\n"));
        return lh_bool_false;
    }
    if (host_route(req, len, host_lit("/ui.rev")))
    {
        char body[32];
        const lh_usize_t n = lh_str_ptr_format_text(
            body, sizeof(body), "%u\n",
            lh_cast_static(lh_uint_t, (gen != lh_null ? lh_ptr_deref(gen) : 0u)));

        host_reply_text(client, 200u, body, n);
        return lh_bool_false;
    }
    if (host_route(req, len, host_lit("/ui.json")))
    {
        if (gen != lh_null && host_query_uint(req, len, host_lit("rev=")) == lh_ptr_deref(gen))
        {
            host_reply_text(client, 304u, host_lit(""));
            return lh_bool_false;
        }
        if (!host_draw(view, lh_ptr_deref(width), lh_ptr_deref(height), json, json_cap,
                       lh_addr_of(web), lh_null))
        {
            host_reply_text(client, 500u, host_lit("draw failed\n"));
            return lh_bool_false;
        }
        host_reply(client, 200u, "application/json; charset=utf-8",
                   lh_ui_canvas_web_bytes(lh_addr_of(web)),
                   lh_ui_canvas_web_length(lh_addr_of(web)));
        return lh_bool_false;
    }
    /* `/` and `/?scale=…` — not a bare `/` prefix, or `/ui.json` would match too. */
    if (host_route(req, len, host_lit("/ ")) || host_route(req, len, host_lit("/?")) ||
        host_route(req, len, host_lit("/index.html")))
    {
        host_reply(client, 200u, "text/html; charset=utf-8",
                   lh_ptr_rcast(const char, host_page_mem), host_page_len);
        return lh_bool_false;
    }
    host_reply_text(client, 404u, host_lit("not found\n"));
    return lh_bool_false;
}

/** Is @p c waiting for a frame it can take now? A client still sending the last
   one is skipped and gets the newest when it is done. */
static lh_bool_t
host_ws_wants(const host_ws_cli_t *c, unsigned gen)
{
    return (c->used && !c->closing && c->seen != gen && c->qhead == lh_null) ? lh_bool_true
                                                                             : lh_bool_false;
}

/** Keep a copy of the frame just sent, to skip sending an equal one. */
static lh_void
host_ws_remember(host_ws_stream_t *stream, const char *body, lh_usize_t len)
{
    if (len > stream->last_cap)
    {
        char *grow =
            lh_ptr_rcast(char, lh_runtime_allocator_realloc(stream->last, stream->last_cap, len));

        if (grow == lh_null)
        {
            stream->last_len = 0u;
            return;
        }
        stream->last = grow;
        stream->last_cap = len;
    }
    lh_memory_copy(stream->last, stream->last_cap, lh_cast_const(lh_ptr, body), len);
    stream->last_len = len;
}

/** Whether @p body equals the last frame sent. */
static lh_bool_t
host_ws_same(const host_ws_stream_t *stream, const char *body, lh_usize_t len)
{
    return (len > 0u && len == stream->last_len &&
            lh_memory_compare(lh_cast_const(lh_ptr, body), len, stream->last, len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

/** Draw once for every client that wants a frame and send it. Covers go once per
   connection: a client that missed a frame, or is new, resets the set so this
   one carries them all. A frame equal to the last one sent is not sent again. */
static lh_void
host_ws_frame(host_ws_cli_t *cli, host_ws_stream_t *stream, lh_ui_view_t *view, unsigned width,
              unsigned height, char *json, lh_usize_t json_cap, unsigned gen)
{
    lh_ui_canvas_web_t web;
    lh_bool_t want = lh_bool_false;
    lh_bool_t same;
    const char *body;
    lh_usize_t len;
    unsigned i;

    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        if (host_ws_wants(cli + i, gen))
        {
            want = lh_bool_true;
            if (!cli[i].synced)
            {
                lh_ui_canvas_web_glyphs_reset(lh_addr_of(stream->glyphs));
            }
        }
    }
    if (!want || !host_draw(view, width, height, json, json_cap, lh_addr_of(web),
                            lh_addr_of(stream->glyphs)))
    {
        return;
    }
    body = lh_ui_canvas_web_bytes(lh_addr_of(web));
    len = lh_ui_canvas_web_length(lh_addr_of(web));
    same = host_ws_same(stream, body, len);
    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        host_ws_cli_t *c = cli + i;

        if (!c->used)
        {
            continue;
        }
        if (!host_ws_wants(c, gen))
        {
            /* Not sent this one: any cover it introduced, this client lacks. */
            if (!same)
            {
                c->synced = lh_bool_false;
            }
            continue;
        }
        if (same && c->synced)
        {
            c->seen = gen;
            continue;
        }
        if (host_ws_enqueue(c, LH_UI_WEB_WS_TEXT, body, len, lh_bool_false))
        {
            c->seen = gen;
            c->synced = lh_bool_true;
            host_ws_flush(c);
        }
    }
    if (!same)
    {
        host_ws_remember(stream, body, len);
    }
}

/** Accept and serve every connection waiting on the listener. */
static lh_void
host_accept_all(lh_os_net_socket_t *listen_fd, host_ws_cli_t *cli, lh_ui_view_t *view,
                unsigned *width, unsigned *height, char *json, lh_usize_t json_cap,
                const lh_ui_web_host_hooks_t *hooks, unsigned *gen, unsigned now)
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
        if (host_handle(view, width, height, json, json_cap, hooks, gen, lh_addr_of(client)))
        {
            host_ws_attach(cli, lh_addr_of(client), now);
        }
        else
        {
            lh_os_net_socket_close(lh_addr_of(client));
        }
    }
}

/** Sleep until a connection, a client's input or room for its queued output --
   or @p timeout_ms, whichever is first. This is what answers an input in the
   time it takes to draw it, rather than in up to one tick. A wait that fails
   (a client socket gone bad under it) waits on the listener alone, so the loop
   still sleeps instead of spinning. */
static lh_void
host_wait(lh_os_net_socket_t *listen_fd, host_ws_cli_t *cli, unsigned timeout_ms)
{
    lh_os_net_socket_t *read[HOST_WS_MAX + 1u];
    lh_os_net_socket_t *write[HOST_WS_MAX];
    lh_usize_t nr = 0u;
    lh_usize_t nw = 0u;
    unsigned i;

    read[nr++] = listen_fd;
    for (i = 0u; i < HOST_WS_MAX; ++i)
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

lh_bool_t
lh_ui_web_host_run(lh_ui_view_t *view, unsigned width, unsigned height, char *json,
                   lh_usize_t json_cap, lh_net_port_t port, const lh_ui_web_host_hooks_t *hooks)
{
    lh_os_net_socket_t listen_fd;
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;

    if (view == lh_null || json == lh_null || json_cap < 64u)
    {
        return lh_bool_false;
    }
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
    if (!lh_ui_web_page_build(host_page_mem, lh_cast_static(lh_usize_t, sizeof(host_page_mem)),
                              lh_addr_of(host_page_len)))
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
        unsigned gen = 1u;
        unsigned i;
        unsigned frame_w = width;
        unsigned frame_h = height;
        host_ws_cli_t cli[HOST_WS_MAX];
        host_ws_stream_t stream;

        lh_memory_set(cli, sizeof(cli), 0);
        lh_memory_set(lh_addr_of(stream), sizeof(stream), 0);
        for (i = 0u; i < HOST_WS_MAX; ++i)
        {
            lh_os_net_socket_init(lh_addr_of(cli[i].sock));
        }
        for (;;)
        {
            lh_ui_rect_t damage;
            const unsigned now = lh_cast_static(unsigned, lh_os_tick_ms());
            unsigned spent;

            if (hooks != lh_null && hooks->tick != lh_null)
            {
                hooks->tick(hooks->ctx);
            }
            host_accept_all(lh_addr_of(listen_fd), cli, view, lh_addr_of(frame_w),
                            lh_addr_of(frame_h), json, json_cap, hooks, lh_addr_of(gen), now);
            for (i = 0u; i < HOST_WS_MAX; ++i)
            {
                host_ws_pump(lh_addr_of(cli[i]), view, hooks, lh_addr_of(frame_w),
                             lh_addr_of(frame_h), lh_addr_of(gen), now);
            }
            /* After the input, not before it: an event and the damage it caused
               are one change and one frame, not two. */
            if (lh_ui_view_take_damage(view, lh_addr_of(damage)))
            {
                ++gen;
            }
            host_ws_frame(cli, lh_addr_of(stream), view, frame_w, frame_h, json, json_cap, gen);
            spent = lh_cast_static(unsigned, lh_os_tick_ms()) - now;
            host_wait(lh_addr_of(listen_fd), cli,
                      (spent >= HOST_TICK_MS) ? 0u : HOST_TICK_MS - spent);
        }
    }
}
