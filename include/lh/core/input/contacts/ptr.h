/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_contacts_t.
 *
 * Public headers write a read-only table as `const lh_input_contacts_ptr`.
 * ::lh_input_contacts_cptr is the same pointee type for `.c` implementations.
 */

#ifndef LH_INPUT_CONTACTS_PTR_H
#define LH_INPUT_CONTACTS_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_contacts;

/**
 * @def lh_input_contacts_ptr
 * @brief Pointer to ::lh_input_contacts_t.
 */
#define lh_input_contacts_ptr lh_ptr_of(struct lh_input_contacts)

/**
 * @def lh_input_contacts_cptr
 * @brief Pointer to `const` ::lh_input_contacts_t, for `.c` files only.
 */
#define lh_input_contacts_cptr lh_cptr_of(struct lh_input_contacts)

#endif /* LH_INPUT_CONTACTS_PTR_H */
