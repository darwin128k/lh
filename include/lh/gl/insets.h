/**
 * @file insets.h
 * @brief Four distances in from the sides of a rect: ::lh_gl_insets_t.
 *
 * What a padding is (and later a border width or a margin): left, top, right,
 * bottom. ::lh_gl_insets_shrink gives the rect inside them; the start / end
 * getters read them along an axis for layouts.
 */

#ifndef LH_GL_INSETS_H
#define LH_GL_INSETS_H

#include <lh/gl/axis.h>
#include <lh/gl/insets/fields.h>

/**
 * @struct lh_gl_insets
 * @brief Left, top, right, bottom.
 */
struct lh_gl_insets
{
    /** Members: ::lh_gl_insets_fields. */
    lh_gl_insets_fields(lh_gl_scalar_t);
};
/** @copybrief lh_gl_insets */
typedef struct lh_gl_insets lh_gl_insets_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set each side of @p self.
 */
lh_void
lh_gl_insets_init(lh_gl_insets_t *self, lh_gl_scalar_t left, lh_gl_scalar_t top,
                  lh_gl_scalar_t right, lh_gl_scalar_t bottom);

/**
 * @brief @p value on every side.
 */
lh_void
lh_gl_insets_init_all(lh_gl_insets_t *self, lh_gl_scalar_t value);

/**
 * @brief Left inset.
 */
lh_gl_scalar_t
lh_gl_insets_get_left(const lh_gl_insets_t *self);

/**
 * @brief Top inset.
 */
lh_gl_scalar_t
lh_gl_insets_get_top(const lh_gl_insets_t *self);

/**
 * @brief Right inset.
 */
lh_gl_scalar_t
lh_gl_insets_get_right(const lh_gl_insets_t *self);

/**
 * @brief Bottom inset.
 */
lh_gl_scalar_t
lh_gl_insets_get_bottom(const lh_gl_insets_t *self);

/**
 * @brief Inset at the start of @p axis: left (horizontal) or top (vertical).
 */
lh_gl_scalar_t
lh_gl_insets_get_start(const lh_gl_insets_t *self, lh_gl_axis_t axis);

/**
 * @brief Inset at the end of @p axis: right (horizontal) or bottom (vertical).
 */
lh_gl_scalar_t
lh_gl_insets_get_end(const lh_gl_insets_t *self, lh_gl_axis_t axis);

/**
 * @brief True when every side is `0`.
 */
lh_bool_t
lh_gl_insets_is_zero(const lh_gl_insets_t *self);

/**
 * @brief @p rect with each side moved in by @p self; the size never goes
 *        below `0`.
 */
lh_void
lh_gl_insets_shrink(const lh_gl_insets_t *self, const lh_gl_rect_t *rect, lh_gl_rect_t *out);

/**
 * @brief Store the left inset (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The insets; checked by ::lh_assert_runtime_ref.
 * @param left The new value.
 */
lh_void
lh_gl_insets_set_left(lh_gl_insets_t *self, lh_gl_scalar_t left);

/**
 * @brief Store the top inset (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The insets; checked by ::lh_assert_runtime_ref.
 * @param top The new value.
 */
lh_void
lh_gl_insets_set_top(lh_gl_insets_t *self, lh_gl_scalar_t top);

/**
 * @brief Store the right inset (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The insets; checked by ::lh_assert_runtime_ref.
 * @param right The new value.
 */
lh_void
lh_gl_insets_set_right(lh_gl_insets_t *self, lh_gl_scalar_t right);

/**
 * @brief Store the bottom inset (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The insets; checked by ::lh_assert_runtime_ref.
 * @param bottom The new value.
 */
lh_void
lh_gl_insets_set_bottom(lh_gl_insets_t *self, lh_gl_scalar_t bottom);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_INSETS_H */
