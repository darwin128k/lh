/**
 * @file host.h
 * @brief Serve one ::lh_ui_view_t over HTTP as canvas JSON. No Python.
 *
 * GET `/` is a Canvas2D player. Two scale modes (query `scale=`):
 * - `layout` (default) — the tab size becomes the view size; the app lays out
 *   again (sharp). The player sends `z w h` / `GET /ui/size` on resize.
 * - `fit` (`?scale=fit`) — contain-fit the fixed frame into the tab (may blur).
 * Clicks map back into view pixels. GET `/ui.json` is ::lh_ui_view_draw
 * through ::lh_ui_canvas_backend_web. `ws://…/ws` (and `wss` when the page
 * is TLS) pushes a frame only when the tree changed. Several tabs may share
 * the listen socket. Frames are `lh/ui/web/ws.h`: ping/pong, close, fragments,
 * a send queue. TLS itself is not in this host — WINVER is XP, loopback is
 * cleartext; `wss` is the same codec on a TLS stream when one exists. HTTP
 * remains for a first paint. A 3D pane is ::lh_ui_viewport3d, not an op.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_WEB_HOST_H
#define LH_UI_WEB_HOST_H

#include <lh/config.h>
#include <lh/net/port.h>
#include <lh/os/controller.h>
#include <lh/os/input/sink/cb.h>
#include <lh/os/net/socket.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/view.h>
#include <lh/ui/web/host/client/fields.h>
#include <lh/ui/web/host/frame/fields.h>
#include <lh/ui/web/host/stream/fields.h>
#include <lh/ui/web/ws.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_OS"
#endif

/**
 * @brief What a window would call: the application, not the JSON player.
 *
 * Any function may be ::lh_null. Every input line of the player arrives as one
 * ::lh_os_input_event_t, the same event a desktop window gets, and takes the same
 * two levels (::lh_os_window_set_on_input, ::lh_os_window_set_controller):
 *
 * - @p input, when set, sees it first, raw: the low-level tap;
 * - @p controller, when set, gets it next (::lh_os_controller_feed), and the host
 *   calls ::lh_os_controller_update on it at every tick, before @p tick;
 * - with neither, the host feeds a controller of its own, made with
 *   `lh_ui_view_controller_handle` and `lh_ui_view_controller_cancel` on the
 *   served view and the default thresholds, so the view gets every event as
 *   ::lh_ui_view_input would have, the gestures go along (and are ignored by it).
 *
 * @p input alone is what it always was: the host hands the event to nothing else,
 * and a hook that wants the view to see it passes it on itself. The player has no
 * focus to lose, so the host never cancels a controller.
 *
 * | Line           | Event                                              |
 * |----------------|----------------------------------------------------|
 * | `p x y`        | `pointer_down`, primary button                     |
 * | `r x y`        | `pointer_up`, primary button                       |
 * | `m x y`        | `pointer_move`, primary button held                |
 * | `w x y dx dy`  | `wheel`, `dx` / `dy` notches times 120             |
 * | `k key 1`/`0`  | `key_down` / `key_up`                              |
 * | `t code`       | `text`                                             |
 *
 * The events carry ::lh_os_tick_ms as their time and no modifiers.
 */
struct lh_ui_web_host_hooks
{
    lh_ptr ctx; /**< Passed to every hook. */
    /** About every 20 ms, and between requests. */
    lh_void (*tick)(lh_ptr ctx);
    /** One input event from the player; its `self` is ::ctx. */
    lh_os_input_sink_cb input;
    /** Layout-mode tab size; ::lh_null keeps the host's initial width/height. */
    lh_void (*resize)(lh_sint_t width, lh_sint_t height, lh_ptr ctx);
    /** Takes every input event after ::input and is updated every tick; the caller's,
        and must outlive the run. ::lh_null with no ::input: the host's own view
        controller. */
    lh_os_controller_t *controller;
};
/** @copybrief lh_ui_web_host_hooks */
typedef struct lh_ui_web_host_hooks lh_ui_web_host_hooks_t;

/** WebSocket clients served at once; one more is closed on arrival. */
#define LH_UI_WEB_HOST_WS_MAX 8u
/** Receive buffer of one client: the largest message plus a frame header. */
#define LH_UI_WEB_HOST_WS_RX (LH_UI_WEB_WS_MAX_MSG + 32u)
/** Bytes a client may have queued for sending before it is dropped as too slow. */
#define LH_UI_WEB_HOST_WS_QMAX (4u * 1024u * 1024u)
/** How long a quiet client goes before the host pings it. */
#define LH_UI_WEB_HOST_WS_PING_MS 15000u
/** How long a client may send nothing at all before it is dropped. */
#define LH_UI_WEB_HOST_WS_DEAD_MS 45000u
/** How long the loop sleeps when nothing at all happens: the tick's period. */
#define LH_UI_WEB_HOST_TICK_MS 20u
/** How long an accepted connection may take to send its request line. */
#define LH_UI_WEB_HOST_REQUEST_MS 2000u
/** Player HTML from ::lh_ui_web_page_build -- room for the script to grow. */
#define LH_UI_WEB_HOST_PAGE_CAP (48u * 1024u)
/** Layout mode: the tab is the view. Clamp so a 1x1 or a 16k request cannot
   blow the JSON scratch or starve the host. Fit mode never resizes. */
#define LH_UI_WEB_HOST_SIZE_MIN 160
/** @copydoc LH_UI_WEB_HOST_SIZE_MIN */
#define LH_UI_WEB_HOST_SIZE_MAX 4096

/**
 * @struct lh_ui_web_host_frame
 * @brief One encoded WebSocket frame waiting to be sent.
 */
struct lh_ui_web_host_frame
{
    /** Members: ::lh_ui_web_host_frame_fields. */
    lh_ui_web_host_frame_fields(struct lh_ui_web_host_frame *, lh_byte_ptr, lh_uint_t);
};
/** @copybrief lh_ui_web_host_frame */
typedef struct lh_ui_web_host_frame lh_ui_web_host_frame_t;

/**
 * @struct lh_ui_web_host_client
 * @brief One WebSocket client: its socket, what it sent, and what is queued for it.
 */
struct lh_ui_web_host_client
{
    /** Members: ::lh_ui_web_host_client_fields. */
    lh_ui_web_host_client_fields(lh_os_net_socket_t, lh_byte_ptr, lh_uint_t,
                                 lh_ui_web_host_frame_t *, lh_tick_t, lh_bool_t);
};
/** @copybrief lh_ui_web_host_client */
typedef struct lh_ui_web_host_client lh_ui_web_host_client_t;

/**
 * @struct lh_ui_web_host_stream
 * @brief What the stream of WS frames shares between iterations.
 */
struct lh_ui_web_host_stream
{
    /** Members: ::lh_ui_web_host_stream_fields. */
    lh_ui_web_host_stream_fields(lh_ui_canvas_web_glyphs_t, lh_str_ptr, lh_usize_t);
};
/** @copybrief lh_ui_web_host_stream */
typedef struct lh_ui_web_host_stream lh_ui_web_host_stream_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Answer an HTTP request with status @p code, content type @p type and @p body. */
lh_void
lh_ui_web_host_reply(lh_os_net_socket_t *client, lh_uint_t code, const lh_str_ptr type,
                     const lh_str_ptr body, lh_usize_t len);

/** ::lh_ui_web_host_reply with a plain-text body. */
lh_void
lh_ui_web_host_reply_text(lh_os_net_socket_t *client, lh_uint_t code, const lh_str_ptr text,
                          lh_usize_t len);

/** The first whole @p needle in @p p, or lh_null. lh_memory_find compares a
   shortened needle at the tail, so a match running past the end is no match. */
const lh_str_ptr
lh_ui_web_host_find(const lh_str_ptr p, lh_usize_t len, const lh_str_ptr needle,
                    lh_usize_t needle_len);

/** `GET <path>` or `POST <path>` at the start of the request. */
lh_bool_t
lh_ui_web_host_route(const lh_str_ptr req, lh_usize_t len, const lh_str_ptr path,
                     lh_usize_t path_len);

/** The value of query parameter @p key (written `x=`), as text up to the next
   `&`, space or line end. Only a key right after `?` or `&` counts, so `x=`
   is never found inside `dx=`. */
lh_usize_t
lh_ui_web_host_query(const lh_str_ptr req, lh_usize_t len, const lh_str_ptr key, lh_usize_t key_len,
                     const lh_str_ptr *value);

/** Query parameter @p key as a signed integer; 0 when absent or not a number. */
lh_sint_t
lh_ui_web_host_query_int(const lh_str_ptr req, lh_usize_t len, const lh_str_ptr key,
                         lh_usize_t key_len);

/** Query parameter @p key as an lh_uint_t integer; 0 when absent or not a number. */
lh_uint_t
lh_ui_web_host_query_uint(const lh_str_ptr req, lh_usize_t len, const lh_str_ptr key,
                          lh_usize_t key_len);

/** Query parameter @p key as a flag: @p absent when it is not there, otherwise true for any
   number but 0. */
lh_bool_t
lh_ui_web_host_query_flag(const lh_str_ptr req, lh_usize_t len, const lh_str_ptr key,
                          lh_usize_t key_len, lh_bool_t absent);

/** @p event to the app's input hook, then to the controller of @p hooks -- the app's, or
   the host's own on the view (see lh_ui_web_host_route_hooks). With neither, which a run never
   hands in, straight to ::lh_ui_view_input. True when the tree may have changed:
   always for a hook, which the host cannot see into, else what the controller says. */
lh_bool_t
lh_ui_web_host_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
                     const lh_os_input_event_t *event);

/** The player's pointer, which is a mouse with one button: @p kind at view pixel
   (@p x, @p y). A move comes only while the button is down, so it holds it. */
lh_bool_t
lh_ui_web_host_pointer(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
                       lh_os_input_kind_t kind, lh_sint_t x, lh_sint_t y);

/** A wheel notch at (@p x, @p y): @p dx, @p dy are notches, 1 is left / up. */
lh_bool_t
lh_ui_web_host_wheel(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_sint_t x,
                     lh_sint_t y, lh_sint_t dx, lh_sint_t dy);

/** Layout mode: make the view @p w x @p h, clamped; true when the size changed. */
lh_bool_t
lh_ui_web_host_resize(const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width, lh_uint_t *height,
                      lh_sint_t w, lh_sint_t h);

/** A key from the player (an ::lh_os_key_t, anything else is `other`) going down (@p pressed)
   or up. */
lh_bool_t
lh_ui_web_host_key(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_sint_t k,
                   lh_bool_t pressed);

/** A typed code point from the player; 0 is ignored. */
lh_bool_t
lh_ui_web_host_text(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_u32_t code);

/** Mark the tree changed: the next frame is due for every client. */
lh_void
lh_ui_web_host_bump(lh_uint_t *gen);

/** Answer a WebSocket handshake (RFC 6455) and make the socket non-blocking. */
lh_bool_t
lh_ui_web_host_ws_upgrade(lh_os_net_socket_t *client, const lh_str_ptr req, lh_usize_t len);

/** One input from the player: `p x y`, `r x y`, `m x y` (a move while a button is down),
   `w x y dx dy`, `k key pressed` (`1` down, `0` up), `t code`, `z w h` (layout-mode size).
   `k key` without the flag is what a player from before key-up sends, and is a key going down. */
lh_void
lh_ui_web_host_event_line(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width,
                          lh_uint_t *height, lh_uint_t *gen, const lh_str_ptr line, lh_usize_t len);

/** Close a client and free everything it holds. */
lh_void
lh_ui_web_host_ws_drop(lh_ui_web_host_client_t *c);

/** Queue one frame for a client, at the front for control frames; drops a client over
 * ::LH_UI_WEB_HOST_WS_QMAX. */
lh_bool_t
lh_ui_web_host_ws_enqueue(lh_ui_web_host_client_t *c, lh_uint_t opcode, const void *payload,
                          lh_usize_t plen, lh_bool_t front);

/** Send what a client's socket takes now; drops it on error or once a close has drained. */
lh_void
lh_ui_web_host_ws_flush(lh_ui_web_host_client_t *c);

/** Append a fragment to the message being assembled; false past ::LH_UI_WEB_WS_MAX_MSG. */
lh_bool_t
lh_ui_web_host_ws_msg_add(lh_ui_web_host_client_t *c, const lh_byte_ptr p, lh_usize_t n);

/** Parse every whole frame received: answer pings and closes, run text messages as input lines. */
lh_void
lh_ui_web_host_ws_ingest(lh_ui_web_host_client_t *c, lh_ui_view_t *view,
                         const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width, lh_uint_t *height,
                         lh_uint_t *gen, lh_uint_t now);

/** Give an upgraded connection a free client slot, or close it when there is none. */
lh_void
lh_ui_web_host_ws_attach(lh_ui_web_host_client_t *slots, lh_os_net_socket_t *client, lh_uint_t now);

/** Ping, time out, read and answer one client; one pass, never blocks. */
lh_void
lh_ui_web_host_ws_pump(lh_ui_web_host_client_t *c, lh_ui_view_t *view,
                       const lh_ui_web_host_hooks_t *hooks, lh_uint_t *width, lh_uint_t *height,
                       lh_uint_t *gen, lh_uint_t now);

/** Draw the whole view as one JSON frame into @p json. */
lh_bool_t
lh_ui_web_host_draw(lh_ui_view_t *view, lh_uint_t width, lh_uint_t height, lh_str_ptr json,
                    lh_usize_t json_cap, lh_ui_canvas_web_t *web,
                    lh_ui_canvas_web_glyphs_t *glyphs);

/** An input over HTTP: the same lines as WS, with the numbers in the query. */
lh_bool_t
lh_ui_web_host_handle_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks,
                            lh_uint_t *width, lh_uint_t *height, const lh_str_ptr req,
                            lh_usize_t len);

/** Serve one HTTP request; true when it became a WebSocket to keep. */
lh_bool_t
lh_ui_web_host_handle(lh_ui_view_t *view, lh_uint_t *width, lh_uint_t *height, lh_str_ptr json,
                      lh_usize_t json_cap, const lh_ui_web_host_hooks_t *hooks, lh_uint_t *gen,
                      lh_os_net_socket_t *client);

/** Is @p c waiting for a frame it can take now? A client still sending the last
   one is skipped and gets the newest when it is done. */
lh_bool_t
lh_ui_web_host_ws_wants(const lh_ui_web_host_client_t *c, lh_uint_t gen);

/** Keep a copy of the frame just sent, to skip sending an equal one. */
lh_void
lh_ui_web_host_ws_remember(lh_ui_web_host_stream_t *stream, const lh_str_ptr body, lh_usize_t len);

/** Whether @p body equals the last frame sent. */
lh_bool_t
lh_ui_web_host_ws_same(const lh_ui_web_host_stream_t *stream, const lh_str_ptr body,
                       lh_usize_t len);

/** Draw once for every client that wants a frame and send it. Covers go once per
   connection: a client that missed a frame, or is new, resets the set so this
   one carries them all. A frame equal to the last one sent is not sent again. */
lh_void
lh_ui_web_host_ws_frame(lh_ui_web_host_client_t *cli, lh_ui_web_host_stream_t *stream,
                        lh_ui_view_t *view, lh_uint_t width, lh_uint_t height, lh_str_ptr json,
                        lh_usize_t json_cap, lh_uint_t gen);

/** Accept and serve every connection waiting on the listener. */
lh_void
lh_ui_web_host_accept_all(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                          lh_ui_view_t *view, lh_uint_t *width, lh_uint_t *height, lh_str_ptr json,
                          lh_usize_t json_cap, const lh_ui_web_host_hooks_t *hooks, lh_uint_t *gen,
                          lh_uint_t now);

/** Sleep until a connection, a client's input or room for its queued output --
   or @p timeout_ms, whichever is first. This is what answers an input in the
   time it takes to draw it, rather than in up to one tick. A wait that fails
   (a client socket gone bad under it) waits on the listener alone, so the loop
   still sleeps instead of spinning. */
lh_void
lh_ui_web_host_wait(lh_os_net_socket_t *listen_fd, lh_ui_web_host_client_t *cli,
                    lh_uint_t timeout_ms);

/** The hooks a run works with, into @p out: a copy of @p hooks (none when ::lh_null), whose
   controller is @p view_controller -- set up on @p view here -- when the app gave neither
   an input hook nor a controller. */
lh_void
lh_ui_web_host_route_hooks(const lh_ui_web_host_hooks_t *hooks, lh_ui_view_t *view,
                           lh_os_controller_t *view_controller, lh_ui_web_host_hooks_t *out);

/**
 * @brief Listen on 127.0.0.1:@p port and serve @p view. Does not return on success.
 *
 * @p json is the caller's scratch for one frame; it is overwritten each GET
 * `/ui.json`. @p width and @p height are the canvas the player should match.
 * @p hooks may be ::lh_null. When @p hooks->tick is set, it runs about every
 * 20 ms, and also between requests, so a session poll is not starved by HTTP.
 *
 * @return ::lh_bool_false when the socket would not listen (or @p view is null).
 */
lh_bool_t
lh_ui_web_host_run(lh_ui_view_t *view, lh_uint_t width, lh_uint_t height, lh_str_ptr json,
                   lh_usize_t json_cap, lh_net_port_t port, const lh_ui_web_host_hooks_t *hooks);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_H */
