/**
 * @file cb.h
 * @brief ::lh_mb_link_clock_cb — pointer to an ::lh_mb_link_clock_fn.
 */

#ifndef LH_NET_MODBUS_LINK_CLOCK_CB_H
#define LH_NET_MODBUS_LINK_CLOCK_CB_H

#include <lh/net/modbus/link/clock/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to the clock of a link; ::lh_null counts no time. */
#define lh_mb_link_clock_cb lh_ptr_of(lh_mb_link_clock_fn)

#endif /* LH_NET_MODBUS_LINK_CLOCK_CB_H */
