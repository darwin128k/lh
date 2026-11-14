/**
 * @file backend.h
 * @brief The opaque drawing target of a canvas: ::lh_gl_canvas_backend_t.
 *
 * A table of calls and nothing else. The canvas translates shapes into root
 * coordinates, drops those that miss the clip, and calls the table with the
 * clip still in force. The target behind the table owns the pixels. A null
 * call means that shape is not drawn.
 */

#ifndef LH_GL_CANVAS_BACKEND_H
#define LH_GL_CANVAS_BACKEND_H

#include <lh/core/ptr.h>
#include <lh/gl/pen.h>
#include <lh/gl/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p rect in root coordinates. @p clip is the canvas clip.
 *        Both are half-open. @p rect already meets @p clip.
 */
typedef lh_void(lh_gl_canvas_fill_rect_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                           const lh_gl_rect_t *rect, const lh_gl_color_t *color);

/**
 * @brief Stroke the border of @p rect with @p pen, in root coordinates.
 *        @p rect is the shape, not the pen's outer bounds.
 */
typedef lh_void(lh_gl_canvas_stroke_rect_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                             const lh_gl_rect_t *rect, const lh_gl_pen_t *pen);

/**
 * @brief Fill the circle of @p radius around @p center, in root coordinates.
 */
typedef lh_void(lh_gl_canvas_fill_circle_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                             lh_gl_point_t center, lh_gl_scalar_t radius,
                                             const lh_gl_color_t *color);

/** @brief Stroke that circle with @p pen. */
typedef lh_void(lh_gl_canvas_stroke_circle_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                               lh_gl_point_t center, lh_gl_scalar_t radius,
                                               const lh_gl_pen_t *pen);

/**
 * @brief Fill @p rect with corners of @p radius, in root coordinates.
 *        @p radius is already limited to half the shorter side.
 */
typedef lh_void(lh_gl_canvas_fill_round_rect_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                                 const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                                 const lh_gl_color_t *color);

/** @brief Stroke that rounded rectangle with @p pen. */
typedef lh_void(lh_gl_canvas_stroke_round_rect_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                                   const lh_gl_rect_t *rect,
                                                   lh_gl_scalar_t radius, const lh_gl_pen_t *pen);

/**
 * @brief Fill the triangle @p a @p b @p c, in root coordinates.
 */
typedef lh_void(lh_gl_canvas_fill_triangle_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                               const lh_gl_point_t *a, const lh_gl_point_t *b,
                                               const lh_gl_point_t *c, const lh_gl_color_t *color);

/**
 * @brief Paint @p count colors along one row, in root coordinates.
 *        @p colors[i] is pixel @p x + i on row @p y. @p clip still applies.
 */
typedef lh_void(lh_gl_canvas_fill_colors_fn)(lh_ptr target, const lh_gl_rect_t *clip,
                                             lh_gl_scalar_t x, lh_gl_scalar_t y,
                                             const lh_gl_color_t *colors, lh_gl_scalar_t count);

LH_COMPILER_EXTERN_C_END

/**
 * @def lh_gl_canvas_backend_fields(...)
 * @brief The drawing calls. A null call skips that shape.
 */
#define lh_gl_canvas_backend_fields(fill_rect_fn, stroke_rect_fn, fill_circle_fn,                  \
                                    stroke_circle_fn, fill_round_rect_fn, stroke_round_rect_fn,   \
                                    fill_triangle_fn, fill_colors_fn)                              \
    fill_rect_fn *fill_rect;                                                                       \
    stroke_rect_fn *stroke_rect;                                                                   \
    fill_circle_fn *fill_circle;                                                                   \
    stroke_circle_fn *stroke_circle;                                                               \
    fill_round_rect_fn *fill_round_rect;                                                           \
    stroke_round_rect_fn *stroke_round_rect;                                                       \
    fill_triangle_fn *fill_triangle;                                                               \
    fill_colors_fn *fill_colors

/** @brief Calls a canvas forwards to its target. */
struct lh_gl_canvas_backend
{
    /** Members: ::lh_gl_canvas_backend_fields. */
    lh_gl_canvas_backend_fields(lh_gl_canvas_fill_rect_fn, lh_gl_canvas_stroke_rect_fn,
                                lh_gl_canvas_fill_circle_fn, lh_gl_canvas_stroke_circle_fn,
                                lh_gl_canvas_fill_round_rect_fn, lh_gl_canvas_stroke_round_rect_fn,
                                lh_gl_canvas_fill_triangle_fn, lh_gl_canvas_fill_colors_fn);
};
/** @copybrief lh_gl_canvas_backend */
typedef struct lh_gl_canvas_backend lh_gl_canvas_backend_t;

/**
 * @brief Brace initializer in field order. A null call skips that shape.
 */
#define lh_gl_canvas_backend_initializer(fill_rect, stroke_rect, fill_circle, stroke_circle,       \
                                         fill_round_rect, stroke_round_rect, fill_triangle,        \
                                         fill_colors)                                              \
    lh_initializer(fill_rect, stroke_rect, fill_circle, stroke_circle, fill_round_rect,            \
                   stroke_round_rect, fill_triangle, fill_colors)

LH_COMPILER_EXTERN_C_BEGIN

lh_gl_canvas_fill_rect_fn *
lh_gl_canvas_backend_get_fill_rect(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_fill_rect(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_rect_fn *fn);

lh_gl_canvas_stroke_rect_fn *
lh_gl_canvas_backend_get_stroke_rect(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_stroke_rect(lh_gl_canvas_backend_t *self, lh_gl_canvas_stroke_rect_fn *fn);

lh_gl_canvas_fill_circle_fn *
lh_gl_canvas_backend_get_fill_circle(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_fill_circle(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_circle_fn *fn);

lh_gl_canvas_stroke_circle_fn *
lh_gl_canvas_backend_get_stroke_circle(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_stroke_circle(lh_gl_canvas_backend_t *self,
                                       lh_gl_canvas_stroke_circle_fn *fn);

lh_gl_canvas_fill_round_rect_fn *
lh_gl_canvas_backend_get_fill_round_rect(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_fill_round_rect(lh_gl_canvas_backend_t *self,
                                         lh_gl_canvas_fill_round_rect_fn *fn);

lh_gl_canvas_stroke_round_rect_fn *
lh_gl_canvas_backend_get_stroke_round_rect(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_stroke_round_rect(lh_gl_canvas_backend_t *self,
                                           lh_gl_canvas_stroke_round_rect_fn *fn);

lh_gl_canvas_fill_triangle_fn *
lh_gl_canvas_backend_get_fill_triangle(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_fill_triangle(lh_gl_canvas_backend_t *self,
                                       lh_gl_canvas_fill_triangle_fn *fn);

lh_gl_canvas_fill_colors_fn *
lh_gl_canvas_backend_get_fill_colors(const lh_gl_canvas_backend_t *self);

lh_void
lh_gl_canvas_backend_set_fill_colors(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_colors_fn *fn);

/**
 * @brief Store the calls. Any of them may be null.
 */
lh_void
lh_gl_canvas_backend_init(lh_gl_canvas_backend_t *self, lh_gl_canvas_fill_rect_fn *fill_rect,
                          lh_gl_canvas_stroke_rect_fn *stroke_rect,
                          lh_gl_canvas_fill_circle_fn *fill_circle,
                          lh_gl_canvas_stroke_circle_fn *stroke_circle,
                          lh_gl_canvas_fill_round_rect_fn *fill_round_rect,
                          lh_gl_canvas_stroke_round_rect_fn *stroke_round_rect,
                          lh_gl_canvas_fill_triangle_fn *fill_triangle,
                          lh_gl_canvas_fill_colors_fn *fill_colors);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_CANVAS_BACKEND_H */
