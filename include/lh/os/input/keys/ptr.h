/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_os_input_keys_t.
 */

#ifndef LH_OS_INPUT_KEYS_PTR_H
#define LH_OS_INPUT_KEYS_PTR_H

#include <lh/core/util/ptr.h>

struct lh_os_input_keys;

/**
 * @def lh_os_input_keys_ptr
 * @brief Pointer to ::lh_os_input_keys_t.
 */
#define lh_os_input_keys_ptr lh_ptr_of(struct lh_os_input_keys)

/**
 * @def lh_os_input_keys_cptr
 * @brief Pointer to `const` ::lh_os_input_keys_t, for `.c` files only.
 */
#define lh_os_input_keys_cptr lh_cptr_of(struct lh_os_input_keys)

#endif /* LH_OS_INPUT_KEYS_PTR_H */
