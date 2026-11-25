/**
 * @file metrics.h
 * @brief What a label measured the last time: ::lh_gl_label_metrics_t.
 *
 * The size of the text, the top of its ink, its ink rect and where it starts,
 * with what they were measured against: the font, the rect, the padding, the
 * alignment and the width it wraps at. Whether they hold is the label's own
 * `metrics_ready`; the label measures again once any input changed, so a repaint draws the glyphs
 * and measures nothing. A part of ::lh_gl_label_t that lives inside it
 * (::lh_gl_label_get_metrics).
 */

#ifndef LH_GL_LABEL_METRICS_H
#define LH_GL_LABEL_METRICS_H

#include <lh/core/compiler/extern/c.h>
#include <lh/gl/insets.h>
#include <lh/gl/label/metrics/fields.h>
#include <lh/gl/point.h>
#include <lh/gl/rect.h>
#include <lh/gl/size.h>
#include <lh/gl/text/align.h>

struct lh_font;

/**
 * @struct lh_gl_label_metrics
 * @brief What a label measured the last time.
 */
struct lh_gl_label_metrics
{
    /** Members: ::lh_gl_label_metrics_fields. */
    lh_gl_label_metrics_fields();
};
/** @copybrief lh_gl_label_metrics */
typedef struct lh_gl_label_metrics lh_gl_label_metrics_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The rect the text was measured in, read in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_rect_t *
lh_gl_label_metrics_get_rect(const lh_gl_label_metrics_t *self);

/**
 * @brief The rect the text was measured in, to change in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_rect_t *
lh_gl_label_metrics_get_rect_mut(lh_gl_label_metrics_t *self);

/**
 * @brief Store the rect the text was measured in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param rect The new value.
 */
lh_void
lh_gl_label_metrics_set_rect(lh_gl_label_metrics_t *self, lh_gl_rect_t rect);

/**
 * @brief The rect the measured ink covers, read in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_rect_t *
lh_gl_label_metrics_get_ink(const lh_gl_label_metrics_t *self);

/**
 * @brief The rect the measured ink covers, to change in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_rect_t *
lh_gl_label_metrics_get_ink_mut(lh_gl_label_metrics_t *self);

/**
 * @brief Store the rect the measured ink covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param ink The new value.
 */
lh_void
lh_gl_label_metrics_set_ink(lh_gl_label_metrics_t *self, lh_gl_rect_t ink);

/**
 * @brief The padding the text was measured inside, read in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_insets_t *
lh_gl_label_metrics_get_padding(const lh_gl_label_metrics_t *self);

/**
 * @brief The padding the text was measured inside, to change in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_insets_t *
lh_gl_label_metrics_get_padding_mut(lh_gl_label_metrics_t *self);

/**
 * @brief Store the padding the text was measured inside (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param padding The new value.
 */
lh_void
lh_gl_label_metrics_set_padding(lh_gl_label_metrics_t *self, lh_gl_insets_t padding);

/**
 * @brief Where the measured text starts, read in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_point_t *
lh_gl_label_metrics_get_origin(const lh_gl_label_metrics_t *self);

/**
 * @brief Where the measured text starts, to change in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_point_t *
lh_gl_label_metrics_get_origin_mut(lh_gl_label_metrics_t *self);

/**
 * @brief Store where the measured text starts (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param origin The new value.
 */
lh_void
lh_gl_label_metrics_set_origin(lh_gl_label_metrics_t *self, lh_gl_point_t origin);

/**
 * @brief The size of the measured text, read in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_size_t *
lh_gl_label_metrics_get_size(const lh_gl_label_metrics_t *self);

/**
 * @brief The size of the measured text, to change in place (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_size_t *
lh_gl_label_metrics_get_size_mut(lh_gl_label_metrics_t *self);

/**
 * @brief Store the size of the measured text (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param size The new value.
 */
lh_void
lh_gl_label_metrics_set_size(lh_gl_label_metrics_t *self, lh_gl_size_t size);

/**
 * @brief The font the text was measured with (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 */
const struct lh_font *
lh_gl_label_metrics_get_font(const lh_gl_label_metrics_t *self);

/**
 * @brief Store the font the text was measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param font The new value.
 */
lh_void
lh_gl_label_metrics_set_font(lh_gl_label_metrics_t *self, const struct lh_font *font);

/**
 * @brief The top of the measured ink (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_label_metrics_get_ink_top(const lh_gl_label_metrics_t *self);

/**
 * @brief Store the top of the measured ink (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param ink_top The new value.
 */
lh_void
lh_gl_label_metrics_set_ink_top(lh_gl_label_metrics_t *self, lh_gl_scalar_t ink_top);

/**
 * @brief The width the text was wrapped at (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_label_metrics_get_wrap(const lh_gl_label_metrics_t *self);

/**
 * @brief Store the width the text was wrapped at (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param wrap The new value.
 */
lh_void
lh_gl_label_metrics_set_wrap(lh_gl_label_metrics_t *self, lh_gl_scalar_t wrap);

/**
 * @brief The alignment across it was measured with (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 */
lh_gl_text_align_h_t
lh_gl_label_metrics_get_align_h(const lh_gl_label_metrics_t *self);

/**
 * @brief Store the alignment across it was measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param align_h The new value.
 */
lh_void
lh_gl_label_metrics_set_align_h(lh_gl_label_metrics_t *self, lh_gl_text_align_h_t align_h);

/**
 * @brief The alignment down it was measured with (level 1).
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 */
lh_gl_text_align_v_t
lh_gl_label_metrics_get_align_v(const lh_gl_label_metrics_t *self);

/**
 * @brief Store the alignment down it was measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The metrics; checked by ::lh_assert_runtime_ref.
 * @param align_v The new value.
 */
lh_void
lh_gl_label_metrics_set_align_v(lh_gl_label_metrics_t *self, lh_gl_text_align_v_t align_v);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_LABEL_METRICS_H */
