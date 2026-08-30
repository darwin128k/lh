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
typedef struct lh_exception_catch
{
    lh_exception_t exception; /**< Filled when unwinding reaches this catch (wraps ::lh_error_t). */
    lh_jump_buffer_t env;     /**< `setjmp` / `longjmp` state for this site (::lh_jump_buffer_t). */
} lh_exception_catch_t;       /**< Typedef for struct ::lh_exception_catch. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mutable pointer to the exception stored in @p self.
 *
 * This accessor intentionally exposes only the thrown payload. The jump buffer
 * remains owned by the runtime catch stack.
 *
 * @param self Catch frame (not null).
 * @return Pointer to @p self->exception.
 */
LH_ATTRIBUTE_SYMBOL
lh_exception_t *
lh_exception_catch_get_exception(lh_exception_catch_t *self);

/**
 * @brief Read-only pointer to the exception stored in @p self.
 * @param self Catch frame (not null).
 * @return Pointer to @p self->exception.
 */
LH_ATTRIBUTE_SYMBOL
const lh_exception_t *
lh_exception_catch_get_exception_as_const(const lh_exception_catch_t *self);

/**
 * @brief Mutable pointer to the embedded error stored in the caught exception.
 *
 * @param self Catch frame (not null).
 * @return Pointer to the embedded ::lh_error_t.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_t *
lh_exception_catch_get_error(lh_exception_catch_t *self);

/**
 * @brief Read-only pointer to the embedded error stored in the caught exception.
 * @param self Catch frame (not null).
 * @return Pointer to the embedded ::lh_error_t.
 */
LH_ATTRIBUTE_SYMBOL
const lh_error_t *
lh_exception_catch_get_error_as_const(const lh_exception_catch_t *self);

/**
 * @brief Return the caught exception error code.
 *
 * @param self Catch frame (not null).
 * @return Code stored in the caught exception.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_exception_catch_get_code(const lh_exception_catch_t *self);

/**
 * @brief Return the caught exception description.
 *
 * @param self Catch frame (not null).
 * @return Description stored in the caught exception; may be null.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_desc_t
lh_exception_catch_get_desc(const lh_exception_catch_t *self);

/**
 * @brief Return the caught exception description or @p fallback when it is null.
 *
 * @param self Catch frame (not null).
 * @param fallback Description returned when the caught exception has no description.
 * @return Stored description when non-null, otherwise @p fallback.
 */
LH_ATTRIBUTE_SYMBOL
lh_error_desc_t
lh_exception_catch_get_desc_or(const lh_exception_catch_t *self, lh_error_desc_t fallback);

/**
 * @brief Test whether the caught exception code is equal to @p code.
 *
 * @param self Catch frame (not null).
 * @param code Error code to compare with.
 * @return ::lh_bool_true when the caught exception carries @p code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_has_code(const lh_exception_catch_t *self, lh_error_code_t code);

/**
 * @brief Test whether the caught exception has the success code.
 *
 * @param self Catch frame (not null).
 * @return ::lh_bool_true when the caught exception stores ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_is_ok(const lh_exception_catch_t *self);

/**
 * @brief Test whether the caught exception has a non-success code.
 *
 * @param self Catch frame (not null).
 * @return ::lh_bool_true when the caught exception does not store ::lh_error_code_ok.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_is_failure(const lh_exception_catch_t *self);

/**
 * @brief Test whether the caught exception has a non-null description.
 *
 * @param self Catch frame (not null).
 * @return ::lh_bool_true when the caught exception stores a description pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_has_desc(const lh_exception_catch_t *self);

/**
 * @brief Test whether the caught exception is the cleared success state.
 *
 * @param self Catch frame (not null).
 * @return ::lh_bool_true when the caught exception stores ::lh_error_code_ok and no description.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_is_empty(const lh_exception_catch_t *self);

/**
 * @brief Test whether two catch frames carry equal exceptions.
 *
 * Jump buffer state is ignored. Debug origin metadata is ignored by
 * ::lh_exception_equals.
 *
 * @param self Catch frame (not null).
 * @param other Catch frame to compare with (not null).
 * @return ::lh_bool_true when both frames carry equal exceptions.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_equals(const lh_exception_catch_t *self, const lh_exception_catch_t *other);

/**
 * @brief Test whether two catch frames carry the same exception code.
 *
 * Descriptions, debug origins, and jump buffer state are ignored.
 *
 * @param self Catch frame (not null).
 * @param other Catch frame to compare with (not null).
 * @return ::lh_bool_true when both frames carry the same code.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_has_same_code(const lh_exception_catch_t *self,
                                 const lh_exception_catch_t *other);

/**
 * @brief Test whether two catch frames carry different exception codes.
 *
 * Descriptions, debug origins, and jump buffer state are ignored.
 *
 * @param self Catch frame (not null).
 * @param other Catch frame to compare with (not null).
 * @return ::lh_bool_true when both frames carry different codes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_exception_catch_has_diff_code(const lh_exception_catch_t *self,
                                 const lh_exception_catch_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXCEPTION_CATCH_H */
