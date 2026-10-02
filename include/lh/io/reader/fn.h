/**
 * @file fn.h
 * @brief Callable signature for stream read routines.
 *
 * A reader needs to know *which* stream it is reading — there can be many
 * at once (one per connection, one per open file, ...) — so @p self
 * carries that identity as part of the callback's own signature rather than
 * as a separate object the callback closes over: the callback is a method,
 * @p self the object it runs on. The allocator callbacks
 * (::lh_memory_allocator_alloc_fn) take their @p self the same way.
 *
 * @see lh_io_writer_write_fn
 */

#ifndef LH_IO_READER_FN_H
#define LH_IO_READER_FN_H

#include <lh/ptr.h>
#include <lh/self.h>
#include <lh/size.h>

/**
 * @typedef lh_io_reader_read_fn
 * @brief Function type `lh_ssize_t(lh_self_ptr, lh_ptr, lh_usize_t)` for reading
 *        from a stream.
 *
 * @param self    Identifies which stream to read (whatever the concrete
 *                reader needs — a socket handle, a `FILE *`, ...).
 * @param buf     Destination buffer.
 * @param size    Maximum number of bytes to read into @p buf.
 *
 * @return Number of bytes actually read (`0` at end of stream), or a
 *         negative value if the read failed.
 */
typedef lh_ssize_t(lh_io_reader_read_fn)(lh_self_ptr self, lh_ptr buf, lh_usize_t size);

#endif /* LH_IO_READER_FN_H */
