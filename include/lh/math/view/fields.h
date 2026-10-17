/**
 * @file fields.h
 * @brief Member fields of ::lh_math_view_t.
 */

#ifndef LH_MATH_VIEW_FIELDS_H
#define LH_MATH_VIEW_FIELDS_H

/**
 * @def lh_math_view_fields(slice_type, size_type)
 * @brief A strided view: `slice`, the elements it spans (its first element is the
 *        view's first); `stride`, how far apart in that slice two neighbouring
 *        elements of the view are; `count`, how many elements the view has.
 *
 * @param slice_type ::lh_math_slice_t.
 * @param size_type  ::lh_usize_t.
 */
#define lh_math_view_fields(slice_type, size_type)                                                 \
    slice_type slice;                                                                              \
    size_type stride;                                                                              \
    size_type count

#endif /* LH_MATH_VIEW_FIELDS_H */
