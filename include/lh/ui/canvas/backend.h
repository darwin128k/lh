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
#include <lh/size.h>
#include <lh/ui/canvas/backend/fields.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/shadow.h>
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
 *
 * **A backend shows what it drew and nothing else.** The canvas clears and draws
 * the damage, which in a partial frame is smaller than the buffer `begin_area`
 * asked for, and a buffer is not required to hold anything else: the pixels a
 * frame never wrote are whatever the buffer held before. Presenting all of it
 * puts them on screen, which is how a strip's leftover picture turns up beside
 * the widget that moved.
 *
 * A backend with a `set_clip` slot knows its drawn region — the union of the
 * clips it was handed, because every clip inside a frame is an intersection
 * with the frame's own — and presents that. One without cannot ask, and presents
 * its whole buffer.
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

/**
 * @typedef lh_ui_canvas_shadow_fn
 * @brief Paint @p shadow behind @p rect, corners rounded by @p radius.
 *
 * @p radius is already clamped by the canvas. The shadow reaches a spread past
 * the box — that is the point of it — so the backend paints outside @p rect and
 * cuts that to its own target like any other write.
 *
 * @return ::lh_bool_false when the backend painted nothing; the canvas then
 *         paints the shadow itself, pixel by pixel.
 */
typedef lh_bool_t(lh_ui_canvas_shadow_fn)(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                         const lh_ui_shadow_t *shadow);

/**
 * @typedef lh_ui_canvas_blur_fn
 * @brief Soften what is already on @p rect of the target, @p blur_radius out.
 *
 * Unlike every other slot this one reads pixels it did not write, so there is
 * nothing for the canvas to do instead: with no slot the effect simply cannot be
 * drawn, and the canvas says so rather than putting down something that only
 * looks like one. That also makes it the one slot that needs memory of its own:
 * @p scratch is the caller's (see ::lh_ui_canvas_set_scratch) and @p bytes how
 * much of it there is; a blur needs ::lh_ui_blur_scratch_size of @p rect, and a
 * backend that is given less returns ::lh_bool_false instead of writing past it.
 */
typedef lh_bool_t(lh_ui_canvas_blur_fn)(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius,
                                         lh_u8_t *scratch, lh_usize_t bytes);

/**
 * @typedef lh_ui_canvas_glass_fn
 * @brief A glass panel over @p rect: blur what is behind it by @p blur_radius,
 *        then lay @p tint over it inside corners rounded by @p corner.
 *
 * Two effects in one call because they are one look: a blurred rectangle without
 * its tint is a smudge, and a tint without the blur is a fill. @p scratch and
 * @p bytes are the caller's, exactly as for ::lh_ui_canvas_blur_fn.
 */
typedef lh_bool_t(lh_ui_canvas_glass_fn)(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                                        lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint, lh_u8_t *scratch,
                                        lh_usize_t bytes);

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
                                lh_ui_canvas_fill_mask_fn, lh_ui_canvas_shadow_fn, lh_ui_canvas_blur_fn,
                                lh_ui_canvas_glass_fn);
};
typedef struct lh_ui_canvas_backend lh_ui_canvas_backend_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Backend that accepts every call and draws nothing.
 */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_null;

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_BACKEND_H */
