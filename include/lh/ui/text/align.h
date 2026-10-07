/**
 * @file align.h
 * @brief Where text sits inside the box it is drawn in, ::lh_ui_text_align_h_t
 *        and ::lh_ui_text_align_v_t.
 */

#ifndef LH_UI_TEXT_ALIGN_H
#define LH_UI_TEXT_ALIGN_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/insets.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Where the text sits across the box.
 *
 * The default, ::lh_ui_text_align_h_left, is what a label did before alignment
 * existed: the text starts at the padding. The other two move the text's left
 * edge to the middle or the right edge of the padded box; text wider than the box
 * starts at the left for all three, because there is nowhere else to start.
 */
typedef enum lh_ui_text_align_h
{
    lh_ui_text_align_h_left = 0,
    lh_ui_text_align_h_center = 1,
    lh_ui_text_align_h_right = 2
} lh_ui_text_align_h_t;

/**
 * @brief Where the text sits down the box.
 *
 * ::lh_ui_text_align_v_top is what a label did before alignment existed;
 * ::lh_ui_text_align_v_center and ::lh_ui_text_align_v_bottom move the text's top
 * edge to the middle or the bottom of the padded box, and text taller than the box
 * starts at the top for all three.
 */
typedef enum lh_ui_text_align_v
{
    lh_ui_text_align_v_top = 0,
    lh_ui_text_align_v_center = 1,
    lh_ui_text_align_v_bottom = 2
} lh_ui_text_align_v_t;

/**
 * @brief The point a text of @p size starts at inside @p box, with @p padding
 *        taken off the box and @p horizontal / @p vertical applied.
 *
 * The one place an alignment turns into a position: every caller that aligns text
 * inside a box comes here, so a label and anything that draws like one cannot
 * disagree about where the text goes.
 *
 * @param box The box to align inside; ::lh_null is read as an empty one.
 * @param padding Space kept off each side before aligning; ::lh_null is none.
 * @param size Measured size of the text, from ::lh_ui_text_get_size.
 */
lh_ui_point_t
lh_ui_text_align_get_origin(const lh_ui_rect_t *box, const lh_ui_insets_t *padding, lh_ui_size_t size,
                            lh_ui_text_align_h_t horizontal, lh_ui_text_align_v_t vertical);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_TEXT_ALIGN_H */