/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_round_box_t.
 */

#ifndef LH_CORE_PIXEL_ROUND_BOX_FIELDS_H
#define LH_CORE_PIXEL_ROUND_BOX_FIELDS_H

/**
 * @def lh_pixel_round_box_fields(fixed_type)
 * @brief A rounded rectangle in ::LH_PIXEL_ROUND_SUBPIXEL units: `lo_x`, `lo_y` its
 *        top-left corner, `hi_x`, `hi_y` its bottom-right one, `r` the radius of
 *        every corner.
 *
 * @param fixed_type Type of a length in sub-pixel units (::lh_s64_t).
 */
#define lh_pixel_round_box_fields(fixed_type)                                                      \
    fixed_type lo_x;                                                                               \
    fixed_type lo_y;                                                                               \
    fixed_type hi_x;                                                                               \
    fixed_type hi_y;                                                                               \
    fixed_type r

#endif /* LH_CORE_PIXEL_ROUND_BOX_FIELDS_H */
