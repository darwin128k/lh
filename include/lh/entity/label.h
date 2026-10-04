/**
 * @file label.h
 * @brief An entity that draws a line of text (LVGL's `lv_label`).
 *
 * It is a 2D entity. The background is the style, the same as any box.
 * The glyphs are the style's text color, drawn by a ::lh_ui_font_t on top
 * of that background. The ink (::lh_ui_font_ink) is placed in the box by
 * ::lh_entity_label_set_align, on both axes. A label that is the parent's
 * only child and still sits at the origin uses that parent's box, so the
 * words inside a button follow the label. The string and the font are not
 * copied and not owned: both must outlive the label.
 */

#ifndef LH_ENTITY_LABEL_H
#define LH_ENTITY_LABEL_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/ui/font.h>

/**
 * @def LH_ENTITY_LABEL_START
 * @brief Pack the ink against the start of the box.
 */
#define LH_ENTITY_LABEL_START 0

/**
 * @def LH_ENTITY_LABEL_CENTER
 * @brief Pack the ink in the middle of the box. The initial value on both axes.
 */
#define LH_ENTITY_LABEL_CENTER 1

/**
 * @def LH_ENTITY_LABEL_END
 * @brief Pack the ink against the end of the box.
 */
#define LH_ENTITY_LABEL_END 2

/**
 * @struct lh_entity_label
 * @brief A 2D entity plus the text it shows and the font it shows it with.
 */
struct lh_entity_label
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_ui_font_t *font;
    const lh_char_t *text;
    lh_int_t align_x;
    lh_int_t align_y;
};
typedef struct lh_entity_label lh_entity_label_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_label_t, derived from ::lh_entity_2d_class.
 *
 * A new label uses ::lh_ui_font_get_default, aligns to the center on both
 * axes, and has no text, so its box is empty until ::lh_entity_label_set_text.
 */
extern const lh_entity_class_t lh_entity_label_class;

/**
 * @brief The font @p self draws with.
 */
const lh_ui_font_t *
lh_entity_label_get_font(const lh_entity_label_t *self);

/**
 * @brief Point @p self at @p font and size the box to the current text.
 *        ::lh_null draws nothing.
 */
lh_void
lh_entity_label_set_font(lh_entity_label_t *self, const lh_ui_font_t *font);

/**
 * @brief The string @p self draws, or ::lh_null when it has none.
 */
const lh_char_t *
lh_entity_label_get_text(const lh_entity_label_t *self);

/**
 * @brief Point @p self at @p text and size the box to it.
 *
 * @p text is not copied. ::lh_null clears the label.
 */
lh_void
lh_entity_label_set_text(lh_entity_label_t *self, const lh_char_t *text);

/**
 * @brief Where the ink sits across the box: ::LH_ENTITY_LABEL_START,
 *        ::LH_ENTITY_LABEL_CENTER or ::LH_ENTITY_LABEL_END.
 */
lh_int_t
lh_entity_label_get_align_x(const lh_entity_label_t *self);

/**
 * @brief Where the ink sits down the box.
 */
lh_int_t
lh_entity_label_get_align_y(const lh_entity_label_t *self);

/**
 * @brief Place the ink on both axes. Anything else becomes
 *        ::LH_ENTITY_LABEL_START.
 */
lh_void
lh_entity_label_set_align(lh_entity_label_t *self, lh_int_t align_x, lh_int_t align_y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_LABEL_H */
