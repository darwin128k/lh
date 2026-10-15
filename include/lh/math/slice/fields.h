/**
 * @file fields.h
 * @brief Member fields of ::lh_math_slice_t.
 */

#ifndef LH_MATH_SLICE_FIELDS_H
#define LH_MATH_SLICE_FIELDS_H

#include <lh/memory/view/slice/fields.h>

/**
 * @def lh_math_slice_fields(element_type)
 * @brief The closed bounds `first` and `second` of a read-only slice: pointers to
 *        its first and its last element, both ::lh_null when it is empty.
 *
 * The same fields as a byte slice (::lh_memory_view_slice_fields), over
 * @p element_type instead of bytes.
 *
 * @param element_type Type of one element (::lh_float_t), not a pointer.
 */
#define lh_math_slice_fields(element_type) lh_memory_view_slice_fields(element_type)

#endif /* LH_MATH_SLICE_FIELDS_H */
