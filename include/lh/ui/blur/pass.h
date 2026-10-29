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

/**
 * @brief The scratch row the pass sums into (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
const lh_u8_t *
lh_ui_blur_pass_get_scratch(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the scratch row the pass sums into (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param scratch The new value.
 */
lh_void
lh_ui_blur_pass_set_scratch(lh_ui_blur_pass_t *self, const lh_u8_t * scratch);

/**
 * @brief The rectangle whose edge the blur follows (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_blur_pass_get_shape(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the rectangle whose edge the blur follows (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param shape The new value.
 */
lh_void
lh_ui_blur_pass_set_shape(lh_ui_blur_pass_t *self, const lh_ui_rect_t * shape);

/**
 * @brief The corner radius of that rectangle (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_blur_pass_get_corner(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the corner radius of that rectangle (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param corner The new value.
 */
lh_void
lh_ui_blur_pass_set_corner(lh_ui_blur_pass_t *self, lh_ui_scalar_t corner);

/**
 * @brief The left edge of the area blurred, in pixels (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_left(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the left edge of the area blurred, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param left The new value.
 */
lh_void
lh_ui_blur_pass_set_left(lh_ui_blur_pass_t *self, lh_s32_t left);

/**
 * @brief The top edge of the area blurred, in pixels (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_top(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the top edge of the area blurred, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param top The new value.
 */
lh_void
lh_ui_blur_pass_set_top(lh_ui_blur_pass_t *self, lh_s32_t top);

/**
 * @brief The width of the area blurred, in pixels (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_width(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the width of the area blurred, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_ui_blur_pass_set_width(lh_ui_blur_pass_t *self, lh_s32_t width);

/**
 * @brief The height of the area blurred, in pixels (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_height(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the height of the area blurred, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_ui_blur_pass_set_height(lh_ui_blur_pass_t *self, lh_s32_t height);

/**
 * @brief The blur radius in pixels (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_radius(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the blur radius in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param radius The new value.
 */
lh_void
lh_ui_blur_pass_set_radius(lh_ui_blur_pass_t *self, lh_s32_t radius);

/**
 * @brief The first row the pass writes (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_y0(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the first row the pass writes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param y0 The new value.
 */
lh_void
lh_ui_blur_pass_set_y0(lh_ui_blur_pass_t *self, lh_s32_t y0);

/**
 * @brief The row after the last one the pass writes (level 1).
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_blur_pass_get_y1(const lh_ui_blur_pass_t *self);

/**
 * @brief Store the row after the last one the pass writes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The blur pass; checked by ::lh_assert_runtime_ref.
 * @param y1 The new value.
 */
lh_void
lh_ui_blur_pass_set_y1(lh_ui_blur_pass_t *self, lh_s32_t y1);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_PASS_H */
