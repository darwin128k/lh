/**
 * @file cb.h
 * @brief ::lh_os_logger_sink_close_cb — pointer to an ::lh_os_logger_sink_close_fn.
 */

#ifndef LH_OS_LOGGER_SINK_CLOSE_CB_H
#define LH_OS_LOGGER_SINK_CLOSE_CB_H

#include <lh/os/logger/sink/close/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to an ::lh_os_logger_sink_close_fn. */
#define lh_os_logger_sink_close_cb lh_ptr_of(lh_os_logger_sink_close_fn)

#endif /* LH_OS_LOGGER_SINK_CLOSE_CB_H */
