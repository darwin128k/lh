/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_os_input_recognizer_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_INITIALIZER_H
#define LH_OS_INPUT_RECOGNIZER_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/os/input/recognizer/config/initializer.h>
#include <lh/os/input/recognizer/pointer/initializer.h>

/**
 * @def lh_os_input_recognizer_initializer(config, pointers)
 * @brief Produces a brace-enclosed initializer for ::lh_os_input_recognizer_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_os_input_recognizer_fields. @p pointers is a brace-enclosed list of
 * ::lh_os_input_recognizer_pointer_idle_initializer; ::lh_os_input_recognizer_init
 * is the simpler way to the same state.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_input_recognizer_t recognizer = lh_os_input_recognizer_initializer(
 *     lh_os_input_recognizer_config_default_initializer(),       // config
 *     {lh_os_input_recognizer_pointer_idle_initializer()});      // pointers
 * @endcode
 *
 * @see lh_initializer
 * @see lh_os_input_recognizer_t
 */
#define lh_os_input_recognizer_initializer(config, pointers) lh_initializer(config, pointers)

#endif /* LH_OS_INPUT_RECOGNIZER_INITIALIZER_H */
