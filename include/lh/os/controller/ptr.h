/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_os_controller_t.
 *
 * Public headers write a read-only controller as `const lh_os_controller_ptr`
 * so the const is visible. ::lh_os_controller_cptr is the same pointee type for
 * `.c` implementations.
 */

#ifndef LH_OS_CONTROLLER_PTR_H
#define LH_OS_CONTROLLER_PTR_H

#include <lh/util/ptr.h>

struct lh_os_controller;

/**
 * @def lh_os_controller_ptr
 * @brief Pointer to ::lh_os_controller_t.
 */
#define lh_os_controller_ptr lh_ptr_of(struct lh_os_controller)

/**
 * @def lh_os_controller_cptr
 * @brief Pointer to `const` ::lh_os_controller_t, for `.c` files only.
 */
#define lh_os_controller_cptr lh_cptr_of(struct lh_os_controller)

#endif /* LH_OS_CONTROLLER_PTR_H */
