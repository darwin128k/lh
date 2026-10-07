/**
 * @file key.h
 * @brief What the keyboard hands an entity: ::lh_ui_key_input_t.
 *
 * ::lh_ui_view_key and ::lh_ui_view_text build one and send it to the focused
 * entity with ::lh_ui_entity_event_key.
 */

#ifndef LH_UI_KEY_H
#define LH_UI_KEY_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/key.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/key/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_key_input
 * @typedef lh_ui_key_input_t
 * @brief A key or a character.
 */
struct lh_ui_key_input
{
    lh_ui_key_input_fields(lh_key_t, lh_bool_t, lh_u32_t);
};
typedef struct lh_ui_key_input lh_ui_key_input_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p key going down (@p pressed) or up.
 */
lh_void
lh_ui_key_input_init_key(lh_ui_key_input_t *self, lh_key_t key, lh_bool_t pressed);

/**
 * @brief One typed character, code point @p code (not 0).
 */
lh_void
lh_ui_key_input_init_text(lh_ui_key_input_t *self, lh_u32_t code);

/**
 * @brief The key (::lh_key_other for text).
 */
lh_key_t
lh_ui_key_input_get_key(const lh_ui_key_input_t *self);

/**
 * @brief True for a key going down (and for text).
 */
lh_bool_t
lh_ui_key_input_is_pressed(const lh_ui_key_input_t *self);

/**
 * @brief The typed code point, 0 for a key.
 */
lh_u32_t
lh_ui_key_input_get_code(const lh_ui_key_input_t *self);

/**
 * @brief True when @p key is going down (not text, not up).
 */
lh_bool_t
lh_ui_key_input_is_down(const lh_ui_key_input_t *self, lh_key_t key);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_KEY_H */
