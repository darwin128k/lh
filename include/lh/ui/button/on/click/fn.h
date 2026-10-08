/**
 * @file fn.h
 * @brief Notify function type for ::lh_ui_button_t.
 *
 * Not a pointer type by itself. ::lh_ui_button_on_click_cb is the pointer.
 */

#ifndef LH_UI_BUTTON_ON_CLICK_FN_H
#define LH_UI_BUTTON_ON_CLICK_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_ui_button;

/**
 * @typedef lh_ui_button_on_click_fn
 * @brief Called when a click lands on ::lh_ui_button_t.
 *
 * The click has already ended by the time this runs: the button has let go of
 * the press (::lh_ui_entity_is_pressed is false) and its style is the resting
 * one, so a callback that opens something does not paint over its own look.
 *
 * @p context is what ::lh_ui_button_set_on_click was given.
 */
typedef lh_void(lh_ui_button_on_click_fn)(struct lh_ui_button *self, lh_ptr context);

#endif /* LH_UI_BUTTON_ON_CLICK_FN_H */