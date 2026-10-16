/**
 * @file flags.h
 * @brief Flags for tracking memory view slice initialization state.
 */

#ifndef LH_MEMORY_VIEW_SLICE_FLAGS_H
#define LH_MEMORY_VIEW_SLICE_FLAGS_H

#include <lh/byte.h>
#include <lh/bit.h>

/** @brief Neither end of the slice is set. */
#define lh_memory_view_slice_flags_uninitialized 0
/** @brief The begin pointer is set. */
#define lh_memory_view_slice_flags_initialized_begin lh_bit_mask(0)
/** @brief The end pointer is set. */
#define lh_memory_view_slice_flags_initialized_end lh_bit_mask(1)
/** @brief Both ends are set. */
#define lh_memory_view_slice_flags_initialized                                                     \
    (lh_memory_view_slice_flags_initialized_begin | lh_memory_view_slice_flags_initialized_end)

/** @brief Mask of `lh_memory_view_slice_flags_*` bits. */
typedef lh_byte_t lh_memory_view_slice_flags_t;

#endif /* LH_MEMORY_VIEW_SLICE_FLAGS_H */
