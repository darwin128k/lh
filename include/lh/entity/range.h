/**
 * @file range.h
 * @brief A number between two ends, and the bar that shows it.
 *
 * The progress bar is this class. A trackbar, a knob, a spin box and a
 * scrollbar are the same fields with their own pointer handling, so a
 * pointer to any of them is also an ::lh_entity_range_t. The track is the
 * style background and the filled part is the text color. Both sit where
 * ::lh_entity_range_track places a bar, and the ends use
 * ::lh_entity_range_cap.
 */

#ifndef LH_ENTITY_RANGE_H
#define LH_ENTITY_RANGE_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/range/fields.h>
#include <lh/numeric/types.h>

/**
 * @struct lh_entity_range
 * @brief Fields via ::lh_entity_fields, ::lh_entity_2d_fields, then
 *        ::lh_entity_range_fields.
 */
struct lh_entity_range
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_entity_range_fields(lh_int_t);
};
typedef struct lh_entity_range lh_entity_range_t;

/**
 * @typedef lh_entity_progress_t
 * @brief A progress bar. The same record as ::lh_entity_range_t, created
 *        with ::lh_entity_progress_class.
 */
typedef lh_entity_range_t lh_entity_progress_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @def LH_ENTITY_RANGE_THICKNESS
 * @brief Bar thickness a new range starts with, in pixels.
 *
 * The default, not a constant in the paint path: ::lh_entity_range_set_thickness
 * changes it, and every bar that draws itself reads it from the range.
 */
#define LH_ENTITY_RANGE_THICKNESS 4

/**
 * @brief Class of a progress bar, derived from ::lh_entity_2d_class.
 *
 * A new bar runs from 0 to 100 and sits at 0.
 */
extern const lh_entity_class_t lh_entity_progress_class;

/**
 * @brief A new range: 0 to 100, start 0, value 0, default bar thickness.
 */
lh_void
lh_entity_range_reset(lh_entity_range_t *self);

/**
 * @brief Hold @p value inside [@p minimum, @p maximum].
 *
 * Public because a caller holding two ranges and building a third out of them
 * needs the same rule the range itself uses.
 */
lh_int_t
lh_entity_range_clamp(lh_int_t value, lh_int_t minimum, lh_int_t maximum);

/**
 * @brief Lower end of @p self.
 */
lh_int_t
lh_entity_range_get_minimum(const lh_entity_range_t *self);

/**
 * @brief Upper end of @p self.
 */
lh_int_t
lh_entity_range_get_maximum(const lh_entity_range_t *self);

/**
 * @brief The value a reset returns @p self to.
 */
lh_int_t
lh_entity_range_get_start(const lh_entity_range_t *self);

/**
 * @brief Current value of @p self, already inside the ends.
 */
lh_int_t
lh_entity_range_get_value(const lh_entity_range_t *self);

/**
 * @brief How far @p self is along its own ends, 0 to 100.
 *
 * 0 at the minimum, 100 at the maximum, and the fraction in between. An extent
 * of no width is 0, because there is nothing to be a fraction of.
 */
lh_int_t
lh_entity_range_to_percent(const lh_entity_range_t *self);

/**
 * @brief How thick the bar that shows @p self is, in pixels.
 */
lh_int_t
lh_entity_range_get_thickness(const lh_entity_range_t *self);

/**
 * @brief How far the value of @p self travels along its own track, in pixels.
 *
 * The long side of the box, because a bar runs along it. Zero when there is no
 * box yet. This is the span every position mapping in this header defaults to,
 * so a widget never has to measure itself.
 */
lh_int_t
lh_entity_range_usable(const lh_entity_range_t *self);

/**
 * @brief Set the ends. The value is pulled back inside them. An upper end
 *        below the lower one is raised to the lower one.
 */
lh_void
lh_entity_range_set_ends(lh_entity_range_t *self, lh_int_t minimum, lh_int_t maximum);

/**
 * @brief Set the value a reset returns to. The current value is pulled back
 *        inside the ends.
 */
lh_void
lh_entity_range_set_start(lh_entity_range_t *self, lh_int_t start);

/**
 * @brief Set the value, pulled back inside the ends.
 */
lh_void
lh_entity_range_set_value(lh_entity_range_t *self, lh_int_t value);

/**
 * @brief Put the value back to the start. What a reset means to a caller.
 */
lh_void
lh_entity_range_to_start(lh_entity_range_t *self);

/**
 * @brief Set the value from a percentage, 0 to 100, with the ends taken out
 *        of it. A percentage outside 0 to 100 is the nearer end.
 */
lh_void
lh_entity_range_set_from_percent(lh_entity_range_t *self, lh_int_t percent);

/**
 * @brief Set the bar thickness. A negative thickness becomes 0, and one past
 *        the short side is kept by the bar that draws it.
 */
lh_void
lh_entity_range_set_thickness(lh_entity_range_t *self, lh_int_t thickness);

/**
 * @brief Set the value from a position @p pos along a length @p span.
 *
 * @p pos of 0 is the minimum. @p pos of @p span is the maximum. For a bar that
 * moves over its own track see ::lh_entity_range_set_from_local_pos; the
 * explicit span is for a caller whose span is not its own size, such as a
 * scrollbar moving over its content.
 */
lh_void
lh_entity_range_set_from_pos(lh_entity_range_t *self, lh_int_t pos, lh_int_t span);

/**
 * @brief How far along @p span the value sits, in pixels.
 */
lh_int_t
lh_entity_range_to_pos(const lh_entity_range_t *self, lh_int_t span);

/**
 * @brief Set the value from a position along @p self's own track, with the
 *        length taken from ::lh_entity_range_usable.
 */
lh_void
lh_entity_range_set_from_local_pos(lh_entity_range_t *self, lh_int_t pos);

/**
 * @brief How far along @p self's own track the value sits, in pixels.
 */
lh_int_t
lh_entity_range_to_local_pos(const lh_entity_range_t *self);

/**
 * @brief Half the shorter side of @p bounds, or 0 when that side is under 2.
 *
 * That is the radius that turns a bar into a capsule. The progress bar,
 * the trackbar, the scrollbar and the switch all use it.
 */
lh_int_t
lh_entity_range_cap(const lh_math_rect_t *bounds);

/**
 * @brief A bar of thickness @p thick centered on the short side of @p bounds.
 *
 * The long side stays the length of @p bounds. A negative thickness
 * becomes 0. A thickness past the short side becomes that side. The ends
 * of this bar are ::lh_entity_range_cap.
 */
lh_math_rect_t
lh_entity_range_track(const lh_math_rect_t *bounds, lh_int_t thick);

/**
 * @brief The filled part of @p track, @p filled pixels along its long side.
 *
 * A wide bar fills from the left. A tall bar fills upward from the bottom.
 * @p filled is pulled into the length of @p track.
 */
lh_math_rect_t
lh_entity_range_fill(const lh_math_rect_t *track, lh_int_t filled);

/**
 * @brief Paint the track and the filled part of @p self into @p canvas.
 *
 * The long side of the box is the track. Both ends are rounded to half
 * the thickness. A taller box fills upward.
 */
lh_void
lh_entity_range_paint(const lh_entity_range_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_RANGE_H */
