/**
 * @file writer.h
 * @brief The runtime writer: the ::lh_io_writer_t library code reaches for.
 *
 * Same idea as ::lh_runtime_allocator_get_mut / ::lh_runtime_logger_get_mut: one instance the
 * host configures, so lh never hardcodes UART, a socket, or `FILE *`. It
 * starts empty until the host installs a sink (`stdout`, a UART, …):
 *
 * @code
 * lh_io_writer_init(lh_runtime_writer_get_mut(), usart_write_cb, lh_null);
 * lh_runtime_writer_write("hi\r\n", 4);
 * @endcode
 *
 * The slot is one object for the whole program, empty until the host fills it
 * through ::lh_runtime_writer_get_mut — before the first use and before it starts
 * other threads (a thread sees everything written before it was created).
 * Every thread, the ones LH starts and the host's own, uses the same object. */

#ifndef LH_RUNTIME_WRITER_H
#define LH_RUNTIME_WRITER_H

#include <lh/io/writer.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the runtime writer, for change.
 *
 * One object for every thread. The host fills it before the first use and
 * before it starts other threads; a change while other threads use it is a
 * data race.
 *
 * @return The runtime ::lh_io_writer_t (never null).
 */
lh_io_writer_t *
lh_runtime_writer_get_mut(void);

/**
 * @brief Return the runtime writer.
 *
 * Same object as ::lh_runtime_writer_get_mut, read-only.
 *
 * @return The runtime ::lh_io_writer_t (never null).
 */
const lh_io_writer_t *
lh_runtime_writer_get(void);

/**
 * @brief Write through ::lh_runtime_writer_get_mut().
 *
 * Same as ::lh_io_writer_write(::lh_runtime_writer_get_mut(), @p buf, @p size).
 */
lh_ssize_t
lh_runtime_writer_write(const lh_ptr buf, lh_usize_t size);

/**
 * @brief Write every byte through ::lh_runtime_writer_get_mut().
 *
 * Same as ::lh_io_writer_write_all(::lh_runtime_writer_get_mut(), @p buf, @p size).
 */
lh_bool_t
lh_runtime_writer_write_all(const lh_ptr buf, lh_usize_t size);

/**
 * @brief printf-style write through ::lh_runtime_writer_get_mut().
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
