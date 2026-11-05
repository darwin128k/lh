/**
 * @file initializer.h
 * @brief Brace-enclosed initializers for ::lh_critical_t.
 */

#ifndef LH_CRITICAL_INITIALIZER_H
#define LH_CRITICAL_INITIALIZER_H

#include <lh/core/critical/enter/cb.h>
#include <lh/core/critical/leave/cb.h>

/**
 * @def lh_critical_initializer(enter, leave)
 * @brief Produces a brace-enclosed initializer for ::lh_critical_t.
 *
 * @param enter ::lh_critical_enter_fn.
 * @param leave ::lh_critical_leave_fn.
 */
#define lh_critical_initializer(enter, leave)                                                      \
    lh_initializer(lh_cast_static(lh_critical_enter_cb, enter),                                    \
                   lh_cast_static(lh_critical_leave_cb, leave))

/**
 * @def lh_critical_empty_initializer()
 * @brief A critical section with no calls: entering and leaving do nothing.
 *
 * The slot starts so, until the host fills it.
 */
#define lh_critical_empty_initializer() lh_critical_initializer(lh_null, lh_null)

#endif /* LH_CRITICAL_INITIALIZER_H */
