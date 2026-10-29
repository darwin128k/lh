/**
 * @file fn.h
 * @brief ::lh_os_logger_sink_emit_fn — write one record.
 *
 * Called on the queue's worker thread only.
 */

#ifndef LH_OS_LOGGER_SINK_EMIT_FN_H
#define LH_OS_LOGGER_SINK_EMIT_FN_H

#include <lh/os/logger/record.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write one record.
 *
 * @param user   The value the sink was given.
 * @param record The record; borrowed for the call.
 */
typedef lh_void(lh_os_logger_sink_emit_fn)(lh_ptr user, const lh_os_logger_record_t *record);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_SINK_EMIT_FN_H */
