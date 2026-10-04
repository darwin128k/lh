/**
 * @file shadow.h
 * @brief A soft shadow effect (::lh_ui_effect_t).
 *
 * The same record hangs on a button and on a window. Spread, offset and
 * which sides cast it are fields of the record. The window uses it for the
 * frame around the card; an entity draws it into the canvas before its fill.
 * The record is not copied and not owned.
 */

#ifndef LH_UI_SHADOW_H
#define LH_UI_SHADOW_H

#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/color.h>
#include <lh/ui/effect.h>
#include <lh/ui/shadow/fields.h>

/**
 * @brief Edges that cast a shadow. Combine them.
 */
typedef lh_byte_t lh_ui_shadow_side_t;

/** @brief The left edge. */
#define LH_UI_SHADOW_SIDE_LEFT lh_cast_static(lh_ui_shadow_side_t, 1U)
/** @brief The right edge. */
#define LH_UI_SHADOW_SIDE_RIGHT lh_cast_static(lh_ui_shadow_side_t, 2U)
/** @brief The top edge. */
#define LH_UI_SHADOW_SIDE_TOP lh_cast_static(lh_ui_shadow_side_t, 4U)
/** @brief The bottom edge. */
#define LH_UI_SHADOW_SIDE_BOTTOM lh_cast_static(lh_ui_shadow_side_t, 8U)
/** @brief Every edge. */
#define LH_UI_SHADOW_SIDE_ALL lh_cast_static(lh_ui_shadow_side_t, 15U)

/**
 * @struct lh_ui_shadow
 * @brief A shadow effect. Fields via ::lh_ui_effect_fields, then
 *        ::lh_ui_shadow_fields.
 */
struct lh_ui_shadow
{
    lh_ui_effect_fields(const lh_ui_effect_class_t *, const lh_ui_effect_t *);
    lh_ui_shadow_fields(lh_ui_color_t, lh_int_t, lh_ui_shadow_side_t);
};
typedef struct lh_ui_shadow lh_ui_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A shadow with zero spread and every side enabled. @p self paints
 *        nothing until a spread and a color are set.
 */
lh_void
lh_ui_shadow_init(lh_ui_shadow_t *self);

/**
 * @brief @p self as an effect, for ::lh_entity_2d_set_effect and
 *        ::lh_ui_effect_set_next.
 */
const lh_ui_effect_t *
lh_ui_shadow_effect(const lh_ui_shadow_t *self);

/**
 * @brief Shadow color. Alpha is the peak opacity.
 */
lh_ui_color_t
lh_ui_shadow_get_color(const lh_ui_shadow_t *self);

/**
 * @brief Set the shadow color. Does not redraw by itself.
 */
lh_void
lh_ui_shadow_set_color(lh_ui_shadow_t *self, lh_ui_color_t color);

/**
 * @brief Fade distance in pixels. Zero paints nothing.
 */
lh_int_t
lh_ui_shadow_get_spread(const lh_ui_shadow_t *self);

/**
 * @brief Set the fade distance. Negative becomes zero. Does not redraw
 *        by itself.
 */
lh_void
lh_ui_shadow_set_spread(lh_ui_shadow_t *self, lh_int_t spread);

/**
 * @brief Horizontal shift. Positive moves the shadow to the right.
 */
lh_int_t
lh_ui_shadow_get_offset_x(const lh_ui_shadow_t *self);

/**
 * @brief Vertical shift. Positive moves the shadow down.
 */
lh_int_t
lh_ui_shadow_get_offset_y(const lh_ui_shadow_t *self);

/**
 * @brief Set the shift. Does not redraw by itself.
 */
lh_void
lh_ui_shadow_set_offset(lh_ui_shadow_t *self, lh_int_t offset_x, lh_int_t offset_y);

/**
 * @brief Which edges cast the shadow.
 */
lh_ui_shadow_side_t
lh_ui_shadow_get_sides(const lh_ui_shadow_t *self);

/**
 * @brief Set which edges cast the shadow. Does not redraw by itself.
 */
lh_void
lh_ui_shadow_set_sides(lh_ui_shadow_t *self, lh_ui_shadow_side_t sides);

/**
 * @brief Pixels of padding that fit this shadow, including its shift.
 *        Zero when the spread is zero.
 */
lh_int_t
lh_ui_shadow_outset(const lh_ui_shadow_t *self);

/**
 * @brief Signed distance from (@p x, @p y) to the rounded box. Negative is
 *        inside. @p right and @p bottom are exclusive.
 */
lh_int_t
lh_ui_shadow_distance(lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top, lh_int_t right,
                      lh_int_t bottom, lh_int_t corner);

/**
 * @brief Alpha of @p self at (@p x, @p y), already scaled by the color's
 *        alpha. Zero inside the box, on a side that does not cast, and past
 *        the spread.
 */
lh_int_t
lh_ui_shadow_alpha(const lh_ui_shadow_t *self, lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top,
                   lh_int_t right, lh_int_t bottom, lh_int_t corner);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SHADOW_H */
