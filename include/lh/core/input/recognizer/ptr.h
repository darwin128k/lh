/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_recognizer_t.
 */

#ifndef LH_INPUT_RECOGNIZER_PTR_H
#define LH_INPUT_RECOGNIZER_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_recognizer;

/**
 * @def lh_input_recognizer_ptr
 * @brief Pointer to ::lh_input_recognizer_t.
 */
#define lh_input_recognizer_ptr lh_ptr_of(struct lh_input_recognizer)

/**
 * @def lh_input_recognizer_cptr
 * @brief Pointer to `const` ::lh_input_recognizer_t, for `.c` files only.
 */
#define lh_input_recognizer_cptr lh_cptr_of(struct lh_input_recognizer)

#endif /* LH_INPUT_RECOGNIZER_PTR_H */
