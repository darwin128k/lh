/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_place_t.
 */

#ifndef LH_GL_PLACE_FIELDS_H
#define LH_GL_PLACE_FIELDS_H

/**
 * @brief Where a shape is drawn, for one call.
 *
 * @p origin and @p clip are root coordinates. @p mask is borrowed and may be
 * null: null means the clip rectangle alone.
 */
#define lh_gl_place_fields(point_type, rect_type, mask_type)                                       \
    point_type origin;                                                                             \
    rect_type clip;                                                                                \
    const mask_type *mask

#endif /* LH_GL_PLACE_FIELDS_H */
