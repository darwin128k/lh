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
#include <lh/os/key.h>
#include <lh/ui/key/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_key_input
 * @brief A key or a character.
 */
struct lh_ui_key_input
{
    /** Members: ::lh_ui_key_input_fields. */
    lh_ui_key_input_fields(lh_os_key_t, lh_bool_t, lh_u32_t);
};
/** @copybrief lh_ui_key_input */
typedef struct lh_ui_key_input lh_ui_key_input_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p key going down (@p pressed) or up.
 */
lh_void
lh_ui_key_input_init_key(lh_ui_key_input_t *self, lh_os_key_t key, lh_bool_t pressed);

/**
 * @brief One typed character, code point @p code (not 0).
 */
lh_void
lh_ui_key_input_init_text(lh_ui_key_input_t *self, lh_u32_t code);

/**
 * @brief The key (::lh_os_key_other for text).
 */
lh_os_key_t
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
lh_ui_key_input_is_down(const lh_ui_key_input_t *self, lh_os_key_t key);

/**
 * @brief Store the key (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key input; checked by ::lh_assert_runtime_ref.
 * @param key The new value.
 */
lh_void
lh_ui_key_input_set_key(lh_ui_key_input_t *self, lh_os_key_t key);

/**
 * @brief Store whether the key went down (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key input; checked by ::lh_assert_runtime_ref.
 * @param pressed The new value.
 */
lh_void
lh_ui_key_input_set_pressed(lh_ui_key_input_t *self, lh_bool_t pressed);

/**
 * @brief Store the code point the key types; 0 for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The key input; checked by ::lh_assert_runtime_ref.
 * @param code The new value.
 */
lh_void
lh_ui_key_input_set_code(lh_ui_key_input_t *self, lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_KEY_H */
