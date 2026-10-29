/**
 * @file cb.h
 * @brief ::lh_os_logger_sink_emit_cb — pointer to an ::lh_os_logger_sink_emit_fn.
 */

#ifndef LH_OS_LOGGER_SINK_EMIT_CB_H
#define LH_OS_LOGGER_SINK_EMIT_CB_H

#include <lh/os/logger/sink/emit/fn.h>

/** @brief Pointer to an ::lh_os_logger_sink_emit_fn. */
#define lh_os_logger_sink_emit_cb lh_ptr_of(lh_os_logger_sink_emit_fn)

#endif /* LH_OS_LOGGER_SINK_EMIT_CB_H */
