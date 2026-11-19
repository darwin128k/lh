/**
 * @file gradient.h
 * @brief Colours that run into each other over a box: ::lh_pixel_gradient_t.
 *
 * A gradient is a row of stops (`lh/core/pixel/gradient/stop.h`), each a colour
 * and a position from `0` to `255`, and a shape: along a line at an angle, or
 * out from the middle. Laid over a box, every pixel of the box has a position on
 * that scale (::lh_pixel_gradient_get_frac_at) and the position a colour
 * (::lh_pixel_gradient_get_color_at): before the first stop the first colour,
 * after the last the last, between two the two mixed in proportion, every
 * channel and alpha alike. Colours are `0xAARRGGBB`.
 *
 * It is data: the owner keeps it, it points at nothing, and a theme can keep
 * one in flash. Drawing a whole box a pixel at a time through these two
 * functions is exact but slow; a box is drawn through a table of its 256
 * colours (`lh/core/pixel/gradient/table.h`) and a sampler that walks a row
 * (`lh/core/pixel/gradient/sampler.h`), which give the same colours.
 *
 * The number of stops a gradient holds is ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS
 * (by default 2, as LVGL's `LV_GRADIENT_MAX_STOPS`).
 */

#ifndef LH_CORE_PIXEL_GRADIENT_H
#define LH_CORE_PIXEL_GRADIENT_H

#include <lh/core/byte/ptr.h>
#include <lh/core/math/rect.h>
#include <lh/core/pixel/gradient/fields.h>
#include <lh/core/pixel/gradient/shape.h>
#include <lh/core/pixel/gradient/stop.h>
#include <lh/core/pixel/gradient/stop/count.h>

/**
 * @struct lh_pixel_gradient
 * @brief Stops and a shape. Members: ::lh_pixel_gradient_fields.
 */
struct lh_pixel_gradient
{
    /** Members: ::lh_pixel_gradient_fields. */
    lh_pixel_gradient_fields(lh_pixel_gradient_stop_t, lh_pixel_gradient_stop_count_t,
                             lh_pixel_gradient_shape_t, lh_s16_t);
};
/** @copybrief lh_pixel_gradient */
typedef struct lh_pixel_gradient lh_pixel_gradient_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The table of stops, ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS long; the
 *        first ::lh_pixel_gradient_get_stops_count are used (level 1).
 *
 * @param self The gradient; checked by ::lh_assert_runtime_ref.
 */
const lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stops(const lh_pixel_gradient_t *self);

/**
 * @brief The table of stops, to change them in place (level 1).
 *
 * @param self The gradient; checked by ::lh_assert_runtime_ref.
 */
lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stops_mut(lh_pixel_gradient_t *self);

/**
 * @brief How many stops are used (level 1).
 *
 * @param self The gradient; checked by ::lh_assert_runtime_ref.
 */
lh_pixel_gradient_stop_count_t
lh_pixel_gradient_get_stops_count(const lh_pixel_gradient_t *self);

/**
 * @brief Store how many stops are used (level 1). Not checked against the
 *        table's length; ::lh_pixel_gradient_init_stops is.
 *
 * @param self        The gradient; checked by ::lh_assert_runtime_ref.
 * @param stops_count The count.
 */
lh_void
lh_pixel_gradient_set_stops_count(lh_pixel_gradient_t *self,
                                  lh_pixel_gradient_stop_count_t stops_count);

/**
 * @brief How the stops lie over a box (level 1).
 *
 * @param self The gradient; checked by ::lh_assert_runtime_ref.
 */
lh_pixel_gradient_shape_t
lh_pixel_gradient_get_shape(const lh_pixel_gradient_t *self);

/**
 * @brief Store how the stops lie over a box (level 1).
 *
 * @param self  The gradient; checked by ::lh_assert_runtime_ref.
 * @param shape The shape.
 */
lh_void
lh_pixel_gradient_set_shape(lh_pixel_gradient_t *self, lh_pixel_gradient_shape_t shape);

/**
 * @brief The direction of a linear gradient, in degrees `0 .. 359`: 0 runs from
 *        the bottom up, 90 from left to right, 180 from the top down (level 1).
 *
 * @param self The gradient; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_gradient_get_angle(const lh_pixel_gradient_t *self);

/**
 * @brief Store the direction as it is (level 1); ::lh_pixel_gradient_set_linear
 *        folds any angle into `0 .. 359`.
 *
 * @param self  The gradient; checked by ::lh_assert_runtime_ref.
 * @param angle Degrees, `0 .. 359`.
 */
lh_void
lh_pixel_gradient_set_angle(lh_pixel_gradient_t *self, lh_s16_t angle);

/**
 * @brief The stop at @p index.
 *
 * @param self  The gradient; must not be null.
 * @param index Below ::lh_pixel_gradient_get_stops_count; checked by
 *              ::lh_assert_runtime_ifn.
 * @return The gradient's own stop.
 */
const lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stop(const lh_pixel_gradient_t *self, lh_pixel_gradient_stop_count_t index);

/**
 * @brief The stop at @p index, to change it in place.
 *
 * @param self  The gradient; must not be null.
 * @param index Below ::lh_pixel_gradient_get_stops_count; checked by
 *              ::lh_assert_runtime_ifn.
 * @return The gradient's own stop.
 */
lh_pixel_gradient_stop_t *
lh_pixel_gradient_get_stop_mut(lh_pixel_gradient_t *self, lh_pixel_gradient_stop_count_t index);

/**
 * @brief A gradient with no stops, linear, from the top down (180 degrees). With
 *        no stops every pixel is clear (`0`).
 *
 * @param self The gradient; checked by the setters.
 */
lh_void
lh_pixel_gradient_init(lh_pixel_gradient_t *self);

/**
 * @brief Lay the gradient along a line at @p degrees, folded into `0 .. 359`.
 *
 * @param self    The gradient; checked by the setters.
 * @param degrees Any whole number of degrees.
 */
lh_void
lh_pixel_gradient_set_linear(lh_pixel_gradient_t *self, lh_s32_t degrees);

/**
 * @brief Lay the gradient out from the middle of the box to its corners.
 *
 * @param self The gradient; checked by ::lh_pixel_gradient_set_shape.
 */
lh_void
lh_pixel_gradient_set_radial(lh_pixel_gradient_t *self);

/**
 * @brief Where pixel @p x, @p y of @p rect falls on the gradient, `0 .. 255`.
 *
 * Linear: along the line at the angle, as long as the box seen along it, so `0`
 * and `255` land on the two corners it meets. Radial: on an ellipse of the box's
 * shape through its corners, `0` in the middle and `255` at every corner. The
 * pixel is taken at its centre. An empty box is `0` everywhere.
 *
 * @param self The gradient; must not be null.
 * @param rect The box it is laid over, whole pixels; must not be null.
 * @param x    The pixel's column.
 * @param y    The pixel's row.
 * @return The position, `0 .. 255`.
 */
lh_byte_t
lh_pixel_gradient_get_frac_at(const lh_pixel_gradient_t *self, const lh_math_rect_t *rect,
                              lh_s32_t x, lh_s32_t y);

/**
 * @brief Whether the gradient runs straight up or down (linear, at 0 or 180
 *        degrees): every pixel of a row is the same colour, so a row is one
 *        colour and only a column has to be walked.
 *
 * @param self The gradient; must not be null.
 */
lh_bool_t
lh_pixel_gradient_is_vertical(const lh_pixel_gradient_t *self);

/**
 * @brief The first stop from @p segment on that stands at or past @p frac — the
 *        end of the span @p frac falls in, walking on from where the last one was.
 *
 * @param self    The gradient; must not be null.
 * @param segment A stop index to start from, `0 ..` the count.
 * @param frac    A position, `0 .. 255`.
 * @return An index, at most ::lh_pixel_gradient_get_stops_count (past every stop).
 */
lh_s32_t
lh_pixel_gradient_get_segment_at(const lh_pixel_gradient_t *self, lh_s32_t segment, lh_s32_t frac);

/**
 * @brief Whether every pixel the gradient colours is opaque: it has stops, and
 *        every stop's alpha is 255 — what lies under a box filled with it does
 *        not show through.
 *
 * @param self The gradient; must not be null.
 * @return True when every stop is opaque; false with no stops.
 */
lh_bool_t
lh_pixel_gradient_is_opaque(const lh_pixel_gradient_t *self);

/**
 * @brief Take @p num_stops stops of @p colors at @p fracs, or spread evenly from
 *        `0` to `255` when @p fracs is ::lh_null (a single stop at `0`). The shape
 *        and the angle are kept.
 *
 * @param self      The gradient; checked by the setters.
 * @param colors    `0xAARRGGBB` each, @p num_stops of them; checked by
 *                  ::lh_assert_runtime_ref. Copied.
 * @param fracs     The positions, in order, or ::lh_null. Copied.
 * @param num_stops `1 ..` ::LH_LIBRARY_OPTION_GL_GRADIENT_MAX_STOPS; checked by
 *                  ::lh_assert_runtime_ifn.
 */
lh_void
lh_pixel_gradient_init_stops(lh_pixel_gradient_t *self, const lh_u32_t *colors, lh_byte_cptr fracs,
                             lh_pixel_gradient_stop_count_t num_stops);

/**
 * @brief The colour at position @p frac: the first stop's before it, the last's
 *        after the last, between two stops the two mixed in proportion (each
 *        channel `(from * (span - at) + to * at) / span`, alpha too). Clear (`0`)
 *        with no stops.
 *
 * @param self The gradient; must not be null.
 * @param frac A position, `0 .. 255`.
 * @return `0xAARRGGBB`.
 */
lh_u32_t
lh_pixel_gradient_get_color_at(const lh_pixel_gradient_t *self, lh_byte_t frac);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_GRADIENT_H */
