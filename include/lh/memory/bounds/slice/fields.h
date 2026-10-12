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
 * @def lh_memory_bounds_slice_fields(elem_type)
 * @brief Generate slice fields for a given element type.
 *
 * @param elem_type Element type (e.g. lh_void, lh_char_t), not a pointer.
 * Expands to interval bounds fields with pointer element type.
 */
#define lh_memory_bounds_slice_fields(elem_type) lh_interval_bounds_fields(elem_type *)

#endif /* LH_MEMORY_BOUNDS_SLICE_FIELDS_H */
