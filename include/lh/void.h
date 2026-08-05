/**
 * @file void.h
 * @brief Alias for the `void` type.
 *
 * Provides ::lh_void — a macro alias for the built-in `void` keyword,
 * keeping the library's naming convention consistent across all type aliases.
 */

#ifndef LH_VOID_H
#define LH_VOID_H

/**
 * @def lh_void
 * @brief Alias for the built-in `void` type.
 *
 * Example usage:
 * @code{.c}
 * lh_void foo(void);
 * @endcode
 */
#define lh_void void

#endif /* LH_VOID_H */