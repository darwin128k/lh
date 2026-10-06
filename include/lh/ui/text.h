/**
 * @file text.h
 * @brief Measuring and drawing text with an ::lh_ui_font_t.
 *
 * Text is a `'\0'`-terminated string of lines split by `'\n'`. Codes are
 * read one at a time by ::lh_ui_text_next_code (bytes for now; UTF-8 later
 * changes that one function). Codes the font has no glyph for take no room.
 * Every text component (label, field, list, button) measures and draws
 * through here; drawing goes through ::lh_ui_canvas_fill_mask and skips
 * lines the canvas clip does not show.
 */

#ifndef LH_UI_TEXT_H
#define LH_UI_TEXT_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/font.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/* ── Reading ─────────────────────────────────────────────────────────────── */

/**
 * @brief The code at @p cursor, and @p cursor moved past it.
 *
 * At the terminating `'\0'` returns `0` and leaves @p cursor there. Bytes
 * for now: the one place to change for UTF-8.
 */
lh_u32_t
lh_ui_text_next_code(const lh_char_t **cursor);

/**
 * @brief True when @p code ends a line: `'\n'` or the terminating `0`.
 */
lh_bool_t
lh_ui_text_is_line_end(lh_u32_t code);

/**
 * @brief Where the line starting at @p line ends: its `'\n'` or the `'\0'`.
 */
const lh_char_t *
lh_ui_text_get_line_end(const lh_char_t *line);

/**
 * @brief Start of the line after @p line, or ::lh_null when @p line is last.
 */
const lh_char_t *
lh_ui_text_get_next_line(const lh_char_t *line);

/**
 * @brief Number of lines in @p text: `""` is one empty line, ::lh_null none.
 */
lh_u32_t
lh_ui_text_count_lines(const lh_char_t *text);

/* ── Measuring ───────────────────────────────────────────────────────────── */

/**
 * @brief Width of the line starting at @p line: the sum of its advances.
 */
lh_ui_scalar_t
lh_ui_text_get_line_width(const lh_ui_font_t *font, const lh_char_t *line);

/**
 * @brief Width of @p text: its widest line.
 */
lh_ui_scalar_t
lh_ui_text_get_width(const lh_ui_font_t *font, const lh_char_t *text);

/**
 * @brief Size of @p text: its width, and its lines times the line height.
 */
lh_ui_size_t
lh_ui_text_get_size(const lh_ui_font_t *font, const lh_char_t *text);

/**
 * @brief The rect @p text covers when it starts at @p origin.
 */
lh_ui_rect_t
lh_ui_text_get_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin);

/**
 * @brief The rect the line starting at @p line covers at @p origin.
 */
lh_ui_rect_t
lh_ui_text_get_line_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin);

/* ── Drawing ─────────────────────────────────────────────────────────────── */

/**
 * @brief Draw the glyph of @p code with its cell at @p origin.
 *
 * @return The advance of @p code (`0` without a glyph).
 */
lh_ui_scalar_t
lh_ui_text_draw_code(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, lh_u32_t code, lh_ui_point_t origin,
                     const lh_ui_color_t *color);

/**
 * @brief Draw the line starting at @p line at @p origin; nothing when the
 *        canvas clip shows none of it.
 */
lh_void
lh_ui_text_draw_line(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *line,
                     lh_ui_point_t origin, const lh_ui_color_t *color);

/**
 * @brief Draw every line of @p text, the first at @p origin, each next one a
 *        line height lower.
 */
lh_void
lh_ui_text_draw(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TEXT_H */
