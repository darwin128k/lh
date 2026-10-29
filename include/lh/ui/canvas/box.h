/**
 * @file box.h
 * @brief ::lh_ui_canvas_fill_box: a box with a radius for each corner and a paint that
 *        may be a gradient.
 */

#ifndef LH_UI_CANVAS_BOX_H
#define LH_UI_CANVAS_BOX_H

#include <lh/ui/canvas.h>
#include <lh/ui/gradient/sampler.h>
#include <lh/ui/paint.h>
#include <lh/ui/radii.h>
#include <lh/ui/radius.h>

/**
 * @struct lh_ui_canvas_box_frame
 * @brief Whole-pixel edges of a box on the target, and the part of it the clip
 *        shows: filled by ::lh_ui_canvas_box_frame_init.
 */
struct lh_ui_canvas_box_frame
{
    lh_s32_t left;        /**< Box left column. */
    lh_s32_t right;       /**< One past the box right column. */
    lh_s32_t top;         /**< Box top row. */
    lh_s32_t bottom;      /**< One past the box bottom row. */
    lh_s32_t shown_left;  /**< First column the clip shows. */
    lh_s32_t shown_right; /**< One past the last column the clip shows. */
    lh_s32_t first;       /**< First row the clip shows. */
    lh_s32_t last;        /**< One past the last row the clip shows. */
};

/** @brief Alias for ::lh_ui_canvas_box_frame. */
typedef struct lh_ui_canvas_box_frame lh_ui_canvas_box_frame_t;

LH_COMPILER_EXTERN_C_BEGIN

/** True when the colour of @p paint does not change across a row (solid, or a
   gradient that runs straight up or down). */
lh_bool_t
lh_ui_canvas_box_rows_are_one_colour(const lh_ui_paint_t *paint);

/** True when the colour of @p paint does not change down a column (a gradient that
   runs straight across). */
lh_bool_t
lh_ui_canvas_box_columns_are_one_colour(const lh_ui_paint_t *paint);

/** The colours of pixels @p x0 .. @p x1 - 1 of row @p y into @p out: the gradient's
   when there is @p sampler, else the paint's one colour. */
lh_void
lh_ui_canvas_box_paint_row(const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler,
                           lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_ui_color_t *out);

/** The one colour of row @p y of a paint whose rows are one colour each. */
lh_void
lh_ui_canvas_box_row_colour(const lh_ui_paint_t *paint, lh_ui_gradient_sampler_t *sampler,
                            lh_s32_t x, lh_s32_t y, lh_ui_color_t *out);

/** Pixels @p x0 .. @p x1 - 1 of row @p y in one colour @p colour, each at its
   coverage by the corner @p corner (::lh_ui_radius_run_init). */
lh_void
lh_ui_canvas_box_corner(lh_ui_canvas_t *self, const lh_ui_color_t *colour,
                        const struct lh_ui_radius_run *corner, lh_s32_t x0, lh_s32_t x1,
                        lh_s32_t y);

/** Pixels @p x0 .. @p x1 - 1 of row @p y; those left of @p near_end are cut by the
   corner @p near and those from @p far_start on by the corner @p far, each the
   corner of a box rounded all round by its radius (::lh_ui_radius_run_init). A row
   of one colour sends its corners as colours and the middle as one span; any other
   goes out whole as a row of colours. */
lh_void
lh_ui_canvas_box_row(lh_ui_canvas_t *self, const lh_ui_paint_t *paint,
                     lh_ui_gradient_sampler_t *sampler, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                     lh_s32_t near_end, const struct lh_ui_radius_run *near, lh_s32_t far_start,
                     const struct lh_ui_radius_run *far);

/** Rows @p y0 .. @p y1 - 1, which no corner reaches: one box when the paint is one
   colour, one box of a colour a row or a colour a column when it changes only one
   way, else a row of colours a row. */
lh_void
lh_ui_canvas_box_band(lh_ui_canvas_t *self, const lh_ui_paint_t *paint,
                      lh_ui_gradient_sampler_t *sampler, lh_s32_t x0, lh_s32_t x1, lh_s32_t y0,
                      lh_s32_t y1);

/**
 * @brief Edges of @p box (already on the target) and of the part @p cut shows,
 *        into @p out. True when at least one column is shown.
 *
 * @p out must not be null (::lh_assert_runtime_ref).
 */
lh_bool_t
lh_ui_canvas_box_frame_init(const lh_ui_rect_t *box, const lh_ui_rect_t *cut,
                            lh_ui_canvas_box_frame_t *out);

/**
 * @brief The four corner runs of @p box, each rounded all round by its radius
 *        in @p fit, into @p out (four entries: top-left, top-right, bottom-left,
 *        bottom-right).
 */
lh_void
lh_ui_canvas_box_corners_init(const lh_ui_rect_t *box, const lh_ui_radii_t *fit,
                              struct lh_ui_radius_run *out);

/**
 * @brief True when row @p y lies in the upper half of @p frame (its corners
 *        are the top ones).
 */
lh_bool_t
lh_ui_canvas_box_row_is_upper(const lh_ui_canvas_box_frame_t *frame, lh_s32_t y);

/**
 * @brief Columns a corner of @p radius cuts on a row @p from_edge rows in from
 *        the box edge: `min(ceil(radius), room)` while the row is within the
 *        corner, 0 past it.
 */
lh_s32_t
lh_ui_canvas_box_zone(lh_s32_t from_edge, lh_ui_scalar_t radius, lh_s32_t room);

/**
 * @brief Where the corners of row @p y of @p frame end (@p near_end) and start
 *        (@p far_start) for radii @p fit. True when a corner reaches the row.
 *
 * Neither out may be null (::lh_assert_runtime_ref).
 */
lh_bool_t
lh_ui_canvas_box_row_reach(const lh_ui_canvas_box_frame_t *frame, const lh_ui_radii_t *fit,
                           lh_s32_t y, lh_s32_t *near_end, lh_s32_t *far_start);

/**
 * @brief Draw the shown rows of @p frame with @p paint (coloured by @p sampler
 *        when it is not ::lh_null): rows no corner reaches as bands, the others
 *        one by one with their corners from @p corners
 *        (::lh_ui_canvas_box_corners_init) at their coverage.
 */
lh_void
lh_ui_canvas_box_rows(lh_ui_canvas_t *self, const lh_ui_paint_t *paint,
                      lh_ui_gradient_sampler_t *sampler, const lh_ui_canvas_box_frame_t *frame,
                      const lh_ui_radii_t *fit, const struct lh_ui_radius_run *corners);

/**
 * @brief Fill @p rect with @p paint, each corner rounded by its own radius in
 *        @p radii (fitted to the box as CSS fits them, ::lh_ui_radii_clamp).
 *
 * A solid paint with four equal radii is ::lh_ui_canvas_fill_round_rect -- the
 * backend's own rounded fill -- and costs what it always did. A gradient or unequal
 * corners are drawn by the canvas from `fill_rect` alone, so every backend draws them
 * the same: straight parts as spans, corners pixel by pixel at their coverage. An
 * empty paint draws nothing. @p rect is moved by the offset and cut by the clip, like
 * every other fill.
 */
lh_void
lh_ui_canvas_fill_box(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_radii_t *radii,
                      const lh_ui_paint_t *paint);

/**
 * @brief The left edge of the box, in pixels (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_left(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the left edge of the box, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param left The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_left(lh_ui_canvas_box_frame_t *self, lh_s32_t left);

/**
 * @brief The right edge of the box, in pixels (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_right(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the right edge of the box, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param right The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_right(lh_ui_canvas_box_frame_t *self, lh_s32_t right);

/**
 * @brief The top edge of the box, in pixels (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_top(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the top edge of the box, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param top The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_top(lh_ui_canvas_box_frame_t *self, lh_s32_t top);

/**
 * @brief The bottom edge of the box, in pixels (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_bottom(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the bottom edge of the box, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param bottom The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_bottom(lh_ui_canvas_box_frame_t *self, lh_s32_t bottom);

/**
 * @brief The left edge of the part of the box inside the clip (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_shown_left(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the left edge of the part of the box inside the clip (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param shown_left The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_shown_left(lh_ui_canvas_box_frame_t *self, lh_s32_t shown_left);

/**
 * @brief The right edge of the part of the box inside the clip (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_shown_right(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the right edge of the part of the box inside the clip (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param shown_right The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_shown_right(lh_ui_canvas_box_frame_t *self, lh_s32_t shown_right);

/**
 * @brief The first row of the box inside the clip (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_first(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the first row of the box inside the clip (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param first The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_first(lh_ui_canvas_box_frame_t *self, lh_s32_t first);

/**
 * @brief The last row of the box inside the clip (level 1).
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_ui_canvas_box_frame_get_last(const lh_ui_canvas_box_frame_t *self);

/**
 * @brief Store the last row of the box inside the clip (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The box frame; checked by ::lh_assert_runtime_ref.
 * @param last The new value.
 */
lh_void
lh_ui_canvas_box_frame_set_last(lh_ui_canvas_box_frame_t *self, lh_s32_t last);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_BOX_H */
