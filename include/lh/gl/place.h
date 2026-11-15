/**
 * @file place.h
 * @brief Origin, clip and mask for one draw call: ::lh_gl_place_t.
 *
 * The layer builds a place and passes it. The canvas does not keep it. Shapes
 * passed to the canvas are still relative to @p origin. The clip and the mask
 * are in root coordinates. A null mask does not cut beyond the clip.
 */

#ifndef LH_GL_PLACE_H
#define LH_GL_PLACE_H

#include <lh/gl/mask.h>
#include <lh/gl/place/fields.h>
#include <lh/gl/point.h>
#include <lh/gl/rect.h>

/**
 * @struct lh_gl_place
 * @brief One draw call's placement. Fields via ::lh_gl_place_fields.
 */
struct lh_gl_place
{
    /** Members: ::lh_gl_place_fields. */
    lh_gl_place_fields(lh_gl_point_t, lh_gl_rect_t, lh_gl_mask_t);
};
/** @copybrief lh_gl_place */
typedef struct lh_gl_place lh_gl_place_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Root origin shapes are shifted by. */
const lh_gl_point_t *
lh_gl_place_get_origin(const lh_gl_place_t *self);

/** @brief Root clip. A miss is not drawn. */
const lh_gl_rect_t *
lh_gl_place_get_clip(const lh_gl_place_t *self);

/** @brief Borrowed mask, or ::lh_null. */
const lh_gl_mask_t *
lh_gl_place_get_mask(const lh_gl_place_t *self);

/** @brief Store the three. @p mask may be ::lh_null. Nothing is borrowed except the mask. */
lh_void
lh_gl_place_init(lh_gl_place_t *self, lh_gl_point_t origin, lh_gl_rect_t clip,
                 const lh_gl_mask_t *mask);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_PLACE_H */
