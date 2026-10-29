/**
 * @file cb.h
 * @brief ::lh_os_logger_sink_flush_cb — pointer to an ::lh_os_logger_sink_flush_fn.
 */

#ifndef LH_OS_LOGGER_SINK_FLUSH_CB_H
#define LH_OS_LOGGER_SINK_FLUSH_CB_H

#include <lh/os/logger/sink/flush/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to an ::lh_os_logger_sink_flush_fn. */
#define lh_os_logger_sink_flush_cb lh_ptr_of(lh_os_logger_sink_flush_fn)

#endif /* LH_OS_LOGGER_SINK_FLUSH_CB_H */
