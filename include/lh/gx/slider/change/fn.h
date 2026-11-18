/**
 * @file fn.h
 * @brief Function type ::lh_gx_slider_change_fn.
 */

#ifndef LH_GX_SLIDER_CHANGE_FN_H
#define LH_GX_SLIDER_CHANGE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_slider;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_slider_change_fn
 * @brief The application's reaction to the user moving @p slider to @p value.
 *
 * Called every time a drag changes the value — as the knob follows the
 * pointer, not only when it is released — and once more when a cancelled drag
 * puts the value back. Not called when the application sets the value itself
 * (::lh_gx_slider_slide). It runs inside the slider's `input`, so it may change
 * the screen but should not detach the slider.
 *
 * @param slider  The slider moved.
 * @param value   Its new value, in its range.
 * @param context What the application gave the slider with the reaction.
 */
typedef lh_void(lh_gx_slider_change_fn)(struct lh_gx_slider *slider, lh_s32_t value,
                                        lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_SLIDER_CHANGE_FN_H */
