/**
 * @file stack.h
 * @brief Nested catch stack for the lh `setjmp` / `longjmp` exception model.
 *
 * Maintains a small thread-local stack of registered ::lh_exception_catch_t frames
 * (depth capped by ::LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX).
 *
 * The stack cursor and backing storage live in the implementation;
 * the functions here expose inspection, navigation,
 * and registration used by the capture macro and throw path.
 *
 * @see lh_exception_catch_t
 * @see lh_jump_buffer_t
 * @see lh_runtime_terminate
 */

#ifndef LH_RUNTIME_EXCEPTION_CATCH_STACK_H
#define LH_RUNTIME_EXCEPTION_CATCH_STACK_H

#include <lh/exception/catch.h>
#include <lh/runtime/terminate.h>
#include <lh/attribute/builtin.h>
#include <lh/attribute/noreturn.h>

/**
 * @def lh_runtime_exception_catch_stack_capture(x)
 * @brief Push catch frame @p x onto the runtime stack
 *        and arm its jump buffer with `setjmp`.
 *
 * Expands to `setjmp(::lh_runtime_exception_catch_stack_push(x)->env)`.
 * The first time the jump buffer returns, the result is `0` (normal forward execution).
 *
 * After a ::lh_runtime_exception_catch_stack_throw, execution resumes here
 * with the value passed to `longjmp` (the runtime’s thrown error code).
 *
 * @param x Pointer to an ::lh_exception_catch_t that outlives this catch region
 *          (typically a local or static); must not be null.
 *
 *          The stack must not be full,
 *          or ::lh_runtime_exception_catch_stack_push
 *          returns null and the expansion is undefined.
 *
 * @note Follow C’s `setjmp` / `longjmp` rules for automatic variables
 *       in the same function (often `volatile` where the standard requires it).
 *
 * @see ::lh_exception_catch_t
 *
 * Example usage:
 * @code{.c}
 * lh_exception_catch_t frame;
 * if (lh_runtime_exception_catch_stack_capture(&frame) == 0) {
 *     // try
 * } else {
 *     // catch: frame.exception is filled
 * }
 * @endcode
 */
#define lh_runtime_exception_catch_stack_capture(x)                                                \
    setjmp(lh_runtime_exception_catch_stack_push(x)->env)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Registered catch pointer held in the stack slot at the internal cursor.
 *
 * The implementation keeps a fixed array of ::lh_exception_catch_t pointers
 * and a cursor into that array.
 * This function returns whatever pointer is stored in the slot at the cursor; that value is often
 * null before anything is pushed into that slot.
 *
 * After ::lh_runtime_exception_catch_stack_throw lands in a handler,
 * the cursor lines up with that innermost frame, so the result is the active
 * catch record (::lh_runtime_exception_catch_stack_rethrow relies on that).
 *
 * @return The ::lh_exception_catch_t stored at the current slot,
 *         or null if the slot is empty.
 */
lh_exception_catch_t *
lh_runtime_exception_catch_stack_get_current(void);

/**
 * @brief Tests whether the cursor is at the bottom of the stack.
 *
 * In that position ::lh_runtime_exception_catch_stack_prev returns null
 * and the stack holds no registered frames below the cursor.
 *
 * @retval lh_bool_true Cursor is at the first slot (initial / fully unwound position).
 * @retval lh_bool_false There is at least one populated slot below the cursor.
 */
lh_bool_t
lh_runtime_exception_catch_stack_is_begin(void);

/**
 * @brief Tests whether the cursor is past the last usable slot (stack full).
 *
 * While this holds, ::lh_runtime_exception_catch_stack_push returns null and
 * ::lh_runtime_exception_catch_stack_next would not advance.
 *
 * @retval lh_bool_true Cursor is past the last slot;
 *                      ::lh_runtime_exception_catch_stack_push
 *                      returns null until the stack moves back.
 * @retval lh_bool_false A new frame can still be registered with
 *                      ::lh_runtime_exception_catch_stack_push.
 */
lh_bool_t
lh_runtime_exception_catch_stack_is_end(void);

/**
 * @brief Advance the cursor to the next slot.
 *
 * @return Pointer stored in the slot after advancing,
 *         or null if the cursor was already at the end.
 */
lh_exception_catch_t *
lh_runtime_exception_catch_stack_next(void);

/**
 * @brief Move the cursor to the previous slot.
 *
 * @return Pointer stored at the new cursor,
 *         or null if the cursor was already at the beginning.
 */
lh_exception_catch_t *
lh_runtime_exception_catch_stack_prev(void);

/**
 * @brief Store catch frame @p e at the current slot,
 *        then advance the cursor.
 *
 * Used with ::lh_runtime_exception_catch_stack_capture;
 * @p e must stay valid until that frame is unwound
 * or the stack moves back over it.
 *
 * @param e Catch site to register.
 * @return @p e on success, or null if the stack was already full
 *         (::lh_runtime_exception_catch_stack_is_end()).
 */
lh_exception_catch_t *
lh_runtime_exception_catch_stack_push(lh_exception_catch_t *e);

/**
 * @brief Throw @p exception to the previous registered catch frame.
 *
 * Moves the stack cursor one frame back (via
 * ::lh_runtime_exception_catch_stack_prev), copies @p exception into that frame, and resumes
 * execution with `longjmp` using the exception error code
 * (`lh_error_get_code(lh_exception_get_error(...))`).
 *
 * If no previous frame exists, the runtime cannot continue unwinding and
 * ::lh_runtime_terminate is called.
 *
 * @param exception Exception value to propagate to the outer catch.
 *                  Must not be null.
 */
LH_ATTRIBUTE_BUILTIN
LH_ATTRIBUTE_NORETURN
void
lh_runtime_exception_catch_stack_throw(const lh_exception_t *exception) {
    lh_exception_catch_t *prev = lh_runtime_exception_catch_stack_prev();
    if (prev) {
        prev->exception = *exception;
        longjmp(prev->env, lh_error_get_code(lh_exception_get_error(&prev->exception)));
    }
    lh_runtime_terminate();
}

/**
 * @brief Re-throw the exception held in the innermost active catch frame.
 *
 * Reads ::lh_exception_catch_t::exception from ::lh_runtime_exception_catch_stack_get_current()
 * and forwards it to ::lh_runtime_exception_catch_stack_throw, so the next outer registered catch
 * receives the same ::lh_exception_t (or ::lh_runtime_terminate runs if there is no outer frame).
 *
 * @pre ::lh_runtime_exception_catch_stack_get_current() must be non-null
 *      and refer to the catch that is currently handling the exception;
 *      otherwise behavior is undefined.
 */
LH_ATTRIBUTE_BUILTIN
LH_ATTRIBUTE_NORETURN
void
lh_runtime_exception_catch_stack_rethrow(void) {
    const lh_exception_catch_t *cur = lh_runtime_exception_catch_stack_get_current();
    lh_runtime_exception_catch_stack_throw(&cur->exception);
}

LH_COMPILER_EXTERN_C_END

#endif // LH_RUNTIME_EXCEPTION_CATCH_STACK_H