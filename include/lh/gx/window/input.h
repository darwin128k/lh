/**
 * @file input.h
 * @brief A window's input as gx events: ::lh_gx_window_input_try_get_event.
 *
 * The operating system reports input as ::lh_os_input_event_t (`lh/os/input.h`):
 * buttons, moves, keys, typed code points, controllers. A gx screen takes
 * ::lh_gx_event_t. These bricks translate the part a screen acts on and refuse
 * the rest:
 *
 * - the primary button going down, moving while held, going up, or the system
 *   taking the pointer away — a press, a move, a release, a cancel, at the
 *   window's client coordinates, which are the screen's: the system's Q24.8
 *   point in whole pixels (::lh_os_input_coord_to_int);
 * - a key going down (or repeating) that a control acts on (::lh_gx_key_t);
 * - a typed code point that types (::lh_gx_event_is_text_code) — the system
 *   reports Enter and Backspace as characters too, and those stay keys.
 *
 * Nothing is kept; the translation is a function of the one event.
 */

#ifndef LH_GX_WINDOW_INPUT_H
#define LH_GX_WINDOW_INPUT_H

#include <lh/gx/event.h>
#include <lh/os/input/event.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The control key @p key stands for: the arrows, Home, End, Backspace,
 *        Delete, Enter, Escape, Tab.
 *
 * @param key A key of the system's.
 * @param out Receives the key; untouched on failure. Must not be null.
 * @return ::lh_bool_false for any other key.
 */
lh_bool_t
lh_gx_window_input_try_get_key(lh_os_key_t key, lh_gx_key_t *out);

/**
 * @brief Translate @p event into the gx event a screen takes.
 *
 * @param event The system's event; must not be null.
 * @param out   Receives the event; untouched on failure. Must not be null.
 * @return ::lh_bool_false for an event a screen does not act on: another
 *         button, a move with the primary button up, a key no control uses, a
 *         character that does not type, a wheel, a controller.
 */
lh_bool_t
lh_gx_window_input_try_get_event(const lh_os_input_event_t *event, lh_gx_event_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WINDOW_INPUT_H */
