/**
 * @file line.h
 * @brief One laid-out line of a text: ::lh_ui_text_line_t.
 *
 * Where a line of a text ends, how wide it is and where its ink is are three
 * questions about the same letters, and asking them one walk at a time is what
 * made measuring a caption cost several walks of it. ::lh_ui_text_line_measure
 * answers all three, and where the next line starts, in one walk, with one font
 * lookup per letter (::lh_ui_font_find_index). Every measure and every draw in
 * `lh/ui/text.h` is built on it.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_TEXT_LINE_H
#define LH_UI_TEXT_LINE_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/font.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/text/align.h>
#include <lh/ui/text/line/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_text_line
 * @brief One laid-out line: its bytes, the next line's start, its width and its ink.
 */
struct lh_ui_text_line
{
    /** Members: ::lh_ui_text_line_fields. */
    lh_ui_text_line_fields(lh_char_t, lh_ui_scalar_t, lh_bool_t);
};
/** @copybrief lh_ui_text_line */
typedef struct lh_ui_text_line lh_ui_text_line_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An empty line: no bytes, no next line, nothing measured.
 */
lh_void
lh_ui_text_line_init(lh_ui_text_line_t *self);

/**
 * @brief Lay out the line of @p text that starts at @p text, broken at @p width.
 *
 * The line ends at a line end (`\n`, `\0`); with @p width above zero it also
 * ends where the next letter would pass @p width: at the last space before it
 * when there is one (a space may hang past the edge), else before that letter,
 * and a first letter wider than @p width still takes the line on its own. The
 * space a line broke at is not part of it, and the next line starts after the
 * spaces that follow it; a word cut in two goes on with the letter that did not fit.
 *
 * @param self  Line to fill.
 * @param font  Font the letters are measured in.
 * @param text  First byte of the line (UTF-8); ::lh_null is no line.
 * Only the layout: bytes, next line and width. The ink is
 * ::lh_ui_text_line_measure_ink, asked when it is wanted; until then the line
 * reads as not inked.
 *
 * @param width Line width to break at; `0` or less never breaks.
 * @return ::lh_bool_false, and @p self an empty line, when @p text is ::lh_null.
 */
lh_bool_t
lh_ui_text_line_measure(lh_ui_text_line_t *self, const lh_ui_font_t *font, const lh_char_t *text,
                        lh_ui_scalar_t width);

/**
 * @brief Measure which rows the letters of a laid-out line ink (its first and last
 *        ink row, and whether it has any), walking the bytes
 *        ::lh_ui_text_line_measure settled.
 *
 * @param font The font the line was laid out in.
 */
lh_void
lh_ui_text_line_measure_ink(lh_ui_text_line_t *self, const lh_ui_font_t *font);

/**
 * @brief First byte of the line.
 */
const lh_char_t *
lh_ui_text_line_get_begin(const lh_ui_text_line_t *self);

/**
 * @brief One past the last byte of the line.
 */
const lh_char_t *
lh_ui_text_line_get_end(const lh_ui_text_line_t *self);

/**
 * @brief Where the next line starts, or ::lh_null after the last line.
 */
const lh_char_t *
lh_ui_text_line_get_next(const lh_ui_text_line_t *self);

/**
 * @brief Pen advance over the line, spaces included.
 */
lh_ui_scalar_t
lh_ui_text_line_get_width(const lh_ui_text_line_t *self);

/**
 * @brief First ink row of the line, down from the top of its line box (`0` when
 *        nothing in it is inked).
 */
lh_ui_scalar_t
lh_ui_text_line_get_ink_top(const lh_ui_text_line_t *self);

/**
 * @brief One past the last ink row of the line, down from the top of its line box.
 */
lh_ui_scalar_t
lh_ui_text_line_get_ink_bottom(const lh_ui_text_line_t *self);

/**
 * @brief True when a glyph of the line has ink.
 */
lh_bool_t
lh_ui_text_line_is_inked(const lh_ui_text_line_t *self);

/**
 * @brief The ink of the line with its line box at @p origin: as wide as the pen
 *        advance, from the first ink row to the last (height `0` when nothing is inked).
 */
lh_ui_rect_t
lh_ui_text_line_get_ink_rect(const lh_ui_text_line_t *self, lh_ui_point_t origin);

/**
 * @brief How far right @p align puts the line inside a block @p block wide:
 *        `0` for left, the rest of the block for right, half of it (rounded up)
 *        for centre, never negative.
 */
lh_ui_scalar_t
lh_ui_text_line_get_shift(const lh_ui_text_line_t *self, lh_ui_scalar_t block, lh_ui_text_align_h_t align);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TEXT_LINE_H */
