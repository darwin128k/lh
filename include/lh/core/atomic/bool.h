/**
 * @file bool.h
 * @brief A flag shared between threads without a lock.
 *
 * ::lh_atomic_bool_t is one yes/no that threads set and test at the same time:
 * "a wake is pending", "stop now". It is a composition, not a second
 * implementation: the flag is 0 or 1 in an ::lh_atomic_uint_t, and every
 * operation here calls the matching `lh_atomic_uint_*` one, with the same
 * memory orders (see `lh/core/atomic/uint.h`).
 *
 * The object owns nothing and needs no teardown. A static one starts from
 * ::lh_atomic_bool_initializer; one being built is set with
 * ::lh_atomic_bool_set_relaxed before another thread can reach it.
 */

#ifndef LH_ATOMIC_BOOL_H
#define LH_ATOMIC_BOOL_H

#include <lh/core/atomic/bool/fields.h>
#include <lh/core/atomic/uint.h>

/**
 * @brief One flag shared between threads.
 */
struct lh_atomic_bool
{
    /** Members: ::lh_atomic_bool_fields. */
    lh_atomic_bool_fields(lh_atomic_uint_t);
};

/** @copybrief lh_atomic_bool */
typedef struct lh_atomic_bool lh_atomic_bool_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The counter that holds the flag (level 1).
 * @param self Flag; must not be null.
 * @return The flag's ::lh_atomic_uint_t, read-only.
 */
const lh_atomic_uint_t *
lh_atomic_bool_get_value(const lh_atomic_bool_t *self);

/**
 * @brief The counter that holds the flag, for change (level 1).
 * @param self Flag; must not be null.
 * @return The flag's ::lh_atomic_uint_t.
 */
lh_atomic_uint_t *
lh_atomic_bool_get_value_mut(lh_atomic_bool_t *self);

/**
 * @brief Test the flag with acquire order.
 * @param self Flag to read; must not be null.
 * @return Whether it is set; what the setter did before its release is visible.
 */
lh_bool_t
lh_atomic_bool_get_acquire(const lh_atomic_bool_t *self);

/**
 * @brief Store @p flag with relaxed order: for a flag no other thread reads yet.
 * @param self Flag to write; must not be null.
 * @param flag New value.
 */
lh_void
lh_atomic_bool_set_relaxed(lh_atomic_bool_t *self, lh_bool_t flag);

/**
 * @brief Store @p flag with release order.
 * @param self Flag to write; must not be null.
 * @param flag New value.
 */
lh_void
lh_atomic_bool_set_release(lh_atomic_bool_t *self, lh_bool_t flag);

/**
 * @brief Replace the flag with @p flag with acquire-release order.
 * @param self Flag to change; must not be null.
 * @param flag New value.
 * @return Whether it was set before.
 */
lh_bool_t
lh_atomic_bool_exchange_acq_rel(lh_atomic_bool_t *self, lh_bool_t flag);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ATOMIC_BOOL_H */
