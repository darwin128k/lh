/**
 * @file shadow.h
 * @brief A soft shadow laid off one rounded box, asked pixel after pixel:
 *        ::lh_pixel_shadow_t.
 *
 * A shadow is the same box, shifted, with its alpha falling off over a spread:
 * the peak a spread inside the shifted box's edge, nothing a spread outside it,
 * and a straight line between. The pixels the casting box itself covers wholly are
 * the box's, and the shadow leaves them at `0`, so the box drawn over it meets it
 * at its edge on a rounded corner too.
 *
 * The geometry is worked out once for the box (::lh_pixel_shadow_init), the half
 * that depends on the row once a row (::lh_pixel_shadow_get_row), and a pixel is
 * then a distance — a square root only past a corner — and a fade that is a
 * multiply and a shift (::lh_pixel_shadow_get_alpha). A whole run of a row
 * (::lh_pixel_shadow_get_span) does better still: between the corners every pixel
 * is the same distance out, the run the box covers wholly is found once
 * (::lh_pixel_round_box_get_full_span), and past a corner the alpha is read off
 * thresholds of the squared distance (`lh/core/pixel/shadow/ramp.h`) a step from
 * the last pixel's — no root at all. Both are one reading of a shadow: the same
 * alpha for every pixel, to the last one.
 *
 * Lengths are in ::LH_PIXEL_ROUND_SUBPIXEL units, alphas `0 .. 255`. A value on the
 * stack of whoever draws the shadow; it points at nothing.
 */

#ifndef LH_CORE_PIXEL_SHADOW_H
#define LH_CORE_PIXEL_SHADOW_H

#include <lh/core/byte/ptr.h>
#include <lh/core/pixel/round/box.h>
#include <lh/core/pixel/shadow/fields.h>

/**
 * @struct lh_pixel_shadow
 * @brief A shadow off one box. Members: ::lh_pixel_shadow_fields.
 */
struct lh_pixel_shadow
{
    /** Members: ::lh_pixel_shadow_fields. */
    lh_pixel_shadow_fields(lh_pixel_round_box_t, lh_s32_t, lh_s64_t);
};
/** @copybrief lh_pixel_shadow */
typedef struct lh_pixel_shadow lh_pixel_shadow_t;

struct lh_pixel_shadow_ramp;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The box that casts the shadow (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @return The shadow's own box.
 */
const lh_pixel_round_box_t *
lh_pixel_shadow_get_box(const lh_pixel_shadow_t *self);

/**
 * @brief Store the box that casts the shadow (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param box  The box; copied; must not be null.
 */
lh_void
lh_pixel_shadow_set_box(lh_pixel_shadow_t *self, const lh_pixel_round_box_t *box);

/**
 * @brief The middle of the shifted box across, sub-pixel units (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_cx(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): the middle of the shifted box across, sub-pixel units.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param cx The value.
 */
lh_void
lh_pixel_shadow_set_cx(lh_pixel_shadow_t *self, lh_s32_t cx);

/**
 * @brief The middle of the shifted box down, sub-pixel units (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_cy(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): the middle of the shifted box down, sub-pixel units.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param cy The value.
 */
lh_void
lh_pixel_shadow_set_cy(lh_pixel_shadow_t *self, lh_s32_t cy);

/**
 * @brief How far the straight edges reach across from the middle, less the radius (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_hx(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): how far the straight edges reach across from the middle, less the
 * radius.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param hx The value.
 */
lh_void
lh_pixel_shadow_set_hx(lh_pixel_shadow_t *self, lh_s32_t hx);

/**
 * @brief How far the straight edges reach down from the middle, less the radius (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_hy(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): how far the straight edges reach down from the middle, less the
 * radius.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param hy The value.
 */
lh_void
lh_pixel_shadow_set_hy(lh_pixel_shadow_t *self, lh_s32_t hy);

/**
 * @brief The radius of the shifted box's corners, sub-pixel units (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_r(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): the radius of the shifted box's corners, sub-pixel units.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param r The value.
 */
lh_void
lh_pixel_shadow_set_r(lh_pixel_shadow_t *self, lh_s32_t r);

/**
 * @brief How far the fade reaches either side of the edge, sub-pixel units (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_spread(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): how far the fade reaches either side of the edge, sub-pixel units.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param spread The value.
 */
lh_void
lh_pixel_shadow_set_spread(lh_pixel_shadow_t *self, lh_s32_t spread);

/**
 * @brief The alpha a spread inside the edge, `0 .. 255`; `0` casts nothing (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_shadow_get_peak(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): the alpha a spread inside the edge, `0 .. 255`; `0` casts nothing.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param peak The value.
 */
lh_void
lh_pixel_shadow_set_peak(lh_pixel_shadow_t *self, lh_s32_t peak);

/**
 * @brief The fade at the edge, in 16.16 (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_shadow_get_start(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): the fade at the edge, in 16.16.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param start The value.
 */
lh_void
lh_pixel_shadow_set_start(lh_pixel_shadow_t *self, lh_s64_t start);

/**
 * @brief What the fade loses a sub-pixel unit outward, in 16.16 (level 1).
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_shadow_get_step(const lh_pixel_shadow_t *self);

/**
 * @brief Store it (level 1): what the fade loses a sub-pixel unit outward, in 16.16.
 *
 * @param self The shadow; checked by ::lh_assert_runtime_ref.
 * @param step The value.
 */
lh_void
lh_pixel_shadow_set_step(lh_pixel_shadow_t *self, lh_s64_t step);

/**
 * @brief Lay a shadow off @p box: the box shifted by @p offset_x, @p offset_y, its
 *        alpha @p peak a @p spread inside the edge and fading to nothing a @p spread
 *        outside it. No shadow (`peak` `0`) for a spread of `0` or less or an empty
 *        box.
 *
 * @param self     The shadow; checked by the setters.
 * @param box      The casting box; must not be null; copied.
 * @param offset_x How far the shadow is shifted right, sub-pixel units.
 * @param offset_y How far down.
 * @param spread   How far the fade reaches either side of the edge, sub-pixel units.
 * @param peak     The alpha inside, `0 .. 255`: the shadow colour's.
 */
lh_void
lh_pixel_shadow_init(lh_pixel_shadow_t *self, const lh_pixel_round_box_t *box, lh_s64_t offset_x,
                     lh_s64_t offset_y, lh_s64_t spread, lh_byte_t peak);

/**
 * @brief Whether @p self and @p other cast the same shadow: the same box, and the
 *        same shift, radius, spread and peak — every pixel's alpha the same. A
 *        caller that keeps the alphas of one shadow (a mask) asks this before it
 *        uses them for another.
 *
 * @param self  A shadow; checked by the getters.
 * @param other Another; checked by the getters.
 */
lh_bool_t
lh_pixel_shadow_is_equal(const lh_pixel_shadow_t *self, const lh_pixel_shadow_t *other);

/**
 * @brief What of a pixel's distance row @p y gives: how far the row's centre is
 *        from the shifted box's straight part, sub-pixel units — negative inside it.
 *        Worked out once a row and handed to ::lh_pixel_shadow_get_alpha.
 *
 * @param self The shadow; checked by the getters.
 * @param y    The row.
 */
lh_s32_t
lh_pixel_shadow_get_row(const lh_pixel_shadow_t *self, lh_s32_t y);

/**
 * @brief The alpha a @p distance from the shifted box's edge gives: the peak a
 *        spread inside and further, nothing a spread outside and further, the
 *        straight line between, rounded.
 *
 * @param self     The shadow; checked by the getters.
 * @param distance Sub-pixel units, negative inside.
 */
lh_byte_t
lh_pixel_shadow_get_fade(const lh_pixel_shadow_t *self, lh_s32_t distance);

/**
 * @brief The alpha of pixel @p x, @p y: `0` where the casting box covers it wholly,
 *        else the fade of its distance to the shifted rounded box.
 *
 * @param self The shadow; checked by the getters.
 * @param row  ::lh_pixel_shadow_get_row of @p y.
 * @param x    The column.
 * @param y    The row.
 * @return `0 .. 255`.
 */
lh_byte_t
lh_pixel_shadow_get_alpha(const lh_pixel_shadow_t *self, lh_s32_t row, lh_s32_t x, lh_s32_t y);

/**
 * @brief The alphas of row @p y from column @p x0 to @p x1: each what
 *        ::lh_pixel_shadow_get_alpha gives, worked out a run at a time.
 *
 * @param self The shadow; checked by the getters.
 * @param ramp Its thresholds (::lh_pixel_shadow_ramp_init of @p self); checked by
 *             ::lh_assert_runtime_ref.
 * @param y    The row.
 * @param x0   The first column.
 * @param x1   One past the last.
 * @param out  Room for `x1 - x0` alphas.
 */
lh_void
lh_pixel_shadow_get_span(const lh_pixel_shadow_t *self, const struct lh_pixel_shadow_ramp *ramp,
                         lh_s32_t y, lh_s32_t x0, lh_s32_t x1, lh_byte_ptr out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_SHADOW_H */
