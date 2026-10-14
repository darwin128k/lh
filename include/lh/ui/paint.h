/**
 * @file paint.h
 * @brief Paint: ::lh_ui_paint_t, nothing or one solid ::lh_ui_color_t.
 *
 * The color is held by value, so a paint never dangles. ::lh_ui_brush_t is an
 * alias of this type; ::lh_ui_pen_t embeds one. Stop maps live on
 * ::lh_ui_gradient_t, laid over the box the paint fills (::lh_ui_paint_init_gradient).
 */

#ifndef LH_UI_PAINT_H
#define LH_UI_PAINT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/color.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint/fields.h>
#include <lh/ui/paint/kind.h>
#include <lh/void.h>

/**
 * @struct lh_ui_paint
 * @brief Nothing, or a solid color (also brush, and the paint of a pen).
 */
struct lh_ui_paint
{
    /** Members: ::lh_ui_paint_fields. */
    lh_ui_paint_fields(lh_ui_paint_kind_t, lh_ui_color_t, lh_ui_gradient_t);
};
/** @copybrief lh_ui_paint */
typedef struct lh_ui_paint lh_ui_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with the empty paint (::lh_ui_paint_kind_none).
 */
lh_void
lh_ui_paint_init(lh_ui_paint_t *self);

/**
 * @brief Fill @p self with a copy of @p color (::lh_ui_paint_kind_solid).
 *
 * ::lh_null gives the empty paint.
 */
lh_void
lh_ui_paint_init_color(lh_ui_paint_t *self, const lh_ui_color_t *color);

/**
 * @brief Fill @p self with a copy of @p other; ::lh_null gives the empty paint.
 */
lh_void
lh_ui_paint_init_copy(lh_ui_paint_t *self, const lh_ui_paint_t *other);

/**
 * @brief Fill @p self with @p gradient (::lh_ui_paint_kind_gradient), pointed at and
 *        not copied: it must outlive every paint made from it. Its first stop's colour
 *        is also the paint's colour, for whatever asks a gradient for one colour.
 */
lh_void
lh_ui_paint_init_gradient(lh_ui_paint_t *self, const lh_ui_gradient_t *gradient);

/**
 * @brief The gradient of @p self, or ::lh_null when it is not one.
 */
const lh_ui_gradient_t *
lh_ui_paint_get_gradient(const lh_ui_paint_t *self);

/**
 * @brief What @p self holds.
 */
lh_ui_paint_kind_t
lh_ui_paint_get_kind(const lh_ui_paint_t *self);

/**
 * @brief True when @p self paints nothing.
 */
lh_bool_t
lh_ui_paint_is_empty(const lh_ui_paint_t *self);

/**
 * @brief Solid color of @p self, or ::lh_null when it is not solid.
 *
 * The pointer is into @p self and lives as long as it does.
 */
const lh_ui_color_t *
lh_ui_paint_get_color(const lh_ui_paint_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PAINT_H */
