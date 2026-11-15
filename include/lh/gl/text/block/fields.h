/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_text_block_t.
 */

#ifndef LH_GL_TEXT_BLOCK_FIELDS_H
#define LH_GL_TEXT_BLOCK_FIELDS_H

/**
 * @def lh_gl_text_block_fields(rect_type, scalar_type, count_type)
 * @brief A whole text broken into lines, measured.
 *
 * `lines` is how many lines it breaks into; `width` the pen advance of the
 * widest; `ink_top` the first ink row of any line, down from the top of that
 * line's own box (`0` when nothing is inked); `ink` the hull of every line's ink
 * with the first line box at `(0, 0)` and the lines left-aligned, a line height
 * apart.
 *
 * @param rect_type   ::lh_gl_rect_t.
 * @param scalar_type ::lh_gl_scalar_t.
 * @param count_type  ::lh_u32_t.
 */
#define lh_gl_text_block_fields(rect_type, scalar_type, count_type)                                \
    rect_type ink;                                                                                 \
    scalar_type width;                                                                             \
    scalar_type ink_top;                                                                           \
    count_type lines

#endif /* LH_GL_TEXT_BLOCK_FIELDS_H */
