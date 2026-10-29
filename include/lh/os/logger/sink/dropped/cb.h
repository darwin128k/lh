/**
 * @file cb.h
 * @brief ::lh_os_logger_sink_dropped_cb — pointer to an ::lh_os_logger_sink_dropped_fn.
 */

#ifndef LH_OS_LOGGER_SINK_DROPPED_CB_H
#define LH_OS_LOGGER_SINK_DROPPED_CB_H

#include <lh/os/logger/sink/dropped/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to an ::lh_os_logger_sink_dropped_fn. */
#define lh_os_logger_sink_dropped_cb lh_ptr_of(lh_os_logger_sink_dropped_fn)

#endif /* LH_OS_LOGGER_SINK_DROPPED_CB_H */
