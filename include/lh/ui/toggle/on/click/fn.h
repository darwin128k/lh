/**
 * @file fn.h
 * @brief Notify function type for ::lh_ui_toggle_t.
 *
 * Not a pointer type by itself. ::lh_ui_toggle_on_click_cb is the pointer.
 */

#ifndef LH_UI_ENTITY_TOGGLE_ON_CLICK_FN_H
#define LH_UI_ENTITY_TOGGLE_ON_CLICK_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_ui_toggle;

/**
 * @typedef lh_ui_toggle_on_click_fn
 * @brief Called when a click lands on ::lh_ui_toggle_t.
 *
 * The click has already ended by the time this runs: the toggle has let go of
 * the press and has already flipped (::lh_ui_toggle_get_checked answers the new
 * state), so a callback that opens something does not paint over its own look
 * and does not have to guess which way it went.
 *
 * @p context is what ::lh_ui_toggle_set_on_click was given.
 */
typedef lh_void(lh_ui_toggle_on_click_fn)(struct lh_ui_toggle *self, lh_ptr context);

#endif /* LH_UI_ENTITY_TOGGLE_ON_CLICK_FN_H */