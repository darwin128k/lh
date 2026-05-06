/**
 * @file fields.h
 * @brief Field macro for slice types.
 *
 * Defines a macro to generate slice field declarations using
 * interval bounds fields with pointer element type.
 */

#ifndef LH_MEMORY_BOUNDS_SLICE_FIELDS_H
#define LH_MEMORY_BOUNDS_SLICE_FIELDS_H

#include <lh/interval/bounds/fields.h>

/**
 * @def lh_memory_bounds_slice_fields(T)
 * @brief Generate slice fields for a given element pointer type.
 *
 * @param T Element pointer type (e.g., lh_void, lh_char_t).
 * Expands to interval bounds fields with pointer element type.
 */
#define lh_memory_bounds_slice_fields(T) lh_interval_bounds_fields(T *)

#endif // LH_MEMORY_BOUNDS_SLICE_FIELDS_H