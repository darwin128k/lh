/**
 * @file noinline.h
 * @brief Public macro that keeps a function a call.
 *
 * Provides ::LH_ATTRIBUTE_NOINLINE — alias for ::LH_COMPILER_ATTRIBUTE_NOINLINE.
 * Place it on the declaration. A long walk that is pasted into every caller
 * costs more than the call.
 */

#ifndef LH_ATTRIBUTE_NOINLINE_H
#define LH_ATTRIBUTE_NOINLINE_H

#include <lh/core/compiler/attribute/noinline.h>

/**
 * @def LH_ATTRIBUTE_NOINLINE
 * @brief Same as ::LH_COMPILER_ATTRIBUTE_NOINLINE.
 */
#define LH_ATTRIBUTE_NOINLINE LH_COMPILER_ATTRIBUTE_NOINLINE

#endif /* LH_ATTRIBUTE_NOINLINE_H */
