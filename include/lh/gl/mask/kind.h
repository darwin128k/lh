/**
 * @file kind.h
 * @brief How two masks combine: ::lh_gl_mask_kind_t.
 */

#ifndef LH_GL_MASK_KIND_H
#define LH_GL_MASK_KIND_H

/**
 * @enum lh_gl_mask_kind
 * @brief A rounded rectangle, or a combination of two borrowed masks.
 */
typedef enum lh_gl_mask_kind
{
    /** One rounded rectangle. Radius 0 is the rectangle. */
    lh_gl_mask_kind_round = 0,

    /** Lesser coverage of the two masks. */
    lh_gl_mask_kind_intersect,

    /** Greater coverage of the two masks. */
    lh_gl_mask_kind_unite,

    /** Left coverage where the right mask is empty. */
    lh_gl_mask_kind_exclude
} lh_gl_mask_kind_t;

#endif /* LH_GL_MASK_KIND_H */
