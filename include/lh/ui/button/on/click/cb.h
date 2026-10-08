/**
 * @file cb.h
 * @brief Pointer to ::lh_ui_button_on_click_fn.
 */

#ifndef LH_UI_BUTTON_ON_CLICK_CB_H
#define LH_UI_BUTTON_ON_CLICK_CB_H

#include <lh/ui/button/on/click/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_ui_button_on_click_cb
 * @brief Pointer to ::lh_ui_button_on_click_fn.
 */
#define lh_ui_button_on_click_cb lh_ptr_of(lh_ui_button_on_click_fn)

#endif /* LH_UI_BUTTON_ON_CLICK_CB_H */