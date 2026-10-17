/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_key_input_t.
 */

#ifndef LH_UI_KEY_INITIALIZER_H
#define LH_UI_KEY_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_key_input_initializer(key, pressed, code)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_key_input_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_key_input_fields.
 *
 * @param key     Key (::lh_os_key_t), ::lh_os_key_other for a text input.
 * @param pressed Whether the key went down; true for a text input.
 * @param code    Unicode code point of a text input, `0` for a key input.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_key_input_t typed = lh_ui_key_input_initializer(lh_os_key_other,  // key
 *                                                                    lh_bool_true,  // pressed
 *                                                                    'a');          // code
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_key_input_t
 */
#define lh_ui_key_input_initializer(key, pressed, code) lh_initializer(key, pressed, code)

#endif /* LH_UI_KEY_INITIALIZER_H */
