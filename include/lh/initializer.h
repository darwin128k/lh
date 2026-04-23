/**
 * @file initializer.h
 * @brief Compound-literal initializer macros.
 *
 * Provides ::lh_initializer, ::lh_initializer_of, ::lh_initializer_of_type,
 * and ::lh_initializer_of_struct — helpers for constructing C compound
 * literals in a readable, type-safe way.
 */

#ifndef LH_INITIALIZER_H
#define LH_INITIALIZER_H

/**
 * @def lh_initializer(...)
 * @brief Wrap arguments in a brace-enclosed initializer list.
 *
 * Expands to `{__VA_ARGS__}`.
 * Intended to be passed as the @p initializer argument to ::lh_initializer_of.
 *
 * @param ... Initializer values or designated initializers.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer(.x = 1, .y = 2)  // expands to { .x = 1, .y = 2 }
 * @endcode
 */
#define lh_initializer(...) {__VA_ARGS__}

/**
 * @def lh_initializer_of(T, initializer, ...)
 * @brief Cast an initializer call to type @p T.
 *
 * Expands to `((T)initializer(__VA_ARGS__))`.
 * This creates a compound literal of type @p T using a custom @p initializer macro.
 *
 * @param T           Target type for the compound literal.
 * @param initializer Initializer macro to invoke (e.g. ::lh_initializer).
 * @param ...         Arguments forwarded to @p initializer.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer_of(struct point, lh_initializer, .x = 1, .y = 2)
 * // expands to ((struct point){ .x = 1, .y = 2 })
 * @endcode
 */
#define lh_initializer_of(T, initializer, ...) ((T)initializer(__VA_ARGS__))

/**
 * @def lh_initializer_of_type(T, ...)
 * @brief Create a compound literal of type @p T.
 *
 * Shorthand for `lh_initializer_of(T, lh_initializer, ...)`.
 *
 * Expands to `((T){__VA_ARGS__})`.
 *
 * @param T   Target type for the compound literal.
 * @param ... Initializer values or designated initializers.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer_of_type(int[3], 1, 2, 3)
 * // expands to ((int[3]){1, 2, 3})
 * @endcode
 */
#define lh_initializer_of_type(T, ...) lh_initializer_of(T, lh_initializer, __VA_ARGS__)

/**
 * @def lh_initializer_of_struct(T, ...)
 * @brief Create a compound literal of type `struct T`.
 *
 * Shorthand for `lh_initializer_of_type(struct T, ...)`.
 *
 * Expands to `((struct T){__VA_ARGS__})`.
 *
 * @param T   Struct tag (without the `struct` keyword).
 * @param ... Initializer values or designated initializers.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer_of_struct(point, .x = 1, .y = 2)
 * // expands to ((struct point){ .x = 1, .y = 2 })
 * @endcode
 */
#define lh_initializer_of_struct(T, ...) lh_initializer_of_type(struct T, __VA_ARGS__)

#endif // LH_INITIALIZER_H
