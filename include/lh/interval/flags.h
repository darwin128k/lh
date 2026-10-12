/**
 * @file flags.h
 * @brief Flags for specifying interval boundary openness
 *
 * This header defines a small set of flags that describe whether
 * the left and/or right boundaries of an interval are included
 * (closed) or excluded (open).
 */

#ifndef LH_INTERVAL_FLAGS_H
#define LH_INTERVAL_FLAGS_H

#include <lh/byte.h>

/**
 * @def lh_interval_flags_closed
 * @brief Both boundaries are included → [a, b]
 *
 * Value: 0 (default / most common case)
 */
#define lh_interval_flags_closed 0

/**
 * @def lh_interval_flags_open_left
 * @brief Left boundary is excluded → (a, …]
 *
 * Bit 0
 */
#define lh_interval_flags_open_left (1u << 0)

/**
 * @def lh_interval_flags_open_right
 * @brief Right boundary is excluded → […, b)
 *
 * Bit 1
 */
#define lh_interval_flags_open_right (1u << 1)

/**
 * @def lh_interval_flags_open
 * @brief Both boundaries are excluded → (a, b)
 *
 * Convenience alias = open_left | open_right
 */
#define lh_interval_flags_open (lh_interval_flags_open_left | lh_interval_flags_open_right)

/**
 * @typedef lh_interval_flags_t
 * @brief Type used to store interval boundary flags
 *
 * Usually an alias for lh_byte_t (typically uint8_t or unsigned char).
 */
typedef lh_byte_t lh_interval_flags_t;

#endif /* LH_INTERVAL_FLAGS_H */