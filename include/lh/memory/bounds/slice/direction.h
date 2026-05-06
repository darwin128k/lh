/**
 * @file direction.h
 * @brief Direction flags for memory slice traversal.
 *
 * This header defines direction indicators used to describe
 * the traversal direction of a memory slice.
 */

#ifndef LH_MEMORY_BOUNDS_SLICE_DIRECTION_H
#define LH_MEMORY_BOUNDS_SLICE_DIRECTION_H

#include <lh/byte.h>

/**
 * @def lh_memory_bounds_slice_direction_unknown
 * @brief Direction is unknown or unspecified.
 *
 * Value: 0
 */
#define lh_memory_bounds_slice_direction_unknown 0

/**
 * @def lh_memory_bounds_slice_direction_forward
 * @brief Forward traversal direction (from beginning to end).
 *
 * Value: 1
 */
#define lh_memory_bounds_slice_direction_forward 1

/**
 * @def lh_memory_bounds_slice_direction_backward
 * @brief Backward traversal direction (from end to beginning).
 *
 * Value: 2
 */
#define lh_memory_bounds_slice_direction_backward 2

/**
 * @typedef lh_memory_bounds_slice_direction_t
 * @brief Type used to store slice direction flags.
 *
 * Usually an alias for lh_byte_t (typically uint8_t or unsigned char).
 */
typedef lh_byte_t lh_memory_bounds_slice_direction_t;

#endif // LH_MEMORY_BOUNDS_SLICE_DIRECTION_H