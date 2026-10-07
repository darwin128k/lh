/**
 * @file fn.h
 * @brief Text notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_text_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_TEXT_FN_H
#define LH_OS_WINDOW_ON_TEXT_FN_H

#include <lh/bool.h>
#include <lh/key.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_text_fn
 * @brief Called with one typed character, as a Unicode code point.
 *
 * Control characters (below U+0020, and U+007F) are not sent: they come as
 * keys.
 */
typedef lh_void(lh_os_window_on_text_fn)(struct lh_os_window *self, lh_u32_t code, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_TEXT_FN_H */
