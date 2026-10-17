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
    lh_void (*resize)(int width, int height, lh_ptr ctx);
    /** Takes every input event after ::input and is updated every tick; the caller's,
        and must outlive the run. ::lh_null with no ::input: the host's own view
        controller. */
    lh_os_controller_t *controller;
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
lh_ui_web_host_run(lh_ui_view_t *view, unsigned width, unsigned height, char *json,
                   lh_usize_t json_cap, lh_net_port_t port, const lh_ui_web_host_hooks_t *hooks);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_H */
