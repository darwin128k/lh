/**
 * @file fields.h
 * @brief Member fields of ::lh_atomic_ptr_t.
 */

#ifndef LH_ATOMIC_PTR_FIELDS_H
#define LH_ATOMIC_PTR_FIELDS_H

#include <lh/core/ptr.h>

/**
 * @def lh_atomic_ptr_fields()
 * @brief The published pointer.
 *
 * Read and written only by the compiler's atomic operations in
 * ::lh_atomic_ptr_get_acquire and ::lh_atomic_ptr_set_release; a plain access
 * would race with them.
 */
#define lh_atomic_ptr_fields() lh_ptr value

#endif /* LH_ATOMIC_PTR_FIELDS_H */
