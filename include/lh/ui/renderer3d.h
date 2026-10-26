/**
 * @file renderer3d.h
 * @brief Allocation-free software triangle renderer into an ARGB8888 pixmap.
 *
 * This brick knows geometry, a camera and depth, not maps, games or windows.
 * The caller owns the pixels and a float depth buffer. Requires UI and MATH_FPU.
 * World coordinates have no fixed up axis: the camera supplies forward and up.
 */
#ifndef LH_UI_RENDERER3D_H
#define LH_UI_RENDERER3D_H
#include <lh/config.h>
#include <lh/math/vec3.h>
#include <lh/ui/pixmap.h>
#include <lh/size.h>
#if !LH_LIBRARY_OPTION_UI || !LH_LIBRARY_OPTION_MATH_FPU
#    error "renderer3d requires LH_LIBRARY_OPTION_UI and LH_LIBRARY_OPTION_MATH_FPU"
#endif
typedef struct lh_ui_renderer3d_camera
{
    lh_math_vec3_t position, forward, right, up;
    lh_math_fscalar_t tangent, near_plane, far_plane;
} lh_ui_renderer3d_camera_t;
typedef struct lh_ui_renderer3d
{
    lh_ui_pixmap_t *target;
    lh_math_fscalar_t *depth;
    lh_ui_renderer3d_camera_t camera;
    lh_s32_t width, height;
    lh_u32_t triangles;
} lh_ui_renderer3d_t;
LH_COMPILER_EXTERN_C_BEGIN
/** Build an orthonormal camera; false for non-finite or degenerate inputs.
 * @p vertical_fov is radians; near > 0, far > near. */
lh_bool_t
lh_ui_renderer3d_camera_init(lh_ui_renderer3d_camera_t *self, lh_math_vec3_t position,
                             lh_math_vec3_t forward, lh_math_vec3_t up,
                             lh_math_fscalar_t vertical_fov, lh_math_fscalar_t near_plane,
                             lh_math_fscalar_t far_plane);
/** Clear @p target and depth and start a frame. No allocations. On failure the
 * renderer is disabled. @p depth_count counts floats, not bytes. */
lh_bool_t
lh_ui_renderer3d_begin(lh_ui_renderer3d_t *self, lh_ui_pixmap_t *target, lh_math_fscalar_t *depth,
                       lh_usize_t depth_count, const lh_ui_renderer3d_camera_t *camera,
                       lh_u32_t background_argb);
/** Draw a double-sided flat-color world triangle. Clips against all six frustum
 * planes and depth-tests each fragment. Non-finite vertices are discarded. */
lh_void
lh_ui_renderer3d_triangle(lh_ui_renderer3d_t *self, lh_math_vec3_t a, lh_math_vec3_t b,
                          lh_math_vec3_t c, lh_u32_t color_argb);
/** Draw a solid axis-aligned box, useful for editor markers and simple entities. */
lh_void
lh_ui_renderer3d_box(lh_ui_renderer3d_t *self, lh_math_vec3_t minimum, lh_math_vec3_t maximum,
                     lh_u32_t color_argb);
/** Number of rasterized triangles after clipping (including box triangles). */
lh_u32_t
lh_ui_renderer3d_get_triangles(const lh_ui_renderer3d_t *self);
LH_COMPILER_EXTERN_C_END
#endif
