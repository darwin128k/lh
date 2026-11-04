/**
 * @file fields.h
 * @brief Member fields of ::lh_atomic_uint_t.
 */

#ifndef LH_ATOMIC_UINT_FIELDS_H
#define LH_ATOMIC_UINT_FIELDS_H

#include <lh/numeric/types.h>

/**
 * @def lh_atomic_uint_fields()
 * @brief The shared counter.
 *
 * Read and written only by the compiler's atomic operations in the
 * `lh_atomic_uint_*` functions; a plain access would race with them.
 */
#define lh_atomic_uint_fields() lh_uint_t value

#endif /* LH_ATOMIC_UINT_FIELDS_H */
