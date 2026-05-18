/**
 * @file throw.h
 * @brief Functions to throw
 *        and rethrow ::lh_exception_t on the runtime catch stack.
 *
 * ::lh_runtime_throw assembles an ::lh_exception_t from an ::lh_runtime_error_t
 * and unwinds to the innermost catch frame registered with
 * ::lh_runtime_exception_catch_stack_capture.
 *
 * In debug builds (@c NDEBUG not defined) the caller supplies a pre-built
 * ::lh_exception_origin_t with raise-site metadata; in release builds
 * (@c NDEBUG defined) the origin parameter is absent and the exception
 * carries only the ::lh_runtime_error_t fields.
 *
 * The origin is typically constructed by the ::lh_runtime_raise macro
 * at the call site using @c __FILE__, @c __LINE__, @c __FUNCTION__,
 * and @c __TIMESTAMP__.
 */

#ifndef LH_RUNTIME_THROW_H
#define LH_RUNTIME_THROW_H

#include <lh/runtime/error.h>
#include <lh/runtime/exception/catch/stack.h>

/**
 * @brief Assemble an ::lh_exception_t from @p error and @p origin, then unwind.
 *
 * Delegates assembly to ::lh_exception_init and passes the result to
 * ::lh_runtime_exception_catch_stack_throw, which copies it into the active
 * catch frame and unwinds via @c longjmp.
 *
 * @param error  Pre-built ::lh_runtime_error_t value (not null).
 * @param origin Pre-built raise-site metadata (not null; debug builds only).
 *
 * @note Does not return if a handler exists;
 *       otherwise ::lh_runtime_terminate is invoked.
 *
 * @see lh_runtime_rethrow
 * @see lh_exception_init
 * @see lh_runtime_error_t
 * @see lh_exception_origin_t
 */
LH_ATTRIBUTE_BUILTIN
LH_ATTRIBUTE_NORETURN
#ifndef NDEBUG
void
lh_runtime_throw(const lh_runtime_error_t *error, const lh_exception_origin_t *origin)
#else
void
lh_runtime_throw(const lh_runtime_error_t *error)
#endif
{
    lh_exception_t exception;
#ifndef NDEBUG
    lh_exception_init_by_error(lh_addr_of(exception), lh_ptr_rcast(const lh_error_t, error),
                                origin);
#else
    lh_exception_init_by_error(lh_addr_of(exception), lh_ptr_rcast(const lh_error_t, error));
#endif
    lh_runtime_exception_catch_stack_throw(lh_addr_of(exception));
}

/**
 * @def lh_runtime_rethrow()
 * @brief Propagate the exception from the current catch frame outward.
 *
 * Expands to ::lh_runtime_exception_catch_stack_rethrow().
 *
 * @pre A catch registered with ::lh_runtime_exception_catch_stack_capture
 *      must be active and hold the exception being rethrown;
 *      otherwise behavior is undefined.
 *
 * @see lh_runtime_exception_catch_stack_rethrow
 * @see lh_runtime_throw
 */
#define lh_runtime_rethrow() lh_runtime_exception_catch_stack_rethrow()

#endif // LH_RUNTIME_THROW_H
