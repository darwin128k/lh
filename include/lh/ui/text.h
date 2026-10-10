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
#include <lh/numeric/types.h>
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
 * @brief How many bytes @p code takes in UTF-8: 0 when it is not encodable.
 *
 * A code point is encodable when it is not a surrogate (`0xD800..0xDFFF`), not
 * above `0x10FFFF`, and not `0` — which is the terminator, and writing it into a
 * buffer as if it were a character would end the text in the middle of it.
 */
lh_u32_t
lh_ui_text_encoded_size(lh_u32_t code);

/**
 * @brief How many bytes the character whose **first byte** is @p lead takes in
 *        UTF-8: 1, 2, 3 or 4.
 *
 * Not the same question as ::lh_ui_text_encoded_size, and answering one with the other
 * is a bug that only shows up outside ASCII. That one takes a **code point**; this one
 * takes the **lead byte**, which is what a walk over a buffer somebody else filled in
 * is actually holding.
 *
 * The two agree by coincidence for every two-byte character -- `0xD0` as a code point
 * is below `0x800` and answers 2, which is right for the Cyrillic letter that `0xD0`
 * starts -- so a suite written with Cyrillic in it stays green over the wrong call.
 * `0xE0` starts a **three**-byte character and is itself a code point below `0x800`,
 * so the other function answers 2 and a field deleting that character cuts two bytes
 * of three and leaves the rest of a code point to be read as the next character.
 *
 * One byte for anything that is not a lead byte: a continuation byte where a lead
 * byte should be is a broken string, and stepping over more than one byte would walk
 * past a character somebody can see.
 */
lh_u32_t
lh_ui_text_lead_size(lh_byte_t lead);

/**
 * @brief Write @p code into @p out as UTF-8 and return how many bytes it took.
 *
 * The counterpart of ::lh_ui_text_next_code, and it belongs beside it rather than in
 * whatever component happens to type: an **input** needs it to put a typed code point
 * in its buffer, and a second implementation in that component would be the same
 * table of magic numbers written twice.
 *
 * Writes nothing and returns `0` when @p code is not encodable or @p bytes is too
 * few, so a buffer that is one byte short gets a refused character rather than half
 * of one — half a code point is not a character, and the next read over it would
 * walk off the end of the text.
 */
lh_u32_t
lh_ui_text_encode_code(lh_char_t *out, lh_u32_t bytes, lh_u32_t code);

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
 * @brief Size of @p text: its width, and how tall its ink is.
 *
 * The height is the ink, not the line box. A line box is taller than what is
 * drawn — Roboto 16 px is a 22 px line with a 17 px ascent, and a capital's ink
 * starts four rows down — so anything that centres *this* box was centring the
 * padding around the text and drew every caption a few pixels low. The width is
 * still the pen's, because a space takes room whether or not it is inked.
 */
lh_ui_size_t
lh_ui_text_get_size(const lh_ui_font_t *font, const lh_char_t *text);

/**
 * @brief How far below the top of a line box the ink of @p text starts.
 *
 * This is what a vertical alignment has to take back: the alignment places the
 * ink, and the line is drawn from a row this many above it. `0` for text with
 * no ink at all.
 */
lh_ui_scalar_t
lh_ui_text_get_ink_top(const lh_ui_font_t *font, const lh_char_t *text);

/**
 * @brief Where the ink of the line starting at @p line is, with its top-left
 *        cell at @p origin.
 *
 * Width is the line's advance width, height is from the top of the first inked
 * glyph to the bottom of the last. A line with no ink at all (a space, an empty
 * line) is a rect of no height.
 */
lh_ui_rect_t
lh_ui_text_get_line_ink_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin);

/**
 * @brief The rect the ink of @p text covers when its first line starts at
 *        @p origin.
 */
lh_ui_rect_t
lh_ui_text_get_ink_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin);

/**
 * @brief The rect @p text covers when it starts at @p origin: its ink, which is
 *        what ::lh_ui_text_draw draws into.
 */
lh_ui_rect_t
lh_ui_text_get_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin);

/**
 * @brief The line box the line starting at @p line covers at @p origin: as wide
 *        as its advances and as tall as ::lh_ui_font_get_line_height, which is
 *        what the next line is placed a line height below.
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
 *
 * @p origin is where the ink goes and every line box starts
 * ::lh_ui_text_get_ink_top rows above it, each next one a line height lower: the
 * pixels land exactly in ::lh_ui_text_get_ink_rect.
 */
lh_void
lh_ui_text_draw(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_text_draw with the ink shift already known.
 *
 * @p ink_top is what ::lh_ui_text_get_ink_top would return. A label that has
 * remembered it (::lh_ui_label_get_text_origin) draws without walking the
 * string to find it again. The pixels are the same as ::lh_ui_text_draw.
 */
lh_void
lh_ui_text_draw_from(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                     lh_ui_point_t origin, const lh_ui_color_t *color, lh_ui_scalar_t ink_top);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TEXT_H */
