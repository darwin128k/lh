/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_mask_t.
 */

#ifndef LH_GL_MASK_FIELDS_H
#define LH_GL_MASK_FIELDS_H

/**
 * @brief A rounded rectangle, or two borrowed masks and a combine.
 *
 * @p rect and @p radius are read for ::lh_gl_mask_kind_round. @p left and
 * @p right are read for the other kinds. The masks are not owned.
 */
#define lh_gl_mask_fields(kind_type, rect_type, radius_type, mask_type)                            \
    kind_type kind;                                                                                \
    rect_type rect;                                                                                \
    radius_type radius;                                                                           \
    const mask_type *left;                                                                         \
    const mask_type *right

#endif /* LH_GL_MASK_FIELDS_H */
