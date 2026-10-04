/**
 * @file field.h
 * @brief One line of text, or several, that the keyboard can edit.
 *
 * The bytes live in a buffer the caller owns. ::lh_entity_field_set_lines
 * of 1 ignores enter. More than 1 stores a new line. Keys arrive as
 * ::LH_ENTITY_EVENT_KEY while the field is the screen focus, which a press
 * on the field takes. The caret is a one-pixel bar in the text color.
 */

#ifndef LH_ENTITY_FIELD_H
#define LH_ENTITY_FIELD_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>
#include <lh/ui/font.h>

/**
 * @struct lh_entity_field
 * @brief An editable buffer.
 */
struct lh_entity_field
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_char_t *text;
    const lh_ui_font_t *font;
    lh_int_t capacity;
    lh_int_t length;
    lh_int_t cursor;
    lh_int_t lines;
};
typedef struct lh_entity_field lh_entity_field_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_field_t, derived from ::lh_entity_2d_class.
 *
 * A new field is one line and uses ::lh_ui_font_basic. It has no buffer
 * until ::lh_entity_field_set_buffer.
 */
extern const lh_entity_class_t lh_entity_field_class;

/**
 * @brief Point @p self at @p text, which holds @p capacity bytes including
 *        the trailing NUL. Not copied. The buffer is cleared.
 */
lh_void
lh_entity_field_set_buffer(lh_entity_field_t *self, lh_char_t *text, lh_int_t capacity);

/**
 * @brief Copy @p text into the buffer, cut to what fits.
 */
lh_void
lh_entity_field_set_text(lh_entity_field_t *self, const lh_char_t *text);

/**
 * @brief The buffer, or ::lh_null.
 */
const lh_char_t *
lh_entity_field_get_text(const lh_entity_field_t *self);

/**
 * @brief How many lines the box keeps. 1 is a single line.
 */
lh_void
lh_entity_field_set_lines(lh_entity_field_t *self, lh_int_t lines);

/**
 * @brief Point @p self at @p font. Not owned.
 */
lh_void
lh_entity_field_set_font(lh_entity_field_t *self, const lh_ui_font_t *font);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_FIELD_H */
