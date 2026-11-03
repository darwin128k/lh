/**
 * @file fn_ptr.h
 * @brief A function pointer published between threads with acquire / release order.
 *
 * ::lh_atomic_fn_ptr_t is ::lh_atomic_ptr_t for code instead of data: one
 * function pointer that threads exchange without a lock, never torn. The
 * writer stores it with ::lh_atomic_fn_ptr_set_release; a reader that loads
 * it with ::lh_atomic_fn_ptr_get_acquire also sees everything the writer did
 * before the store (for example the state the function reads).
 *
 * It is a separate brick because C does not convert a function pointer into a
 * data pointer. The stored type is ::lh_atomic_fn_ptr_cb (`void (void)`); a
 * caller with another signature casts to it and back before the call.
 *
 * The object owns nothing and needs no teardown. A static one starts from
 * ::lh_atomic_fn_ptr_initializer; any other is set with
 * ::lh_atomic_fn_ptr_set_release before another thread can reach it. Built on
 * the compiler's atomic operations on an ordinary object (GCC/Clang
 * `__atomic`, MSVC `_Interlocked`), the same in C and C++; a function pointer
 * that is not lock-free on the target fails the build.
 */

#ifndef LH_ATOMIC_FN_PTR_H
#define LH_ATOMIC_FN_PTR_H

#include <lh/compiler/extern/c.h>
#include <lh/atomic/fn_ptr/fields.h>

/**
 * @brief One function pointer published with acquire / release order.
 */
struct lh_atomic_fn_ptr
{
    /** Members: ::lh_atomic_fn_ptr_fields. */
    lh_atomic_fn_ptr_fields();
};

/** @copybrief lh_atomic_fn_ptr */
typedef struct lh_atomic_fn_ptr lh_atomic_fn_ptr_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Load the function pointer with acquire order.
 *
 * Everything the thread that stored this value did before its
 * ::lh_atomic_fn_ptr_set_release is visible to the caller after this call.
 *
 * @param self Object to read; must not be null.
 * @return The function pointer last published (or the initial one; may be null).
 */
lh_atomic_fn_ptr_cb
lh_atomic_fn_ptr_get_acquire(const lh_atomic_fn_ptr_t *self);

/**
 * @brief Store @p fn with release order.
 *
 * Everything the caller did before this call is visible to a thread that
 * loads @p fn with ::lh_atomic_fn_ptr_get_acquire.
 *
 * @param self Object to write; must not be null.
 * @param fn   Function pointer to publish (may be ::lh_null).
 */
lh_void
lh_atomic_fn_ptr_set_release(lh_atomic_fn_ptr_t *self, lh_atomic_fn_ptr_cb fn);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ATOMIC_FN_PTR_H */
