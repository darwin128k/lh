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
#include <lh/ui/view.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_OS"
#endif

/**
 * @brief What a window would call: the application, not the JSON player.
 *
 * Any function may be ::lh_null. A missing press/release falls back to
 * ::lh_ui_view_press / ::lh_ui_view_release on the served view.
 */
struct lh_ui_web_host_hooks
{
    lh_ptr ctx; /**< Passed to every hook. */
    /** About every 20 ms, and between requests. */
    lh_void (*tick)(lh_ptr ctx);
    /** Pointer down at view pixel (@p x, @p y). */
    lh_void (*press)(int x, int y, lh_ptr ctx);
    /** Pointer up at view pixel (@p x, @p y). */
    lh_void (*release)(int x, int y, lh_ptr ctx);
    /** One wheel notch at (@p x, @p y): @p dx, @p dy are -1, 0 or 1 (1 is left / up). */
    lh_void (*wheel)(int x, int y, int dx, int dy, lh_ptr ctx);
    /** Key @p key went down (@p pressed) or up. */
    lh_void (*key)(lh_key_t key, lh_bool_t pressed, lh_ptr ctx);
    /** One typed character, as a Unicode code point. */
    lh_void (*text)(lh_u32_t code, lh_ptr ctx);
    /** Layout-mode tab size; ::lh_null keeps the host's initial width/height. */
    lh_void (*resize)(int width, int height, lh_ptr ctx);
};
/** @copybrief lh_ui_web_host_hooks */
typedef struct lh_ui_web_host_hooks lh_ui_web_host_hooks_t;

LH_COMPILER_EXTERN_C_BEGIN

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
lh_ui_web_host_run(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
                   lh_net_port_t port, const lh_ui_web_host_hooks_t *hooks);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_H */
