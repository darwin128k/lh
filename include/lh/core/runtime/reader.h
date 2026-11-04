/**
 * @file reader.h
 * @brief The runtime reader: the ::lh_io_reader_t library code reaches for.
 *
 * Mirror of ::lh_runtime_writer_get_mut for the read direction. It starts
 * empty until the host installs a source (`stdin`, UART RX, …):
 *
 * @code
 * lh_io_reader_init(lh_runtime_reader_get_mut(), usart_read_cb, lh_null);
 * @endcode
 *
 * The slot is one object for the whole program, empty until the host fills it
 * through ::lh_runtime_reader_get_mut — before the first use and before it starts
 * other threads (a thread sees everything written before it was created).
 * Every thread, the ones LH starts and the host's own, uses the same object. */

#ifndef LH_RUNTIME_READER_H
#define LH_RUNTIME_READER_H

#include <lh/core/io/reader.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the runtime reader, for change.
 *
 * One object for every thread. The host fills it before the first use and
 * before it starts other threads; a change while other threads use it is a
 * data race.
 *
 * @return The runtime ::lh_io_reader_t (never null).
 */
lh_io_reader_t *
lh_runtime_reader_get_mut(void);

/**
 * @brief Return the runtime reader.
 *
 * Same object as ::lh_runtime_reader_get_mut, read-only.
 *
 * @return The runtime ::lh_io_reader_t (never null).
 */
const lh_io_reader_t *
lh_runtime_reader_get(void);

/**
 * @brief Read through ::lh_runtime_reader_get_mut().
 *
 * Same as ::lh_io_reader_read(::lh_runtime_reader_get_mut(), @p buf, @p size).
 */
lh_ssize_t
lh_runtime_reader_read(lh_ptr buf, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_READER_H */
