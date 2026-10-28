/**
 * @file camera.h
 * @brief ::lh_ui_renderer3d_camera_t — where the camera stands, where it looks, and the frustum it cuts.
 *
 * The camera owns no memory. ::lh_ui_renderer3d_camera_init builds it from a
 * position and two directions; ::lh_ui_renderer3d_camera_to_view takes a world
 * point into its space, the space the renderer clips and projects in: `x` to
 * the right, `y` up, `z` forward along the view.
 */

#ifndef LH_UI_RENDERER3D_CAMERA_H
#define LH_UI_RENDERER3D_CAMERA_H

#include <lh/math/vec3.h>

/**
 * @struct lh_ui_renderer3d_camera
 * @brief Where the camera stands, where it looks, and the frustum it cuts.
 */
typedef struct lh_ui_renderer3d_camera
{
    /** @brief Eye position in world coordinates. */
    lh_math_vec3_t position;
    /** @brief Unit direction the camera looks in. */
    lh_math_vec3_t forward;
    /** @brief Unit direction to the right of the camera. */
    lh_math_vec3_t right;
    /** @brief Unit direction up from the camera. */
    lh_math_vec3_t up;
    /** @brief Tangent of half the vertical field of view. */
    lh_math_fscalar_t tangent;
    /** @brief Distance of the near clipping plane. */
    lh_math_fscalar_t near_plane;
    /** @brief Distance of the far clipping plane. */
    lh_math_fscalar_t far_plane;
} lh_ui_renderer3d_camera_t; /**< The camera a frame is drawn from. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Build an orthonormal camera.
 *
 * The forward and up directions are made perpendicular to each other and of unit
 * length; the world has no fixed up axis, so the camera supplies both. The vertical
 * field of view must be between `0.1` and `3` radians.
 *
 * @param self Camera to fill.
 * @param position Eye position in world coordinates.
 * @param forward Direction the camera looks in; need not be normalized.
 * @param up Direction up from the camera; need not be perpendicular to @p forward.
 * @param vertical_fov Vertical field of view in radians.
 * @param near_plane Distance of the near clipping plane; above `0`.
 * @param far_plane Distance of the far clipping plane; above @p near_plane.
 * @return ::lh_bool_true on success; ::lh_bool_false for a non-finite or degenerate
 *         input, with @p self untouched.
 */
lh_bool_t
lh_ui_renderer3d_camera_init(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t position,
                             lh_math_vec3_t forward, lh_math_vec3_t up,
                             lh_math_fscalar_t vertical_fov, lh_math_fscalar_t near_plane,
                             lh_math_fscalar_t far_plane);

/**
 * @brief Whether @p self can draw: a finite, positive tangent and
 *        `0 < near_plane < far_plane`.
 *
 * True for every camera ::lh_ui_renderer3d_camera_init filled; a camera filled
 * by hand is checked with it before a frame starts.
 *
 * @param self Camera to check; must not be null.
 */
lh_bool_t
lh_ui_renderer3d_camera_is_usable(const lh_ui_renderer3d_camera_t *self);

/**
 * @brief @p world in the camera's space: `x` along right, `y` along up, `z`
 *        along forward, measured from the eye.
 *
 * Composed of ::lh_math_vec3_sub and ::lh_math_vec3_dot.
 *
 * @param self Camera to look from; must not be null.
 * @param world Point in world coordinates.
 * @param out Receives the point in camera space.
 */
lh_void
lh_ui_renderer3d_camera_to_view(const lh_ui_renderer3d_camera_t *self, lh_math_vec3_t world,
                                lh_math_vec3_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RENDERER3D_CAMERA_H */
