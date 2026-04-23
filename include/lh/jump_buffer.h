/**
 * @file jump_buffer.h
 * @brief Type alias for the C standard non-local jump buffer (`jmp_buf`).
 *
 * `setjmp.h` defines `jmp_buf`: an opaque array type that holds enough state
 * for `setjmp` to mark a return point and for `longjmp` to jump back there
 * later, unwinding the stack like a controlled “goto” across functions.
 *
 * `lh` uses this as the storage inside a catch site (see `lh/exception/catch.h`
 * and the runtime stack in `lh/runtime/exception/catch/stack.h`).
 *
 * You normally do not touch the bytes of a `jmp_buf` yourself — only pass
 * it to `setjmp` and `longjmp` as the standard specifies.
 *
 * @note C places strict rules on `setjmp`/`longjmp` (when `setjmp` may appear,
 *       lifetime of the buffer, and when local variables keep defined values).
 *       Violations are undefined behavior.
 */

#ifndef LH_JUMP_BUFFER_H
#define LH_JUMP_BUFFER_H

#include <setjmp.h>

/**
 * @typedef lh_jump_buffer_t
 * @brief Same as `jmp_buf` from `<setjmp.h>`.
 *
 * Keeps a portable name in the `lh` namespace.
 * Semantics are exactly those of the platform’s `jmp_buf`
 * (size and layout are implementation-defined).
 *
 * @see lh_exception_catch_t
 */
typedef jmp_buf lh_jump_buffer_t;

#endif // LH_JUMP_BUFFER_H
