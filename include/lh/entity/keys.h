/**
 * @file keys.h
 * @brief An on-screen keyboard made of buttons.
 *
 * A key is a ::lh_entity_button_t with a label for its caption.
 * ::lh_entity_keys_add appends one to the current row and returns that
 * button, so a layout can resize a key. ::lh_entity_keys_break starts the
 * next row. The caption is copied. A click sends @p code to the screen
 * focus (::LH_ENTITY_EVENT_KEY): a character's Unicode code point, or one
 * of the ::LH_ENTITY_KEY_* names. English, German and Cyrillic are the
 * same calls with different captions and codes. The font has to contain
 * the caption's glyphs; the built-in Roboto covers ASCII.
 */

#ifndef LH_ENTITY_KEYS_H
#define LH_ENTITY_KEYS_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/button.h>
#include <lh/ui/font.h>
#include <lh/ui/style.h>

/**
 * @struct lh_entity_keys
 * @brief Rows of key buttons.
 */
struct lh_entity_keys
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_ui_font_t *font;
    const lh_ui_style_t *pressed;
    lh_entity_t *row;
    lh_int_t key_width;
    lh_int_t key_height;
    lh_int_t gap;
};
typedef struct lh_entity_keys lh_entity_keys_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_keys_t, derived from ::lh_entity_2d_class.
 *
 * A new keyboard lays rows in a column, keys 26 by 26 with a gap of 4,
 * and uses ::lh_ui_font_get_default. Set the style, the font and the key
 * size before ::lh_entity_keys_add.
 */
extern const lh_entity_class_t lh_entity_keys_class;

/**
 * @brief Point @p self at @p font. Not owned. Used by keys added afterwards.
 */
lh_void
lh_entity_keys_set_font(lh_entity_keys_t *self, const lh_ui_font_t *font);

/**
 * @brief Size of a key added afterwards, in pixels.
 */
lh_void
lh_entity_keys_set_key_size(lh_entity_keys_t *self, lh_int_t width, lh_int_t height);

/**
 * @brief Gap between keys and between rows, in pixels. Rows added
 *        afterwards pick it up.
 */
lh_void
lh_entity_keys_set_gap(lh_entity_keys_t *self, lh_int_t gap);

/**
 * @brief Style a key uses while it is down. ::lh_null keeps the ordinary
 *        style. Not owned. Keys added afterwards pick it up.
 */
lh_void
lh_entity_keys_set_pressed_style(lh_entity_keys_t *self, const lh_ui_style_t *style);

/**
 * @brief Add a key button labelled @p text that sends @p code. @p text is
 *        copied. The button is a child of the current row.
 */
lh_entity_button_t *
lh_entity_keys_add(lh_entity_keys_t *self, const lh_char_t *text, lh_uint_t code);

/**
 * @brief Start the next row.
 */
lh_void
lh_entity_keys_break(lh_entity_keys_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_KEYS_H */
