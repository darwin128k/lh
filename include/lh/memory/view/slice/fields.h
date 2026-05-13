/**
 * @file fields.h
 * @brief Field macro for read-only slice view types.
 *
 * Defines a macro to generate const-qualified slice field declarations using
 * interval bounds fields with pointer element type.
 */

#ifndef LH_MEMORY_VIEW_SLICE_FIELDS_H
#define LH_MEMORY_VIEW_SLICE_FIELDS_H

#include <lh/interval/bounds/fields.h>

/**
 * @def lh_memory_view_slice_fields(T)
 * @brief Generate read-only slice fields for a given element type.
 *
 * @param T Element type (e.g., lh_void, lh_char_t).
 * Expands to interval bounds fields with const pointer element type.
 */
#define lh_memory_view_slice_fields(T) lh_interval_bounds_fields(const T *)

#endif // LH_MEMORY_VIEW_SLICE_FIELDS_H
