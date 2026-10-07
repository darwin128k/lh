/**
 * @file insets.h
 * @brief Four distances in from the sides of a rect: ::lh_ui_insets_t.
 *
 * What a padding is (and later a border width or a margin): left, top, right,
 * bottom. ::lh_ui_insets_shrink gives the rect inside them; the start / end
 * getters read them along an axis for layouts.
 */

#ifndef LH_UI_INSETS_H
#define LH_UI_INSETS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ui/axis.h>
#include <lh/ui/insets/fields.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_ui_insets
 * @typedef lh_ui_insets_t
 * @brief Left, top, right, bottom.
 */
struct lh_ui_insets
{
    lh_ui_insets_fields(lh_ui_scalar_t);
};
typedef struct lh_ui_insets lh_ui_insets_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set each side of @p self.
 */
lh_void
lh_ui_insets_init(lh_ui_insets_t *self, lh_ui_scalar_t left, lh_ui_scalar_t top, lh_ui_scalar_t right,
                  lh_ui_scalar_t bottom);

/**
 * @brief @p value on every side.
 */
lh_void
lh_ui_insets_init_all(lh_ui_insets_t *self, lh_ui_scalar_t value);

/**
 * @brief Left inset.
 */
lh_ui_scalar_t
lh_ui_insets_get_left(const lh_ui_insets_t *self);

/**
 * @brief Top inset.
 */
lh_ui_scalar_t
lh_ui_insets_get_top(const lh_ui_insets_t *self);

/**
 * @brief Right inset.
 */
lh_ui_scalar_t
lh_ui_insets_get_right(const lh_ui_insets_t *self);

/**
 * @brief Bottom inset.
 */
lh_ui_scalar_t
lh_ui_insets_get_bottom(const lh_ui_insets_t *self);

/**
 * @brief Inset at the start of @p axis: left (horizontal) or top (vertical).
 */
lh_ui_scalar_t
lh_ui_insets_get_start(const lh_ui_insets_t *self, lh_ui_axis_t axis);

/**
 * @brief Inset at the end of @p axis: right (horizontal) or bottom (vertical).
 */
lh_ui_scalar_t
lh_ui_insets_get_end(const lh_ui_insets_t *self, lh_ui_axis_t axis);

/**
 * @brief True when every side is `0`.
 */
lh_bool_t
lh_ui_insets_is_zero(const lh_ui_insets_t *self);

/**
 * @brief @p rect with each side moved in by @p self; the size never goes
 *        below `0`.
 */
lh_ui_rect_t
lh_ui_insets_shrink(const lh_ui_insets_t *self, const lh_ui_rect_t *rect);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_INSETS_H */
