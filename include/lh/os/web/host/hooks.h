/**
 * @file hooks.h
 * @brief ::lh_os_web_host_hooks_t — what a window would call: the application, not the JSON player.
 */

#ifndef LH_OS_WEB_HOST_HOOKS_H
#define LH_OS_WEB_HOST_HOOKS_H

#include <lh/config.h>
#include <lh/core/input/recognizer.h>
#include <lh/os/web/host/stream.h>

/**
 * @brief What a window would call: the application, not the JSON player.
 *
 * Any function may be ::lh_null. Every input line of the player arrives as one
 * ::lh_input_event_t, the same event a desktop window gets, and goes down the
 * same chain (::lh_os_window_set_on_input, ::lh_os_window_set_recognizer):
 *
 * - @p input is the sink at the end of the chain;
 * - @p recognizer, when set, stands in front of it: every event goes through
 *   ::lh_input_recognizer_feed, which hands @p input the event and the gestures
 *   it completes, and the host calls ::lh_input_recognizer_update at every
 *   tick, before @p tick;
 * - with neither, the event is the application's to route: the gl player, for
 *   one, then puts a recognizer of its own in front of the view it serves
 *   (`lh_gl_view_sink`), so the view gets every event as `lh_gl_view_input`
 *   would have.
 *
 * @p input alone gets the raw events only, and a hook that wants the view to see
 * them passes them on itself. The player has no focus to lose, so the host never
 * cancels a recognizer.
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
struct lh_os_web_host_hooks
{
    lh_ptr ctx; /**< Passed to every hook. */
    /** About every 20 ms, and between requests. */
    lh_void (*tick)(lh_ptr ctx);
    /** One input event from the player; its `self` is ::ctx. */
    lh_input_sink_cb input;
    /** Layout-mode tab size; ::lh_null keeps the host's initial width/height. */
    lh_void (*resize)(lh_sint_t width, lh_sint_t height, lh_ptr ctx);
    /** Stands in front of ::input and is updated every tick; the caller's, and must
        outlive the run. ::lh_null with no ::input: the player's own, in front of its
        view. */
    lh_input_recognizer_t *recognizer;
};
/** @copybrief lh_os_web_host_hooks */
typedef struct lh_os_web_host_hooks lh_os_web_host_hooks_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The value passed to the hooks (level 1).
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_web_host_hooks_get_ctx(const lh_os_web_host_hooks_t *self);

/**
 * @brief Store the value passed to the hooks (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 * @param ctx The new value.
 */
lh_void
lh_os_web_host_hooks_set_ctx(lh_os_web_host_hooks_t *self, lh_ptr ctx);

/**
 * @brief The callback the input events go to (level 1).
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 */
lh_input_sink_cb
lh_os_web_host_hooks_get_input_fn(const lh_os_web_host_hooks_t *self);

/**
 * @brief Store the callback the input events go to (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 * @param input The new value.
 */
lh_void
lh_os_web_host_hooks_set_input_fn(lh_os_web_host_hooks_t *self, lh_input_sink_cb input);

/**
 * @brief The callback that runs about every 20 ms (level 1).
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 */
lh_void (*lh_os_web_host_hooks_get_tick_fn(const lh_os_web_host_hooks_t *self))(lh_ptr ctx);

/**
 * @brief Store the callback that runs about every 20 ms (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self    The hooks; checked by ::lh_assert_runtime_ref.
 * @param tick_fn The new value.
 */
lh_void
lh_os_web_host_hooks_set_tick_fn(lh_os_web_host_hooks_t *self, lh_void (*tick_fn)(lh_ptr ctx));

/**
 * @brief The callback the page's size goes to (level 1).
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 */
lh_void (*lh_os_web_host_hooks_get_resize_fn(const lh_os_web_host_hooks_t *self))(lh_sint_t width,
                                                                                  lh_sint_t height,
                                                                                  lh_ptr ctx);

/**
 * @brief Store the callback the page's size goes to (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self      The hooks; checked by ::lh_assert_runtime_ref.
 * @param resize_fn The new value.
 */
lh_void
lh_os_web_host_hooks_set_resize_fn(lh_os_web_host_hooks_t *self,
                                   lh_void (*resize_fn)(lh_sint_t width, lh_sint_t height,
                                                        lh_ptr ctx));

/**
 * @brief The recognizer in front of the input hook (level 1).
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 */
lh_input_recognizer_t *
lh_os_web_host_hooks_get_recognizer(const lh_os_web_host_hooks_t *self);

/**
 * @brief Store the recognizer in front of the input hook (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The hooks; checked by ::lh_assert_runtime_ref.
 * @param recognizer The new value.
 */
lh_void
lh_os_web_host_hooks_set_recognizer(lh_os_web_host_hooks_t *self,
                                    lh_input_recognizer_t *recognizer);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WEB_HOST_HOOKS_H */
