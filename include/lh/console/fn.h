/**
 * @file fn.h
 * @brief ::lh_console_fn — a command's handler.
 */

#ifndef LH_CONSOLE_FN_H
#define LH_CONSOLE_FN_H

#include <lh/bool.h>
#include <lh/console/call.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Run a command.
 *
 * @param call What was typed and where the reply goes; borrowed for the call.
 * @param user The value the command was registered with.
 * @return False when the arguments are wrong: the caller shows the command's usage.
 */
typedef lh_bool_t(lh_console_fn)(const lh_console_call_t *call, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CONSOLE_FN_H */
