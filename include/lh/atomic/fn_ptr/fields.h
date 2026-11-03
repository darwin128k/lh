/**
 * @file fields.h
 * @brief Member fields of ::lh_atomic_fn_ptr_t.
 */

#ifndef LH_ATOMIC_FN_PTR_FIELDS_H
#define LH_ATOMIC_FN_PTR_FIELDS_H

#include <lh/atomic/fn_ptr/cb.h>

/**
 * @def lh_atomic_fn_ptr_fields()
 * @brief The published function pointer.
 *
 * Read and written only by the compiler's atomic operations in
 * ::lh_atomic_fn_ptr_get_acquire and ::lh_atomic_fn_ptr_set_release; a plain
 * access would race with them.
 */
#define lh_atomic_fn_ptr_fields() lh_atomic_fn_ptr_cb value

#endif /* LH_ATOMIC_FN_PTR_FIELDS_H */
