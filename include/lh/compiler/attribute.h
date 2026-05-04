/**
 * @file attribute.h
 * @brief Umbrella header for all compiler attribute macros.
 *
 * Including this file pulls in every `compiler_attribute_*` header:
 * builtin, noreturn, symbol visibility, thread-local, and unused.
 * Prefer individual headers when only one attribute is needed.
 */

#ifndef LH_COMPILER_ATTRIBUTE_H
#define LH_COMPILER_ATTRIBUTE_H

#include <lh/compiler/attribute/builtin.h>
#include <lh/compiler/attribute/noreturn.h>
#include <lh/compiler/attribute/symbol.h>
#include <lh/compiler/attribute/thread_local.h>
#include <lh/compiler/attribute/unused.h>

#endif // LH_COMPILER_ATTRIBUTE_H