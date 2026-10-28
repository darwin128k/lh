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
#include <lh/ui/renderer3d/camera.h>
#if !LH_LIBRARY_OPTION_UI || !LH_LIBRARY_OPTION_MATH_FPU
#    error "renderer3d requires LH_LIBRARY_OPTION_UI and LH_LIBRARY_OPTION_MATH_FPU"
#endif

/**
 * @struct lh_ui_renderer3d
 * @brief A frame in progress: the target, the depth buffer and the camera.
 */
typedef struct lh_ui_renderer3d
{
    /** @brief Caller-owned ARGB8888 target pixmap. */
    lh_ui_pixmap_t *target;
    /** @brief Caller-owned float depth buffer, width * height elements. */
    lh_math_fscalar_t *depth;
    /** @brief The camera the frame is drawn from. */
    lh_ui_renderer3d_camera_t camera;
    /** @brief Target width in pixels. */
    lh_s32_t width;
    /** @brief Target height in pixels. */
    lh_s32_t height;
    /** @brief Triangles rasterized so far this frame. */
    lh_u32_t triangles;
} lh_ui_renderer3d_t; /**< A frame in progress. */
LH_COMPILER_EXTERN_C_BEGIN
/**
 * @brief Clear the target and the depth buffer and start a frame.
 *
 * No allocations. On failure the renderer is disabled: it holds no target, and every
 * draw is dropped until a begin succeeds.
 *
 * @param self Renderer to start the frame in.
 * @param target Caller-owned ARGB8888 pixmap to draw into.
 * @param depth Caller-owned float depth buffer.
 * @param depth_count How many floats @p depth holds; it must cover width times height.
 * @param camera The camera to draw from.
 * @param background_argb Color the target is cleared to.
 * @return ::lh_bool_true on success; ::lh_bool_false when the target is not ARGB8888,
 *         is empty or larger than 8192 per axis, when @p depth is too small, or when
 *         @p camera is not usable.
 */
lh_bool_t
lh_ui_renderer3d_begin(lh_ui_renderer3d_t *self, lh_ui_pixmap_t *target, lh_math_fscalar_t *depth,
                       lh_usize_t depth_count, const lh_ui_renderer3d_camera_t *camera,
                       lh_u32_t background_argb);
/**
 * @brief Draw a double-sided flat-color world triangle.
 *
 * Clips against all six frustum planes and depth-tests each fragment. A triangle with
 * a non-finite vertex is discarded.
 *
 * @param self Renderer with a frame in progress.
 * @param a First vertex in world coordinates.
 * @param b Second vertex in world coordinates.
 * @param c Third vertex in world coordinates.
 * @param color_argb Flat color the triangle is filled with.
 */
lh_void
lh_ui_renderer3d_triangle(lh_ui_renderer3d_t *self, lh_math_vec3_t a, lh_math_vec3_t b,
                          lh_math_vec3_t c, lh_u32_t color_argb);
/**
 * @brief Draw a solid axis-aligned box, useful for editor markers and simple entities.
 *
 * The box is drawn as twelve triangles and counts as such.
 *
 * @param self Renderer with a frame in progress.
 * @param minimum Corner with the smallest coordinates.
 * @param maximum Corner with the largest coordinates.
 * @param color_argb Flat color the box is filled with.
 */
lh_void
lh_ui_renderer3d_box(lh_ui_renderer3d_t *self, lh_math_vec3_t minimum, lh_math_vec3_t maximum,
                     lh_u32_t color_argb);
/**
 * @brief Number of triangles rasterized since the frame began.
 *
 * @param self Renderer to read.
 * @return The count; `0` when @p self is null.
 */
lh_u32_t
lh_ui_renderer3d_get_triangles(const lh_ui_renderer3d_t *self);
LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RENDERER3D_H */
