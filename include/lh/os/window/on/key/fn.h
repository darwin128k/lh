/**
 * @file fn.h
 * @brief Key notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_key_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_KEY_FN_H
#define LH_OS_WINDOW_ON_KEY_FN_H

#include <lh/bool.h>
#include <lh/key.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_key_fn
 * @brief Called when a key goes down (@p pressed true, repeated while held) or up.
 *
 * @p key is the portable code (::lh_key_t); keys outside it come as
 * ::lh_key_other.
 */
typedef lh_void(lh_os_window_on_key_fn)(struct lh_os_window *self, lh_key_t key, lh_bool_t pressed, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_KEY_FN_H */
