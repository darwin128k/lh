/**
 * @file cb.h
 * @brief ::lh_console_cb — pointer to an ::lh_console_fn.
 */

#ifndef LH_CONSOLE_CB_H
#define LH_CONSOLE_CB_H

#include <lh/console/fn.h>

/** @brief Pointer to a command's handler. */
#define lh_console_cb lh_ptr_of(lh_console_fn)

#endif /* LH_CONSOLE_CB_H */
