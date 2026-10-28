/**
 * @file cb.h
 * @brief ::lh_mb_link_change_cb — pointer to an ::lh_mb_link_change_fn.
 */

#ifndef LH_NET_MODBUS_LINK_CHANGE_CB_H
#define LH_NET_MODBUS_LINK_CHANGE_CB_H

#include <lh/net/modbus/link/change/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to the change callback of a link; ::lh_null for none. */
#define lh_mb_link_change_cb lh_ptr_of(lh_mb_link_change_fn)

#endif /* LH_NET_MODBUS_LINK_CHANGE_CB_H */
