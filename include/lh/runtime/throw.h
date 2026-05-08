/**
 * @file throw.h
 * @brief Macros to throw and rethrow ::lh_exception_t on the runtime catch stack.
 *
 * These wrap ::lh_exception_initializer and the `setjmp` / `longjmp`
 * unwinder (::lh_runtime_exception_catch_stack_throw).
 *
 * Use with ::lh_runtime_exception_catch_stack_capture
 * in the same translation unit’s catch regions.
 */

#ifndef LH_RUNTIME_THROW_H
#define LH_RUNTIME_THROW_H

#include <lh/exception/initializer.h>
#include <lh/runtime/exception/catch/stack.h>

/**
 * @def lh_runtime_throw(...)
 * @brief Construct an exception from @p __VA_ARGS__ and unwind to the inner catch.
 *
 * Expands to a compound statement: builds a temporary ::lh_exception_t with
 * ::lh_exception_initializer(__VA_ARGS__), then calls ::lh_runtime_exception_catch_stack_throw
 * with its address.
 *
 * @note Does not return if a handler exists;
 *       otherwise ::lh_runtime_terminate is invoked.
 *
 * @param ... Arguments forwarded to ::lh_exception_initializer
 * (typically an error code and optional description for the embedded ::lh_error_t).
 *
 * Example usage:
 * @code{.c}
 * lh_runtime_throw(ENOENT, "open failed");
 * @endcode
 *
 * @see lh_exception_initializer
 * @see lh_runtime_exception_catch_stack_throw
 * @see lh_runtime_exception_catch_stack_capture
 * @see lh_runtime_throw_with_code
 * @see lh_runtime_throw_by_desc
 */
#define lh_runtime_throw(...)                                                                      \
    do                                                                                             \
    {                                                                                              \
        const lh_exception_t e = lh_exception_initializer(__VA_ARGS__);                            \
        lh_runtime_exception_catch_stack_throw(&e);                                                \
    } while (0)

/**
 * @def lh_runtime_rethrow()
 * @brief Propagate the exception from the current catch frame outward.
 *
 * Expands to ::lh_runtime_exception_catch_stack_rethrow().
 *
 * @pre A catch registered with ::lh_runtime_exception_catch_stack_capture must
 *      be active and hold the exception being rethrown; otherwise behavior is
 *      undefined.
 *
 * @see lh_runtime_exception_catch_stack_rethrow
 * @see lh_runtime_throw
 */
#define lh_runtime_rethrow() lh_runtime_exception_catch_stack_rethrow()

#endif // LH_RUNTIME_THROW_H
