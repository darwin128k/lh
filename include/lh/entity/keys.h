/**
 * @file keys.h
 * @brief An on-screen keyboard. A press sends that key to the screen focus.
 *
 * The drawing is the keys. There is no child button per key, so a small
 * display pays for one entity. The letters are the style text color on the
 * style background.
 */

#ifndef LH_ENTITY_KEYS_H
#define LH_ENTITY_KEYS_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/ui/font.h>

/**
 * @struct lh_entity_keys
 * @brief A grid of keys.
 */
struct lh_entity_keys
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_ui_font_t *font;
};
typedef struct lh_entity_keys lh_entity_keys_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_keys_t, derived from ::lh_entity_2d_class.
 *
 * A new keyboard uses ::lh_ui_font_get_default.
 */
extern const lh_entity_class_t lh_entity_keys_class;

/**
 * @brief Point @p self at @p font. Not owned.
 */
lh_void
lh_entity_keys_set_font(lh_entity_keys_t *self, const lh_ui_font_t *font);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_KEYS_H */
