/**
 * @file fn.h
 * @brief ::lh_console_sink_fn — where a command's reply goes.
 *
 * A console does not know where it is printed: a game server's console, a terminal,
 * a remote connection. Whoever runs a command passes the sink and its value.
 */

#ifndef LH_CONSOLE_SINK_FN_H
#define LH_CONSOLE_SINK_FN_H

#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write @p text, as it is: the console adds no newline of its own.
 *
 * @param out  The value the sink was passed with.
 * @param text NUL-terminated text, borrowed for the call.
 */
typedef lh_void(lh_console_sink_fn)(lh_ptr out, const lh_char_t *text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CONSOLE_SINK_FN_H */
