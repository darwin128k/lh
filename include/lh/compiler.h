/**
 * @file compiler.h
 * @brief Umbrella header for all `lh` compiler macros.
 *
 * Including this file pulls in every `compiler_*` header provided by the library:
 * attributes, constructor/destructor, C linkage, standard version,
 * unreachable hint, and compiler version.
 */

#ifndef LH_COMPILER_H
#define LH_COMPILER_H

#include <lh/compiler/attribute.h>
#include <lh/compiler/constructor.h>
#include <lh/compiler/destructor.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/std/version.h>
#include <lh/compiler/unreachable.h>
#include <lh/compiler/version.h>

/**
 * @def LH_COMPILER(name)
 * @brief Compose a compiler macro name from a token.
 *
 * Expands to `LH_COMPILER_##name`, allowing compiler macros to be
 * referenced by their short token rather than their full name.
 *
 * @param name  Token suffix (e.g. `TYPE`, `OS`, `BIT_WIDTH`).
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER(TYPE) == LH_COMPILER(TYPE_MSVC)
 *     // MSVC-specific code
 * #endif
 * @endcode
 */
#define LH_COMPILER(name) LH_COMPILER_##name

#endif // LH_COMPILER_H