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

#include <lh/byte/ptr.h>
#include <lh/ui/shadow.h>
#include <lh/ui/shadow/shape/fields.h>

/**
 * @struct lh_ui_shadow_shape
 * @brief A shadow over one box, worked out once.
 */
struct lh_ui_shadow_shape
{
    /** Members: ::lh_ui_shadow_shape_fields. */
    lh_ui_shadow_shape_fields(lh_ui_rect_t, lh_ui_scalar_t);
};
/** @copybrief lh_ui_shadow_shape */
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
lh_ui_shadow_shape_init(lh_ui_shadow_shape_t *self, const lh_ui_shadow_t *shadow,
                        const lh_ui_rect_t *rect, lh_ui_scalar_t radius);

/**
 * @brief The part of a pixel's distance that only its row decides, for
 *        ::lh_ui_shadow_shape_alpha.
 */
lh_s32_t
lh_ui_shadow_shape_row(const lh_ui_shadow_shape_t *self, lh_s32_t y);

/** The alpha @p distance past the edge: the peak a spread inside it, nothing a spread
   outside, and the fade between. */
lh_byte_t
lh_ui_shadow_shape_fade(const lh_ui_shadow_shape_t *self, lh_s32_t distance);

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
lh_ui_shadow_shape_span(const lh_ui_shadow_shape_t *self, const struct lh_ui_shadow_ramp *ramp,
                        lh_s32_t y, lh_s32_t x0, lh_s32_t x1, lh_byte_ptr out);

/**
 * @brief The box that casts the shadow (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_shadow_shape_get_box(const lh_ui_shadow_shape_t *self);

/**
 * @brief The box that casts the shadow, to change in place (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_shadow_shape_get_box_mut(lh_ui_shadow_shape_t *self);

/**
 * @brief Store the box that casts the shadow (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param box The new value.
 */
lh_void
lh_ui_shadow_shape_set_box(lh_ui_shadow_shape_t *self, lh_ui_rect_t box);

/**
 * @brief The corner radius of the box (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_shadow_shape_get_corner(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the corner radius of the box (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param corner The new value.
 */
lh_void
lh_ui_shadow_shape_set_corner(lh_ui_shadow_shape_t *self, lh_ui_scalar_t corner);

/**
 * @brief The left edge of the box, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_box_x0(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the left edge of the box, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param box_x0 The new value.
 */
lh_void
lh_ui_shadow_shape_set_box_x0(lh_ui_shadow_shape_t *self, lh_s32_t box_x0);

/**
 * @brief The right edge of the box, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_box_x1(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the right edge of the box, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param box_x1 The new value.
 */
lh_void
lh_ui_shadow_shape_set_box_x1(lh_ui_shadow_shape_t *self, lh_s32_t box_x1);

/**
 * @brief The top edge of the box, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_box_y0(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the top edge of the box, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param box_y0 The new value.
 */
lh_void
lh_ui_shadow_shape_set_box_y0(lh_ui_shadow_shape_t *self, lh_s32_t box_y0);

/**
 * @brief The bottom edge of the box, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_box_y1(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the bottom edge of the box, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param box_y1 The new value.
 */
lh_void
lh_ui_shadow_shape_set_box_y1(lh_ui_shadow_shape_t *self, lh_s32_t box_y1);

/**
 * @brief The horizontal center of the box, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_cx(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the horizontal center of the box, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param cx The new value.
 */
lh_void
lh_ui_shadow_shape_set_cx(lh_ui_shadow_shape_t *self, lh_s32_t cx);

/**
 * @brief The vertical center of the box, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_cy(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the vertical center of the box, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param cy The new value.
 */
lh_void
lh_ui_shadow_shape_set_cy(lh_ui_shadow_shape_t *self, lh_s32_t cy);

/**
 * @brief The half width of the box less the corner, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_hx(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the half width of the box less the corner, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param hx The new value.
 */
lh_void
lh_ui_shadow_shape_set_hx(lh_ui_shadow_shape_t *self, lh_s32_t hx);

/**
 * @brief The half height of the box less the corner, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_hy(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the half height of the box less the corner, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param hy The new value.
 */
lh_void
lh_ui_shadow_shape_set_hy(lh_ui_shadow_shape_t *self, lh_s32_t hy);

/**
 * @brief The corner radius, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_r(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the corner radius, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param r The new value.
 */
lh_void
lh_ui_shadow_shape_set_r(lh_ui_shadow_shape_t *self, lh_s32_t r);

/**
 * @brief How far the shadow spreads, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_spread(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store how far the shadow spreads, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param spread The new value.
 */
lh_void
lh_ui_shadow_shape_set_spread(lh_ui_shadow_shape_t *self, lh_s32_t spread);

/**
 * @brief The strongest coverage the shadow reaches (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_shadow_shape_get_peak(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the strongest coverage the shadow reaches (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param peak The new value.
 */
lh_void
lh_ui_shadow_shape_set_peak(lh_ui_shadow_shape_t *self, lh_s32_t peak);

/**
 * @brief The distance the falloff starts at, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_shadow_shape_get_start(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store the distance the falloff starts at, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param start The new value.
 */
lh_void
lh_ui_shadow_shape_set_start(lh_ui_shadow_shape_t *self, lh_s64_t start);

/**
 * @brief How far the falloff moves per entry, fixed point (level 1).
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_ui_shadow_shape_get_step(const lh_ui_shadow_shape_t *self);

/**
 * @brief Store how far the falloff moves per entry, fixed point (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The shape; checked by ::lh_assert_runtime_ref.
 * @param step The new value.
 */
lh_void
lh_ui_shadow_shape_set_step(lh_ui_shadow_shape_t *self, lh_s64_t step);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SHADOW_SHAPE_H */
