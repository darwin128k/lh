/**
 * @file pass.h
 * @brief ::lh_ui_blur_pass_t — what the column pass of ::lh_ui_blur_columns reads for every column: filled by ::lh_ui_blur_pass_init.
 */

#ifndef LH_UI_BLUR_PASS_H
#define LH_UI_BLUR_PASS_H

#include <lh/ui/blur/window.h>

/**
 * @struct lh_ui_blur_pass
 * @brief What the column pass of ::lh_ui_blur_columns reads for every column:
 *        filled by ::lh_ui_blur_pass_init.
 */
struct lh_ui_blur_pass
{
    const lh_u8_t *scratch;    /**< What ::lh_ui_blur_rows left, addressed from row `y0`. */
    const lh_ui_rect_t *shape; /**< Rounded shape to cut the write to, or ::lh_null. */
    lh_ui_scalar_t corner;     /**< Its corner radius, clamped; `0` with no shape. */
    lh_s32_t left;             /**< Rect left column. */
    lh_s32_t top;              /**< Rect top row. */
    lh_s32_t width;            /**< Rect width. */
    lh_s32_t height;           /**< Rect height. */
    lh_s32_t radius;           /**< Blur radius in pixels. */
    lh_s32_t y0;               /**< First row held in the scratch. */
    lh_s32_t y1;               /**< One past the last row held in the scratch. */
};
/** @copybrief lh_ui_blur_pass */
typedef struct lh_ui_blur_pass lh_ui_blur_pass_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Prepare the column pass of @p rect over @p pixmap into @p out
 *        (parameters as ::lh_ui_blur_columns). False when there is nothing to
 *        write: a negative @p radius, null @p scratch or an empty rect.
 *
 * @p out must not be null (::lh_assert_runtime_ref).
 */
lh_bool_t
lh_ui_blur_pass_init(const lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius,
                     const lh_u8_t *scratch, const lh_ui_rect_t *shape, lh_ui_scalar_t corner,
                     lh_ui_blur_pass_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_PASS_H */
