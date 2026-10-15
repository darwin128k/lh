/**
 * @file block.h
 * @brief A whole text broken into lines and measured: ::lh_ui_text_block_t.
 *
 * Its size, the first ink row, the widest line and the line count are four
 * questions about the same lines. ::lh_ui_text_block_measure lays every line out
 * once (::lh_ui_text_line_measure) and answers all four; the measures of
 * `lh/ui/text.h` that a layout asks are this, read.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_TEXT_BLOCK_H
#define LH_UI_TEXT_BLOCK_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/font.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/ui/text/block/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_text_block
 * @brief A text broken into lines: line count, widest line, first ink and the ink hull.
 */
struct lh_ui_text_block
{
    /** Members: ::lh_ui_text_block_fields. */
    lh_ui_text_block_fields(lh_ui_rect_t, lh_ui_scalar_t, lh_u32_t);
};
/** @copybrief lh_ui_text_block */
typedef struct lh_ui_text_block lh_ui_text_block_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An empty block: no lines, nothing inked.
 */
lh_void
lh_ui_text_block_init(lh_ui_text_block_t *self);

/**
 * @brief Break @p text into lines @p width wide (`0` or less: at line breaks only)
 *        and measure them all, in one walk of the text.
 *
 * @param self  Block to fill.
 * @param font  Font the letters are measured in.
 * @param text  UTF-8 text; ::lh_null is an empty block.
 * @param width Line width to break at.
 */
lh_void
lh_ui_text_block_measure(lh_ui_text_block_t *self, const lh_ui_font_t *font, const lh_char_t *text,
                         lh_ui_scalar_t width);

/**
 * @brief ::lh_ui_text_block_measure without the ink: the line count and the widest
 *        line, which is all a width or a line count asks. The ink fields are those
 *        of an empty block.
 */
lh_void
lh_ui_text_block_measure_layout(lh_ui_text_block_t *self, const lh_ui_font_t *font, const lh_char_t *text,
                                lh_ui_scalar_t width);

/**
 * @brief How many lines the text broke into.
 */
lh_u32_t
lh_ui_text_block_get_lines(const lh_ui_text_block_t *self);

/**
 * @brief Pen advance of the widest line: the block an alignment works in.
 */
lh_ui_scalar_t
lh_ui_text_block_get_width(const lh_ui_text_block_t *self);

/**
 * @brief First ink row of any line, down from the top of that line's own box
 *        (`0` when nothing is inked).
 */
lh_ui_scalar_t
lh_ui_text_block_get_ink_top(const lh_ui_text_block_t *self);

/**
 * @brief The ink hull of the left-aligned lines, placed so that its first ink row
 *        is at @p origin (the first line box ::lh_ui_text_block_get_ink_top above it).
 */
lh_ui_rect_t
lh_ui_text_block_get_ink_rect(const lh_ui_text_block_t *self, lh_ui_point_t origin);

/**
 * @brief The size a label takes for the text: as wide as its ink, and from the cap
 *        line of the first line to the baseline of the last (`0` high when nothing
 *        is inked). ::lh_ui_text_get_size_wrapped is this of a measured block.
 *
 * @param font The font the block was measured in.
 */
lh_ui_size_t
lh_ui_text_block_get_size(const lh_ui_text_block_t *self, const lh_ui_font_t *font);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TEXT_BLOCK_H */
