/**
 * @file align.h
 * @brief Where text sits inside the box it is drawn in, ::lh_gl_text_align_h_t
 *        and ::lh_gl_text_align_v_t.
 */

#ifndef LH_GL_TEXT_ALIGN_H
#define LH_GL_TEXT_ALIGN_H

#include <lh/gl/insets.h>
#include <lh/gl/text/align/h.h>
#include <lh/gl/text/align/v.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The point a text of @p size starts at inside @p box, with @p padding
 *        taken off the box and @p horizontal / @p vertical applied.
 *
 * The one place an alignment turns into a position: every caller that aligns text
 * inside a box comes here, so a label and anything that draws like one cannot
 * disagree about where the text goes.
 *
 * A centre is rounded **half up on both axes**: an odd ink inside an even box
 * cannot land on the middle, and the leftover half-pixel goes to the bottom and
 * to the right. Measured on the demo's 28-row Hide panel button: the box centres
 * on 194.0, a 15-row caption centres on 193.5 or 194.5, and truncating put it on
 * the first — half a pixel high, which is visible as "a bit high".
 *
 * @param box       The box to align inside; ::lh_null is read as an empty one.
 * @param padding   Space kept off each side before aligning; ::lh_null is none.
 * @param size      Measured size of the text, from ::lh_gl_text_get_size.
 * @param horizontal Horizontal alignment inside the padded box.
 * @param vertical   Vertical alignment inside the padded box.
 * @param out        Receives the point the text starts at.
 */
lh_void
lh_gl_text_align_get_origin(const lh_gl_rect_t *box, const lh_gl_insets_t *padding,
                            lh_gl_size_t size, lh_gl_text_align_h_t horizontal,
                            lh_gl_text_align_v_t vertical, lh_gl_point_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_TEXT_ALIGN_H */
