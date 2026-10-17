/**
 * @file kind.h
 * @brief What happened: ::lh_os_input_kind_t.
 *
 * Needs no ::LH_LIBRARY_OPTION_OS: a window backend, the web host and a
 * microcontroller touch driver build the same events.
 */

#ifndef LH_OS_INPUT_KIND_H
#define LH_OS_INPUT_KIND_H

/**
 * @enum lh_os_input_kind
 * @brief The kind of one ::lh_os_input_event_t.
 *
 * The order is fixed; new kinds are only ever added at the end. The kinds
 * from ::lh_os_input_kind_click on are synthesized by
 * ::lh_os_input_recognizer_t, never by a backend.
 */
typedef enum lh_os_input_kind
{
    lh_os_input_kind_none = 0,               /**< No event (a cleared one). */
    lh_os_input_kind_pointer_down,           /**< A button of a pointer went down. */
    lh_os_input_kind_pointer_up,             /**< A button of a pointer went up. */
    lh_os_input_kind_pointer_move,           /**< A pointer moved. */
    lh_os_input_kind_pointer_enter,          /**< A pointer came over the window. */
    lh_os_input_kind_pointer_leave,          /**< A pointer left the window. */
    lh_os_input_kind_pointer_cancel,         /**< The system took the pointer away. */
    lh_os_input_kind_wheel,                  /**< A wheel turned, `dx` / `dy` in 1/120. */
    lh_os_input_kind_key_down,               /**< A key went down (or repeats). */
    lh_os_input_kind_key_up,                 /**< A key went up. */
    lh_os_input_kind_text,                   /**< One typed code point in `code`. */
    lh_os_input_kind_controller_button_down, /**< A controller button went down. */
    lh_os_input_kind_controller_button_up,   /**< A controller button went up. */
    lh_os_input_kind_controller_axis,        /**< A controller axis moved, in `value`. */
    lh_os_input_kind_device_added,           /**< A device was connected. */
    lh_os_input_kind_device_removed,         /**< A device was disconnected. */
    lh_os_input_kind_focus_in,               /**< The window got the keyboard focus. */
    lh_os_input_kind_focus_out,              /**< The window lost the keyboard focus. */
    lh_os_input_kind_click,                  /**< Down and up without a drag (`count`). */
    lh_os_input_kind_double_click,           /**< The second click of a quick pair. */
    lh_os_input_kind_long_press,             /**< Held without moving for long enough. */
    lh_os_input_kind_drag_begin,             /**< Moved past the slop while down. */
    lh_os_input_kind_drag,                   /**< Moved while dragging (`dx` / `dy`). */
    lh_os_input_kind_drag_end,               /**< Let go (or cancelled) while dragging. */
    lh_os_input_kind_navigate,               /**< Move the focus (not produced yet). */
    lh_os_input_kind_activate                /**< Activate the focus (not produced yet). */
} lh_os_input_kind_t;                        /**< The kind of one input event. */

#endif /* LH_OS_INPUT_KIND_H */
