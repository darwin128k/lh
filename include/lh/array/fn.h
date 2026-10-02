/**
 * @file fn.h
 * @brief Callback signatures for ::lh_array_t.
 */

#ifndef LH_ARRAY_FN_H
#define LH_ARRAY_FN_H

#include <lh/bool.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>

/**
 * @typedef lh_array_cmp_fn
 * @brief Order two elements for ::lh_array_sort and ::lh_array_unique.
 *
 * @param a       Pointer to an element.
 * @param b       Pointer to another element.
 * @param context What the caller passed alongside the callback.
 * @return Negative if @p a goes first, positive if @p b does, `0` if they
 *         are equal.
 */
typedef lh_int_t(lh_array_cmp_fn)(const lh_ptr a, const lh_ptr b, lh_ptr context);

/**
 * @typedef lh_array_pred_fn
 * @brief Decide about one element, for ::lh_array_filter.
 *
 * @param value   Pointer to the element.
 * @param context What the caller passed alongside the callback.
 */
typedef lh_bool_t(lh_array_pred_fn)(const lh_ptr value, lh_ptr context);

#endif /* LH_ARRAY_FN_H */
