/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_corners_t.
 */

#ifndef LH_GX_CORNERS_FIELDS_H
#define LH_GX_CORNERS_FIELDS_H

/**
 * @def lh_gx_corners_fields(scalar_type)
 * @brief One length per corner of a rectangle, clockwise from the top left — the
 *        order of CSS's `border-radius`.
 *
 * @param scalar_type Type of a length (::lh_gx_scalar_t).
 */
#define lh_gx_corners_fields(scalar_type)                                                          \
    scalar_type top_left;                                                                          \
    scalar_type top_right;                                                                         \
    scalar_type bottom_right;                                                                      \
    scalar_type bottom_left

#endif /* LH_GX_CORNERS_FIELDS_H */
