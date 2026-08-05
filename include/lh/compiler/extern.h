/**
 * @file extern.h
 * @brief Overridable `extern` keyword macro.
 *
 * Provides ::LH_COMPILER_EXTERN — a thin, overridable wrapper
 * around the `extern` keyword used internally by other library headers.
 */

#ifndef LH_COMPILER_EXTERN_H
#define LH_COMPILER_EXTERN_H

#ifndef LH_COMPILER_EXTERN
/**
 * @def LH_COMPILER_EXTERN
 * @brief Expands to `extern`.
 *
 * Overridable wrapper used by ::LH_COMPILER_EXTERN_C and related macros.
 * Can be redefined before including this header if a different linkage
 * specifier is needed.
 */
#    define LH_COMPILER_EXTERN extern
#endif /* LH_COMPILER_EXTERN */

#endif /* LH_COMPILER_EXTERN_H */