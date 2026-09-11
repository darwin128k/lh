/**
 * @file fn.h
 * @brief Callable signature for stream write routines.
 *
 * @see lh_io_reader_read_fn
 */

#ifndef LH_IO_WRITER_FN_H
#define LH_IO_WRITER_FN_H

#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @typedef lh_io_writer_write_fn
 * @brief Function type `lh_ssize_t(lh_ptr, const lh_ptr, lh_usize_t)` for
 *        writing to a stream.
 *
 * @param context Identifies which stream to write (whatever the concrete
 *                writer needs — a socket handle, a `FILE *`, ...).
 * @param buf     Source buffer.
 * @param size    Number of bytes from @p buf to write.
 *
 * @return Number of bytes actually written, or a negative value if the
 *         write failed.
 */
typedef lh_ssize_t(lh_io_writer_write_fn)(lh_ptr context, const lh_ptr buf, lh_usize_t size);

#endif /* LH_IO_WRITER_FN_H */
