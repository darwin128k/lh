/**
 * @file cb.h
 * @brief ::lh_mb_server_write_cb — pointer to an ::lh_mb_server_write_fn.
 */

#ifndef LH_NET_MODBUS_SERVER_WRITE_CB_H
#define LH_NET_MODBUS_SERVER_WRITE_CB_H

#include <lh/core/net/modbus/server/write/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to the write of a server; ::lh_null refuses every write as an
 *  illegal function. */
#define lh_mb_server_write_cb lh_ptr_of(lh_mb_server_write_fn)

#endif /* LH_NET_MODBUS_SERVER_WRITE_CB_H */
