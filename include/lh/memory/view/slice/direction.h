/**
 * @file direction.h
 * @brief Direction flags for memory view slice traversal.
 */

#ifndef LH_MEMORY_VIEW_SLICE_DIRECTION_H
#define LH_MEMORY_VIEW_SLICE_DIRECTION_H

#include <lh/byte.h>

#define lh_memory_view_slice_direction_unknown 0
#define lh_memory_view_slice_direction_forward 1
#define lh_memory_view_slice_direction_backward 2

typedef lh_byte_t lh_memory_view_slice_direction_t;

#endif // LH_MEMORY_VIEW_SLICE_DIRECTION_H
