/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_recognizer_pointer_t.
 */

#ifndef LH_INPUT_RECOGNIZER_POINTER_PTR_H
#define LH_INPUT_RECOGNIZER_POINTER_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_recognizer_pointer;

/**
 * @def lh_input_recognizer_pointer_ptr
 * @brief Pointer to ::lh_input_recognizer_pointer_t.
 */
#define lh_input_recognizer_pointer_ptr lh_ptr_of(struct lh_input_recognizer_pointer)

/**
 * @def lh_input_recognizer_pointer_cptr
 * @brief Pointer to `const` ::lh_input_recognizer_pointer_t, for `.c` files only.
 */
#define lh_input_recognizer_pointer_cptr lh_cptr_of(struct lh_input_recognizer_pointer)

#endif /* LH_INPUT_RECOGNIZER_POINTER_PTR_H */
