/**
 * @file symbol.h
 * @brief ::LH_ATTRIBUTE_SYMBOL — marks the public `lh` API.
 *
 * `lh` is built and linked only as a static library, so a public function
 * needs no export or import annotation: it is an ordinary external symbol
 * inside the final link. The macro stays on every public declaration as a
 * marker of "this is API" and expands to nothing.
 */

#ifndef LH_ATTRIBUTE_SYMBOL_H
#define LH_ATTRIBUTE_SYMBOL_H

/**
 * @def LH_ATTRIBUTE_SYMBOL
 * @brief Marks a public `lh` function. Expands to nothing (static library).
 */
#define LH_ATTRIBUTE_SYMBOL

#endif /* LH_ATTRIBUTE_SYMBOL_H */
