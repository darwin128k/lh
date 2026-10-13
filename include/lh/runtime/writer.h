/**
 * @file writer.h
 * @brief The runtime writer: the ::lh_io_writer_t library code reaches for.
 *
 * Same idea as ::lh_runtime_allocator / ::lh_runtime_logger: one instance the
 * host configures, so lh never hardcodes UART, a socket, or `FILE *`. With
 * ::LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB on (the default) it starts as
 * `stdout`; off, it starts empty until the application installs a sink:
 *
 * @code
 * lh_io_writer_set(lh_runtime_writer(), usart_write_cb, lh_null);
 * lh_runtime_writer_write("hi\r\n", 4);
 * @endcode
 *
 * When thread-local mode is enabled, each thread has its own instance.
 */

#ifndef LH_RUNTIME_WRITER_H
#define LH_RUNTIME_WRITER_H

#include <lh/compiler/extern/c.h>
#include <lh/io/writer.h>
#include <lh/ptr.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return address of the runtime writer instance.
 * @return Pointer to the internal ::lh_io_writer_t (never null).
 */
lh_io_writer_t *
lh_runtime_writer(void);

/**
 * @brief Write through ::lh_runtime_writer().
 *
 * Same as ::lh_io_writer_write(::lh_runtime_writer(), @p buf, @p size).
 */
lh_ssize_t
lh_runtime_writer_write(const lh_ptr buf, lh_usize_t size);

/**
 * @brief Write every byte through ::lh_runtime_writer().
 *
 * Same as ::lh_io_writer_write_all(::lh_runtime_writer(), @p buf, @p size).
 */
lh_bool_t
lh_runtime_writer_write_all(const lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_WRITER_H */
