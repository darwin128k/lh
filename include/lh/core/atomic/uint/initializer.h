/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_atomic_uint_t.
 */

#ifndef LH_ATOMIC_UINT_INITIALIZER_H
#define LH_ATOMIC_UINT_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_atomic_uint_initializer(number)
 * @brief Produces a brace-enclosed initializer for ::lh_atomic_uint_t holding
 *        @p number.
 *
 * For an object no other thread can reach yet (a static one, or one being
 * built), so no ordering is needed:
 *
 * @code{.c}
 * lh_atomic_uint_t count = lh_atomic_uint_initializer(0U);
 * @endcode
 *
 * @param number Initial value, an ::lh_uint_t.
 */
#define lh_atomic_uint_initializer(number) lh_initializer(number)

#endif /* LH_ATOMIC_UINT_INITIALIZER_H */
