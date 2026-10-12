/**
 * @file serial.h
 * @brief A serial port (COM port, `/dev/tty*`) as an ::lh_io_stream_t.
 *
 * Opened by name, rate, parity and stop bits, eight data bits, and read without
 * waiting: a read hands over what has arrived and answers ::LH_IO_PENDING when
 * nothing has. A serial line has no end of stream, so a read never answers `0` --
 * a quiet line is a line that is quiet, not one that has gone.
 *
 * Everything above it sees a stream (::lh_os_serial_get_stream) and does not know
 * a port is behind it: the same Modbus link runs over a socket or over this.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SERIAL_H
#define LH_OS_SERIAL_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/io/stream.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/serial/fields.h>
#include <lh/os/serial/parity.h>
#include <lh/os/system/serial/handle.h>
#include <lh/ptr.h>
#include <lh/self.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/serial.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_serial
 * @typedef lh_os_serial_t
 * @brief One serial port.
 */
struct lh_os_serial
{
    lh_os_serial_fields(lh_os_system_serial_handle_t);
};
typedef struct lh_os_serial lh_os_serial_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Start closed.
 */
void
lh_os_serial_init(lh_os_serial_t *self);

/**
 * @brief Open the port @p name. Closes whatever @p self had open first.
 *
 * @param name      `COM3` on Windows, `/dev/ttyUSB0` on POSIX, UTF-8.
 * @param baud      Bits per second.
 * @param parity    The parity bit.
 * @param stop_bits 1 or 2.
 *
 * @return ::lh_bool_true when open; on failure the reason is in
 *         ::lh_os_system_last_error.
 */
lh_bool_t
lh_os_serial_open(lh_os_serial_t *self, lh_str_cptr name, lh_u32_t baud, lh_os_serial_parity_t parity,
                  lh_u8_t stop_bits);

/**
 * @brief Close the port. Safe on one that is not open.
 */
void
lh_os_serial_close(lh_os_serial_t *self);

/**
 * @brief Whether @p self has a port open.
 */
lh_bool_t
lh_os_serial_is_open(const lh_os_serial_t *self);

/**
 * @brief ::lh_io_reader_read_fn over @p self (an ::lh_os_serial_t *).
 *
 * @return Bytes read, ::LH_IO_PENDING when nothing has arrived, or a negative
 *         value on failure.
 */
lh_ssize_t
lh_os_serial_read(lh_self_ptr self, lh_ptr buf, lh_usize_t size);

/**
 * @brief ::lh_io_writer_write_fn over @p self (an ::lh_os_serial_t *).
 *
 * @return Bytes written, ::LH_IO_PENDING when none could be taken now, or a
 *         negative value on failure.
 */
lh_ssize_t
lh_os_serial_write(lh_self_ptr self, const lh_ptr buf, lh_usize_t size);

/**
 * @brief The port as a stream.
 *
 * @param self Port; must outlive the stream.
 */
lh_io_stream_t
lh_os_serial_get_stream(lh_os_serial_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SERIAL_H */
