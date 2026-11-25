/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_controller_t.
 *
 * Public headers write a read-only controller as `const lh_controller_ptr`
 * so the const is visible. ::lh_controller_cptr is the same pointee type for
 * `.c` implementations.
 */

#ifndef LH_CONTROLLER_PTR_H
#define LH_CONTROLLER_PTR_H

#include <lh/core/util/ptr.h>

struct lh_controller;

/**
 * @def lh_controller_ptr
 * @brief Pointer to ::lh_controller_t.
 */
#define lh_controller_ptr lh_ptr_of(struct lh_controller)

/**
 * @def lh_controller_cptr
 * @brief Pointer to `const` ::lh_controller_t, for `.c` files only.
 */
#define lh_controller_cptr lh_cptr_of(struct lh_controller)

#endif /* LH_CONTROLLER_PTR_H */
