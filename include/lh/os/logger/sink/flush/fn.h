/**
 * @file fn.h
 * @brief ::lh_os_logger_sink_flush_fn — put what was written on disk, or wherever it goes.
 *
 * Called on the queue's worker thread only.
 */

#ifndef LH_OS_LOGGER_SINK_FLUSH_FN_H
#define LH_OS_LOGGER_SINK_FLUSH_FN_H

#include <lh/ptr.h>
#include <lh/void.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Put what was written on disk, or wherever it goes.
 *
 * @param user The value the sink was given.
 */
typedef lh_void(lh_os_logger_sink_flush_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_SINK_FLUSH_FN_H */
