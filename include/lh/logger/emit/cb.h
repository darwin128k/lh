/**
 * @file cb.h
 * @brief Callable wrapper for log emit callbacks.
 *
 * @see lh_io_writer_write_cb
 */

#ifndef LH_LOGGER_EMIT_CB_H
#define LH_LOGGER_EMIT_CB_H

#include <lh/logger/emit/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_logger_emit_cb
 * @brief Pointer to ::lh_logger_emit_fn.
 */
#define lh_logger_emit_cb lh_ptr_of(lh_logger_emit_fn)

#endif /* LH_LOGGER_EMIT_CB_H */
