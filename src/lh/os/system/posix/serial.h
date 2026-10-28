/**
 * @file serial.h
 * @brief Backend-private: baud number → POSIX `speed_t` (`B9600`, …).
 *
 * Pure map; a rate this platform does not define becomes `B0` (refused).
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_SERIAL_H
#define LH_SRC_OS_SYSTEM_POSIX_SERIAL_H

#include <lh/numeric/fixed/types.h>

#include <termios.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/** @p baud as a termios speed constant, or `B0` when unknown. */
speed_t
lh_os_system_serial_speed(lh_u32_t baud);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_SERIAL_H */
