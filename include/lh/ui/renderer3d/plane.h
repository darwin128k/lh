/**
 * @file plane.h
 * @brief ::lh_ui_renderer3d_plane_t — the six planes of the view frustum.
 *
 * A triangle is clipped against each in turn (::lh_ui_renderer3d_clip_plane);
 * the side a point is on is the sign of ::lh_ui_renderer3d_get_plane_distance.
 */

#ifndef LH_UI_RENDERER3D_PLANE_H
#define LH_UI_RENDERER3D_PLANE_H

/**
 * @enum lh_ui_renderer3d_plane
 * @brief One plane of the view frustum, in camera space.
 */
typedef enum lh_ui_renderer3d_plane
{
    lh_ui_renderer3d_plane_near = 0,   /**< `z = near`: nothing closer than the near plane. */
    lh_ui_renderer3d_plane_far = 1,    /**< `z = far`: nothing farther than the far plane. */
    lh_ui_renderer3d_plane_left = 2,   /**< The left edge of the field of view. */
    lh_ui_renderer3d_plane_right = 3,  /**< The right edge of the field of view. */
    lh_ui_renderer3d_plane_bottom = 4, /**< The bottom edge of the field of view. */
    lh_ui_renderer3d_plane_top = 5,    /**< The top edge of the field of view. */
    lh_ui_renderer3d_plane_count = 6   /**< Number of planes; not a plane. */
} lh_ui_renderer3d_plane_t;            /**< One plane of the view frustum. */

#endif /* LH_UI_RENDERER3D_PLANE_H */
