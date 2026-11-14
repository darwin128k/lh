/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_layout_t.
 */

#ifndef LH_GL_LAYOUT_FIELDS_H
#define LH_GL_LAYOUT_FIELDS_H

/**
 * @def lh_gl_layout_fields(gap_type)
 * @brief The space inserted after each packed child.
 *
 * @param gap_type ::lh_gl_scalar_t. The member is `gap`. A negative gap pulls
 *                 the next child back. The layout stores nothing else.
 */
#define lh_gl_layout_fields(gap_type)                                                              \
    gap_type gap

#endif /* LH_GL_LAYOUT_FIELDS_H */
