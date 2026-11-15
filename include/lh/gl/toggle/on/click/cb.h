/**
 * @file cb.h
 * @brief Pointer to ::lh_gl_toggle_on_click_fn.
 */

#ifndef LH_GL_TOGGLE_ON_CLICK_CB_H
#define LH_GL_TOGGLE_ON_CLICK_CB_H

#include <lh/gl/toggle/on/click/fn.h>
#include <lh/core/util/ptr.h>

/**
 * @def lh_gl_toggle_on_click_cb
 * @brief Pointer to ::lh_gl_toggle_on_click_fn.
 */
#define lh_gl_toggle_on_click_cb lh_ptr_of(lh_gl_toggle_on_click_fn)

#endif /* LH_GL_TOGGLE_ON_CLICK_CB_H */