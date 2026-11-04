/**
 * @file uint.h
 * @brief An unsigned counter shared between threads without a lock.
 *
 * ::lh_atomic_uint_t holds one ::lh_uint_t that several threads read and
 * change at the same time: a producer's and a consumer's position, a count of
 * requests, a dropped-record tally. Every operation names its memory order:
 *
 * - `_relaxed` — the value itself is never torn and every change counts, but
 *   nothing else the threads wrote is ordered around it (a statistic);
 * - `_acquire` (reads) — everything the writer did before its `_release` is
 *   visible after the read;
 * - `_release` (writes) — everything done before the write is visible to a
 *   thread that reads the value with `_acquire`;
 * - `_acq_rel` (read-modify-write) — both at once.
 *
 * The object owns nothing and needs no teardown. A static one starts from
 * ::lh_atomic_uint_initializer; one being built is set with
 * ::lh_atomic_uint_set_relaxed before another thread can reach it.
 *
 * Built on the compiler's atomic operations on an ordinary field (GCC/Clang
 * `__atomic`, MSVC `_Interlocked`): the struct is the same in C and C++ and can
 * sit in any other struct. A counter that is not lock-free on the target fails
 * the build. ::lh_atomic_bool_t is built on this one.
 */

#ifndef LH_ATOMIC_UINT_H
#define LH_ATOMIC_UINT_H

#include <lh/core/atomic/uint/fields.h>
#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>

/**
 * @brief One unsigned counter shared between threads.
 */
struct lh_atomic_uint
{
    /** Members: ::lh_atomic_uint_fields. */
    lh_atomic_uint_fields();
};

/** @copybrief lh_atomic_uint */
typedef struct lh_atomic_uint lh_atomic_uint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether the counter is lock-free on this target.
 *
 * Always ::lh_bool_true: a target where it is not fails the build.
 */
lh_bool_t
lh_atomic_uint_is_lock_free(void);

/**
 * @brief Load the value with relaxed order.
 * @param self Counter to read; must not be null.
 * @return The current value.
 */
lh_uint_t
lh_atomic_uint_get_relaxed(const lh_atomic_uint_t *self);

/**
 * @brief Load the value with acquire order.
 * @param self Counter to read; must not be null.
 * @return The current value; what the writer did before its release is visible.
 */
lh_uint_t
lh_atomic_uint_get_acquire(const lh_atomic_uint_t *self);

/**
 * @brief Store @p value with relaxed order.
 *
 * For a counter no other thread reads yet, or one whose readers need no
 * ordering.
 *
 * @param self  Counter to write; must not be null.
 * @param value New value.
 */
lh_void
lh_atomic_uint_set_relaxed(lh_atomic_uint_t *self, lh_uint_t value);

/**
 * @brief Store @p value with release order.
 * @param self  Counter to write; must not be null.
 * @param value New value.
 */
lh_void
lh_atomic_uint_set_release(lh_atomic_uint_t *self, lh_uint_t value);

/**
 * @brief Add @p amount with relaxed order, wrapping modulo 2^N.
 * @param self   Counter to change; must not be null.
 * @param amount Amount to add.
 * @return The value before the addition.
 */
lh_uint_t
lh_atomic_uint_fetch_add_relaxed(lh_atomic_uint_t *self, lh_uint_t amount);

/**
 * @brief Add @p amount with acquire-release order, wrapping modulo 2^N.
 * @param self   Counter to change; must not be null.
 * @param amount Amount to add.
 * @return The value before the addition.
 */
lh_uint_t
lh_atomic_uint_fetch_add_acq_rel(lh_atomic_uint_t *self, lh_uint_t amount);

/**
 * @brief Replace the value with @p value with acquire-release order.
 * @param self  Counter to change; must not be null.
 * @param value New value.
 * @return The value before the replacement.
 */
lh_uint_t
lh_atomic_uint_exchange_acq_rel(lh_atomic_uint_t *self, lh_uint_t value);

/**
 * @brief Subtract @p amount with acquire-release order, wrapping modulo 2^N.
 * @param self   Counter to change; must not be null.
 * @param amount Amount to subtract.
 * @return The value before the subtraction.
 */
lh_uint_t
lh_atomic_uint_fetch_sub_acq_rel(lh_atomic_uint_t *self, lh_uint_t amount);

/**
 * @brief Replace the value with @p desired only if it is still @p expected,
 *        with acquire-release order.
 *
 * The test and the store are one step: no other thread's change can fall
 * between them. The usual caller reads the value, decides, and retries from a
 * fresh read when this returns false.
 *
 * @param self     Counter to change; must not be null.
 * @param expected The value the caller last read.
 * @param desired  The value to store.
 * @return ::lh_bool_true when the value was @p expected and is now @p desired;
 *         ::lh_bool_false when another thread changed it first (nothing changed).
 */
lh_bool_t
lh_atomic_uint_try_compare_exchange_acq_rel(lh_atomic_uint_t *self, lh_uint_t expected,
                                            lh_uint_t desired);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ATOMIC_UINT_H */
