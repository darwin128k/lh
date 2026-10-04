/**
 * @file range.h
 * @brief A number between two ends, and the bar that shows it.
 *
 * The progress bar is this class. A trackbar, a knob, a spin box and a
 * scrollbar are the same fields with their own pointer handling, so a
 * pointer to any of them is also an ::lh_entity_range_t. The track is the
 * style background. The filled part is the style text color.
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
 * @brief Class of a progress bar, derived from ::lh_entity_2d_class.
 *
 * A new bar runs from 0 to 100 and sits at 0.
 */
extern const lh_entity_class_t lh_entity_progress_class;

/**
 * @brief A new range: 0 to 100, value 0.
 */
lh_void
lh_entity_range_reset(lh_entity_range_t *self);

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
 * @brief Current value of @p self, already inside the ends.
 */
lh_int_t
lh_entity_range_get_value(const lh_entity_range_t *self);

/**
 * @brief Set the ends. The value is pulled back inside them. An upper end
 *        below the lower one is raised to the lower one.
 */
lh_void
lh_entity_range_set_ends(lh_entity_range_t *self, lh_int_t minimum, lh_int_t maximum);

/**
 * @brief Set the value, pulled back inside the ends.
 */
lh_void
lh_entity_range_set_value(lh_entity_range_t *self, lh_int_t value);

/**
 * @brief Set the value from a position @p pos along a length @p span.
 *
 * @p pos of 0 is the minimum. @p pos of @p span is the maximum.
 */
lh_void
lh_entity_range_set_from_pos(lh_entity_range_t *self, lh_int_t pos, lh_int_t span);

/**
 * @brief How far along @p span the value sits, in pixels.
 */
lh_int_t
lh_entity_range_to_pos(const lh_entity_range_t *self, lh_int_t span);

/**
 * @brief Paint the track and the filled part of @p self into @p canvas.
 *
 * The long side of the box is the track. A taller box fills upward.
 */
lh_void
lh_entity_range_paint(const lh_entity_range_t *self, lh_ui_canvas_t *canvas);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_RANGE_H */
