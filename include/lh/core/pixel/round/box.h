/**
 * @file box.h
 * @brief A rounded rectangle in sub-pixel units: ::lh_pixel_round_box_t.
 *
 * The coverage kernels of `lh/core/pixel/round.h` take a rounded rectangle as five
 * numbers in ::LH_PIXEL_ROUND_SUBPIXEL units. This holds the five, so that what is
 * asked of one box many times — the coverage of a pixel, the run of a row the box
 * covers wholly, the whole pixels it touches — is asked of one value. A box at a
 * fractional place keeps its fraction to 1/::LH_PIXEL_ROUND_SUBPIXEL of a pixel.
 *
 * A value; it points at nothing. A shadow (`lh/core/pixel/shadow.h`) is laid off
 * one.
 */

#ifndef LH_CORE_PIXEL_ROUND_BOX_H
#define LH_CORE_PIXEL_ROUND_BOX_H

#include <lh/core/pixel/round.h>
#include <lh/core/bool.h>
#include <lh/core/pixel/round/box/fields.h>
#include <lh/core/void.h>

/**
 * @struct lh_pixel_round_box
 * @brief A rounded rectangle. Members: ::lh_pixel_round_box_fields.
 */
struct lh_pixel_round_box
{
    /** Members: ::lh_pixel_round_box_fields. */
    lh_pixel_round_box_fields(lh_s64_t);
};
/** @copybrief lh_pixel_round_box */
typedef struct lh_pixel_round_box lh_pixel_round_box_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The left edge, sub-pixel units (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_round_box_get_lo_x(const lh_pixel_round_box_t *self);

/**
 * @brief Store the left edge (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param lo_x Sub-pixel units.
 */
lh_void
lh_pixel_round_box_set_lo_x(lh_pixel_round_box_t *self, lh_s64_t lo_x);

/**
 * @brief The top edge, sub-pixel units (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_round_box_get_lo_y(const lh_pixel_round_box_t *self);

/**
 * @brief Store the top edge (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param lo_y Sub-pixel units.
 */
lh_void
lh_pixel_round_box_set_lo_y(lh_pixel_round_box_t *self, lh_s64_t lo_y);

/**
 * @brief The right edge, sub-pixel units (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_round_box_get_hi_x(const lh_pixel_round_box_t *self);

/**
 * @brief Store the right edge (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param hi_x Sub-pixel units.
 */
lh_void
lh_pixel_round_box_set_hi_x(lh_pixel_round_box_t *self, lh_s64_t hi_x);

/**
 * @brief The bottom edge, sub-pixel units (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_round_box_get_hi_y(const lh_pixel_round_box_t *self);

/**
 * @brief Store the bottom edge (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param hi_y Sub-pixel units.
 */
lh_void
lh_pixel_round_box_set_hi_y(lh_pixel_round_box_t *self, lh_s64_t hi_y);

/**
 * @brief The radius of every corner, sub-pixel units (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_round_box_get_r(const lh_pixel_round_box_t *self);

/**
 * @brief Store the radius (level 1). Not clamped: at most half the shorter side is
 *        the caller's to keep.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param r    Sub-pixel units.
 */
lh_void
lh_pixel_round_box_set_r(lh_pixel_round_box_t *self, lh_s64_t r);

/**
 * @brief A box from @p lo_x, @p lo_y to @p hi_x, @p hi_y with corners of @p r, all in
 *        sub-pixel units.
 *
 * @param self The box; checked by the setters.
 * @param lo_x Left.
 * @param lo_y Top.
 * @param hi_x Right.
 * @param hi_y Bottom.
 * @param r    The radius.
 */
lh_void
lh_pixel_round_box_init(lh_pixel_round_box_t *self, lh_s64_t lo_x, lh_s64_t lo_y, lh_s64_t hi_x,
                        lh_s64_t hi_y, lh_s64_t r);

/**
 * @brief Whether @p self and @p other are the same box: the same five numbers.
 *
 * @param self  A box; checked by the getters.
 * @param other Another; checked by the getters.
 */
lh_bool_t
lh_pixel_round_box_is_equal(const lh_pixel_round_box_t *self, const lh_pixel_round_box_t *other);

/**
 * @brief The first column the box touches: the whole pixel its left edge is in.
 *
 * @param self The box; checked by the getters.
 */
lh_s32_t
lh_pixel_round_box_get_left(const lh_pixel_round_box_t *self);

/**
 * @brief The first row the box touches.
 *
 * @param self The box; checked by the getters.
 */
lh_s32_t
lh_pixel_round_box_get_top(const lh_pixel_round_box_t *self);

/**
 * @brief One past the last column the box touches: its right edge rounded up.
 *
 * @param self The box; checked by the getters.
 */
lh_s32_t
lh_pixel_round_box_get_right(const lh_pixel_round_box_t *self);

/**
 * @brief One past the last row the box touches.
 *
 * @param self The box; checked by the getters.
 */
lh_s32_t
lh_pixel_round_box_get_bottom(const lh_pixel_round_box_t *self);

/**
 * @brief How much of pixel @p x, @p y the box covers, `0 .. 255`
 *        (::lh_pixel_round_coverage).
 *
 * @param self The box; checked by the getters.
 * @param x    The column.
 * @param y    The row.
 */
lh_byte_t
lh_pixel_round_box_get_coverage(const lh_pixel_round_box_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief The run of row @p y the box covers wholly: every pixel from @p out_x0 up to
 *        @p out_x1 is at coverage 255 and every other pixel of the row is short of
 *        it, so a caller asks no pixel's coverage there.
 *
 * Coverage is decided from a pixel's centre (::lh_pixel_round_coverage), and so is
 * the run. Between the corners it is every pixel whose centre is inside the box; in
 * a corner's rows it starts at the first pixel whose centre is inside the corner's
 * circle by half a pixel — one square root a row — and ends at the last such pixel
 * of the other corner, each worked out from its own edge. A row outside the box is
 * an empty run.
 *
 * Two things differ from gl's run, found when the code moved here (2026-11-19) by
 * holding the run to the coverage pixel by pixel: the right end was the left one
 * mirrored, which holds only for a box on whole pixels — at a fractional place the
 * run took pixels short of 255; and a corner's reach was `inner * inner`, where a
 * pixel is at 255 up to `(inner + 1)^2` less one, so a few pixels at 255 were left
 * out of the run. Now the run is exactly the pixels at 255.
 *
 * The root is not walked down from the row before: the reach is in sub-pixel
 * units, so a row moves it by about ::LH_PIXEL_ROUND_SUBPIXEL, and a walk costs
 * that many steps where `lh_math_isqrt_u64` costs 62 ns for the whole root —
 * measured against it in gl, the walk made a 240 by 160 rectangle of radius 8 18 %
 * slower (radius 16, 22 %).
 *
 * @param self   The box; checked by the getters.
 * @param y      The row.
 * @param out_x0 The run's first column; checked by ::lh_assert_runtime_ref.
 * @param out_x1 One past its last; must not be null.
 */
lh_void
lh_pixel_round_box_get_full_span(const lh_pixel_round_box_t *self, lh_s32_t y, lh_s32_t *out_x0,
                                 lh_s32_t *out_x1);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_ROUND_BOX_H */
