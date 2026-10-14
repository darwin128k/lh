/**
 * @file direction.h
 * @brief Direction flags for memory view slice traversal.
 */

#ifndef LH_MEMORY_VIEW_SLICE_DIRECTION_H
#define LH_MEMORY_VIEW_SLICE_DIRECTION_H

#include <lh/byte.h>

/** @brief Not both ends are set, so there is no direction. */
#define lh_memory_view_slice_direction_unknown 0
/** @brief The begin is at or before the end. */
#define lh_memory_view_slice_direction_forward 1
/** @brief The begin is after the end. */
#define lh_memory_view_slice_direction_backward 2

/** @brief One of the `lh_memory_view_slice_direction_*` values. */
typedef lh_byte_t lh_memory_view_slice_direction_t;

#endif /* LH_MEMORY_VIEW_SLICE_DIRECTION_H */
