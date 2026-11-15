/**
 * @file mask.h
 * @brief Coverage of a shape: ::lh_gl_mask_t.
 *
 * A mask answers how much of a pixel belongs to a shape, from 0 to 255. A
 * rounded rectangle is one mask. Intersection keeps the lesser coverage, union
 * the greater, exclusion the left coverage where the right one is 0. The
 * combine borrows the two masks and does not copy their pixels. Nothing is
 * allocated. The canvas does not store a mask: the caller passes one for the
 * call that paints.
 */

#ifndef LH_GL_MASK_H
#define LH_GL_MASK_H

#include <lh/gl/color.h>
#include <lh/gl/mask/fields.h>
#include <lh/gl/mask/kind.h>
#include <lh/gl/rect.h>

/**
 * @struct lh_gl_mask
 * @brief One shape or a combination of two. Fields via ::lh_gl_mask_fields.
 */
struct lh_gl_mask
{
    /** Members: ::lh_gl_mask_fields. */
    lh_gl_mask_fields(lh_gl_mask_kind_t, lh_gl_rect_t, lh_gl_scalar_t, struct lh_gl_mask);
};
/** @copybrief lh_gl_mask */
typedef struct lh_gl_mask lh_gl_mask_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Which combination this mask is. */
lh_gl_mask_kind_t
lh_gl_mask_get_kind(const lh_gl_mask_t *self);

/** @brief Rectangle of a rounded mask, in the coordinates coverage is asked. */
const lh_gl_rect_t *
lh_gl_mask_get_rect(const lh_gl_mask_t *self);

/** @brief Corner radius. Zero is a rectangle. A negative value is stored as zero. */
lh_gl_scalar_t
lh_gl_mask_get_radius(const lh_gl_mask_t *self);

/** @brief Left mask of a combination, or ::lh_null. */
const lh_gl_mask_t *
lh_gl_mask_get_left(const lh_gl_mask_t *self);

/** @brief Right mask of a combination, or ::lh_null. */
const lh_gl_mask_t *
lh_gl_mask_get_right(const lh_gl_mask_t *self);

/**
 * @brief A rounded rectangle. @p radius below 0 is 0. The edge fades across one
 *        pixel: inside the arc is 255, outside is 0, the rim is in between.
 */
lh_void
lh_gl_mask_init_round(lh_gl_mask_t *self, lh_gl_rect_t rect, lh_gl_scalar_t radius);

/** @brief Coverage is the lesser of @p left and @p right. Both are borrowed. */
lh_void
lh_gl_mask_init_intersect(lh_gl_mask_t *self, const lh_gl_mask_t *left, const lh_gl_mask_t *right);

/** @brief Coverage is the greater of @p left and @p right. Both are borrowed. */
lh_void
lh_gl_mask_init_unite(lh_gl_mask_t *self, const lh_gl_mask_t *left, const lh_gl_mask_t *right);

/**
 * @brief Coverage of @p left where @p right is 0. A partial right mask scales
 *        the left down. Both are borrowed.
 */
lh_void
lh_gl_mask_init_exclude(lh_gl_mask_t *self, const lh_gl_mask_t *left, const lh_gl_mask_t *right);

/**
 * @brief Coverage of pixel (@p x, @p y). A null child of a combination covers
 *        nothing. The rounded edge is one pixel wide.
 */
lh_gl_color_channel_t
lh_gl_mask_coverage(const lh_gl_mask_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_MASK_H */
