/**
 * @file pass.h
 * @brief ::lh_gl_canvas_sw_shadow_pass_t — one box shadow ready to draw row by row: filled by ::lh_gl_canvas_sw_shadow_pass_init.
 */

#ifndef LH_GL_CANVAS_SW_SHADOW_PASS_H
#define LH_GL_CANVAS_SW_SHADOW_PASS_H

#include <lh/gl/canvas.h>
#include <lh/gl/canvas/sw/fields.h>
#include <lh/gl/pixmap.h>
#include <lh/gl/radius.h>
#include <lh/gl/shadow/ramp.h>

/**
 * @struct lh_gl_canvas_sw_shadow_pass
 * @brief One box shadow ready to draw row by row: filled by
 *        ::lh_gl_canvas_sw_shadow_pass_init.
 */
struct lh_gl_canvas_sw_shadow_pass
{
    lh_gl_shadow_shape_t shape; /**< The shadow's shape (::lh_gl_shadow_shape_init). */
    lh_gl_shadow_ramp_t ramp;   /**< Its alpha ramp (::lh_gl_shadow_ramp_init). */
    const lh_gl_rect_t *rect;   /**< The box casting it. */
    lh_gl_scalar_t corner;      /**< The box radius, clamped to the box. */
    lh_u32_t rgb;               /**< Shadow colour, `0x00RRGGBB`. */
    lh_bool_t rounded;          /**< True when the canvas clip is rounded. */
    lh_s32_t left;              /**< Left of the box grown by the outset. */
    lh_s32_t top;               /**< Its top. */
    lh_s32_t right;             /**< One past its right. */
    lh_s32_t bottom;            /**< One past its bottom. */
};

/** @brief Alias for ::lh_gl_canvas_sw_shadow_pass. */
typedef struct lh_gl_canvas_sw_shadow_pass lh_gl_canvas_sw_shadow_pass_t;

struct lh_gl_canvas_sw;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Prepare the shadow of @p rect rounded by @p radius on @p self into
 *        @p out. False, with only the edges set, when the shadow has no outset
 *        and draws nothing. @p out must not be null (::lh_assert_runtime_ref).
 */
lh_bool_t
lh_gl_canvas_sw_shadow_pass_init(const struct lh_gl_canvas_sw *self, const lh_gl_rect_t *rect,
                                 lh_gl_scalar_t radius, const lh_gl_shadow_t *shadow,
                                 lh_gl_canvas_sw_shadow_pass_t *out);

/**
 * @brief The edges and colour of ::lh_gl_canvas_sw_shadow_pass_init, without the
 *        shape or the ramp.
 *
 * False, with the edges set, when the shadow has no outset. The caller that
 * already holds an alpha mask skips the ramp, which depends only on the box
 * size, the radius and the shadow, not on where the box sits.
 */
lh_bool_t
lh_gl_canvas_sw_shadow_pass_init_bounds(const struct lh_gl_canvas_sw *self, const lh_gl_rect_t *rect,
                                        lh_gl_scalar_t radius, const lh_gl_shadow_t *shadow,
                                        lh_gl_canvas_sw_shadow_pass_t *out);

/**
 * @brief The shape of the shadow (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_shadow_shape_t *
lh_gl_canvas_sw_shadow_pass_get_shape(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief The shape of the shadow, to change in place (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_shadow_shape_t *
lh_gl_canvas_sw_shadow_pass_get_shape_mut(lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the shape of the shadow (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param shape The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_shape(lh_gl_canvas_sw_shadow_pass_t *self, lh_gl_shadow_shape_t shape);

/**
 * @brief The falloff of the shadow (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_shadow_ramp_t *
lh_gl_canvas_sw_shadow_pass_get_ramp(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief The falloff of the shadow, to change in place (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_shadow_ramp_t *
lh_gl_canvas_sw_shadow_pass_get_ramp_mut(lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the falloff of the shadow (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param ramp The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_ramp(lh_gl_canvas_sw_shadow_pass_t *self, lh_gl_shadow_ramp_t ramp);

/**
 * @brief The rectangle that casts the shadow (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_rect_t *
lh_gl_canvas_sw_shadow_pass_get_rect(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the rectangle that casts the shadow (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param rect The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_rect(lh_gl_canvas_sw_shadow_pass_t *self, const lh_gl_rect_t * rect);

/**
 * @brief The corner radius of that rectangle (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_canvas_sw_shadow_pass_get_corner(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the corner radius of that rectangle (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param corner The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_corner(lh_gl_canvas_sw_shadow_pass_t *self, lh_gl_scalar_t corner);

/**
 * @brief The color of the shadow, without alpha (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_gl_canvas_sw_shadow_pass_get_rgb(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the color of the shadow, without alpha (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param rgb The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_rgb(lh_gl_canvas_sw_shadow_pass_t *self, lh_u32_t rgb);

/**
 * @brief Whether the rectangle has rounded corners (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gl_canvas_sw_shadow_pass_is_rounded(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store whether the rectangle has rounded corners (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param rounded The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_rounded(lh_gl_canvas_sw_shadow_pass_t *self, lh_bool_t rounded);

/**
 * @brief The left edge of the area the shadow covers (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_left(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the left edge of the area the shadow covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param left The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_left(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t left);

/**
 * @brief The top edge of the area the shadow covers (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_top(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the top edge of the area the shadow covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param top The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_top(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t top);

/**
 * @brief The right edge of the area the shadow covers (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_right(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the right edge of the area the shadow covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param right The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_right(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t right);

/**
 * @brief The bottom edge of the area the shadow covers (level 1).
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_canvas_sw_shadow_pass_get_bottom(const lh_gl_canvas_sw_shadow_pass_t *self);

/**
 * @brief Store the bottom edge of the area the shadow covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shadow pass; checked by ::lh_assert_runtime_ref.
 * @param bottom The new value.
 */
lh_void
lh_gl_canvas_sw_shadow_pass_set_bottom(lh_gl_canvas_sw_shadow_pass_t *self, lh_s32_t bottom);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_SW_SHADOW_PASS_H */
