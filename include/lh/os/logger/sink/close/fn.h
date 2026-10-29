/**
 * @file fn.h
 * @brief ::lh_os_logger_sink_close_fn — release everything: no record follows.
 *
 * Called on the queue's worker thread only.
 */

#ifndef LH_OS_LOGGER_SINK_CLOSE_FN_H
#define LH_OS_LOGGER_SINK_CLOSE_FN_H

#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Release everything: no record follows.
 *
 * @param user The value the sink was given.
 */
typedef lh_void(lh_os_logger_sink_close_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_SINK_CLOSE_FN_H */
