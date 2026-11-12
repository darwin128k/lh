/**
 * @file fn.h
 * @brief ::lh_mb_clock_fn — the clock a Modbus link or server measures time with.
 *
 * Neither reads the system clock itself: a test has to be able to move time by
 * hand, and an application decides what a microsecond is worth. One type for both
 * sides of the line (::lh_mb_link_t asks with it how long a question has waited,
 * ::lh_mb_server_t how long the line has been silent), so an application hands the
 * same clock to each.
 */

#ifndef LH_NET_MODBUS_CLOCK_FN_H
#define LH_NET_MODBUS_CLOCK_FN_H

#include <lh/core/ptr.h>

/**
 * @brief Current time in microseconds.
 *
 * @param self The pointer the link or server was given with the clock
 *             (::lh_mb_link_get_clock, ::lh_mb_server_get_clock).
 * @return Microseconds on a monotonic scale; only differences are used.
 */
typedef lh_u64_t(lh_mb_clock_fn)(lh_ptr self);

#endif /* LH_NET_MODBUS_CLOCK_FN_H */
