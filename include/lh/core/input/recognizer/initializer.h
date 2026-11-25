/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_input_recognizer_t.
 */

#ifndef LH_INPUT_RECOGNIZER_INITIALIZER_H
#define LH_INPUT_RECOGNIZER_INITIALIZER_H

#include <lh/core/input/recognizer/config/initializer.h>
#include <lh/core/input/recognizer/pointer/initializer.h>

/**
 * @def lh_input_recognizer_initializer(config, pointers)
 * @brief Produces a brace-enclosed initializer for ::lh_input_recognizer_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_input_recognizer_fields. @p pointers is a brace-enclosed list of
 * ::lh_input_recognizer_pointer_idle_initializer; ::lh_input_recognizer_init
 * is the simpler way to the same state.
 *
 * Example usage:
 * @code{.c}
 * static lh_input_recognizer_t recognizer = lh_input_recognizer_initializer(
 *     lh_input_recognizer_config_default_initializer(),       // config
 *     {lh_input_recognizer_pointer_idle_initializer()});      // pointers
 * @endcode
 *
 * @see lh_initializer
 * @see lh_input_recognizer_t
 */
#define lh_input_recognizer_initializer(config, pointers)                                          \
    lh_initializer(config, pointers, 0, 0U, 0U, lh_bool_false, 0, {lh_bool_false}, {0})

#endif /* LH_INPUT_RECOGNIZER_INITIALIZER_H */
