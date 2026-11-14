/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_scroll_axis_t.
 */

#ifndef LH_GL_SCROLL_AXIS_FIELDS_H
#define LH_GL_SCROLL_AXIS_FIELDS_H

/**
 * @def lh_gl_scroll_axis_fields(offset_type)
 * @brief The one number a scroll axis stores.
 *
 * @param offset_type ::lh_gl_scalar_t. The member is `offset`. Positive moves
 *                    the content toward the start of that axis. The axis does
 *                    not know whether it is horizontal or vertical, and it
 *                    stores nothing else.
 */
#define lh_gl_scroll_axis_fields(offset_type)                                                      \
    offset_type offset

#endif /* LH_GL_SCROLL_AXIS_FIELDS_H */
