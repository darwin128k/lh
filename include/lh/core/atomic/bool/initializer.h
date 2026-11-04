/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_atomic_bool_t.
 */

#ifndef LH_ATOMIC_BOOL_INITIALIZER_H
#define LH_ATOMIC_BOOL_INITIALIZER_H

#include <lh/core/atomic/uint/initializer.h>

/**
 * @def lh_atomic_bool_initializer(flag)
 * @brief Produces a brace-enclosed initializer for ::lh_atomic_bool_t holding
 *        @p flag.
 *
 * For an object no other thread can reach yet:
 *
 * @code{.c}
 * lh_atomic_bool_t stopping = lh_atomic_bool_initializer(lh_bool_false);
 * @endcode
 *
 * @param flag Initial value, ::lh_bool_true or ::lh_bool_false.
 */
#define lh_atomic_bool_initializer(flag)                                                           \
    lh_initializer(lh_atomic_uint_initializer((flag) ? 1U : 0U))

#endif /* LH_ATOMIC_BOOL_INITIALIZER_H */
