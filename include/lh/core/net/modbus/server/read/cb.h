/**
 * @file cb.h
 * @brief ::lh_mb_server_read_cb — pointer to an ::lh_mb_server_read_fn.
 */

#ifndef LH_NET_MODBUS_SERVER_READ_CB_H
#define LH_NET_MODBUS_SERVER_READ_CB_H

#include <lh/core/net/modbus/server/read/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to the read of a server; ::lh_null refuses every read as an
 *  illegal function. */
#define lh_mb_server_read_cb lh_ptr_of(lh_mb_server_read_fn)

#endif /* LH_NET_MODBUS_SERVER_READ_CB_H */
