/**
 * @file stream.h
 * @brief Duplex view of ::lh_runtime_reader and ::lh_runtime_writer.
 *
 * Returns an ::lh_io_stream_t whose halves track the current runtime reader
 * and writer (refreshed on each call). lh still does not own a peripheral —
 * the host wires those two; this is only the pair for APIs that take a stream.
 */

#ifndef LH_RUNTIME_STREAM_H
#define LH_RUNTIME_STREAM_H

#include <lh/io/stream.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the runtime duplex stream (reader + writer).
 *
 * @return Pointer to an internal ::lh_io_stream_t (never null). Halves match
 *         ::lh_runtime_reader() / ::lh_runtime_writer() at the time of the call.
 */
lh_io_stream_t *
lh_runtime_stream(void);

/**
 * @brief Read via ::lh_runtime_stream().
 */
lh_ssize_t
lh_runtime_stream_read(lh_ptr buf, lh_usize_t size);

/**
 * @brief Write via ::lh_runtime_stream().
 */
lh_ssize_t
lh_runtime_stream_write(const lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_STREAM_H */
