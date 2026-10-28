/**
 * @file fn.h
 * @brief ::lh_mb_link_clock_fn — the clock a link measures time with.
 *
 * A link does not read the system clock itself: a test has to be able to move
 * time by hand, and an application decides what a microsecond is worth.
 */

#ifndef LH_NET_MODBUS_LINK_CLOCK_FN_H
#define LH_NET_MODBUS_LINK_CLOCK_FN_H

#include <lh/ptr.h>

/**
 * @brief Current time in microseconds.
 *
 * @param self The clock the link was given (::lh_mb_link_get_clock).
 * @return Microseconds on a monotonic scale; only differences are used.
 */
typedef lh_u64_t(lh_mb_link_clock_fn)(lh_ptr self);

#endif /* LH_NET_MODBUS_LINK_CLOCK_FN_H */
