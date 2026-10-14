/**
 * @file parity.h
 * @brief ::lh_os_serial_parity_t: the ninth bit of a serial character.
 */

#ifndef LH_OS_SERIAL_PARITY_H
#define LH_OS_SERIAL_PARITY_H

/**
 * @enum lh_os_serial_parity
 * @brief Whether each character carries a parity bit, and which.
 *
 * Modbus RTU's default is even parity with one stop bit; no parity takes two stop
 * bits instead, so that a character is eleven bits either way. Devices are set
 * up every way there is, which is why this is an argument and not a constant.
 */
typedef enum lh_os_serial_parity
{
    lh_os_serial_parity_none = 0,
    lh_os_serial_parity_odd = 1,
    lh_os_serial_parity_even = 2
} lh_os_serial_parity_t; /**< Whether each character carries a parity bit, and which. */

#endif /* LH_OS_SERIAL_PARITY_H */
