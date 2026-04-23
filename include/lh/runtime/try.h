/**
 * @file try.h
 * @brief Exception handling macros for EYA runtime (try-catch implementation)
 *
 * Provides a set of macros that implement try-catch exception handling mechanism
 * in C using setjmp/longjmp-like functionality through exception frames.
 *
 * The implementation consists of:
 * - `lh_runtime_try`: Creates a protected block and exception frame
 * - `lh_runtime_catch`: Marks the beginning of exception handling code
 */

#ifndef LH_RUNTIME_TRY_H
#define LH_RUNTIME_TRY_H

#include <lh/exception/catch/initializer.h>
#include <lh/runtime/exception/catch/stack.h>
#include <lh/runtime/return.h>

/**
 * @def lh_runtime_try
 * @brief Macro for organizing an exception handling block (try-block)
 *
 * Creates a code block with exception handling that performs:
 * 1. Creation of an exception frame with automatic storage
 * 2. Setting an entry point for exception handling
 * 3. Checking the execution context (initial entry or jump on exception)
 *
 * @param e Name of the exception frame variable (automatically created)
 *
 * @note Used as a foundation for implementing try/catch blocks
 * @warning Requires a corresponding exception handling block
 * @warning Only supports local exception frames (automatic storage)
 *
 * Usage example:
 * @code
 * lh_runtime_try(frame) {
 *     // Code that might throw an exception
 * }
 * @endcode
 *
 * @see lh_runtime_catch()
 * @see lh_runtime_exception_catch_stack_capture()
 * @see lh_exception_catch_t
 * @see lh_exception_catch_empty_initializer()
 */
#define lh_runtime_try(e)                                                                          \
    lh_exception_catch_t e = lh_exception_catch_empty_initializer();                               \
    if (lh_runtime_exception_catch_stack_capture(&e) == 0)

/**
 * @def lh_runtime_catch
 * @brief Macro for organizing an exception handling catch-block
 *
 * Forms the second part of the exception handling construct (catch-block)
 * that follows a try-block created with lh_runtime_try().
 *
 * This macro simply expands to the 'else' keyword, creating a syntactic
 * construct that resembles traditional try-catch blocks while leveraging
 * C's native conditional flow control.
 *
 * @note Must be used in conjunction with lh_runtime_try()
 * @note The actual exception handling logic is implemented through
 *       the exception frame created by lh_runtime_try()
 *
 * Usage example:
 * @code
 * lh_runtime_try(frame) {
 *     // Try block code
 * } lh_runtime_catch {
 *     // Catch block code
 * }
 * @endcode
 *
 * @see lh_runtime_try()
 * @see lh_exception_catch_t
 * @see lh_runtime_exception_catch_stack_capture()
 */
#define lh_runtime_catch else

/**
 * @def lh_runtime_try_finalize
 * @brief Finalizes a `try` block by restoring the frame state pointer.
 *
 * The `lh_runtime_try_finalize` macro is used
 * to properly finalize a `try` block.
 *
 * If the `try` block execution was successful (no error occurred),
 * the frame state pointer needs to be moved back to its previous position
 * to prevent inconsistencies in the execution frame state management.
 *
 * The macro calls the `lh_runtime_frame_state_prev` function,
 * which moves the pointer to the previous frame state.
 *
 * Usage example:
 * @code
 * lh_runtime_try {
 *     // Code executed in the try block.
 *     lh_runtime_try_finalize();
 * } lh_runtime_catch {
 *     // Error handling.
 * }
 * @endcode
 *
 * @note Using `lh_runtime_try_finalize` is mandatory in `try` blocks
 *       to ensure proper frame state restoration when no error occurs.
 */
#define lh_runtime_try_finalize() lh_runtime_exception_catch_stack_prev()

/**
 * @def lh_runtime_try_return
 * @brief Terminates a `try` block with a return value.
 *
 * The `lh_runtime_try_return` macro ends the current `try` block by calling
 * `lh_runtime_try_finalize` to properly restore the frame state,
 * and then executes `return` with the provided `__VA_ARGS__` arguments.
 *
 * This macro is used for early exit from a `try` block, for example,
 * when immediate termination with a specific return value is required.
 * It ensures that the frame state is correctly restored before exiting the block.
 *
 * @param ... Arguments to be passed to the `return` statement.
 *
 * Usage example:
 * @code
 * lh_runtime_try {
 *     if (some_condition) {
 *         // Terminates try and returns `nullptr`
 *         lh_runtime_try_return(nullptr);
 *     }
 *     // Remaining code that won't execute if the early return triggers
 * } lh_runtime_catch {
 *     // Error handling
 * }
 * @endcode
 *
 * @note This macro must be used inside an `lh_runtime_try` block to ensure
 *       proper termination and frame state restoration.
 *
 *       Without calling `lh_runtime_try_finalize`,
 *       frame state management inconsistencies may occur.
 */
#define lh_runtime_try_return(...)                                                                 \
    lh_runtime_try_finalize();                                                                     \
    lh_runtime_return(__VA_ARGS__)

#endif // LH_RUNTIME_TRY_H