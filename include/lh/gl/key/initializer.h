/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_key_input_t.
 */

#ifndef LH_GL_KEY_INITIALIZER_H
#define LH_GL_KEY_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_key_input_initializer(key, pressed, code)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_key_input_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_key_input_fields.
 *
 * @param key     Key (::lh_key_t), ::lh_key_other for a text input.
 * @param pressed Whether the key went down; true for a text input.
 * @param code    Unicode code point of a text input, `0` for a key input.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_key_input_t typed = lh_gl_key_input_initializer(lh_key_other,  // key
 *                                                                    lh_bool_true,  // pressed
 *                                                                    'a');          // code
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_key_input_t
 */
#define lh_gl_key_input_initializer(key, pressed, code) lh_initializer(key, pressed, code)

#endif /* LH_GL_KEY_INITIALIZER_H */
