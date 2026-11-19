/**
 * @file web.h
 * @brief The canvas as JSON for a browser: ::lh_gx_backend_web_t.
 *
 * A backend of the canvas contract (`lh/gx/canvas.h`) that draws nothing: it
 * writes every call it gets as one JSON command, and the browser replays the
 * commands on its own canvas. The renderer stays in C — the damage, the walk,
 * the clip, the classes — and the browser only knows the canvas's few calls,
 * so the two sides cannot drift apart (`docs/design/gx.md`, section 5).
 *
 * A frame is one JSON array of commands, opened by ::lh_gx_backend_web_begin
 * and closed by ::lh_gx_backend_web_end. A command is an array whose first
 * element is the name of the canvas slot; its arguments follow in the slot's
 * order. A rectangle is `[x, y, width, height]` in whole screen pixels, a
 * colour the 32-bit ARGB value as one number:
 *
 * - `["fill_rect", clip, rect, color]`
 * - `["fill_mask", clip, rect, color, "coverage"]` — `rect` is where the mask
 *   lands and how big it is; the coverage is its `a8` pixels row by row, two
 *   hex digits a pixel, with no gap between the rows.
 *
 * A frame with one white rectangle:
 * `[["fill_rect",[0,0,64,32],[0,0,64,32],4294967295]]`.
 *
 * A call whose shape lies wholly outside its clip is not written: the browser
 * would draw nothing for it. Nothing else is skipped or remembered; a glyph is
 * sent again every time it is drawn.
 *
 * Ownership: the backend is the application's, as are the writer and the
 * string the writer appends to. The backend allocates nothing; the string
 * grows through its own allocator. The canvas table is filled by
 * ::lh_gx_backend_web_canvas_init, and the backend is the context every call
 * of the table gets.
 *
 * Example usage:
 * @code{.c}
 * lh_str_t text;
 * lh_web_json_writer_t writer;
 * lh_gx_backend_web_t web;
 * lh_gx_canvas_t canvas;
 * lh_gx_render_t render;
 * lh_gx_style_t style;
 * lh_gx_entity_t card;
 * lh_gx_damage_t drawn;
 *
 * lh_str_init(&text);
 * lh_web_json_writer_init(&writer, &text, 0U);
 * lh_gx_backend_web_init(&web, &writer);
 * lh_gx_backend_web_canvas_init(&canvas);
 * lh_gx_render_init(&render, &canvas, &web, 320, 240);
 * lh_gx_style_init(&style, 0xFF0000FFU, 0U, 0, 0, 0U, lh_null);
 * lh_gx_entity_init(&card, &lh_gx_box_class, &style, 10, 10, 100, 40);
 * lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &card);
 *
 * lh_gx_backend_web_begin(&web);
 * lh_gx_render_draw(&render, &drawn);
 * lh_gx_backend_web_end(&web);
 * // text: [["fill_rect",[10,10,100,40],[10,10,100,40],4278190335]]
 * @endcode
 */

#ifndef LH_GX_BACKEND_WEB_H
#define LH_GX_BACKEND_WEB_H

#include <lh/core/web/json/writer.h>
#include <lh/gx/backend/web/fields.h>
#include <lh/gx/canvas.h>

/**
 * @struct lh_gx_backend_web
 * @brief The writer the commands go through, and the count of the frame.
 */
struct lh_gx_backend_web
{
    /** Members: ::lh_gx_backend_web_fields. */
    lh_gx_backend_web_fields(lh_web_json_writer_t, lh_usize_t);
};
/** @copybrief lh_gx_backend_web */
typedef struct lh_gx_backend_web lh_gx_backend_web_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The writer the commands go through, to write with (level 1).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 * @return The writer; not owned.
 */
lh_web_json_writer_t *
lh_gx_backend_web_get_writer_mut(const lh_gx_backend_web_t *self);

/**
 * @brief Make @p writer the one the commands go through (level 1).
 *
 * @param self   The backend; checked by ::lh_assert_runtime_ref.
 * @param writer The writer; not owned, it must outlive the backend's use.
 */
lh_void
lh_gx_backend_web_set_writer(lh_gx_backend_web_t *self, lh_web_json_writer_t *writer);

/**
 * @brief How many commands the current frame holds (level 1).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 * @return The count since the last ::lh_gx_backend_web_begin.
 */
lh_usize_t
lh_gx_backend_web_get_count(const lh_gx_backend_web_t *self);

/**
 * @brief Store how many commands the current frame holds (level 1).
 *
 * @param self  The backend; checked by ::lh_assert_runtime_ref.
 * @param count The count.
 */
lh_void
lh_gx_backend_web_set_count(lh_gx_backend_web_t *self, lh_usize_t count);

/**
 * @brief A backend writing through @p writer, with no frame open.
 *
 * @param self   The backend to set up; checked by ::lh_assert_runtime_ref.
 * @param writer The writer; not owned. A compact layout (indent `0`) keeps
 *               the frames small, but any layout is valid JSON.
 */
lh_void
lh_gx_backend_web_init(lh_gx_backend_web_t *self, lh_web_json_writer_t *writer);

/**
 * @brief Open a frame: append `[` and start the count at `0`.
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gx_backend_web_begin(lh_gx_backend_web_t *self);

/**
 * @brief Close the frame: append `]`. The text then holds one whole JSON array.
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gx_backend_web_end(lh_gx_backend_web_t *self);

/**
 * @brief Open a command: a comma when it is not the frame's first, `[`, and
 *        @p name as a string. Counts the command.
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 * @param name The name of the canvas slot (`"fill_rect"`, ...).
 */
lh_void
lh_gx_backend_web_begin_command(lh_gx_backend_web_t *self, lh_str_view_t name);

/**
 * @brief Close the command: append `]`.
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gx_backend_web_end_command(lh_gx_backend_web_t *self);

/**
 * @brief Append @p rect as the command's next argument: a comma, then
 *        `[x, y, width, height]` in whole pixels (::lh_gx_rect_to_rect).
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 * @param rect The rectangle; must not be null.
 */
lh_void
lh_gx_backend_web_put_rect(lh_gx_backend_web_t *self, const lh_gx_rect_t *rect);

/**
 * @brief Append @p value as the command's next argument: a comma, then the
 *        number.
 *
 * @param self  The backend; checked by ::lh_assert_runtime_ref.
 * @param value The number.
 */
lh_void
lh_gx_backend_web_put_uint(lh_gx_backend_web_t *self, lh_u32_t value);

/**
 * @brief Append the pixels of @p mask as the command's next argument: a comma,
 *        then a string of two hex digits a pixel, row by row.
 *
 * Only `width` bytes of each row are written; the padding a stride leaves at
 * the end of a row is not.
 *
 * @param self The backend; checked by ::lh_assert_runtime_ref.
 * @param mask The coverage, format `a8`; must not be null.
 */
lh_void
lh_gx_backend_web_put_mask(lh_gx_backend_web_t *self, const lh_gx_pixmap_t *mask);

/**
 * @brief Append @p value, which may be below zero, as the command's next argument:
 *        a comma, then the number.
 *
 * @param self  The backend; checked by ::lh_assert_runtime_ref.
 * @param value The number.
 */
lh_void
lh_gx_backend_web_put_sint(lh_gx_backend_web_t *self, lh_s32_t value);

/**
 * @brief Append @p color as the command's next argument: a comma, then the
 *        ARGB value as a number (::lh_gx_backend_web_put_uint).
 *
 * @param self  The backend; checked by ::lh_assert_runtime_ref.
 * @param color The colour.
 */
lh_void
lh_gx_backend_web_put_color(lh_gx_backend_web_t *self, lh_gx_color_t color);

/**
 * @brief Append the gradient of @p brush as the command's next argument: a comma,
 *        then `["gradient", [x, y, width, height], shape, angle, frac, color, ...]`
 *        — the box it is laid over, `0` linear or `1` radial
 *        (::lh_pixel_gradient_shape_t), the angle in degrees, and each stop's
 *        position `0 .. 255` and ARGB colour. The player draws it as the
 *        browser's gradient laid the same way (`lh/core/pixel/gradient.h`).
 *
 * @param self  The backend; checked by ::lh_assert_runtime_ref.
 * @param brush A gradient brush; must not be null.
 */
lh_void
lh_gx_backend_web_put_gradient(lh_gx_backend_web_t *self, const lh_gx_brush_t *brush);

/**
 * @brief The canvas's `fill_shadow` (::lh_gx_canvas_fill_shadow_fn) as a
 *        `fill_shadow` command: the clip, the box, then the radius, the shift
 *        across and down, the spread and the colour, whole pixels — the player
 *        works every pixel's alpha out as the core does (`lh/core/pixel/shadow.h`).
 *
 * Nothing is written when the shadow's bounds do not meet @p clip.
 *
 * @param context The backend (::lh_gx_backend_web_t); must not be null.
 * @param clip    Where drawing is allowed; screen coordinates.
 * @param box     The casting box; screen coordinates.
 * @param radius  Its corners' radius, pixels.
 * @param shadow  The shadow.
 */
lh_void
lh_gx_backend_web_fill_shadow(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *box,
                              lh_gx_scalar_t radius, const lh_gx_shadow_t *shadow);

/**
 * @brief Append @p brush as the command's next argument: its colour as a number
 *        (::lh_gx_backend_web_put_color), or its gradient
 *        (::lh_gx_backend_web_put_gradient).
 *
 * @param self  The backend; checked by ::lh_assert_runtime_ref.
 * @param brush The brush; must not be null.
 */
lh_void
lh_gx_backend_web_put_brush(lh_gx_backend_web_t *self, const lh_gx_brush_t *brush);

/**
 * @brief The canvas's `fill_rect` (::lh_gx_canvas_fill_rect_fn) as a
 *        `fill_rect` command.
 *
 * Nothing is written when @p rect does not meet @p clip.
 *
 * @param context The backend (::lh_gx_backend_web_t); must not be null.
 * @param clip    Where drawing is allowed; screen coordinates.
 * @param rect    The rectangle; screen coordinates.
 * @param brush   What to fill with (::lh_gx_backend_web_put_brush).
 */
lh_void
lh_gx_backend_web_fill_rect(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_rect_t *rect,
                            const lh_gx_brush_t *brush);

/**
 * @brief The canvas's `fill_mask` (::lh_gx_canvas_fill_mask_fn) as a
 *        `fill_mask` command, the mask's pixels included.
 *
 * Nothing is written when the mask, placed at @p at, does not meet @p clip.
 *
 * @param context The backend (::lh_gx_backend_web_t); must not be null.
 * @param clip    Where drawing is allowed; screen coordinates.
 * @param mask    The coverage, format `a8`; the caller's.
 * @param at      Where the mask's top-left pixel lands; screen coordinates.
 * @param brush   What to fill with (::lh_gx_backend_web_put_brush).
 */
lh_void
lh_gx_backend_web_fill_mask(lh_ptr context, const lh_gx_rect_t *clip, const lh_gx_pixmap_t *mask,
                            lh_gx_point_t at, const lh_gx_brush_t *brush);

/**
 * @brief Fill @p table with this backend's calls: `fill_rect`, `fill_mask` and
 *        `fill_shadow`.
 *
 * @param table The table; checked by ::lh_gx_canvas_init.
 */
lh_void
lh_gx_backend_web_canvas_init(lh_gx_canvas_t *table);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BACKEND_WEB_H */
