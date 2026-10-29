/**
 * @file fn.h
 * @brief ::lh_os_logger_sink_dropped_fn — note that @p count records were dropped because the queue was full.
 *
 * Called on the queue's worker thread only.
 */

#ifndef LH_OS_LOGGER_SINK_DROPPED_FN_H
#define LH_OS_LOGGER_SINK_DROPPED_FN_H

#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Note that @p count records were dropped because the queue was full.
 *
 * @param user  The value the sink was given.
 * @param count How many were dropped since the last call.
 */
typedef lh_void(lh_os_logger_sink_dropped_fn)(lh_ptr user, lh_u32_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_SINK_DROPPED_FN_H */
