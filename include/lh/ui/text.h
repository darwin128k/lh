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

#include <lh/ui/canvas.h>
#include <lh/ui/text/block.h>
#include <lh/ui/text/line.h>

/* -- Reading --------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The code at @p cursor, and @p cursor moved past it.
 *
 * At the terminating `'\0'` returns `0` and leaves @p cursor there. Bytes
 * for now: the one place to change for UTF-8.
 *
 * @param cursor Read cursor; moved past the code that was read.
 * @return The code, or `0` at the terminating `'\0'`.
 */
lh_u32_t
lh_ui_text_next_code(const lh_char_t **cursor);

/**
 * @brief True when @p code ends a line: `'\n'` or the terminating `0`.
 *
 * @param code The code to test.
 * @return ::lh_bool_true when @p code ends a line.
 */
lh_bool_t
lh_ui_text_is_line_end(lh_u32_t code);

/**
 * @brief Where the line starting at @p line ends: its `'\n'` or the `'\0'`.
 *
 * @param line First byte of the line.
 * @return The `'\n'` or the `'\0'` that ends the line.
 */
const lh_char_t *
lh_ui_text_get_line_end(const lh_char_t *line);

/**
 * @brief Start of the line after @p line, or ::lh_null when @p line is last.
 *
 * @param line First byte of the line.
 * @return The first byte of the next line, or ::lh_null after the last one.
 */
const lh_char_t *
lh_ui_text_get_next_line(const lh_char_t *line);

/**
 * @brief Number of lines in @p text: `""` is one empty line, ::lh_null none.
 *
 * @param text The text to count the lines of.
 * @return How many lines @p text has; `0` for ::lh_null.
 */
lh_u32_t
lh_ui_text_count_lines(const lh_char_t *text);

/* -- Measuring ------------------------------------------------------------- */

/**
 * @brief Width of the line starting at @p line: the sum of its advances.
 *
 * @param font The font the letters are measured in.
 * @param line First byte of the line.
 * @return The advance width of the line.
 */
lh_ui_scalar_t
lh_ui_text_get_line_width(const lh_ui_font_t *font, const lh_char_t *line);

/**
 * @brief Width of @p text: its widest line.
 *
 * @param font The font the letters are measured in.
 * @param text The text to measure.
 * @return The width of the widest line of @p text.
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
 *
 * @param font The font the letters are measured in.
 * @param text The text to measure.
 * @param out  Receives the size.
 */
lh_void
lh_ui_text_get_size(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_size_t *out);

/**
 * @brief How far below the top of a line box the ink of @p text starts.
 *
 * This is what a vertical alignment has to take back: the alignment places the
 * ink, and the line is drawn from a row this many above it. `0` for text with
 * no ink at all.
 *
 * @param font The font the letters are measured in.
 * @param text The text to measure.
 * @return The first ink row, down from the top of a line box.
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
 *
 * @param font   The font the letters are measured in.
 * @param line   First byte of the line.
 * @param origin Top-left of the line box.
 * @param out    Receives the rect.
 */
lh_void
lh_ui_text_get_line_ink_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin,
                             lh_ui_rect_t *out);

/**
 * @brief The rect the ink of @p text covers when its first line starts at
 *        @p origin.
 *
 * @param font   The font the letters are measured in.
 * @param text   The text to measure.
 * @param origin Where the ink of the first line goes.
 * @param out    Receives the rect.
 */
lh_void
lh_ui_text_get_ink_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                        lh_ui_rect_t *out);

/**
 * @brief The rect @p text covers when it starts at @p origin: its ink, which is
 *        what ::lh_ui_text_draw draws into.
 *
 * @param font   The font the letters are measured in.
 * @param text   The text to measure.
 * @param origin Where the ink of the first line goes.
 * @param out    Receives the rect.
 */
lh_void
lh_ui_text_get_rect(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_point_t origin,
                    lh_ui_rect_t *out);

/**
 * @brief The line box the line starting at @p line covers at @p origin: as wide
 *        as its advances and as tall as ::lh_ui_font_get_line_height, which is
 *        what the next line is placed a line height below.
 *
 * @param font   The font the letters are measured in.
 * @param line   First byte of the line.
 * @param origin Top-left of the line box.
 * @param out    Receives the rect.
 */
lh_void
lh_ui_text_get_line_rect(const lh_ui_font_t *font, const lh_char_t *line, lh_ui_point_t origin,
                         lh_ui_rect_t *out);

/* -- Drawing --------------------------------------------------------------- */

/**
 * @brief Draw the glyph at @p index (::lh_ui_font_find_index) with its cell at
 *        @p origin: what ::lh_ui_text_draw_code does once the code is looked up.
 *
 * @param canvas Canvas to draw into.
 * @param font   The font the glyph comes from.
 * @param index  Glyph index, from ::lh_ui_font_find_index.
 * @param origin Top-left of the line box the glyph sits in.
 * @param color  Color the glyph is drawn in.
 * @return The advance of the glyph.
 */
lh_ui_scalar_t
lh_ui_text_draw_glyph_at(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, lh_u32_t index,
                         lh_ui_point_t origin, const lh_ui_color_t *color);

/**
 * @brief Draw a line laid out by ::lh_ui_text_line_measure with its line box at
 *        @p origin; nothing when the clip misses its ink.
 *
 * @param canvas Canvas to draw into.
 * @param font   The font the letters are drawn in.
 * @param line   The laid-out line to draw.
 * @param origin Top-left of the line box.
 * @param color  Color the glyphs are drawn in.
 */
lh_void
lh_ui_text_draw_laid_line(lh_ui_canvas_t *canvas, const lh_ui_font_t *font,
                          const lh_ui_text_line_t *line, lh_ui_point_t origin,
                          const lh_ui_color_t *color);

/**
 * @brief Draw the glyph of @p code with its cell at @p origin.
 *
 * @param canvas Canvas to draw into.
 * @param font   The font the glyph comes from.
 * @param code   Code of the glyph to draw.
 * @param origin Top-left of the line box the glyph sits in.
 * @param color  Color the glyph is drawn in.
 * @return The advance of @p code (`0` without a glyph).
 */
lh_ui_scalar_t
lh_ui_text_draw_code(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, lh_u32_t code,
                     lh_ui_point_t origin, const lh_ui_color_t *color);

/**
 * @brief Draw the line starting at @p line at @p origin; nothing when the
 *        canvas clip shows none of it.
 *
 * @param canvas Canvas to draw into.
 * @param font   The font the letters are drawn in.
 * @param line   First byte of the line.
 * @param origin Top-left of the line box.
 * @param color  Color the glyphs are drawn in.
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
 *
 * @param canvas Canvas to draw into.
 * @param font   The font the letters are drawn in.
 * @param text   The text to draw.
 * @param origin Where the ink of the first line goes.
 * @param color  Color the glyphs are drawn in.
 */
lh_void
lh_ui_text_draw(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                lh_ui_point_t origin, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_text_draw with the ink shift already known.
 *
 * @p ink_top is what ::lh_ui_text_get_ink_top would return. A label that has
 * remembered it (::lh_ui_label_get_text_origin) draws without walking the
 * string to find it again. The pixels are the same as ::lh_ui_text_draw.
 *
 * @param canvas  Canvas to draw into.
 * @param font    The font the letters are drawn in.
 * @param text    The text to draw.
 * @param origin  Where the ink of the first line goes.
 * @param color   Color the glyphs are drawn in.
 * @param ink_top What ::lh_ui_text_get_ink_top would return.
 */
lh_void
lh_ui_text_draw_from(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                     lh_ui_point_t origin, const lh_ui_color_t *color, lh_ui_scalar_t ink_top);

/* -- Wrapped --------------------------------------------------------------- */

/*
 * The same measures and the same drawing for text broken into lines @p width wide:
 * at a line break, before a word that would not fit, or inside a word wider than the
 * whole width -- never before its first letter, so a line always takes something.
 * The spaces a line breaks at belong to neither line. A @p width of 0 or less breaks
 * at line breaks only, which is exactly the functions above; they are these with no
 * width.
 *
 * @p align places each line inside the widest one, so a centred paragraph has every
 * line centred, as CSS centres it, rather than the paragraph centred as one block with
 * its lines flush left inside it.
 */

/**
 * @brief ::lh_ui_text_get_size of @p text broken into lines @p width wide.
 *
 * @param font  The font the letters are measured in.
 * @param text  The text to measure.
 * @param width Line width to break at.
 * @param out   Receives the size.
 */
lh_void
lh_ui_text_get_size_wrapped(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_scalar_t width,
                            lh_ui_size_t *out);

/**
 * @brief ::lh_ui_text_get_ink_top of @p text broken into lines @p width wide.
 *
 * @param font  The font the letters are measured in.
 * @param text  The text to measure.
 * @param width Line width to break at.
 * @return The first ink row, down from the top of a line box.
 */
lh_ui_scalar_t
lh_ui_text_get_ink_top_wrapped(const lh_ui_font_t *font, const lh_char_t *text,
                               lh_ui_scalar_t width);

/**
 * @brief Width of the widest line of @p text broken into lines @p width wide
 *        (`0` or less: at line breaks only), the block an alignment works in.
 *
 * @param font  The font the letters are measured in.
 * @param text  The text to measure.
 * @param width Line width to break at.
 * @return The width of the widest line of @p text.
 */
lh_ui_scalar_t
lh_ui_text_get_wrapped_width(const lh_ui_font_t *font, const lh_char_t *text, lh_ui_scalar_t width);

/**
 * @brief How many lines @p text is broken into at @p width (`0` or less: at
 *        line breaks only, the same as ::lh_ui_text_count_lines).
 *
 * @param font  The font the letters are measured in.
 * @param text  The text to count the lines of.
 * @param width Line width to break at.
 * @return How many lines @p text breaks into.
 */
lh_u32_t
lh_ui_text_count_lines_wrapped(const lh_ui_font_t *font, const lh_char_t *text,
                               lh_ui_scalar_t width);

/**
 * @brief ::lh_ui_text_draw_wrapped with the first ink row already known: what a
 *        caller that measured the text once (a label) draws every frame with.
 *
 * @param canvas  Canvas to draw into.
 * @param font    The font the letters are drawn in.
 * @param text    The text to draw.
 * @param origin  Where the ink of the first line goes.
 * @param color   Color the glyphs are drawn in.
 * @param ink_top ::lh_ui_text_get_ink_top_wrapped of the same text and width.
 * @param width   Line width to break at.
 * @param align   Horizontal alignment of each line inside the widest one.
 */
lh_void
lh_ui_text_draw_wrapped_from(lh_ui_canvas_t *canvas, const lh_ui_font_t *font,
                             const lh_char_t *text, lh_ui_point_t origin,
                             const lh_ui_color_t *color, lh_ui_scalar_t ink_top,
                             lh_ui_scalar_t width, lh_ui_text_align_h_t align);

/**
 * @brief ::lh_ui_text_get_ink_rect of @p text broken into lines @p width wide, each
 *        placed by @p align inside the widest.
 *
 * @param font   The font the letters are measured in.
 * @param text   The text to measure.
 * @param origin Where the ink of the first line goes.
 * @param width  Line width to break at.
 * @param align  Horizontal alignment of each line inside the widest one.
 * @param out    Receives the rect.
 */
lh_void
lh_ui_text_get_ink_rect_wrapped(const lh_ui_font_t *font, const lh_char_t *text,
                                lh_ui_point_t origin, lh_ui_scalar_t width,
                                lh_ui_text_align_h_t align, lh_ui_rect_t *out);

/**
 * @brief ::lh_ui_text_draw of @p text broken into lines @p width wide, each placed
 *        by @p align inside the widest. The pixels land in
 *        ::lh_ui_text_get_ink_rect_wrapped.
 *
 * @param canvas Canvas to draw into.
 * @param font   The font the letters are drawn in.
 * @param text   The text to draw.
 * @param origin Where the ink of the first line goes.
 * @param color  Color the glyphs are drawn in.
 * @param width  Line width to break at.
 * @param align  Horizontal alignment of each line inside the widest one.
 */
lh_void
lh_ui_text_draw_wrapped(lh_ui_canvas_t *canvas, const lh_ui_font_t *font, const lh_char_t *text,
                        lh_ui_point_t origin, const lh_ui_color_t *color, lh_ui_scalar_t width,
                        lh_ui_text_align_h_t align);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TEXT_H */
