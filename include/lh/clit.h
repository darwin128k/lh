/**
 * @file clit.h
 * @brief Compound-literal pointer helper macros.
 *
 * Provides ::lh_clit_of and ::lh_clit to build a typed compound literal
 * via initializer helpers and immediately take its address.
 */

#ifndef LH_CLIT_H
#define LH_CLIT_H

#include <lh/initializer.h>
#include <lh/ref.h>

/**
 * @def lh_clit_of(T, initializer, ...)
 * @brief Create a compound literal of type @p T using @p initializer and return its address.
 *
 * Expands to:
 * `lh_ref(lh_initializer_of(T, initializer, __VA_ARGS__))`
 *
 * @param T           Target type of the compound literal.
 * @param initializer Initializer macro to invoke.
 * @param ...         Arguments forwarded to @p initializer.
 *
 * @return Pointer to the created compound literal expression.
 */
#define lh_clit_of(T, initializer, ...) lh_ref(lh_initializer_of(T, initializer, __VA_ARGS__))

/**
 * @def lh_clit(T, ...)
 * @brief Create a compound literal of type @p T
 *        with ::lh_initializer_of_type and return its address.
 *
 * Shorthand for:
 * `lh_clit_of(T, lh_initializer_of_type, __VA_ARGS__)`.
 *
 * @param T   Target type of the compound literal.
 * @param ... Initializer values or designated initializers.
 *
 * @return Pointer to the created compound literal expression.
 */
#define lh_clit(T, ...) lh_clit_of(T, lh_initializer_of_type, __VA_ARGS__)

#endif // LH_CLIT_H