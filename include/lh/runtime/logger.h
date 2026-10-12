/**
 * @file logger.h
 * @brief The runtime logger: the ::lh_logger_t library code writes to.
 *
 * The same idea as ::lh_runtime_allocator: one instance the library reaches
 * for without being handed one, which the application reconfigures instead
 * of the library hardcoding a sink. With
 * ::LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB on (the default) it starts
 * with every level going to `stderr`, one line per record; off, it starts
 * with no slots and drops everything until a sink is installed:
 *
 * @code
 * lh_logger_set(lh_runtime_logger(), lh_logger_level_flags_all, emit, ..., context);
 * lh_logger_info(lh_runtime_logger(), "listening on %u", (lh_uint_t)port);
 * @endcode
 *
 * A record is one line: the sink appends the line end, the caller does not.
 * When thread-local mode is enabled, each thread has its own instance.
 */

#ifndef LH_RUNTIME_LOGGER_H
#define LH_RUNTIME_LOGGER_H

#include <lh/compiler/extern/c.h>
#include <lh/logger.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return address of the runtime logger instance.
 *
 * @return Pointer to the internal ::lh_logger_t (never null).
 */
lh_logger_t *
lh_runtime_logger(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_LOGGER_H */
