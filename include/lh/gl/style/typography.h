/**
 * @file typography.h
 * @brief The typography of a style: how an entity draws its text: ::lh_gl_style_typography_t.
 *
 * The font (not owned, may be ::lh_null), the paint the text is drawn with
 * (held by value), where inside the padded box the text starts (`align_h`,
 * `align_v`), what a line too wide for it draws (`overflow`) and whether the
 * text is broken into lines that fit (`wrap`). A part of ::lh_gl_style_t that
 * lives inside it (::lh_gl_style_get_typography).
 */

#ifndef LH_GL_STYLE_TYPOGRAPHY_H
#define LH_GL_STYLE_TYPOGRAPHY_H

#include <lh/gl/font.h>
#include <lh/gl/paint.h>
#include <lh/gl/style/typography/fields.h>
#include <lh/gl/text/align.h>
#include <lh/gl/text/overflow.h>

/**
 * @struct lh_gl_style_typography
 * @brief The typography of a style: how an entity draws its text.
 */
struct lh_gl_style_typography
{
    /** Members: ::lh_gl_style_typography_fields. */
    lh_gl_style_typography_fields(lh_gl_paint_t, lh_gl_font_t);
};
/** @copybrief lh_gl_style_typography */
typedef struct lh_gl_style_typography lh_gl_style_typography_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The paint the text is drawn with, read in place (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_paint_t *
lh_gl_style_typography_get_paint(const lh_gl_style_typography_t *self);

/**
 * @brief The paint the text is drawn with, to change in place (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_paint_t *
lh_gl_style_typography_get_paint_mut(lh_gl_style_typography_t *self);

/**
 * @brief Store the paint the text is drawn with (level 1).
 *
 * A level-1 setter: the value is copied as given and is not validated.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @param paint The new value.
 */
lh_void
lh_gl_style_typography_set_paint(lh_gl_style_typography_t *self, lh_gl_paint_t paint);

/**
 * @brief The font the text is drawn with (not owned, may be ::lh_null) (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_font_t *
lh_gl_style_typography_get_font(const lh_gl_style_typography_t *self);

/**
 * @brief Store the font the text is drawn with (not owned, may be ::lh_null) (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @param font The new value.
 */
lh_void
lh_gl_style_typography_set_font(lh_gl_style_typography_t *self, const lh_gl_font_t *font);

/**
 * @brief Where across the padded box the text starts (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 */
lh_gl_text_align_h_t
lh_gl_style_typography_get_align_h(const lh_gl_style_typography_t *self);

/**
 * @brief Store where across the padded box the text starts (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @param align_h The new value.
 */
lh_void
lh_gl_style_typography_set_align_h(lh_gl_style_typography_t *self, lh_gl_text_align_h_t align_h);

/**
 * @brief Where down the padded box the text starts (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 */
lh_gl_text_align_v_t
lh_gl_style_typography_get_align_v(const lh_gl_style_typography_t *self);

/**
 * @brief Store where down the padded box the text starts (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @param align_v The new value.
 */
lh_void
lh_gl_style_typography_set_align_v(lh_gl_style_typography_t *self, lh_gl_text_align_v_t align_v);

/**
 * @brief What a line too wide for the padded box draws (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 */
lh_gl_text_overflow_t
lh_gl_style_typography_get_overflow(const lh_gl_style_typography_t *self);

/**
 * @brief Store what a line too wide for the padded box draws (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @param overflow The new value.
 */
lh_void
lh_gl_style_typography_set_overflow(lh_gl_style_typography_t *self, lh_gl_text_overflow_t overflow);

/**
 * @brief Whether the text is broken into lines that fit (level 1).
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 */
lh_gl_text_wrap_t
lh_gl_style_typography_get_wrap(const lh_gl_style_typography_t *self);

/**
 * @brief Store whether the text is broken into lines that fit (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 * @param wrap The new value.
 */
lh_void
lh_gl_style_typography_set_wrap(lh_gl_style_typography_t *self, lh_gl_text_wrap_t wrap);

/**
 * @brief Make @p self the part of a fresh style (::lh_gl_style_init): The default font
 * (::lh_gl_font_get_default), opaque black, top left, clipped and unwrapped.
 *
 * @param self The typography; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_style_typography_init(lh_gl_style_typography_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_STYLE_TYPOGRAPHY_H */
