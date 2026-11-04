/**
 * @file logger.h
 * @brief The runtime logger: the ::lh_logger_t library code writes to.
 *
 * The same idea as ::lh_runtime_allocator_get_mut: one instance the library reaches
 * for without being handed one, which the application reconfigures instead
 * of the library hardcoding a sink. It starts with every level off and drops
 * everything until the host installs a sink (`stderr`, a UART, a server):
 *
 * @code
 * lh_logger_set(lh_runtime_logger_get_mut(), lh_logger_level_flags_all, emit, ..., context);
 * lh_logger_info(lh_runtime_logger_get_mut(), "listening on %u", (lh_uint_t)port);
 * @endcode
 *
 * A record is one line: the sink appends the line end, the caller does not.
 * The slot is one object for the whole program, empty until the host fills it
 * through ::lh_runtime_logger_get_mut — before the first use and before it starts
 * other threads (a thread sees everything written before it was created).
 * Every thread, the ones LH starts and the host's own, uses the same object. */

#ifndef LH_RUNTIME_LOGGER_H
#define LH_RUNTIME_LOGGER_H

#include <lh/core/logger.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the runtime logger, for change.
 *
 * One object for every thread. The host fills it before the first use and
 * before it starts other threads; a change while other threads use it is a
 * data race.
 *
 * @return The runtime ::lh_logger_t (never null).
 */
lh_logger_t *
lh_runtime_logger_get_mut(void);

/**
 * @brief Return the runtime logger.
 *
 * Same object as ::lh_runtime_logger_get_mut, read-only.
 *
 * @return The runtime ::lh_logger_t (never null).
 */
const lh_logger_t *
lh_runtime_logger_get(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_LOGGER_H */
