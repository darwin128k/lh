/**
 * @file hooks.h
 * @brief ::lh_ui_web_host_hooks_t — what a window would call: the application, not the JSON player.
 */

#ifndef LH_UI_WEB_HOST_HOOKS_H
#define LH_UI_WEB_HOST_HOOKS_H

#include <lh/config.h>
#include <lh/ui/web/host/stream.h>

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

#endif /* LH_UI_WEB_HOST_HOOKS_H */
