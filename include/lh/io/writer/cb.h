/**
 * @file cb.h
 * @brief Callable wrapper for stream write callbacks.
 *
 * Provides a convenient type alias for function pointer to
 * ::lh_io_writer_write_fn.
 *
 * @see lh_io_reader_read_cb
 */

#ifndef LH_IO_WRITER_CB_H
#define LH_IO_WRITER_CB_H

#include <lh/io/writer/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_io_writer_write_cb
 * @brief Pointer to ::lh_io_writer_write_fn.
 *
 * Expands to a function pointer type for stream write callbacks.
 */
#define lh_io_writer_write_cb lh_ptr_of(lh_io_writer_write_fn)

#endif /* LH_IO_WRITER_CB_H */
