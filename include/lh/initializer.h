/**
 * @file initializer.h
 * @brief Compound-literal initializer macros.
 *
 * Provides ::lh_initializer, ::lh_initializer_of, ::lh_initializer_of_type,
 * and ::lh_initializer_of_struct — helpers for constructing a typed brace
 * initializer in a readable way. C uses a compound literal; C++ uses
 * list-initialization so MSVC does not hit C4576 on `(T){ ... }`.
 */

#ifndef LH_INITIALIZER_H
#define LH_INITIALIZER_H

#include <lh/compiler/cxx.h>

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
 * @brief Typed value from an initializer macro @p initializer.
 *
 * Invokes @p initializer with the remaining arguments and yields a value of
 * type @p T:
 *
 * | Language | Expansion                         |
 * |----------|-----------------------------------|
 * | C        | `((T)initializer(__VA_ARGS__))`   |
 * | C++      | `(T initializer(__VA_ARGS__))`    |
 *
 * C is a compound literal. C++ is list-initialization (`T{ ... }`); a
 * parenthesized type before a brace list is a GNU extension and MSVC C4576.
 *
 * @param T           Target type.
 * @param initializer Initializer macro to invoke (e.g. ::lh_initializer).
 * @param ...         Arguments forwarded to @p initializer.
 *
 * @p T must be a type name (`lh_str_view_t`, a typedef). A `struct` tag
 * goes through ::lh_initializer_of_struct — in C++ `(struct T{ ... })`
 * is a type definition, not a value.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer_of(point_t, lh_initializer, .x = 1, .y = 2)
 * @endcode
 */
#ifdef LH_COMPILER_CXX
#    define lh_initializer_of(T, initializer, ...) (T initializer(__VA_ARGS__))
#else
#    define lh_initializer_of(T, initializer, ...) ((T)initializer(__VA_ARGS__))
#endif /* LH_COMPILER_CXX */

/**
 * @def lh_initializer_of_type(T, ...)
 * @brief Typed brace initializer of type @p T.
 *
 * Shorthand for `lh_initializer_of(T, lh_initializer, ...)`.
 *
 * | Language | Expansion              |
 * |----------|------------------------|
 * | C        | `((T){__VA_ARGS__})`   |
 * | C++      | `(T{__VA_ARGS__})`     |
 *
 * @param T   Target type.
 * @param ... Initializer values or designated initializers.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer_of_type(int[3], 1, 2, 3)
 * @endcode
 */
#define lh_initializer_of_type(T, ...) lh_initializer_of(T, lh_initializer, __VA_ARGS__)

/**
 * @def lh_initializer_of_struct(T, ...)
 * @brief Typed brace initializer for struct tag @p T.
 *
 * @p T is the tag without the `struct` keyword.
 *
 * | Language | Expansion                     |
 * |----------|-------------------------------|
 * | C        | `((struct T){__VA_ARGS__})`   |
 * | C++      | `(T{__VA_ARGS__})`            |
 *
 * C++ cannot write `(struct T{ ... })` — that parses as a type definition.
 * The injected class name `T` is used instead.
 *
 * @param T   Struct tag (without the `struct` keyword).
 * @param ... Initializer values or designated initializers.
 *
 * Example usage:
 * @code{.c}
 * lh_initializer_of_struct(point, .x = 1, .y = 2)
 * @endcode
 */
#ifdef LH_COMPILER_CXX
#    define lh_initializer_of_struct(T, ...) lh_initializer_of(T, lh_initializer, __VA_ARGS__)
#else
#    define lh_initializer_of_struct(T, ...) lh_initializer_of_type(struct T, __VA_ARGS__)
#endif /* LH_COMPILER_CXX */

#endif /* LH_INITIALIZER_H */
