/**
 * @file foreach.h
 * @brief Generic "iterate n elements, bind each to a name" loop macro.
 *
 * Unlike ::lh_memory_std_copy / ::lh_memory_std_set and the rest of the byte
 * kernels, this one carries no operation of its own — it is a general
 * control-flow primitive, the same kind of thing ::lh_optional_ref is (a
 * macro that expands to a loop/branch header rather than a full statement).
 */

#ifndef LH_FOREACH_H
#define LH_FOREACH_H

#include <lh/size.h>
#include <lh/util/arg.h>
#include <lh/util/ptr.h>

/**
 * @def lh_foreach(T, var, ptr, n)
 * @brief Iterate over @p n elements starting at @p ptr, binding each in turn to @p var.
 *
 * Expands to a `for` loop header — like ::lh_optional_ref expands to an `if`
 * header — so follow it with the loop body as a statement or block.
 *
 * @param T   Element type.
 * @param var Name for the loop variable (declared as `T *`, bound to each element
 *            in turn) — a fresh identifier, not an existing variable.
 * @param ptr Sequence pointer.
 * @param n   Number of elements to iterate.
 *
 * Example usage:
 * @code{.c}
 * int v[3] = {1, 2, 3};
 * lh_foreach(int, it, v, 3)
 * {
 *     *it *= 2;
 * }
 * // v == {2, 4, 6}
 * @endcode
 */
#define lh_foreach(T, var, ptr, n)                                                                \
    for (T *var = lh_ptr_cast(T, ptr), *lh_arg_concat(var, _end) = var + (n);                     \
         var != lh_arg_concat(var, _end); ++var)

#endif /* LH_FOREACH_H */
