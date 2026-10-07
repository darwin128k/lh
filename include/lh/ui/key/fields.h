/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_key_input_t.
 */

#ifndef LH_UI_KEY_FIELDS_H
#define LH_UI_KEY_FIELDS_H

/**
 * @def lh_ui_key_input_fields(key_type, bool_type, code_type)
 * @brief A key going down or up, or one typed character.
 *
 * A key input has `key` and `pressed`, `code` 0. A text input has `code`
 * (a Unicode code point), `key` ::lh_key_other and `pressed` true.
 *
 * @param key_type  ::lh_key_t.
 * @param bool_type ::lh_bool_t.
 * @param code_type ::lh_u32_t.
 */
#define lh_ui_key_input_fields(key_type, bool_type, code_type)                                      \
    key_type key;                                                                                   \
    bool_type pressed;                                                                              \
    code_type code

#endif /* LH_UI_KEY_FIELDS_H */
