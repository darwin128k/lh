/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_os_input_recognizer_pointer_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_POINTER_PTR_H
#define LH_OS_INPUT_RECOGNIZER_POINTER_PTR_H

#include <lh/util/ptr.h>

struct lh_os_input_recognizer_pointer;

/**
 * @def lh_os_input_recognizer_pointer_ptr
 * @brief Pointer to ::lh_os_input_recognizer_pointer_t.
 */
#define lh_os_input_recognizer_pointer_ptr lh_ptr_of(struct lh_os_input_recognizer_pointer)

/**
 * @def lh_os_input_recognizer_pointer_cptr
 * @brief Pointer to `const` ::lh_os_input_recognizer_pointer_t, for `.c` files only.
 */
#define lh_os_input_recognizer_pointer_cptr lh_cptr_of(struct lh_os_input_recognizer_pointer)

#endif /* LH_OS_INPUT_RECOGNIZER_POINTER_PTR_H */
