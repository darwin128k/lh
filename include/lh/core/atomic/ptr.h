/**
 * @file ptr.h
 * @brief A data pointer published between threads with acquire / release order.
 *
 * ::lh_atomic_ptr_t holds one pointer that threads exchange without a lock.
 * The writer completes an object, then stores its address with
 * ::lh_atomic_ptr_set_release. A reader that loads that address with
 * ::lh_atomic_ptr_get_acquire also sees everything the writer did before the
 * store: the object is whole, never half written. A reader that loads an
 * earlier address gets the earlier object, whole as well. The pointer itself
 * is never torn.
 *
 * The object owns nothing and needs no teardown. A static one starts from
 * ::lh_atomic_ptr_initializer; any other is set with ::lh_atomic_ptr_set_release
 * before another thread can reach it. Its pointee belongs to the writer, which
 * keeps it alive for as long as any reader may still hold the address.
 *
 * Built on the compiler's atomic operations on an ordinary object (GCC/Clang
 * `__atomic`, MSVC `_Interlocked`): the struct is the same in C and C++ and can
 * sit in any other struct. A pointer that is not lock-free on the target fails
 * the build.
 */

#ifndef LH_ATOMIC_PTR_H
#define LH_ATOMIC_PTR_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/atomic/ptr/fields.h>

/**
 * @brief One pointer published with acquire / release order.
 */
struct lh_atomic_ptr
{
    /** Members: ::lh_atomic_ptr_fields. */
    lh_atomic_ptr_fields();
};

/** @copybrief lh_atomic_ptr */
typedef struct lh_atomic_ptr lh_atomic_ptr_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Load the pointer with acquire order.
 *
 * Everything the thread that stored this value did before its
 * ::lh_atomic_ptr_set_release is visible to the caller after this call.
 *
 * @param self Object to read; must not be null.
 * @return The pointer last published (or the initial one).
 */
lh_ptr
lh_atomic_ptr_get_acquire(const lh_atomic_ptr_t *self);

/**
 * @brief Store @p value with release order.
 *
 * Everything the caller did before this call is visible to a thread that
 * loads @p value with ::lh_atomic_ptr_get_acquire. The previous value is
 * dropped; whatever it pointed to stays the caller's.
 *
 * @param self  Object to write; must not be null.
 * @param value Pointer to publish (may be ::lh_null).
 */
lh_void
lh_atomic_ptr_set_release(lh_atomic_ptr_t *self, lh_ptr value);

/**
 * @brief Store @p value with relaxed order.
 *
 * The pointer itself is never torn, but nothing else is ordered around it: for
 * an object no other thread can reach yet, or one that a later release
 * operation publishes.
 *
 * @param self  Object to write; must not be null.
 * @param value Pointer to store (may be ::lh_null).
 */
lh_void
lh_atomic_ptr_set_relaxed(lh_atomic_ptr_t *self, lh_ptr value);

/**
 * @brief Replace the pointer with @p value with acquire-release order.
 *
 * One indivisible step: no other thread's store lands between the read of the
 * old value and the write of the new one. What the caller did before is
 * visible to whoever acquires @p value, and what the thread that stored the
 * old value did before is visible to the caller.
 *
 * @param self  Object to change; must not be null.
 * @param value New pointer (may be ::lh_null).
 * @return The pointer before the replacement.
 */
lh_ptr
lh_atomic_ptr_exchange_acq_rel(lh_atomic_ptr_t *self, lh_ptr value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ATOMIC_PTR_H */
