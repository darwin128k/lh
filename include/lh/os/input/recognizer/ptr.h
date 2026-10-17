/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_os_input_recognizer_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_PTR_H
#define LH_OS_INPUT_RECOGNIZER_PTR_H

#include <lh/util/ptr.h>

struct lh_os_input_recognizer;

/**
 * @def lh_os_input_recognizer_ptr
 * @brief Pointer to ::lh_os_input_recognizer_t.
 */
#define lh_os_input_recognizer_ptr lh_ptr_of(struct lh_os_input_recognizer)

/**
 * @def lh_os_input_recognizer_cptr
 * @brief Pointer to `const` ::lh_os_input_recognizer_t, for `.c` files only.
 */
#define lh_os_input_recognizer_cptr lh_cptr_of(struct lh_os_input_recognizer)

#endif /* LH_OS_INPUT_RECOGNIZER_PTR_H */
