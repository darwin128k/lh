/**
 * @file fn.h
 * @brief ::lh_mb_server_read_fn — what a server asks its application for a read.
 *
 * A server owns the wire and nothing else: which registers exist and what is in
 * them is the application's. A read that has passed every check the protocol makes
 * (a supported code, 1..125 registers, inside the 65536-register space) is handed
 * here, and what comes back is either the words or the reason they cannot be had.
 */

#ifndef LH_NET_MODBUS_SERVER_READ_FN_H
#define LH_NET_MODBUS_SERVER_READ_FN_H

#include <lh/core/ptr.h>

/**
 * @brief Fill @p values with @p count registers from @p address.
 *
 * @param owner   The pointer the server was given (::lh_mb_server_get_owner).
 * @param fc      ::LH_MB_FC_READ_HOLDING or ::LH_MB_FC_READ_INPUT.
 * @param address First register; `address + count` does not pass 65536.
 * @param count   How many, 1..::LH_MB_READ_MAX.
 * @param values  Where the words go: @p count of them, first register first.
 *
 * @return 0 when @p values is filled; otherwise the exception code the device
 *         answers with (::LH_MB_EXCEPTION_ILLEGAL_ADDRESS when a register is not on
 *         it, ::LH_MB_EXCEPTION_DEVICE_FAILURE, ::LH_MB_EXCEPTION_DEVICE_BUSY).
 */
typedef lh_u8_t(lh_mb_server_read_fn)(lh_ptr owner, lh_u8_t fc, lh_u16_t address, lh_u16_t count,
                                      lh_u16_t *values);

#endif /* LH_NET_MODBUS_SERVER_READ_FN_H */
