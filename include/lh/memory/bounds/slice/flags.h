/**
 * @file flags.h
 * @brief Flags for tracking memory slice initialization state
 *
 * This header defines flags that track whether the beginning,
 * end, or entire contents of a memory slice have been initialized.
 */

#ifndef LH_MEMORY_BOUNDS_SLICE_FLAGS_H
#define LH_MEMORY_BOUNDS_SLICE_FLAGS_H

#include <lh/byte.h>
#include <lh/util/bit.h>

/**
 * @def lh_memory_bounds_slice_flags_uninitialized
 * @brief No initialization flags set → slice contents are uninitialized
 *
 * Value: 0
 */
#define lh_memory_bounds_slice_flags_uninitialized 0

/**
 * @def lh_memory_bounds_slice_flags_initialized_begin
 * @brief Beginning of the slice is initialized → [a, …]
 *
 * Bit 0
 */
#define lh_memory_bounds_slice_flags_initialized_begin lh_bit_mask(0)

/**
 * @def lh_memory_bounds_slice_flags_initialized_end
 * @brief End of the slice is initialized → […, b]
 *
 * Bit 1
 */
#define lh_memory_bounds_slice_flags_initialized_end lh_bit_mask(1)

/**
 * @def lh_memory_bounds_slice_flags_initialized
 * @brief Entire slice is initialized → [a, b]
 *
 * Convenience alias = initialized_begin | initialized_end
 */
#define lh_memory_bounds_slice_flags_initialized                                                   \
    (lh_memory_bounds_slice_flags_initialized_begin | lh_memory_bounds_slice_flags_initialized_end)

/**
 * @typedef lh_memory_bounds_slice_flags_t
 * @brief Type used to store slice initialization flags.
 *
 * Usually an alias for lh_byte_t (typically uint8_t or unsigned char).
 */
typedef lh_byte_t lh_memory_bounds_slice_flags_t;

#endif // LH_MEMORY_BOUNDS_SLICE_FLAGS_H