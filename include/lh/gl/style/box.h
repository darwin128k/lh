/**
 * @file box.h
 * @brief The box of a style: what is painted under, around and on the edge of an entity:
 * ::lh_gl_style_box_t.
 *
 * The fill (a paint held by value; the empty paint is no fill), its corner
 * radii, the padding that keeps content inside the rect, the shadow cast by
 * the fill, and the border painted after the children in its own paint. The
 * smallest of the `radii`, which is what a clipping parent cuts its children
 * with, is read from them (::lh_gl_style_get_radius), not kept. A box is a
 * part of ::lh_gl_style_t and lives inside it (::lh_gl_style_get_box).
 */

#ifndef LH_GL_STYLE_BOX_H
#define LH_GL_STYLE_BOX_H

#include <lh/gl/insets.h>
#include <lh/gl/paint.h>
#include <lh/gl/radii.h>
#include <lh/gl/shadow.h>
#include <lh/gl/style/box/fields.h>

/**
 * @struct lh_gl_style_box
 * @brief The box of a style: what is painted under, around and on the edge of an entity.
 */
struct lh_gl_style_box
{
    /** Members: ::lh_gl_style_box_fields. */
    lh_gl_style_box_fields(lh_gl_paint_t, lh_gl_insets_t, lh_gl_shadow_t, lh_gl_radii_t);
};
/** @copybrief lh_gl_style_box */
typedef struct lh_gl_style_box lh_gl_style_box_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The paint the background is filled with, read in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_paint_t *
lh_gl_style_box_get_fill(const lh_gl_style_box_t *self);

/**
 * @brief The paint the background is filled with, to change in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_paint_t *
lh_gl_style_box_get_fill_mut(lh_gl_style_box_t *self);

/**
 * @brief Store the paint the background is filled with (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param fill The new value.
 */
lh_void
lh_gl_style_box_set_fill(lh_gl_style_box_t *self, lh_gl_paint_t fill);

/**
 * @brief The corner radii, read in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_radii_t *
lh_gl_style_box_get_radii(const lh_gl_style_box_t *self);

/**
 * @brief The corner radii, to change in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_radii_t *
lh_gl_style_box_get_radii_mut(lh_gl_style_box_t *self);

/**
 * @brief Store the corner radii (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param radii The new value.
 */
lh_void
lh_gl_style_box_set_radii(lh_gl_style_box_t *self, lh_gl_radii_t radii);

/**
 * @brief The padding inside the border, read in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_insets_t *
lh_gl_style_box_get_padding(const lh_gl_style_box_t *self);

/**
 * @brief The padding inside the border, to change in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_insets_t *
lh_gl_style_box_get_padding_mut(lh_gl_style_box_t *self);

/**
 * @brief Store the padding inside the border (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param padding The new value.
 */
lh_void
lh_gl_style_box_set_padding(lh_gl_style_box_t *self, lh_gl_insets_t padding);

/**
 * @brief The shadow cast by the fill, read in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_shadow_t *
lh_gl_style_box_get_shadow(const lh_gl_style_box_t *self);

/**
 * @brief The shadow cast by the fill, to change in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_shadow_t *
lh_gl_style_box_get_shadow_mut(lh_gl_style_box_t *self);

/**
 * @brief Store the shadow cast by the fill (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param shadow The new value.
 */
lh_void
lh_gl_style_box_set_shadow(lh_gl_style_box_t *self, lh_gl_shadow_t shadow);

/**
 * @brief The border widths, read in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_insets_t *
lh_gl_style_box_get_border(const lh_gl_style_box_t *self);

/**
 * @brief The border widths, to change in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_insets_t *
lh_gl_style_box_get_border_mut(lh_gl_style_box_t *self);

/**
 * @brief Store the border widths (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param border The new value.
 */
lh_void
lh_gl_style_box_set_border(lh_gl_style_box_t *self, lh_gl_insets_t border);

/**
 * @brief The paint the border is drawn with, read in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_paint_t *
lh_gl_style_box_get_border_fill(const lh_gl_style_box_t *self);

/**
 * @brief The paint the border is drawn with, to change in place (level 1).
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_paint_t *
lh_gl_style_box_get_border_fill_mut(lh_gl_style_box_t *self);

/**
 * @brief Store the paint the border is drawn with (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 * @param border_fill The new value.
 */
lh_void
lh_gl_style_box_set_border_fill(lh_gl_style_box_t *self, lh_gl_paint_t border_fill);

/**
 * @brief Make @p self the part of a fresh style (::lh_gl_style_init): The empty fill, square
 * corners, no padding, no shadow, no border and the empty border paint.
 *
 * @param self The box; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_style_box_init(lh_gl_style_box_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_STYLE_BOX_H */
