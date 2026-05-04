/**
 * @file attribute.h
 * @brief Umbrella header for all `lh` attribute macros.
 *
 * Including this file pulls in every attribute header provided by the library.
 * Prefer including individual attribute headers when only a specific attribute is needed.
 */

#ifndef LH_ATTRIBUTE_H
#define LH_ATTRIBUTE_H

#include <lh/attribute/builtin.h>
#include <lh/attribute/noreturn.h>
#include <lh/attribute/symbol.h>
#include <lh/attribute/thread_local.h>
#include <lh/attribute/unused.h>

/**
 * @def LH_ATTRIBUTE(name)
 * @brief Compose an attribute macro name from a token.
 *
 * Expands to `LH_ATTRIBUTE_##name`, allowing attribute macros to be
 * referenced by their short token rather than their full name.
 *
 * @param name  Attribute token (e.g. `BUILTIN`, `NORETURN`,
 *              `THREAD_LOCAL`, `UNUSED`).
 *
 * Example usage:
 * @code{.c}
 * LH_ATTRIBUTE(BUILTIN) int add(int a, int b) { return a + b; }
 * LH_ATTRIBUTE(NORETURN) void fatal(const char *msg);
 * @endcode
 */
#define LH_ATTRIBUTE(name) LH_ATTRIBUTE_##name

#endif // LH_ATTRIBUTE_H