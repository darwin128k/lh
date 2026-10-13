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
 * lh_runtime_writer_init(usart_write_cb, lh_null);
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
#include <lh/str/ptr.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return address of the runtime writer instance.
 * @return Pointer to the internal ::lh_io_writer_t (never null).
 */
lh_io_writer_t *
lh_runtime_writer(void);

/**
 * @brief Install a sink on ::lh_runtime_writer().
 *
 * Same as ::lh_io_writer_init(::lh_runtime_writer(), @p write_cb, @p context).
 *
 * @param write_cb Write callback (may be ::lh_null to disable).
 * @param context  Passed to @p write_cb on every call.
 */
lh_void
lh_runtime_writer_init(lh_io_writer_write_cb write_cb, lh_ptr context);

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

/**
 * @brief printf-style write through ::lh_runtime_writer().
 *
 * Formats with ::lh_str_ptr_format_text (not libc printf) into a
 * ::LH_IO_WRITER_PRINT_CAPACITY stack buffer, then
 * ::lh_runtime_writer_write_all. Same conversions as that formatter.
 *
 * @code
 * lh_runtime_writer_print("blink %u\r\n", (lh_uint_t)n);
 * @endcode
 */
lh_ssize_t
lh_runtime_writer_print(const lh_str_ptr fmt, ...);

/**
 * @brief `va_list` form of ::lh_runtime_writer_print.
 */
lh_ssize_t
lh_runtime_writer_print_v(const lh_str_ptr fmt, va_list args);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_WRITER_H */
