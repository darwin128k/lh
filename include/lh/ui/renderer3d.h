/**
 * @file renderer3d.h
 * @brief Allocation-free software triangle renderer into an ARGB8888 pixmap.
 *
 * This brick knows geometry, a camera and depth, not maps, games or windows.
 * The caller owns the pixels and a float depth buffer. Requires UI and MATH_FPU.
 * World coordinates have no fixed up axis: the camera supplies forward and up.
 *
 * A triangle goes through public bricks, each usable on its own:
 * ::lh_ui_renderer3d_camera_to_view (world to camera space),
 * ::lh_ui_renderer3d_clip_plane once per ::lh_ui_renderer3d_plane_t,
 * ::lh_ui_renderer3d_project (camera space to pixels), then
 * ::lh_ui_renderer3d_raster, which tests ::lh_ui_renderer3d_get_edge per pixel
 * and writes through ::lh_ui_renderer3d_put_fragment.
 */

#ifndef LH_UI_RENDERER3D_H
#define LH_UI_RENDERER3D_H

#include <lh/config.h>
#include <lh/math/vec3.h>
#include <lh/ui/pixmap.h>
#include <lh/size.h>
#include <lh/ui/renderer3d/camera.h>
#include <lh/ui/renderer3d/plane.h>

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

/**
 * @brief Fill the whole target with @p background_argb and the depth buffer with
 *        ::LH_FLOAT_T_MAX.
 *
 * @param self Renderer whose target, depth, width and height are set.
 * @param background_argb Color the target is cleared to.
 * @return ::lh_bool_false when a target row cannot be read; the renderer then
 *         holds no target and drops every draw.
 */
lh_bool_t
lh_ui_renderer3d_clear(lh_ui_renderer3d_t *self, lh_u32_t background_argb);

/**
 * @brief Signed distance of camera-space point @p view to @p plane.
 *
 * Positive on the inside of the frustum, negative outside. The side planes
 * lean by the camera tangent vertically and by the tangent times the target's
 * width over height horizontally.
 *
 * @param self Renderer with a frame in progress.
 * @param view Point in camera space (::lh_ui_renderer3d_camera_to_view).
 * @param plane Plane to measure against.
 */
lh_math_fscalar_t
lh_ui_renderer3d_get_plane_distance(const lh_ui_renderer3d_t *self, lh_math_vec3_t view,
                                    lh_ui_renderer3d_plane_t plane);

/**
 * @brief Clip the convex polygon @p in against one frustum plane
 *        (Sutherland-Hodgman).
 *
 * Keeps the vertices inside @p plane and adds the point where each edge crosses
 * it (::lh_math_vec3_lerp). @p in and @p out must not overlap.
 *
 * @param self Renderer with a frame in progress.
 * @param plane Plane to clip against.
 * @param in @p count camera-space vertices in order.
 * @param count Number of vertices in @p in.
 * @param out Receives the clipped polygon.
 * @param capacity Number of vertices @p out holds.
 * @return Number of vertices written (0 when nothing is inside), or `-1` when
 *         @p out is too small.
 */
lh_s32_t
lh_ui_renderer3d_clip_plane(const lh_ui_renderer3d_t *self, lh_ui_renderer3d_plane_t plane,
                            const lh_math_vec3_t *in, lh_s32_t count, lh_math_vec3_t *out,
                            lh_s32_t capacity);

/**
 * @brief Camera-space @p view in pixels: `x` and `y` on the target, `z` the
 *        inverse depth `1 / z`.
 *
 * @p view must lie in front of the near plane (`z > 0`).
 *
 * @param self Renderer with a frame in progress.
 * @param view Point in camera space.
 * @param out Receives the projected point.
 */
lh_void
lh_ui_renderer3d_project(const lh_ui_renderer3d_t *self, lh_math_vec3_t view,
                         lh_math_vec3_t *out);

/**
 * @brief Edge function of pixel point (@p x, @p y) against the edge @p a → @p b.
 *
 * Twice the signed area of the triangle `a, b, (x, y)`: zero on the edge's line,
 * opposite signs on its two sides. Only `x` and `y` of @p a and @p b are used.
 */
lh_math_fscalar_t
lh_ui_renderer3d_get_edge(lh_math_vec3_t a, lh_math_vec3_t b, lh_math_fscalar_t x,
                          lh_math_fscalar_t y);

/**
 * @brief Depth-test one pixel and write it when it is nearer.
 *
 * @param self Renderer with a frame in progress.
 * @param x Column, inside the target.
 * @param y Row, inside the target.
 * @param depth Camera-space depth of the fragment.
 * @param color_argb Color written when the test passes.
 * @return ::lh_bool_true when the pixel was written.
 */
lh_bool_t
lh_ui_renderer3d_put_fragment(lh_ui_renderer3d_t *self, lh_s32_t x, lh_s32_t y,
                              lh_math_fscalar_t depth, lh_u32_t color_argb);

/**
 * @brief Fill the projected triangle @p a, @p b, @p c (::lh_ui_renderer3d_project).
 *
 * Covers the pixel centres inside all three edges, interpolates the inverse
 * depth and writes each through ::lh_ui_renderer3d_put_fragment. A degenerate
 * or non-finite triangle draws nothing and is not counted.
 *
 * @param self Renderer with a frame in progress.
 * @param a First projected vertex.
 * @param b Second projected vertex.
 * @param c Third projected vertex.
 * @param color_argb Flat color the triangle is filled with.
 */
lh_void
lh_ui_renderer3d_raster(lh_ui_renderer3d_t *self, lh_math_vec3_t a, lh_math_vec3_t b,
                        lh_math_vec3_t c, lh_u32_t color_argb);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RENDERER3D_H */
