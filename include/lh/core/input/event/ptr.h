/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_event_t.
 *
 * Public headers write a read-only event as `const lh_input_event_ptr` so
 * the const is visible. ::lh_input_event_cptr is the same pointee type for
 * `.c` implementations.
 */

#ifndef LH_INPUT_EVENT_PTR_H
#define LH_INPUT_EVENT_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_event;

/**
 * @def lh_input_event_ptr
 * @brief Pointer to ::lh_input_event_t.
 */
#define lh_input_event_ptr lh_ptr_of(struct lh_input_event)

/**
 * @def lh_input_event_cptr
 * @brief Pointer to `const` ::lh_input_event_t, for `.c` files only.
 */
#define lh_input_event_cptr lh_cptr_of(struct lh_input_event)

#endif /* LH_INPUT_EVENT_PTR_H */
