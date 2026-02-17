/**
 * @file interval_flags.h
 * @brief Interval boundary flags
 *        for mathematical interval representation
 *
 * This file defines flags and a type for specifying
 * whether the endpoints of an interval are open or closed.
 */

#ifndef LH_INTERVAL_FLAGS_H
#define LH_INTERVAL_FLAGS_H

#include <lh/char.h>

enum {
    /** @brief Closed interval flag (both endpoints included) */
    lh_interval_flags_closed = 0,

    /** @brief Open left endpoint flag (left boundary is excluded) */
    lh_interval_flags_open_left = (1 << 0),

    /** @brief Open right endpoint flag (right boundary is excluded) */
    lh_interval_flags_open_right = (1 << 1),

    /** @brief Open interval flag (both endpoints excluded) */
    lh_interval_flags_open =
        (lh_interval_flags_open_left | lh_interval_flags_open_right)
};

/**
 * @typedef lh_interval_flags_t
 * @brief Type for storing interval boundary flags
 *
 * This unsigned character type is used
 * to store combinations of the interval boundary flags.
 *
 * It can hold the following values:
 * - lh_interval_flags_closed     = 0b00 (both ends closed: [a,b])
 * - lh_interval_flags_open_left  = 0b01 (left open, right closed: (a,b])
 * - lh_interval_flags_open_right = 0b10 (left closed, right open: [a,b))
 * - lh_interval_flags_open       = 0b11 (both ends open: (a,b))
 *
 * @see lh_interval_flags_closed
 * @see lh_interval_flags_open_left
 * @see lh_interval_flags_open_right
 * @see lh_interval_flags_open
 */
typedef lh_uchar_t lh_interval_flags_t;

#endif // LH_INTERVAL_FLAGS_H
