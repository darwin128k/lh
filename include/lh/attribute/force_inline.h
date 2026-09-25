/**
 * @file force_inline.h
 * @brief Library alias: mandate that a function be inlined at every call site.
 *
 * ::LH_ATTRIBUTE_FORCE_INLINE is the `lh` naming layer; it expands like
 * ::LH_COMPILER_ATTRIBUTE_FORCE_INLINE — see that macro for per-compiler
 * spelling and when to reach for it (small, hot, measured-not-guessed).
 */

#ifndef LH_ATTRIBUTE_FORCE_INLINE_H
#define LH_ATTRIBUTE_FORCE_INLINE_H

#include <lh/compiler/attribute/force_inline.h>

/**
 * @def LH_ATTRIBUTE_FORCE_INLINE
 * @brief Write in place of `static inline` to mandate inlining.
 *
 * Unlike an ordinary `lh` function (declared in a header, defined once in
 * a `.c` file), a force-inline function is defined entirely in the header —
 * there is no `.c` file and no single shared copy.
 *
 * @see LH_COMPILER_ATTRIBUTE_FORCE_INLINE
 */
#define LH_ATTRIBUTE_FORCE_INLINE LH_COMPILER_ATTRIBUTE_FORCE_INLINE

#endif /* LH_ATTRIBUTE_FORCE_INLINE_H */
