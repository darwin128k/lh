/**
 * @file pass.h
 * @brief ::lh_ui_canvas_sw_shadow_pass_t — one box shadow ready to draw row by row: filled by ::lh_ui_canvas_sw_shadow_pass_init.
 */

#ifndef LH_UI_CANVAS_SW_SHADOW_PASS_H
#define LH_UI_CANVAS_SW_SHADOW_PASS_H

#include <lh/byte/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw/fields.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/radius.h>
#include <lh/ui/shadow/ramp.h>
#include <lh/compiler/extern/c.h>

/**
 * @struct lh_ui_canvas_sw_shadow_pass
 * @brief One box shadow ready to draw row by row: filled by
 *        ::lh_ui_canvas_sw_shadow_pass_init.
 */
struct lh_ui_canvas_sw_shadow_pass
{
    lh_ui_shadow_shape_t shape; /**< The shadow's shape (::lh_ui_shadow_shape_init). */
    lh_ui_shadow_ramp_t ramp;   /**< Its alpha ramp (::lh_ui_shadow_ramp_init). */
    const lh_ui_rect_t *rect;   /**< The box casting it. */
    lh_ui_scalar_t corner;      /**< The box radius, clamped to the box. */
    lh_u32_t rgb;               /**< Shadow colour, `0x00RRGGBB`. */
    lh_bool_t rounded;          /**< True when the canvas clip is rounded. */
    lh_s32_t left;              /**< Left of the box grown by the outset. */
    lh_s32_t top;               /**< Its top. */
    lh_s32_t right;             /**< One past its right. */
    lh_s32_t bottom;            /**< One past its bottom. */
};

/** @brief Alias for ::lh_ui_canvas_sw_shadow_pass. */
typedef struct lh_ui_canvas_sw_shadow_pass lh_ui_canvas_sw_shadow_pass_t;

struct lh_ui_canvas_sw;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Prepare the shadow of @p rect rounded by @p radius on @p self into
 *        @p out. False, with only the edges set, when the shadow has no outset
 *        and draws nothing. @p out must not be null (::lh_assert_runtime_ref).
 */
lh_bool_t
lh_ui_canvas_sw_shadow_pass_init(const struct lh_ui_canvas_sw *self, const lh_ui_rect_t *rect,
                                 lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow,
                                 lh_ui_canvas_sw_shadow_pass_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_SW_SHADOW_PASS_H */
