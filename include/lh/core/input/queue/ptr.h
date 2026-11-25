/**
 * @file ptr.h
 * @brief Pointer aliases of ::lh_input_queue_t.
 *
 * Public headers write a read-only queue as `const lh_input_queue_ptr`.
 * ::lh_input_queue_cptr is the same pointee type for `.c` implementations.
 */

#ifndef LH_INPUT_QUEUE_PTR_H
#define LH_INPUT_QUEUE_PTR_H

#include <lh/core/util/ptr.h>

struct lh_input_queue;

/**
 * @def lh_input_queue_ptr
 * @brief Pointer to ::lh_input_queue_t.
 */
#define lh_input_queue_ptr lh_ptr_of(struct lh_input_queue)

/**
 * @def lh_input_queue_cptr
 * @brief Pointer to `const` ::lh_input_queue_t, for `.c` files only.
 */
#define lh_input_queue_cptr lh_cptr_of(struct lh_input_queue)

#endif /* LH_INPUT_QUEUE_PTR_H */
