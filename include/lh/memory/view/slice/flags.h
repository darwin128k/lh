/**
 * @file flags.h
 * @brief Flags for tracking memory view slice initialization state.
 */

#ifndef LH_MEMORY_VIEW_SLICE_FLAGS_H
#define LH_MEMORY_VIEW_SLICE_FLAGS_H

#include <lh/byte.h>
#include <lh/util/bit.h>

#define lh_memory_view_slice_flags_uninitialized 0
#define lh_memory_view_slice_flags_initialized_begin lh_bit_mask(0)
#define lh_memory_view_slice_flags_initialized_end lh_bit_mask(1)
#define lh_memory_view_slice_flags_initialized                                                     \
    (lh_memory_view_slice_flags_initialized_begin | lh_memory_view_slice_flags_initialized_end)

typedef lh_byte_t lh_memory_view_slice_flags_t;

#endif // LH_MEMORY_VIEW_SLICE_FLAGS_H
