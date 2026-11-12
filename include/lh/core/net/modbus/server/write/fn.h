/**
 * @file fn.h
 * @brief ::lh_mb_server_write_fn — what a server asks its application for a write.
 *
 * Both writes come here: "write one register" (0x06) as a count of one and "write
 * many registers" (0x10) as it was sent, after every check the protocol makes.
 */

#ifndef LH_NET_MODBUS_SERVER_WRITE_FN_H
#define LH_NET_MODBUS_SERVER_WRITE_FN_H

#include <lh/core/ptr.h>

/**
 * @brief Store the @p count words of @p values from @p address on.
 *
 * All or nothing: a write the application refuses must leave every register as it
 * was, because the master is told it did not happen.
 *
 * @param owner   The pointer the server was given (::lh_mb_server_get_owner).
 * @param address First register; `address + count` does not pass 65536.
 * @param count   How many, 1..::LH_MB_WRITE_MAX.
 * @param values  The words, first register first; read only during the call.
 *
 * @return 0 when they are stored; otherwise the exception code the device answers
 *         with (see ::lh_mb_server_read_fn).
 */
typedef lh_u8_t(lh_mb_server_write_fn)(lh_ptr owner, lh_u16_t address, lh_u16_t count,
                                       const lh_u16_t *values);

#endif /* LH_NET_MODBUS_SERVER_WRITE_FN_H */
