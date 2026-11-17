/**
 * @file window.h
 * @brief One window — ::lh_os_window_t.
 *
 * Top-level windows are opened into an ::lh_os_app_t from outside; the first
 * linked window is main (index 0). A window may own modal children. Set
 * ::lh_os_window_set_on_close to learn when the window dies — including
 * when the user closes it (reason ::lh_os_window_close_reason_os).
 *
 * The message pump lives on ::lh_os_app_run, not on the window.
 *
 * A caller can style the window instead of taking the OS frame, and none of it is
 * hardcoded here: ::lh_os_window_set_frame_v says whose frame it is,
 * ::lh_os_window_set_corner_radius cuts its corners, and
 * ::lh_os_window_set_on_zone answers point by point what the window system should do
 * — move it, resize it, or leave the press to the app. What that looks like is the
 * caller's, so a project draws and hit-tests its own title bar and its own buttons.
 *
 * Input has two levels. The standard one is a controller
 * (::lh_os_window_set_controller): the window feeds it every event, cancels it when
 * the focus goes and runs its gesture timers, so a view behind
 * `lh_gl_view_controller_handle_event_is_handled` needs nothing else wired. The low level is the
 * sink (::lh_os_window_set_on_input), which sees every raw event first; with no
 * controller it is the only consumer.
 *
 * **The window is the application's; what is below it is a contract**
 * (::lh_os_window_backend_t). The window calls the table it took from its
 * application when it was opened — Win32, X11 and AppKit through the system
 * backend, a board's panel, a test's fake — and the backend calls the window back
 * through the `on_native_*` up-calls. The window knows nothing of which one it
 * got, and the same code runs with an operating system and without one. What the
 * backend keeps for a window is one opaque pointer (::lh_os_window_get_native).
 */

#ifndef LH_OS_WINDOW_H
#define LH_OS_WINDOW_H

#include <lh/config.h>
#include <lh/os/controller.h>
#include <lh/os/window/backend.h>
#include <lh/os/window/fields.h>

struct lh_os_app;

/**
 * @struct lh_os_window
 * @brief One window: app or parent, modal children, the reactions, and the place.
 */
struct lh_os_window
{
    /** Members: ::lh_os_window_fields. */
    lh_os_window_fields(struct lh_os_app, struct lh_os_window, struct lh_os_controller,
                        lh_os_window_backend_t);
};
/** @copybrief lh_os_window */
typedef struct lh_os_window lh_os_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Owning app when top-level, or ::lh_null.
 */
struct lh_os_app *
lh_os_window_get_app(const lh_os_window_t *self);

/**
 * @brief Parent when a modal child, or ::lh_null.
 */
lh_os_window_t *
lh_os_window_get_parent(const lh_os_window_t *self);

/**
 * @brief Make @p controller the consumer of @p self's input; ::lh_null for none.
 *
 * The standard input path of a window, and all of it in one call:
 *
 * - every event ::lh_os_window_on_native_input gets goes to
 *   ::lh_os_controller_feed, after the sink of ::lh_os_window_set_on_input, if any;
 * - a focus loss (::lh_os_input_kind_focus_out) is fed and then
 *   ::lh_os_controller_cancel'led, so every press and gesture is let go;
 * - every tick of the window calls ::lh_os_controller_update at ::lh_os_tick_ms,
 *   before `on_tick`. Without an `on_tick` period (::lh_os_window_set_on_tick) the
 *   window arms its timer by itself, for the recognizer's next deadline
 *   (::lh_os_input_recognizer_get_next_deadline) and for that only, so a long
 *   press arrives on time and an idle window has no timer. With a period, the
 *   update rides on it: a deadline is then late by up to one period.
 *
 * The window does not repaint for the controller: its handler, or `on_tick`, asks
 * for the paint (`lh_gl_view_take_damage`, ::lh_os_window_invalidate_rect), as a
 * sink did. Do not call ::lh_os_controller_update for this controller from
 * `on_tick` too: the window already has. The controller is the caller's and must
 * outlive the window, or be cleared first. A focus that leaves while the
 * controller is replaced is not seen by the new one.
 *
 * The OS layer knows nothing of the UI: a view becomes the handler through
 * `lh_gl_view_controller_handle_event_is_handled` (`lh/gl/view/controller.h`).
 *
 * Example usage:
 * @code{.c}
 * lh_os_controller_init(lh_addr_of(controller), lh_gl_view_controller_handle_event_is_handled, //
 * the view lh_addr_of(view), lh_null);                           // defaults
 * lh_os_controller_set_cancel_cb(lh_addr_of(controller), lh_gl_view_controller_cancel);
 * lh_os_window_set_controller(lh_addr_of(window), lh_addr_of(controller));
 * @endcode
 */
lh_void
lh_os_window_set_controller(lh_os_window_t *self, lh_os_controller_t *controller);

/** @brief The controller @p self feeds (::lh_os_window_set_controller), or ::lh_null. */
lh_os_controller_t *
lh_os_window_get_controller(const lh_os_window_t *self);

/**
 * @brief True while @p self is maximized.
 */
lh_bool_t
lh_os_window_is_maximized(const lh_os_window_t *self);

/**
 * @brief Where @p self opens, or ::lh_os_window_placement_default.
 */
lh_os_window_placement_t
lh_os_window_get_placement(const lh_os_window_t *self);

/**
 * @brief The mouse buttons held over @p self as the last event of pointer `0`
 *        left them (::lh_os_input_button_to_mask); `0` after its cancel or a focus
 *        loss. Touch and pen contacts (pointer `1` and up) leave it alone.
 */
lh_u32_t
lh_os_window_get_pressed_buttons(const lh_os_window_t *self);

/**
 * @brief What the backend paints on in the current ::lh_os_window_on_paint_fn, or
 *        ::lh_null outside a paint cycle (level 1).
 *
 * The backend's own: an `HDC` from `BeginPaint` on Win32, the backend's paint
 * context on X11 and AppKit, a pixmap on a board.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_paint_target(const lh_os_window_t *self);

/**
 * @brief The node that links @p self into its application's window list (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
const lh_list_node_t *
lh_os_window_get_link(const lh_os_window_t *self);

/**
 * @brief The node that links @p self into its application's window list, to change
 *        in place (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_node_t *
lh_os_window_get_link_mut(lh_os_window_t *self);

/**
 * @brief Store the application the window belongs to (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param app The new value.
 */
lh_void
lh_os_window_set_app(lh_os_window_t *self, struct lh_os_app *app);

/**
 * @brief Store the window this one is a child of; ::lh_null for a top-level window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param parent The new value.
 */
lh_void
lh_os_window_set_parent(lh_os_window_t *self, struct lh_os_window *parent);

/**
 * @brief The list of child windows (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
const lh_list_t *
lh_os_window_get_children(const lh_os_window_t *self);

/**
 * @brief The list of child windows, to change in place (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_list_t *
lh_os_window_get_children_mut(lh_os_window_t *self);

/**
 * @brief Whether the window is modal to its parent (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_window_is_modal(const lh_os_window_t *self);

/**
 * @brief Store whether the window is modal to its parent (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param modal The new value.
 */
lh_void
lh_os_window_set_modal(lh_os_window_t *self, lh_bool_t modal);

/**
 * @brief Whether the window is being closed (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_window_is_closing(const lh_os_window_t *self);

/**
 * @brief Store whether the window is being closed (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param closing The new value.
 */
lh_void
lh_os_window_set_closing(lh_os_window_t *self, lh_bool_t closing);

/**
 * @brief The left edge of the window's place (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_x(const lh_os_window_t *self);

/**
 * @brief Store the left edge of the window's place (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param x The new value.
 */
lh_void
lh_os_window_set_x(lh_os_window_t *self, int x);

/**
 * @brief The top edge of the window's place (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_y(const lh_os_window_t *self);

/**
 * @brief Store the top edge of the window's place (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param y The new value.
 */
lh_void
lh_os_window_set_y(lh_os_window_t *self, int y);

/**
 * @brief The width of the window's place (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_width(const lh_os_window_t *self);

/**
 * @brief Store the width of the window's place (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_os_window_set_width(lh_os_window_t *self, int width);

/**
 * @brief The height of the window's place (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_height(const lh_os_window_t *self);

/**
 * @brief Store the height of the window's place (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_os_window_set_height(lh_os_window_t *self, int height);

/**
 * @brief The callback told when the window changes size (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_window_on_resize_cb
lh_os_window_get_on_resize_fn(const lh_os_window_t *self);

/**
 * @brief Store the callback told when the window changes size (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_resize The new value.
 */
lh_void
lh_os_window_set_on_resize_fn(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize);

/**
 * @brief The value passed to the resize callback (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_on_resize_context(const lh_os_window_t *self);

/**
 * @brief Store the value passed to the resize callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_resize_context The new value.
 */
lh_void
lh_os_window_set_on_resize_context(lh_os_window_t *self, lh_ptr on_resize_context);

/**
 * @brief The callback told when the window is to be drawn (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_window_on_paint_cb
lh_os_window_get_on_paint_fn(const lh_os_window_t *self);

/**
 * @brief Store the callback told when the window is to be drawn (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_paint The new value.
 */
lh_void
lh_os_window_set_on_paint_fn(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint);

/**
 * @brief The value passed to the paint callback (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_on_paint_context(const lh_os_window_t *self);

/**
 * @brief Store the value passed to the paint callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_paint_context The new value.
 */
lh_void
lh_os_window_set_on_paint_context(lh_os_window_t *self, lh_ptr on_paint_context);

/**
 * @brief The callback that takes every raw input event (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_input_sink_cb
lh_os_window_get_on_input_fn(const lh_os_window_t *self);

/**
 * @brief Store the callback that takes every raw input event (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_input The new value.
 */
lh_void
lh_os_window_set_on_input_fn(lh_os_window_t *self, lh_os_input_sink_cb on_input);

/**
 * @brief The value passed to the input callback (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_on_input_context(const lh_os_window_t *self);

/**
 * @brief Store the value passed to the input callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_input_context The new value.
 */
lh_void
lh_os_window_set_on_input_context(lh_os_window_t *self, lh_ptr on_input_context);

/**
 * @brief The callback told when the window dies (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_window_on_close_cb
lh_os_window_get_on_close_fn(const lh_os_window_t *self);

/**
 * @brief Store the callback told when the window dies (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_close The new value.
 */
lh_void
lh_os_window_set_on_close_fn(lh_os_window_t *self, lh_os_window_on_close_cb on_close);

/**
 * @brief The value passed to the close callback (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_on_close_context(const lh_os_window_t *self);

/**
 * @brief Store the value passed to the close callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_close_context The new value.
 */
lh_void
lh_os_window_set_on_close_context(lh_os_window_t *self, lh_ptr on_close_context);

/**
 * @brief The callback told on the window's timer (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_window_on_tick_cb
lh_os_window_get_on_tick_fn(const lh_os_window_t *self);

/**
 * @brief Store the callback told on the window's timer (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_tick The new value.
 */
lh_void
lh_os_window_set_on_tick_fn(lh_os_window_t *self, lh_os_window_on_tick_cb on_tick);

/**
 * @brief The value passed to the tick callback (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_on_tick_context(const lh_os_window_t *self);

/**
 * @brief Store the value passed to the tick callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_tick_context The new value.
 */
lh_void
lh_os_window_set_on_tick_context(lh_os_window_t *self, lh_ptr on_tick_context);

/**
 * @brief The period the window's timer runs at (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The period in milliseconds; `0` for no timer.
 */
lh_u32_t
lh_os_window_get_tick_ms(const lh_os_window_t *self);

/**
 * @brief Store the period the window's timer runs at (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param tick_ms The new value.
 */
lh_void
lh_os_window_set_tick_ms(lh_os_window_t *self, lh_u32_t tick_ms);

/**
 * @brief Whether the window's timer runs for the controller alone (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_window_is_controller_armed(const lh_os_window_t *self);

/**
 * @brief Store whether the window's timer runs for the controller alone (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param controller_armed The new value.
 */
lh_void
lh_os_window_set_controller_armed(lh_os_window_t *self, lh_bool_t controller_armed);

/**
 * @brief The backend table @p self calls down through, or ::lh_null before it is
 *        opened (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
const lh_os_window_backend_t *
lh_os_window_get_backend(const lh_os_window_t *self);

/**
 * @brief Store the backend table @p self calls down through (level 1).
 *
 * Set by ::lh_os_window_open and ::lh_os_window_open_modal from the application or
 * the parent; a level-1 setter, not validated.
 *
 * @param self    The window; checked by ::lh_assert_runtime_ref.
 * @param backend The table; the caller's, and it must outlive the window.
 */
lh_void
lh_os_window_set_backend(lh_os_window_t *self, const lh_os_window_backend_t *backend);

/**
 * @brief The context passed to every call of the backend (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_backend_context(const lh_os_window_t *self);

/**
 * @brief Store the context passed to every call of the backend (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param backend_context The new value.
 */
lh_void
lh_os_window_set_backend_context(lh_os_window_t *self, lh_ptr backend_context);

/**
 * @brief The backend's own state for @p self; ::lh_null while it is not open (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_native(const lh_os_window_t *self);

/**
 * @brief Store the backend's own state for @p self; ::lh_null while it is not open (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param native The new value.
 */
lh_void
lh_os_window_set_native(lh_os_window_t *self, lh_ptr native);

/**
 * @brief Store what the backend paints on in the current paint (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self         The window; checked by ::lh_assert_runtime_ref.
 * @param paint_target The new value.
 */
lh_void
lh_os_window_set_paint_target(lh_os_window_t *self, lh_ptr paint_target);

/**
 * @brief The left edge of the rectangle the current paint asks for (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_paint_left(const lh_os_window_t *self);

/**
 * @brief Store the left edge of the rectangle the current paint asks for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param paint_left The new value.
 */
lh_void
lh_os_window_set_paint_left(lh_os_window_t *self, int paint_left);

/**
 * @brief The top edge of the rectangle the current paint asks for (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_paint_top(const lh_os_window_t *self);

/**
 * @brief Store the top edge of the rectangle the current paint asks for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param paint_top The new value.
 */
lh_void
lh_os_window_set_paint_top(lh_os_window_t *self, int paint_top);

/**
 * @brief The right edge (exclusive) of the rectangle the current paint asks for (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_paint_right(const lh_os_window_t *self);

/**
 * @brief Store the right edge (exclusive) of the rectangle the current paint asks for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param paint_right The new value.
 */
lh_void
lh_os_window_set_paint_right(lh_os_window_t *self, int paint_right);

/**
 * @brief The bottom edge (exclusive) of the rectangle the current paint asks for (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_paint_bottom(const lh_os_window_t *self);

/**
 * @brief Store the bottom edge (exclusive) of the rectangle the current paint asks for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param paint_bottom The new value.
 */
lh_void
lh_os_window_set_paint_bottom(lh_os_window_t *self, int paint_bottom);

/**
 * @brief Whose frame the window has, read by the backend in `open` (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_window_frame_t
lh_os_window_get_frame(const lh_os_window_t *self);

/**
 * @brief Store whose frame the window has, read by the backend in `open` (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param frame The new value.
 */
lh_void
lh_os_window_set_frame(lh_os_window_t *self, lh_os_window_frame_t frame);

/**
 * @brief The radius the corners are cut to; `0` is square (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_window_get_corner(const lh_os_window_t *self);

/**
 * @brief Store the radius the corners are cut to; `0` is square (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param corner The new value.
 */
lh_void
lh_os_window_set_corner(lh_os_window_t *self, int corner);

/**
 * @brief Store where the window opens (level 1).
 *
 * A level-1 setter: the value is stored as given. ::lh_os_window_set_placement_v
 * checks that the window is not open.
 *
 * @param self      The window; checked by ::lh_assert_runtime_ref.
 * @param placement The new value.
 */
lh_void
lh_os_window_set_placement(lh_os_window_t *self, lh_os_window_placement_t placement);

/**
 * @brief Store whether the window is maximized, as the backend reported (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self      The window; checked by ::lh_assert_runtime_ref.
 * @param maximized The new value.
 */
lh_void
lh_os_window_set_maximized(lh_os_window_t *self, lh_bool_t maximized);

/**
 * @brief The callback that answers the hit test (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_window_on_zone_cb
lh_os_window_get_on_zone_fn(const lh_os_window_t *self);

/**
 * @brief Store the callback that answers the hit test (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self    The window; checked by ::lh_assert_runtime_ref.
 * @param on_zone The new value.
 */
lh_void
lh_os_window_set_on_zone_fn(lh_os_window_t *self, lh_os_window_on_zone_cb on_zone);

/**
 * @brief The value passed to the hit-test callback (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_window_get_on_zone_context(const lh_os_window_t *self);

/**
 * @brief Store the value passed to the hit-test callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param on_zone_context The new value.
 */
lh_void
lh_os_window_set_on_zone_context(lh_os_window_t *self, lh_ptr on_zone_context);

/**
 * @brief Store the mouse buttons held over the window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self            The window; checked by ::lh_assert_runtime_ref.
 * @param pressed_buttons The new value.
 */
lh_void
lh_os_window_set_pressed_buttons(lh_os_window_t *self, lh_u32_t pressed_buttons);

/**
 * @brief Take @p self out of its app's window list and forget its app, parent and modal state.
 */
lh_void
lh_os_window_unlink(lh_os_window_t *self);

/**
 * @brief Set @p self to the empty (not-yet-open) state.
 *
 * No backend, the system frame, square corners, the default placement. Calls
 * nothing below.
 */
lh_void
lh_os_window_init(lh_os_window_t *self);

/**
 * @brief First modal child, or ::lh_null.
 */
lh_os_window_t *
lh_os_window_get_first_child(const lh_os_window_t *self);

/**
 * @brief Modal child after @p child, or ::lh_null.
 */
lh_os_window_t *
lh_os_window_get_next_child(const lh_os_window_t *self, const lh_os_window_t *child);

/**
 * @brief Notify @p on_close when the window dies.
 *
 * @p context is passed through. ::lh_null clears the slot. Fires for both
 * ::lh_os_window_close_reason_api and ::lh_os_window_close_reason_os.
 */
lh_void
lh_os_window_set_on_close(lh_os_window_t *self, lh_os_window_on_close_cb on_close, lh_ptr context);

/**
 * @brief Notify @p on_paint when the window is to be drawn.
 *
 * @p context is passed through. ::lh_null clears the slot. While the callback
 * runs, ::lh_os_window_get_paint_target returns what the backend paints on and
 * ::lh_os_window_get_paint_rect the rectangle it asked for.
 */
lh_void
lh_os_window_set_on_paint(lh_os_window_t *self, lh_os_window_on_paint_cb on_paint, lh_ptr context);

/**
 * @brief Set what ::lh_os_window_set_on_resize tells @p self about its size.
 *
 * Without it a caller cannot tell a window apart from one whose size simply never
 * changes, which matters as soon as the window can be resized: a drag of one of the
 * zones named through ::lh_os_window_set_on_zone, or the window system's own
 * maximize, and neither announces itself through a paint.
 */
lh_void
lh_os_window_set_on_resize(lh_os_window_t *self, lh_os_window_on_resize_cb on_resize,
                           lh_ptr context);

/**
 * @brief Hand every input event of @p self to @p on_input, with @p context as
 *        its `self`.
 *
 * The low-level input slot of a window: pointer down, up, move and cancel for
 * every mouse button and every touch or pen contact, the wheel in 1/120 notch,
 * keys down (with repeats) and up, typed text, focus in and out — each one
 * ::lh_os_input_event_t with its modifiers and a time on the ::lh_os_tick_ms
 * clock, raw: no gestures. ::lh_null clears the slot.
 *
 * The sink sees each event before the controller of ::lh_os_window_set_controller
 * does, and the controller sees it whatever the sink did with it: the sink is a
 * tap (a log, a shortcut, an app that routes input itself), the controller the
 * consumer. Without a controller the sink is the only one. Do not also plug the
 * window's own controller in here through ::lh_os_controller_sink: it would get
 * every event twice.
 *
 * Example usage:
 * @code{.c}
 * static lh_void
 * on_input(lh_self_ptr self, const lh_os_input_event_ptr event)
 * {
 *     struct my_main *my = lh_ptr_rcast(struct my_main, self);
 *
 *     if (lh_gl_view_input(lh_addr_of(my->view), event))
 *     {
 *         lh_os_window_invalidate(lh_addr_of(my->window));
 *     }
 * }
 *
 * lh_os_window_set_on_input(lh_addr_of(window), on_input, lh_addr_of(self));
 * @endcode
 */
lh_void
lh_os_window_set_on_input(lh_os_window_t *self, lh_os_input_sink_cb on_input, lh_ptr context);

/**
 * @brief Where @p self is and how big it is, into @p x, @p y, @p width and @p height.
 *
 * The **place** of the window: the rectangle it occupies. With an operating system
 * the system owns the size and says so (::lh_os_window_on_native_resize); without
 * one the application says so itself (:lh_os_window_set_size). Either way the place
 * is here, and the application can always ask.
 */
lh_void
lh_os_window_get_rect(const lh_os_window_t *self, int *x, int *y, int *width, int *height);

/**
 * @brief How big @p self is, into @p width and @p height.
 */
lh_void
lh_os_window_get_size(const lh_os_window_t *self, int *width, int *height);

/**
 * @brief Make @p self @p width by @p height.
 *
 * The application's own door into the size. The change is told to the application
 * (:lh_os_window_set_on_resize), so a layout has one place to hear it from, whether
 * the size came from the system or from here.
 */
lh_void
lh_os_window_set_size(lh_os_window_t *self, int width, int height);

/** When the recognizer of @p self's controller next has something to send, into @p out;
   false, @p out untouched, with no controller, a disabled one, no gestures or nothing
   waiting on time. A disabled controller does not advance on update, so a deadline it
   cannot meet yet must not keep a 1 ms timer spinning. */
lh_bool_t
lh_os_window_get_controller_deadline(lh_os_window_t *self, lh_tick_t *out);

/**
 * @brief True while the backend holds a window for @p self
 *        (::lh_os_window_get_native is not ::lh_null).
 */
lh_bool_t
lh_os_window_is_open(const lh_os_window_t *self);

/**
 * @brief Name what the window system should do with each point of @p self.
 *
 * The one hook for styling a window's behaviour around its content, and the reason
 * none of it is hardcoded here: only the caller knows where its own chrome is. Answer
 * ::lh_os_window_zone_caption over a strip it drew itself and a press there moves the
 * window, with the OS's own move loop, its rules about the screen edges and the right
 * cursor; answer one of the edge and corner zones over a border and the OS resizes
 * the window; answer ::lh_os_window_zone_client — the default, and the answer when no
 * callback is set — and the press reaches the app as an ordinary one, so a button the
 * app drew on its own title bar keeps working.
 *
 * Set at any time: it is consulted per press and per pointer move, not read once.
 */
lh_void
lh_os_window_set_on_zone(lh_os_window_t *self, lh_os_window_on_zone_cb on_zone, lh_ptr context);

/**
 * @brief Client paint rectangle for the current paint cycle (right/bottom
 *        exclusive), or zeros outside a paint.
 */
lh_void
lh_os_window_get_paint_rect(const lh_os_window_t *self, int *left, int *top, int *right,
                            int *bottom);

/**
 * @brief Called from the backend inside a paint cycle: what it paints on and the
 *        rectangle it asks for.
 *
 * Holds @p target and the rectangle for ::lh_os_window_get_paint_target and
 * ::lh_os_window_get_paint_rect while the paint callback runs, and empties them
 * after.
 */
lh_void
lh_os_window_on_native_paint(lh_os_window_t *self, lh_ptr target, int left, int top, int right,
                             int bottom);

/**
 * @brief What the window system should do with the client point (@p x, @p y).
 *
 * The backend's way of asking ::lh_os_window_set_on_zone, so the answer is made in
 * one place whatever the system asks for. ::lh_os_window_zone_client when no callback
 * is set. @p x and @p y are client-area coordinates.
 */
lh_os_window_zone_t
lh_os_window_zone_at(lh_os_window_t *self, int x, int y);

/** The mouse buttons @p event leaves held over @p self: an event of pointer `0` says, its
   cancel or a focus loss lets go of all of them, anything else -- a touch contact
   included -- leaves them as they were. */
lh_void
lh_os_window_track_buttons(lh_os_window_t *self, const lh_os_input_event_ptr event);

/**
 * @brief Move @p self to @p x, @p y and make it @p width by @p height.
 *
 * The application's own door into the place: where there is a system the system
 * owns the size and moves the window itself, and this is for the case where there
 * is none. The size that comes with it tells the application about the change
 * (:lh_os_window_set_on_resize), so a layout has one place to hear it from.
 */
lh_void
lh_os_window_set_rect(lh_os_window_t *self, int x, int y, int width, int height);

/**
 * @brief Open @p self as a top-level window under @p app and show it.
 *
 * The window takes @p app's window backend (::lh_os_app_install_window_backend) and
 * calls its `open`. Links @p self at the end of @p app's list. The first window
 * opened is main (index 0).
 *
 * An application with no window backend, or with one that lacks a required slot,
 * is a checked failure (::lh_runtime_check_if): nothing is installed by default.
 *
 * @param app    The application; must not be null.
 * @param self   The window; not open and not linked.
 * @param title  The title; must not be null.
 * @param width  The client width asked for.
 * @param height The client height asked for.
 * @return True when the backend opened the window; false when it could not.
 */
lh_bool_t
lh_os_window_open(struct lh_os_app *app, lh_os_window_t *self, const lh_char_t *title, int width,
                  int height);

/**
 * @brief Open @p self as a modal child of @p parent and show it.
 *
 * The child takes @p parent's backend. Disables @p parent until @p self closes,
 * when the backend can (its `set_enabled` slot). Links into @p parent's children.
 *
 * @param parent The parent; open.
 * @param self   The window; not open, not linked and not @p parent.
 * @param title  The title; must not be null.
 * @param width  The client width asked for.
 * @param height The client height asked for.
 * @return True when the backend opened the window.
 */
lh_bool_t
lh_os_window_open_modal(lh_os_window_t *parent, lh_os_window_t *self, const lh_char_t *title,
                        int width, int height);

/**
 * @brief Ask the backend for a paint of the whole client of @p self.
 *
 * A no-op when the window is closed.
 */
lh_void
lh_os_window_invalidate(lh_os_window_t *self);

/**
 * @brief Ask the backend for a paint of a client rectangle of @p self
 *        (right/bottom exclusive).
 *
 * A no-op when the window is closed.
 */
lh_void
lh_os_window_invalidate_rect(lh_os_window_t *self, int left, int top, int right, int bottom);

/**
 * @brief Minimize, maximize or restore @p self, the way its own frame's buttons do.
 *
 * These are here because a window with a frame of its own has no system buttons to
 * press: `minimize` sends it to the taskbar in its minimized state, `set_maximized`
 * fills the screen and puts it back, and the restore is the same call with the other
 * argument. A maximized window answers ::lh_os_window_is_maximized so a button knows
 * which way round to draw itself.
 *
 * A no-op when the window is closed, or when the backend has no such slot. A
 * maximized window is resized by the backend, which is why
 * ::lh_os_window_set_on_resize exists.
 */
lh_void
lh_os_window_minimize(lh_os_window_t *self);

/**
 * @brief Maximize @p self, or restore it when @p maximized is false. See
 *        ::lh_os_window_minimize.
 */
lh_void
lh_os_window_maximize(lh_os_window_t *self, lh_bool_t maximized);

/**
 * @brief Get @p self's client area size, or false when it is closed.
 *
 * The same two numbers ::lh_os_window_set_on_resize hands over, for a caller that
 * wants them once instead of waiting to be told. The backend is asked when it can
 * answer (`get_client_size`); otherwise the size the window was last told.
 */
lh_bool_t
lh_os_window_get_client_size(const lh_os_window_t *self, int *width, int *height);

/**
 * @brief Where @p self is on the screen, into @p x and @p y: the top-left corner in
 *        screen coordinates, which is what ::lh_os_window_set_placement_v counts in.
 *
 * The window system may answer differently from what placement asked for — a
 * window dragged somewhere, or restored from a maximize — so this is the position,
 * not the request. The backend is asked when it can answer (`get_position`);
 * otherwise the place the window keeps.
 *
 * @return True when the position was read; false when the window is closed.
 */
lh_bool_t
lh_os_window_get_position(const lh_os_window_t *self, int *x, int *y);

/**
 * @brief Draw @p self's frame and chrome instead of the OS's.
 *
 * Call before ::lh_os_window_open — what it says is read at creation time.
 * ::lh_os_window_frame_own creates the window with no OS frame at all: its client is
 * the whole window, so the width and height given to open are the client size with
 * nothing subtracted for a frame. Everything that was drawn on that frame is now the
 * app's to paint and the app's to hit test; tell the window system which of it moves
 * and resizes the window with ::lh_os_window_set_on_zone.
 *
 * The default, ::lh_os_window_frame_system, is the ordinary OS frame with its
 * caption, menu and resize borders, and needs nothing else set.
 */
lh_void
lh_os_window_set_frame_v(lh_os_window_t *self, lh_os_window_frame_t frame);

/**
 * @brief Cut @p self's corners to a rounded region of @p radius pixels.
 *
 * The corners are clipped away, not painted over: the window is cut to a rounded
 * region, which is the only way to round a window on a system with no compositor
 * behind it. Works with either frame and may be set before or after opening; `0` or
 * less is square.
 *
 * @p radius is in pixels and is clamped by the window system to what fits.
 */
lh_void
lh_os_window_set_corner_radius(lh_os_window_t *self, int radius);

/** The window timer of @p self for its controller, when no `on_tick` period runs it: armed
   for the recognizer's next deadline -- again when @p rearm, else only when it is not
   running yet -- and stopped once nothing waits on time. */
lh_void
lh_os_window_arm_controller(lh_os_window_t *self, lh_bool_t rearm);

/**
 * @brief Called from the backend when its window is gone.
 *
 * Invokes the on-close callback, clears the native state, re-enables a modal
 * parent, and unlinks from app or parent.
 */
lh_void
lh_os_window_on_native_destroy(lh_os_window_t *self);

/**
 * @brief Called from the backend after the client area changed.
 *
 * Records the maximized state and then fires ::lh_os_window_set_on_resize, so both
 * facts a caller needs about a size change arrive the same way.
 *
 * An empty client (either dimension zero) records the maximized state but fires
 * nothing: a window that has just been created sends its size before creation has
 * applied the one it was asked for, and a minimized window has no client at all.
 * Neither is a size to lay out for; the next message carrying a real one is.
 */
lh_void
lh_os_window_on_native_resize(lh_os_window_t *self, int width, int height, lh_bool_t maximized);

/**
 * @brief Notify @p on_tick every @p ms (::lh_os_window_on_tick_fn), or @p ms `0` to
 *        stop.
 *
 * This exists because of a way an app that polls something goes wrong without it. The
 * only thing a window gets back on its own is a **paint**, so the poll was run inside
 * the paint and its answer -- "I still have requests due" -- was thrown away. The
 * device was asked three questions and then went quiet, with a status line saying it had
 * answered everything. The obvious repair is to invalidate when the poll is not done,
 * and that is a **spin**: the next paint comes back at once, the gap between two
 * questions to the same device has not elapsed, the poll sends nothing and says it is
 * still busy, and 219 requests turn into 219 requests times the gap in wasted frames at
 * full CPU. A timer is what turns "ask for the next frame" into "come back at 20 ms".
 *
 * @p ms below the window system's minimum is raised to it rather than refused: a caller
 * that asks for 1 ms on a system whose timer resolution is 15.6 ms asked for as fast as
 * it can, and refusing turns a too-fast request into no timer at all.
 *
 * The period is kept either way; the backend's timer (its `set_timer` slot) is armed
 * when the window is open, and a period asked for before the window existed is armed
 * on opening. A backend without a timer leaves the ticks to whoever calls
 * ::lh_os_window_on_native_tick. @p on_tick ::lh_null stops the timer.
 */
lh_void
lh_os_window_set_on_tick(lh_os_window_t *self, lh_os_window_on_tick_cb on_tick, lh_ptr context,
                         lh_u32_t ms);

/**
 * @brief Close children, then have the backend destroy its window and unlink
 *        @p self.
 *
 * Ends in ::lh_os_window_on_native_destroy: from the backend, or from here when the
 * backend did not get to it. A window that was never opened is only unlinked.
 */
lh_void
lh_os_window_close(lh_os_window_t *self);

/**
 * @brief Say where @p self opens for the first time.
 *
 * Call before ::lh_os_window_open, like ::lh_os_window_set_frame_v: what it says is
 * read at creation. The default, ::lh_os_window_placement_default, is the window
 * system's own choice, which on Win32 is a cascade from the top-left corner and not
 * where a user expects a window to be.
 *
 * ::lh_os_window_placement_center puts it in the middle of the work area of the
 * monitor it opens on — a modal child on the monitor its owner is on — and keeps
 * the title reachable when the window is larger than that area.
 *
 * This is where a window *opens*. Moving one that is already up is the window
 * system's drag, and ::lh_os_window_maximize restores it to the rectangle it
 * was opened at.
 */
lh_void
lh_os_window_set_placement_v(lh_os_window_t *self, lh_os_window_placement_t placement);

/** The controller's timers at @p self's tick: what is due by now, then the timer for the
   next deadline. */
lh_void
lh_os_window_update_controller(lh_os_window_t *self);

/** @p event to @p self's controller, if any: fed, the whole controller cancelled after a
   focus loss, and its timer armed for a deadline the event may have set. */
lh_void
lh_os_window_feed_controller(lh_os_window_t *self, const lh_os_input_event_ptr event);

/**
 * @brief Close if needed and clear @p self.
 *
 * An open window is closed first (::lh_os_window_close); its close reaction is
 * cleared either way.
 */
lh_void
lh_os_window_deinit(lh_os_window_t *self);

/**
 * @brief Called from the backend when the window's timer goes off.
 *
 * Updates the controller first (::lh_os_window_set_controller), then invokes the
 * on-tick callback if there is one. The native side owns the schedule; this
 * is only the delivery, and it is a no-op for a window with no callback rather than a
 * crash -- a window whose timer was stopped by the window system, or one that was never
 * given a period, still gets messages.
 */
lh_void
lh_os_window_on_native_tick(lh_os_window_t *self);

/**
 * @brief Called from the backend with one input event.
 *
 * Records the buttons a mouse event leaves held
 * (::lh_os_window_get_pressed_buttons; a pointer cancel and a focus loss clear
 * them), then hands @p event to ::lh_os_window_set_on_input's sink, if any, and
 * then to the controller of ::lh_os_window_set_controller, if any (cancelled
 * after a focus loss, its timer rearmed). Every backend delivers through here —
 * Win32, X11, AppKit, a board's touch driver. The window may be closed when this returns: the sink
 * or the controller's handler may close it.
 */
lh_void
lh_os_window_on_native_input(lh_os_window_t *self, const lh_os_input_event_ptr event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_H */
