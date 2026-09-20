/**
 * @file stream.h
 * @brief Duplex stream: an ::lh_io_reader_t and an ::lh_io_writer_t together.
 *
 * Not a file and not a directory. A file owns a path and a handle;
 * ::lh_os_fs_file_read talks to the OS. A stream is only the adapter:
 * whoever owns the bytes (file, TCP socket, a test buffer) plugs
 * its read/write callbacks into the two halves. Everything above this
 * point works against ::lh_io_stream_read / ::lh_io_stream_write without
 * knowing which concrete thing is behind it. A directory listing is
 * names, not bytes — it does not plug in here.
 */

#ifndef LH_IO_STREAM_H
#define LH_IO_STREAM_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/io/reader.h>
#include <lh/io/writer.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_io_stream
 * @typedef lh_io_stream_t
 * @brief An ::lh_io_reader_t paired with an ::lh_io_writer_t.
 */
struct lh_io_stream
{
    lh_io_reader_t reader;
    lh_io_writer_t writer;
};
typedef struct lh_io_stream lh_io_stream_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── construct / set ─────────────────────────────────────────────────────── */

/**
 * @brief Construct an ::lh_io_stream_t from a reader and a writer.
 *
 * @param reader Read half.
 * @param writer Write half.
 * @return Constructed ::lh_io_stream_t value.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_stream_t
lh_io_stream_make(const lh_io_reader_t *reader, const lh_io_writer_t *writer);

/**
 * @brief Replace both halves of @p self.
 *
 * @param self   Stream to modify.
 * @param reader New read half.
 * @param writer New write half.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_stream_set(lh_io_stream_t *self, const lh_io_reader_t *reader, const lh_io_writer_t *writer);

/**
 * @brief Copy the stream state from @p other into @p self.
 * @param self  Destination stream.
 * @param other Source stream.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_stream_assign(lh_io_stream_t *self, const lh_io_stream_t *other);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the read half of @p self.
 * @param self Stream to read from.
 * @return Current ::lh_io_reader_t.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_reader_t
lh_io_stream_get_reader(const lh_io_stream_t *self);

/**
 * @brief Replace the read half of @p self.
 * @param self   Stream to modify.
 * @param reader New read half.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_stream_set_reader(lh_io_stream_t *self, const lh_io_reader_t *reader);

/**
 * @brief Return the write half of @p self.
 * @param self Stream to read from.
 * @return Current ::lh_io_writer_t.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_writer_t
lh_io_stream_get_writer(const lh_io_stream_t *self);

/**
 * @brief Replace the write half of @p self.
 * @param self   Stream to modify.
 * @param writer New write half.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_io_stream_set_writer(lh_io_stream_t *self, const lh_io_writer_t *writer);

/* ── operations ──────────────────────────────────────────────────────────── */

/**
 * @brief Read via @p self's reader. Same contract as ::lh_io_reader_read.
 * @param self Stream to read from.
 * @param buf  Destination buffer.
 * @param size Maximum bytes to read into @p buf.
 * @return Bytes read (`0` at end of stream), or negative on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_io_stream_read(lh_io_stream_t *self, lh_ptr buf, lh_usize_t size);

/**
 * @brief Write via @p self's writer. Same contract as ::lh_io_writer_write.
 * @param self Stream to write to.
 * @param buf  Source buffer.
 * @param size Number of bytes from @p buf to write.
 * @return Bytes written, or negative on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_io_stream_write(lh_io_stream_t *self, const lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_IO_STREAM_H */
