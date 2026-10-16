/**
 * @file reader.h
 * @brief The runtime reader: the ::lh_io_reader_t library code reaches for.
 *
 * Mirror of ::lh_runtime_writer for the read direction. With
 * ::LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB on it starts as `stdin`; off, empty
 * until the host installs a source (UART RX, …):
 *
 * @code
 * lh_runtime_reader_init(usart_read_cb, lh_null);
 * @endcode
 *
 * When thread-local mode is enabled, each thread has its own instance.
 */

#ifndef LH_RUNTIME_READER_H
#define LH_RUNTIME_READER_H

#include <lh/io/reader.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return address of the runtime reader instance.
 * @return Pointer to the internal ::lh_io_reader_t (never null).
 */
lh_io_reader_t *
lh_runtime_reader(void);

/**
 * @brief Install a source on ::lh_runtime_reader().
 *
 * Same as ::lh_io_reader_init(::lh_runtime_reader(), @p read_cb, @p context).
 *
 * @param read_cb Read callback (may be ::lh_null to disable).
 * @param context Passed to @p read_cb on every call.
 */
lh_void
lh_runtime_reader_init(lh_io_reader_read_cb read_cb, lh_ptr context);

/**
 * @brief Read through ::lh_runtime_reader().
 *
 * Same as ::lh_io_reader_read(::lh_runtime_reader(), @p buf, @p size).
 */
lh_ssize_t
lh_runtime_reader_read(lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_READER_H */
