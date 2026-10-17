/**
 * @file controller.h
 * @brief Between an input source and the object its events are for:
 *        ::lh_os_controller_t.
 *
 * A source (a window backend, the web host, a queue, a microcontroller driver)
 * hands its ::lh_os_input_event_t to the controller, which runs the gesture
 * recognizer (::lh_os_input_recognizer_t) and gives the raw events and the
 * gestures to one handler, `handle_cb`, as functions of
 * ::lh_os_controller_handlers_handle_fn. For a screen the handler is the view's
 * (`lh_ui_view_controller_handle` in `lh/ui/view/controller.h`), which calls
 * `lh_ui_view_input`; the controller itself knows nothing of the UI.
 *
 * No vtable: three plain handler pointers (`handle_cb`, `update_cb`,
 * `cancel_cb`) and the context every one of them gets as `self`, as
 * ::lh_memory_allocator_t holds its callbacks.
 *
 * | Call                        | What reaches the handlers                          |
 * |-----------------------------|----------------------------------------------------|
 * | ::lh_os_controller_feed     | the event, then its gestures (with the gestures    |
 * |                             | flag), or just the event (without it)              |
 * | ::lh_os_controller_handle   | the event, past the recognizer                     |
 * | ::lh_os_controller_update   | the gestures due by `now` (while enabled), then    |
 * |                             | `update_cb`                                        |
 * | ::lh_os_controller_cancel   | `cancel_cb`, once the recognizer forgot everything |
 * | ::lh_os_controller_sink     | what ::lh_os_controller_feed sends                 |
 *
 * A window (::lh_os_window_set_controller) and the web host (the `controller` of
 * `lh_ui_web_host_hooks_t`) drive a controller themselves: they feed it, cancel
 * it when the focus goes and call ::lh_os_controller_update on their own tick.
 * ::lh_os_controller_sink is for a source that has only a sink, such as
 * ::lh_os_input_queue_drain.
 *
 * Without ::lh_os_controller_flags_enabled nothing reaches `handle_cb`, and
 * neither ::lh_os_controller_feed nor ::lh_os_controller_update moves the
 * recognizer: a muted controller stays where the last enabled call left it.
 * `update_cb` still runs. Needs no ::LH_LIBRARY_OPTION_OS: time is the events'
 * `time` and the `now` given to ::lh_os_controller_update.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_controller_t controller;
 *
 * lh_os_controller_init(lh_addr_of(controller), lh_ui_view_controller_handle, // the view's
 *                       lh_addr_of(view), lh_null);                           // default config
 * lh_os_controller_set_cancel_cb(lh_addr_of(controller), lh_ui_view_controller_cancel);
 * lh_os_window_set_controller(lh_addr_of(window), lh_addr_of(controller)); // fed, ticked
 * @endcode
 */

#ifndef LH_OS_CONTROLLER_H
#define LH_OS_CONTROLLER_H

#include <lh/compiler/extern/c.h>
#include <lh/os/controller/fields.h>
#include <lh/os/controller/flags.h>
#include <lh/os/controller/flags/value.h>
#include <lh/os/controller/handlers/cancel/cb.h>
#include <lh/os/controller/handlers/handle/cb.h>
#include <lh/os/controller/handlers/update/cb.h>
#include <lh/os/controller/ptr.h>
#include <lh/os/input/recognizer.h>

/**
 * @struct lh_os_controller
 * @brief The handlers, their context, the recognizer and the flags.
 *
 * - `handle_cb` takes every event (::lh_os_controller_handlers_handle_fn);
 * - `update_cb` is called on each ::lh_os_controller_update (optional);
 * - `cancel_cb` is called on each ::lh_os_controller_cancel (optional);
 * - `context` is passed to all three as `self`;
 * - `recognizer` makes the gestures;
 * - `flags` are ::lh_os_controller_flags_value_t bits.
 *
 * The concrete fields are expanded by ::lh_os_controller_fields.
 */
struct lh_os_controller
{
    /** Members: ::lh_os_controller_fields. */
    lh_os_controller_fields(lh_os_controller_handlers_handle_fn,
                            lh_os_controller_handlers_update_fn,
                            lh_os_controller_handlers_cancel_fn, lh_self_ptr);
};
/** @copybrief lh_os_controller */
typedef struct lh_os_controller lh_os_controller_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- Setting up ------------------------------------------------------------ */

/**
 * @brief Make @p self give its events to @p handle_cb on @p context, with a
 *        recognizer using a copy of @p config (::lh_null for the defaults).
 *
 * No update or cancel handler; ::lh_os_controller_flags_enabled and
 * ::lh_os_controller_flags_gestures are set.
 * ::lh_os_controller_default_initializer is the same state for a static.
 */
lh_void
lh_os_controller_init(lh_os_controller_t *self, lh_os_controller_handlers_handle_cb handle_cb,
                      lh_self_ptr context, const lh_os_input_recognizer_config_ptr config);

/** @brief Set the event handler (::lh_null for none: every event is dropped). */
lh_void
lh_os_controller_set_handle_cb(lh_os_controller_t *self,
                               lh_os_controller_handlers_handle_cb handle_cb);

/** @brief The event handler, or ::lh_null. */
lh_os_controller_handlers_handle_cb
lh_os_controller_get_handle_cb(const lh_os_controller_ptr self);

/** @brief Set the timer handler (::lh_null for none). */
lh_void
lh_os_controller_set_update_cb(lh_os_controller_t *self,
                               lh_os_controller_handlers_update_cb update_cb);

/** @brief The timer handler, or ::lh_null. */
lh_os_controller_handlers_update_cb
lh_os_controller_get_update_cb(const lh_os_controller_ptr self);

/** @brief Set the cancel handler (::lh_null for none). */
lh_void
lh_os_controller_set_cancel_cb(lh_os_controller_t *self,
                               lh_os_controller_handlers_cancel_cb cancel_cb);

/** @brief The cancel handler, or ::lh_null. */
lh_os_controller_handlers_cancel_cb
lh_os_controller_get_cancel_cb(const lh_os_controller_ptr self);

/** @brief Set the object every handler gets as `self`. */
lh_void
lh_os_controller_set_context(lh_os_controller_t *self, lh_self_ptr context);

/** @brief The object every handler gets as `self`. */
lh_self_ptr
lh_os_controller_get_context(const lh_os_controller_ptr self);

/**
 * @brief The recognizer of @p self, to read its phase or deadline
 *        (::lh_os_input_recognizer_get_next_deadline).
 */
lh_os_input_recognizer_ptr
lh_os_controller_get_recognizer(lh_os_controller_t *self);

/* -- Flags ----------------------------------------------------------------- */

/** @brief Replace every flag of @p self with @p flags. */
lh_void
lh_os_controller_set_flags(lh_os_controller_t *self, lh_os_controller_flags_t flags);

/** @brief The flags of @p self. */
lh_os_controller_flags_t
lh_os_controller_get_flags(const lh_os_controller_ptr self);

/** @brief True when every bit of @p flag is set in @p self. */
lh_bool_t
lh_os_controller_has_flag(const lh_os_controller_ptr self, lh_os_controller_flags_t flag);

/** @brief True when `handle_cb` receives events (::lh_os_controller_flags_enabled). */
lh_bool_t
lh_os_controller_is_enabled(const lh_os_controller_ptr self);

/** @brief Set or clear ::lh_os_controller_flags_enabled, keeping the other flags. */
lh_void
lh_os_controller_set_enabled(lh_os_controller_t *self, lh_bool_t enabled);

/* -- Events ---------------------------------------------------------------- */

/**
 * @brief Give @p event straight to `handle_cb`, past the recognizer.
 *
 * @return What `handle_cb` returned; false when @p self is not enabled or has
 *         no `handle_cb`.
 */
lh_bool_t
lh_os_controller_handle(lh_os_controller_t *self, const lh_os_input_event_ptr event);

/**
 * @brief One event from the source.
 *
 * With ::lh_os_controller_flags_gestures it goes through the recognizer
 * (::lh_os_input_recognizer_feed): `handle_cb` gets the raw event, then the
 * gestures it completes, marked ::LH_OS_INPUT_FLAG_SYNTHESIZED. Without it the
 * event goes to `handle_cb` alone. Without ::lh_os_controller_flags_enabled
 * nothing happens, not even in the recognizer.
 *
 * @return True when `handle_cb` acted on any of the events this call sent.
 */
lh_bool_t
lh_os_controller_feed(lh_os_controller_t *self, const lh_os_input_event_ptr event);

/**
 * @brief The recognizer's timers at @p now (a long press, say), with
 *        ::lh_os_controller_flags_gestures, then `update_cb`, if any.
 *
 * Call it from a timer on the same ::lh_tick_t clock as the events' `time`.
 * Without ::lh_os_controller_flags_enabled the recognizer is left as it is,
 * the same as ::lh_os_controller_feed; `update_cb` is still called. The next
 * enabled ::lh_os_controller_update, or a ::lh_os_controller_feed whose event
 * is already past a deadline, sends what came due in between.
 *
 * @return True when `handle_cb` acted on a gesture this call sent.
 */
lh_bool_t
lh_os_controller_update(lh_os_controller_t *self, lh_tick_t now);

/**
 * @brief Drop every gesture in progress, then call `cancel_cb`, if any.
 *
 * The recognizer starts over with the thresholds it had and sends nothing:
 * no pointer cancel, no drag end, no click series left to continue. It is
 * `cancel_cb` that tells the handler to let go of what it holds.
 */
lh_void
lh_os_controller_cancel(lh_os_controller_t *self);

/**
 * @brief An ::lh_os_input_sink_fn that feeds the controller @p self
 *        (an ::lh_os_controller_t) with @p event, as ::lh_os_controller_feed.
 *
 * Plugs a controller into a source that has only a sink, such as
 * ::lh_os_input_queue_drain. A window takes its controller directly
 * (::lh_os_window_set_controller), which also cancels and ticks it; plugging the
 * same controller into its ::lh_os_window_set_on_input as well feeds it twice.
 */
lh_void
lh_os_controller_sink(lh_self_ptr self, const lh_os_input_event_ptr event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_CONTROLLER_H */
