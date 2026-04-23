/**
 * @file noreturn.h
 * @brief Library alias: mark a function that never returns to its caller.
 *
 * **Noreturn** means control **never** resumes after the call in the normal
 * way — for example the function terminates the process (`abort`, `exit`),
 * runs forever, or transfers control elsewhere.
 *
 * The compiler then knows there is no fall-through:
 * * it can drop dead code after the call, tune warnings (e.g.
 * “missing return” on code paths that only call such a function), and model
 * control flow more accurately.
 *
 * The annotation must match reality: if a function **can** return,
 * marking it noreturn is undefined behavior from the compiler’s
 * point of view and may break optimizations.
 *
 * ::LH_ATTRIBUTE_NORETURN is the `lh` naming layer; it expands like
 * ::LH_COMPILER_ATTRIBUTE_NORETURN — see that macro for per-compiler spelling.
 */

#ifndef LH_ATTRIBUTE_NORETURN_H
#define LH_ATTRIBUTE_NORETURN_H

#include <lh/compiler/attribute/noreturn.h>

/**
 * @def LH_ATTRIBUTE_NORETURN
 * @brief Declare that this function does not return to the caller.
 *
 * Apply it like other function attributes
 * (before the return type / name, depending on your style and compiler).
 * Equivalent to ::LH_COMPILER_ATTRIBUTE_NORETURN.
 *
 * @see LH_COMPILER_ATTRIBUTE_NORETURN
 */
#define LH_ATTRIBUTE_NORETURN LH_COMPILER_ATTRIBUTE_NORETURN

#endif // LH_ATTRIBUTE_NORETURN_H
