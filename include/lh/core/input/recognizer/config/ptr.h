/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_recognizer_config_t.
 */

#ifndef LH_INPUT_RECOGNIZER_CONFIG_PTR_H
#define LH_INPUT_RECOGNIZER_CONFIG_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_recognizer_config;

/**
 * @def lh_input_recognizer_config_ptr
 * @brief Pointer to ::lh_input_recognizer_config_t.
 */
#define lh_input_recognizer_config_ptr lh_ptr_of(struct lh_input_recognizer_config)

/**
 * @def lh_input_recognizer_config_cptr
 * @brief Pointer to `const` ::lh_input_recognizer_config_t, for `.c` files only.
 */
#define lh_input_recognizer_config_cptr lh_cptr_of(struct lh_input_recognizer_config)

#endif /* LH_INPUT_RECOGNIZER_CONFIG_PTR_H */
