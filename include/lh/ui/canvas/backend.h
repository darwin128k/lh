/**
 * @file backend.h
 * @brief One pixel sink: ::lh_ui_canvas_backend_t (function table, no state).
 *
 * GDI, GL, software and STM32 paths each ship one of these, wherever they
 * live (`lh/os`, an application). ::lh_ui_canvas_t binds a backend to a
 * context and dispatches through it.
 */

#ifndef LH_UI_CANVAS_BACKEND_H
#define LH_UI_CANVAS_BACKEND_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/canvas/backend/fields.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_ui_canvas_begin_fn
 * @brief Start a frame on @p context.
 */
typedef lh_void(lh_ui_canvas_begin_fn)(lh_ptr context);

/**
 * @typedef lh_ui_canvas_begin_area_fn
 * @brief Start a frame for @p area alone: its top-left pixel is the backend's
 *        `(0, 0)`.
 *
 * @p area is in target space, on whole pixels. The canvas sends every
 * primitive, clip and rounded cut already moved into that buffer space, so the
 * backend draws it as if the area were the whole target, and presents it back
 * to @p area in `end`. A backend whose buffer is @p area sized (or smaller) is
 * how a partial renderer keeps its memory down.
 *
 * Optional: without it the canvas calls ::lh_ui_canvas_begin_fn and the backend
 * draws the whole target.
 */
typedef lh_void(lh_ui_canvas_begin_area_fn)(lh_ptr context, const lh_ui_rect_t *area);

/**
 * @typedef lh_ui_canvas_end_fn
 * @brief Finish a frame on @p context (present / flush).
 */
typedef lh_void(lh_ui_canvas_end_fn)(lh_ptr context);

/**
 * @typedef lh_ui_canvas_clear_fn
 * @brief Fill the whole target with @p color.
 */
typedef lh_void(lh_ui_canvas_clear_fn)(lh_ptr context, const lh_ui_color_t *color);

/**
 * @typedef lh_ui_canvas_fill_rect_fn
 * @brief Fill @p rect with the solid @p color (axis-aligned, half-open).
 */
typedef lh_void(lh_ui_canvas_fill_rect_fn)(lh_ptr context, const lh_ui_rect_t *rect,
                                           const lh_ui_color_t *color);

/**
 * @typedef lh_ui_canvas_fill_round_rect_fn
 * @brief Fill @p rect with @p color, corners rounded by @p radius, anti-aliased.
 *
 * @p radius is already clamped by the canvas: `1 .. min(w, h) / 2`.
 *
 * @return ::lh_bool_false when the backend drew nothing (e.g. its renderer is
 *         not available this frame); the canvas then draws the shape itself.
 */
typedef lh_bool_t(lh_ui_canvas_fill_round_rect_fn)(lh_ptr context, const lh_ui_rect_t *rect,
                                                 lh_ui_scalar_t radius, const lh_ui_color_t *color);

/**
 * @typedef lh_ui_canvas_set_clip_fn
 * @brief Cut every later primitive to @p clip, in target space: its rect,
 *        and the rounded cuts on it (::lh_ui_canvas_clip_coverage).
 *
 * ::lh_null removes the cut. An empty clip rect cuts everything away. The
 * canvas calls it from ::lh_ui_canvas_push and ::lh_ui_canvas_pop whenever
 * the clip changes; primitives then arrive uncut. The rounds @p clip points
 * at belong to the canvas and stay valid until the next call.
 */
typedef lh_void(lh_ui_canvas_set_clip_fn)(lh_ptr context, const lh_ui_canvas_clip_t *clip);

/**
 * @typedef lh_ui_canvas_fill_mask_fn
 * @brief Paint @p mask in @p color, its top-left pixel at @p origin.
 *
 * @p origin is in target space, on whole pixels. Each pixel is @p color with
 * its alpha scaled by the mask coverage. The canvas sends only masks it need
 * not cut itself.
 *
 * @return ::lh_bool_false when the backend drew nothing; the canvas then
 *         paints the mask itself.
 */
typedef lh_bool_t(lh_ui_canvas_fill_mask_fn)(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                                           const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_ui_canvas_backend
 * @typedef lh_ui_canvas_backend_t
 * @brief Function table for one renderer implementation.
 *
 * Usually a static const; not owned by ::lh_ui_canvas_t. A slot may be
 * ::lh_null until that primitive exists; dispatch skips null slots.
 */
struct lh_ui_canvas_backend
{
    lh_ui_canvas_backend_fields(lh_ui_canvas_begin_fn, lh_ui_canvas_begin_area_fn, lh_ui_canvas_end_fn,
                                lh_ui_canvas_clear_fn, lh_ui_canvas_fill_rect_fn,
                                lh_ui_canvas_fill_round_rect_fn, lh_ui_canvas_set_clip_fn,
                                lh_ui_canvas_fill_mask_fn);
};
typedef struct lh_ui_canvas_backend lh_ui_canvas_backend_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Backend that accepts every call and draws nothing.
 */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_null;

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_BACKEND_H */
