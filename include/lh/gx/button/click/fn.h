/**
 * @file fn.h
 * @brief Function type ::lh_gx_button_click_fn.
 */

#ifndef LH_GX_BUTTON_CLICK_FN_H
#define LH_GX_BUTTON_CLICK_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_button;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_button_click_fn
 * @brief The application's reaction to a click of @p button.
 *
 * Called once per click — a press on the button and a release on it — after
 * the button is drawn released again. The pointer is already free
 * (::lh_gx_pointer_end), so the reaction may change the screen, detach the
 * button or start another gesture.
 *
 * @param button  The button clicked.
 * @param context What the application gave the button with the reaction.
 */
typedef lh_void(lh_gx_button_click_fn)(struct lh_gx_button *button, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BUTTON_CLICK_FN_H */
