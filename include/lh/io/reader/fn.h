/**
 * @file fn.h
 * @brief Callable signature for stream read routines.
 *
 * Unlike ::lh_memory_allocator_alloc_fn (which is global/stateless, like
 * `malloc`), a reader needs to know *which* stream it is reading — there
 * can be many at once (one per connection, one per open file, ...) — so
 * @p context carries that identity as part of the callback's own signature
 * rather than as a separate object the callback closes over.
 *
 * @see lh_io_writer_write_fn
 */

#ifndef LH_IO_READER_FN_H
#define LH_IO_READER_FN_H

#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @typedef lh_io_reader_read_fn
 * @brief Function type `lh_ssize_t(lh_ptr, lh_ptr, lh_usize_t)` for reading
 *        from a stream.
 *
 * @param context Identifies which stream to read (whatever the concrete
 *                reader needs — a socket handle, a `FILE *`, ...).
 * @param buf     Destination buffer.
 * @param size    Maximum number of bytes to read into @p buf.
 *
 * @return Number of bytes actually read (`0` at end of stream), or a
 *         negative value if the read failed.
 */
typedef lh_ssize_t(lh_io_reader_read_fn)(lh_ptr context, lh_ptr buf, lh_usize_t size);

#endif /* LH_IO_READER_FN_H */
