/**
 * @file shape.h
 * @brief A shadow laid over one box, ready to be asked pixel after pixel:
 *        ::lh_ui_shadow_shape_t.
 *
 * ::lh_ui_shadow_alpha_at_pixel is one question about one pixel, and every one of them
 * worked the box's geometry out again. A shadow is a few thousand pixels, all of them
 * about the same box, so the geometry is worked out here once, the half that depends
 * on the row once a row (::lh_ui_shadow_shape_row), and a pixel is then a distance --
 * a square root only in a corner -- and a fade that is a multiply and a shift.
 *
 * It is the one reading of a shadow: ::lh_ui_shadow_alpha_at_pixel is this, and so is
 * every backend that draws a shadow pixel by pixel, so a backend's shadow and the
 * canvas's own cannot disagree by a single alpha.
 */

#ifndef LH_UI_SHADOW_SHAPE_H
#define LH_UI_SHADOW_SHAPE_H

#include <lh/compiler/extern/c.h>
#include <lh/byte/ptr.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/shadow.h>
#include <lh/ui/shadow/shape/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_shadow_shape
 * @typedef lh_ui_shadow_shape_t
 * @brief A shadow over one box, worked out once.
 */
struct lh_ui_shadow_shape
{
    lh_ui_shadow_shape_fields(lh_ui_rect_t, lh_ui_scalar_t);
};
typedef struct lh_ui_shadow_shape lh_ui_shadow_shape_t;

struct lh_ui_shadow_ramp;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p shadow over @p rect, its corners rounded by @p radius (clamped here).
 *
 * A shadow that paints nothing -- no spread, no colour, an empty box -- has a peak of
 * `0`, and every pixel of it is `0`.
 */
lh_void
lh_ui_shadow_shape_init(lh_ui_shadow_shape_t *self, const lh_ui_shadow_t *shadow, const lh_ui_rect_t *rect,
                        lh_ui_scalar_t radius);

/**
 * @brief The part of a pixel's distance that only its row decides, for
 *        ::lh_ui_shadow_shape_alpha.
 */
lh_s32_t
lh_ui_shadow_shape_row(const lh_ui_shadow_shape_t *self, lh_s32_t y);

/**
 * @brief The alpha of the shadow at pixel (@p x, @p y), whose row is @p row
 *        (::lh_ui_shadow_shape_row): `0` where the fill covers the pixel wholly and
 *        past the fade, the peak well inside the shadow, and the fade between.
 */
lh_byte_t
lh_ui_shadow_shape_alpha(const lh_ui_shadow_shape_t *self, lh_s32_t row, lh_s32_t x, lh_s32_t y);

/**
 * @brief ::lh_ui_shadow_shape_alpha for the pixels @p x0 .. @p x1 - 1 of row @p y,
 *        into @p out: the same alphas, a row's worth of work done once.
 *
 * The fill's wholly covered pixels are the one span ::lh_ui_radius_full_span finds,
 * and a corner's pixels are read off @p ramp (::lh_ui_shadow_ramp_init of this shape)
 * instead of each taking a square root.
 */
lh_void
lh_ui_shadow_shape_span(const lh_ui_shadow_shape_t *self, const struct lh_ui_shadow_ramp *ramp, lh_s32_t y,
                        lh_s32_t x0, lh_s32_t x1, lh_byte_ptr out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SHADOW_SHAPE_H */
