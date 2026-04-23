/**
 * @file catch.h
 * @brief Catch-site record for C-style non-local jumps (`setjmp` / `longjmp`).
 *
 * C has no built-in exceptions like C++.
 * This library models a **catch** as a small struct:
 * a saved execution context (::lh_jump_buffer_t, from `setjmp.h`)
 * and a slot for the thrown value (::lh_exception_t, wrapping ::lh_error_t).
 *
 * Arming the catch calls `setjmp` on `env`; throwing unwinds by `longjmp`
 * and fills `exception` before resuming the catch site.
 *
 * The runtime stack that manages nested catches lives
 * in `lh/runtime/exception/catch/stack.h` (::lh_runtime_exception_catch_stack_push,
 * ::lh_runtime_exception_catch_stack_capture, ::lh_runtime_exception_catch_stack_throw).
 */

#ifndef LH_EXCEPTION_CATCH_H
#define LH_EXCEPTION_CATCH_H

#include <lh/exception.h>
#include <lh/jump_buffer.h>

/**
 * @struct lh_exception_catch
 * @brief One active catch frame: jump target plus stored thrown value.
 *
 * On throw, the runtime copies the thrown ::lh_exception_t into `exception`
 * and resumes at `env`.
 * Public typedef: ::lh_exception_catch_t.
 *
 * `env` must be used like any `jmp_buf`: pair each `longjmp` with a `setjmp`
 * that is still active, and respect C’s rules for locals across `setjmp`
 * (often `volatile` where required).
 */
typedef struct lh_exception_catch {
    lh_exception_t exception; /**< Filled when unwinding reaches this catch (wraps ::lh_error_t). */
    lh_jump_buffer_t env;     /**< `setjmp` / `longjmp` state for this site (::lh_jump_buffer_t). */
} lh_exception_catch_t;       /**< Typedef for struct ::lh_exception_catch. */

#endif // LH_EXCEPTION_CATCH_H